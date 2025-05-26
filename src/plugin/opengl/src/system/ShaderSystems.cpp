#include "OpenGL.pch.hpp"

#include "Camera.hpp"
#include "DirectionalLight.hpp"
#include "LightInfo.hpp"
#include "ShaderManager.hpp"
#include "ShaderSystems.hpp"

void ES::Plugin::OpenGL::System::LoadDefaultShader(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440

        layout (location = 0) in vec4 VertexPosition;
        layout (location = 1) in vec3 VertexNormal;

        out vec3 Position;
        out vec3 Normal;

        uniform mat4 ModelMatrix;
        uniform mat3 NormalMatrix;
        uniform mat4 MVP;

        void main()
        {
            Normal = normalize(NormalMatrix * VertexNormal);
            Position = (ModelMatrix * VertexPosition).xyz;
            gl_Position = MVP * VertexPosition;
        }
    )";

    const char *fragmentShader = R"(
        #version 440

        in vec3 Position;
        in vec3 Normal;

        uniform vec3 CamPos;

        uniform int NumberLights;

        struct LightInfo {
            vec4 Position;      // Light position (x, y, z) + w (Type of light)
            vec4 Colour;        // Light colour (x, y, z) + w (Intensity)
        };

        layout(std140, binding = 0) buffer LightBuffer {
            LightInfo Light[];
        };

        struct MaterialInfo {
            vec3 Ka; // Ambient reflectivity
            vec3 Kd; // Diffuse reflectivity
            vec3 Ks; // Specular reflectivity
            float Shiness; // Specular shininess factor (phong exponent)
        };
        uniform MaterialInfo Material;

        out vec4 FragColor;

        void main() {
            vec3 finalColor = vec3(0.0, 0.0, 0.0);
            vec3 ambient = vec3(0.0, 0.0, 0.0);

            for (int i = 0; i < NumberLights; i++) {
                int type = int(Light[i].Position.w);
                vec3 pos = Light[i].Position.xyz;
                vec3 colour = Light[i].Colour.rgb;

                if (type == 0) { // Point light
                    vec3 L = normalize(pos - Position);
                    vec3 V = normalize(CamPos - Position);
                    vec3 HalfwayVector = normalize(V + L);

                    vec3 diffuse = Material.Kd * colour * max(dot(L, Normal), 0.0);
                    vec3 specular = Material.Ks * colour * pow(max(dot(HalfwayVector, Normal), 0.0), Material.Shiness);
                    finalColor += diffuse + specular;
                } else if (type == 1) { // Ambient light
                    ambient += Material.Ka * colour;
                }
            }

            FragColor = vec4(finalColor + ambient, 1.0);
        }
    )";

    auto &shaderManager = core.GetResource<Resource::ShaderManager>();
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"default"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::OpenGL::System::LoadDefaultTextShader(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440
        layout (location = 0) in vec4 vertex;

        out vec2 TexCoords;

        uniform mat4 Projection;

        void main() {
            gl_Position = Projection * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex.zw;
        }
    )";

    const char *fragmentShader = R"(
        #version 440 core
        in vec2 TexCoords;
        out vec4 FragColor;

        uniform sampler2D Text;
        uniform vec3 TextColor;

        void main() {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(Text, TexCoords).r);
            FragColor = vec4(TextColor, 1.0) * sampled;
        }
    )";

    auto &shaderManager = core.GetResource<Resource::ShaderManager>();
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"textDefault"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::OpenGL::System::LoadDefaultSpriteShader(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 model;
        uniform mat4 projection;


        void main()
        {
            // gl_Position = vec4(aPos, 1.0) + vec4(0.5, 0.5, 0.0, 0.0);
            gl_Position = projection * model * vec4(aPos, 1.0);
        }
    )";

    const char *fragmentShader = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec4 color;
        void main()
        {
            FragColor = color;
        }
    )";

    auto &shaderManager = core.GetResource<Resource::ShaderManager>();
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"2DDefault"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::OpenGL::System::SetupShaderUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"default"});

    // Add uniforms
    m_shaderProgram.AddUniform("MVP");
    m_shaderProgram.AddUniform("ModelMatrix");  // View*Model : mat4
    m_shaderProgram.AddUniform("NormalMatrix"); // Refer next slide : mat3

    m_shaderProgram.AddUniform("NumberLights");
    m_shaderProgram.AddSSBO("LightBuffer", 0, sizeof(ES::Plugin::OpenGL::Utils::LightInfo));
    m_shaderProgram.AddUniform("Material.Ka");
    m_shaderProgram.AddUniform("Material.Kd");
    m_shaderProgram.AddUniform("Material.Ks");
    m_shaderProgram.AddUniform("Material.Shiness");

    m_shaderProgram.AddUniform("CamPos");
}

void ES::Plugin::OpenGL::System::SetupTextShaderUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"textDefault"});

    m_shaderProgram.AddUniform("Projection");
    m_shaderProgram.AddUniform("Text");
    m_shaderProgram.AddUniform("TextColor");
}

void ES::Plugin::OpenGL::System::SetupSpriteShaderUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"2DDefault"});

    m_shaderProgram.AddUniform("color");
    m_shaderProgram.AddUniform("model");
    m_shaderProgram.AddUniform("projection");
}

void ES::Plugin::OpenGL::System::LoadNoTextureLightShadowShader(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440

        layout (location = 0) in vec4 VertexPosition;
        layout (location = 1) in vec3 VertexNormal;
        layout (location = 2) in vec2 VertexTexCoord;

        out vec3 Position;
        out vec3 Normal;
        out vec2 TexCoord;
        out vec4 FragPosLightSpace;

        uniform mat4 ModelMatrix;
        uniform mat3 NormalMatrix;
        uniform mat4 MVP;
        uniform mat4 lightSpaceMatrix;

        void main()
        {
            Normal = normalize(NormalMatrix * VertexNormal);
            Position = (ModelMatrix * VertexPosition).xyz;
            TexCoord = VertexTexCoord;
            FragPosLightSpace = lightSpaceMatrix * vec4(Position, 1.0);
            gl_Position = MVP * VertexPosition;
        }
    )";

    const char *fragmentShader = R"(
        #version 440

        in vec3 Position;
        in vec3 Normal;
        in vec2 TexCoord;
        in vec4 FragPosLightSpace;

        uniform sampler2D shadowMap;

        uniform vec3 CamPos;

        uniform int NumberLights;

        struct LightInfo {
            vec4 Position;   // Light position (x, y, z) + w (Type of light)
            vec4 Colour;     // Light colour (r, g, b) + a (intensity)
        };

        layout(std140, binding = 0) buffer LightBuffer {
            LightInfo Light[];
        };

        struct MaterialInfo {
            vec3 Ka; // Ambient reflectivity
            vec3 Kd; // Diffuse reflectivity
            vec3 Ks; // Specular reflectivity
            float Shiness; // Specular shininess factor (phong exponent)
        };
        uniform MaterialInfo Material;

        out vec4 FragColor;

        float ShadowCalculation(vec4 fragPosLightSpace)
        {
            // perform perspective divide
            vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
            // transform to [0,1] range
            projCoords = projCoords * 0.5 + 0.5;
            // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
            float closestDepth = texture(shadowMap, projCoords.xy).r; 
            // get depth of current fragment from light's perspective
            float currentDepth = projCoords.z;
            // check whether current frag pos is in shadow
            float bias = 0.005;
            // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
            float shadow = 0.0;

            vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
            for(int x = -1; x <= 1; ++x)
            {
                for(int y = -1; y <= 1; ++y)
                {
                    float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                    shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
                }    
            }
            shadow /= 9.0;

            return shadow;
        }

        void main() {
            vec3 finalColor = vec3(0.0, 0.0, 0.0);
            vec3 ambient = vec3(0.0, 0.0, 0.0);

            float shadow = ShadowCalculation(FragPosLightSpace); 

            for (int i = 0; i < NumberLights; i++) {
                int type = int(Light[i].Position.w);
                vec3 pos = Light[i].Position.xyz;
                vec3 colour = Light[i].Colour.rgb;

                if (type == 0) { // Point light
                    vec3 L = normalize(pos - Position);
                    vec3 V = normalize(CamPos - Position);
                    vec3 HalfwayVector = normalize(V + L);

                    vec3 diffuse = Material.Kd * colour * max(dot(L, Normal), 0.0);
                    vec3 specular = Material.Ks * colour * pow(max(dot(HalfwayVector, Normal), 0.0), Material.Shiness);
                    // Apply shadow to diffuse and specular
                    finalColor += (diffuse + specular) * (1.0 - shadow);
                } else if (type == 1) { // Ambient light
                    ambient += Material.Ka * colour;
                }
            }
            finalColor += ambient;
            FragColor = vec4(finalColor, 1.0);
        }
    )";

    auto &shaderManager = core.GetResource<Resource::ShaderManager>();
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"noTextureLightShadow"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::OpenGL::System::SetupNoTextureLightShadowShader(ES::Engine::Core &core)
{
    auto &m_shaderProgram =
        core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"noTextureLightShadow"});
    m_shaderProgram.AddUniform("shadowMap");
    m_shaderProgram.AddUniform("lightSpaceMatrix");
    m_shaderProgram.AddUniform("MVP");
    m_shaderProgram.AddUniform("ModelMatrix");
    m_shaderProgram.AddUniform("NormalMatrix");
    m_shaderProgram.AddUniform("NumberLights");
    m_shaderProgram.AddSSBO("LightBuffer", 0, sizeof(ES::Plugin::OpenGL::Utils::LightInfo));
    m_shaderProgram.AddUniform("Material.Ka");
    m_shaderProgram.AddUniform("Material.Kd");
    m_shaderProgram.AddUniform("Material.Ks");
    m_shaderProgram.AddUniform("Material.Shiness");
    m_shaderProgram.AddUniform("CamPos");
}

void ES::Plugin::OpenGL::System::LoadDepthMapShader(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 330 core
        layout (location = 0) in vec4 VertexPosition;

        uniform mat4 lightSpaceMatrix;
        uniform mat4 model;

        void main()
        {
            gl_Position = lightSpaceMatrix * model * VertexPosition;
        } 
    )";

    const char *fragmentShader = R"(
        #version 330 core

        void main()
        {             
            // We don't need to output anything for the depth map shader
            // The depth value will be written to the depth buffer automatically
            // so we can leave this empty.
        }  
    )";

    auto &shaderManager = core.GetResource<Resource::ShaderManager>();
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"depthMap"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::OpenGL::System::SetupDepthMapShader(ES::Engine::Core &core)
{
    auto &m_shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"depthMap"});
    m_shaderProgram.AddUniform("lightSpaceMatrix");
    m_shaderProgram.AddUniform("model");
}

void ES::Plugin::OpenGL::System::UpdateNoTextureLightShadowShader(ES::Engine::Core &core)
{
    const auto &light = core.GetResource<ES::Plugin::OpenGL::Resource::DirectionalLight>();
    auto &shaderProgram = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(
        entt::hashed_string{"noTextureLightShadow"});
    shaderProgram.Use();

    // Link texture to the shader
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, light.depthMap);
    glUniform1i(shaderProgram.GetUniform("shadowMap"), 1);

    // Link Light Space Matrix to the shader
    glUniformMatrix4fv(shaderProgram.GetUniform("lightSpaceMatrix"), 1, GL_FALSE,
                       glm::value_ptr(light.lightSpaceMatrix));

    // Link Camera Position to the shader
    glUniform3fv(shaderProgram.GetUniform("CamPos"), 1,
                 glm::value_ptr(core.GetResource<OpenGL::Resource::Camera>().viewer.getViewPoint()));

    shaderProgram.Disable();
}

void ES::Plugin::OpenGL::System::UpdateDepthMapShader(ES::Engine::Core &core)
{
    const auto &light = core.GetResource<ES::Plugin::OpenGL::Resource::DirectionalLight>();
    auto &shaderProgram =
        core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"depthMap"});

    shaderProgram.Use();

    // Link Light Space Matrix to the shader
    glUniformMatrix4fv(shaderProgram.GetUniform("lightSpaceMatrix"), 1, GL_FALSE,
                       glm::value_ptr(light.lightSpaceMatrix));

    shaderProgram.Disable();
}
