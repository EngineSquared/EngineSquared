#include "ShaderSystems.hpp"
#include "ShaderManager.hpp"

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

        struct LightInfo {
            vec4 Position; // Light position in eye coords.
            vec3 Intensity; // Light intensity
        };
        uniform LightInfo Light[5];

        struct MaterialInfo {
            vec3 Ka; // Ambient reflectivity
            vec3 Kd; // Diffuse reflectivity
            vec3 Ks; // Specular reflectivity
            float Shiness; // Specular shininess factor (phong exponent)
        };
        uniform MaterialInfo Material;

        out vec4 FragColor;

        void main() {
            vec3 finalColor = vec3(0,0,0);
            vec3 ambient = Material.Ka * Light[0].Intensity;
            for (int i = 0; i < 4; i++) {
                vec3 L = normalize(Light[i].Position.xyz - Position);
                vec3 V = normalize(CamPos - Position);
                vec3 diffuse = Material.Kd * Light[i].Intensity * max( dot(L, Normal), 0.0);
                vec3 HalfwayVector = normalize(V + L);
                vec3 specular = Material.Ks * Light[i].Intensity * pow( max( dot( HalfwayVector, Normal), 0.0), Material.Shiness);
                finalColor = finalColor + diffuse + specular;
            }
            vec3 L = normalize(Light[4].Position.xyz);
            vec3 V = normalize(CamPos - Position);
            vec3 diffuse = Material.Kd * Light[4].Intensity * max( dot(L, Normal), 0.0);
            vec3 HalfwayVector = normalize(V + L);
            vec3 specular = Material.Ks * Light[4].Intensity * pow( max( dot( HalfwayVector, Normal), 0.0), Material.Shiness);
            finalColor = finalColor + diffuse + specular;
            finalColor = ambient + finalColor;
            FragColor = vec4(finalColor, 1.0);
        }
    )";

    auto &shaderManager = core.GetResource<Resource::ShaderManager>();
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"default"});
    sp.Create();
    sp.initFromStrings(vertexShader, fragmentShader);
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
    sp.initFromStrings(vertexShader, fragmentShader);
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
    sp.initFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::OpenGL::System::SetupShaderUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"default"});

    // Add uniforms
    m_shaderProgram.addUniform("MVP");
    m_shaderProgram.addUniform("ModelMatrix");  // View*Model : mat4
    m_shaderProgram.addUniform("NormalMatrix"); // Refer next slide : mat3

    for (int i = 0; i < 5; i++)
    {
        m_shaderProgram.addUniform(fmt::format("Light[{}].Position", i));
        m_shaderProgram.addUniform(fmt::format("Light[{}].Intensity", i));
    }
    m_shaderProgram.addUniform("Material.Ka");
    m_shaderProgram.addUniform("Material.Kd");
    m_shaderProgram.addUniform("Material.Ks");
    m_shaderProgram.addUniform("Material.Shiness");

    m_shaderProgram.addUniform("CamPos");
}

void ES::Plugin::OpenGL::System::SetupTextShaderUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"textDefault"});

    m_shaderProgram.addUniform("Projection");
    m_shaderProgram.addUniform("Text");
    m_shaderProgram.addUniform("TextColor");
}

void ES::Plugin::OpenGL::System::SetupSpriteShaderUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"2DDefault"});

    m_shaderProgram.addUniform("color");
    m_shaderProgram.addUniform("model");
    m_shaderProgram.addUniform("projection");
}
