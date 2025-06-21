/*
 * This file provides system implementation of the Rmlui shaders
 * It is based on the official implementation
 * See: https://github.com/mikke89/RmlUi/blob/0c96c0eed25c5d86ae300887eae4a0333b34b22b/Backends/RmlUi_Renderer_GL3.cpp
 */

#include "OpenGL.pch.hpp"

#include "ShaderManager.hpp"
#include "ShaderSystems.hpp"

void ES::Plugin::UI::System::LoadShaderVertColor(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440

        uniform vec2 _translate;
        uniform mat4 _transform;

        in vec2 inPosition;
        in vec4 inColor0;
        in vec2 inTexCoord0;

        out vec2 fragTexCoord;
        out vec4 fragColor;

        void main() {
            fragTexCoord = inTexCoord0;
            fragColor = inColor0;

            vec2 translatedPos = inPosition + _translate;
            vec4 outPos = _transform * vec4(translatedPos, 0.0, 1.0);

            gl_Position = outPos;
        }
    )";

    const char *fragmentShader = R"(
        #version 440

        in vec2 fragTexCoord;
        in vec4 fragColor;

        out vec4 finalColor;

        void main() {
            finalColor = fragColor;
        }
    )";

    auto &shaderManager = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
    ES::Plugin::OpenGL::Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"RmlVertexColor"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::UI::System::LoadShaderVertTexture(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440

        uniform vec2 _translate;
        uniform mat4 _transform;

        in vec2 inPosition;
        in vec4 inColor0;
        in vec2 inTexCoord0;

        out vec2 fragTexCoord;
        out vec4 fragColor;

        void main() {
            fragTexCoord = inTexCoord0;
            fragColor = inColor0;

            vec2 translatedPos = inPosition + _translate;
            vec4 outPos = _transform * vec4(translatedPos, 0.0, 1.0);

            gl_Position = outPos;
        }
    )";

    const char *fragmentShader = R"(
        #version 440

        uniform sampler2D _tex;
        in vec2 fragTexCoord;
        in vec4 fragColor;

        out vec4 finalColor;

        void main() {
            vec4 texColor = texture(_tex, fragTexCoord);
            finalColor = fragColor * texColor;
        }
    )";

    auto &shaderManager = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
    ES::Plugin::OpenGL::Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"RmlVertexTexture"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::UI::System::LoadShaderVertGradient(ES::Engine::Core &core)
{
    const int maxNumStop = 16;

    std::string shaderHeader = fmt::format("#version 440\n#define MAX_NUM_STOPS {}\n", std::to_string(maxNumStop));

    const char *vertexShader = R"(
        #version 440

        uniform vec2 _translate;
        uniform mat4 _transform;

        in vec2 inPosition;
        in vec4 inColor0;
        in vec2 inTexCoord0;

        out vec2 fragTexCoord;
        out vec4 fragColor;

        void main() {
            fragTexCoord = inTexCoord0;
            fragColor = inColor0;

            vec2 translatedPos = inPosition + _translate;
            vec4 outPos = _transform * vec4(translatedPos, 0.0, 1.0);

            gl_Position = outPos;
        }
    )";

    std::string fragmentShader = shaderHeader +
                                 R"(
        #define LINEAR 0
        #define RADIAL 1
        #define CONIC 2
        #define REPEATING_LINEAR 3
        #define REPEATING_RADIAL 4
        #define REPEATING_CONIC 5
        #define PI 3.14159265

        uniform int _func; // one of the above definitions
        uniform vec2 _p;   // linear: starting point,         radial: center,                        conic: center
        uniform vec2 _v;   // linear: vector to ending point, radial: 2d curvature (inverse radius), conic: angled unit vector
        uniform vec4 _stop_colors[MAX_NUM_STOPS];
        uniform float _stop_positions[MAX_NUM_STOPS]; // normalized, 0 -> starting point, 1 -> ending point
        uniform int _num_stops;

        in vec2 fragTexCoord;
        in vec4 fragColor;
        out vec4 finalColor;

        vec4 mix_stop_colors(float t) {
            vec4 color = _stop_colors[0];

            for (int i = 1; i < _num_stops; i++)
                color = mix(color, _stop_colors[i], smoothstep(_stop_positions[i-1], _stop_positions[i], t));

            return color;
        }

        void main() {
            float t = 0.0;

            if (_func == LINEAR || _func == REPEATING_LINEAR)
            {
                float dist_square = dot(_v, _v);
                vec2 V = fragTexCoord - _p;
                t = dot(_v, V) / dist_square;
            }
            else if (_func == RADIAL || _func == REPEATING_RADIAL)
            {
                vec2 V = fragTexCoord - _p;
                t = length(_v * V);
            }
            else if (_func == CONIC || _func == REPEATING_CONIC)
            {
                mat2 R = mat2(_v.x, -_v.y, _v.y, _v.x);
                vec2 V = R * (fragTexCoord - _p);
                t = 0.5 + atan(-V.x, V.y) / (2.0 * PI);
            }

            if (_func == REPEATING_LINEAR || _func == REPEATING_RADIAL || _func == REPEATING_CONIC)
            {
                float t0 = _stop_positions[0];
                float t1 = _stop_positions[_num_stops - 1];
                t = t0 + mod(t - t0, t1 - t0);
            }

            finalColor = fragColor * mix_stop_colors(t);
        }
    )";

    auto &shaderManager = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
    ES::Plugin::OpenGL::Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"RmlVertexGradient"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader.c_str());
}

void ES::Plugin::UI::System::LoadShaderCreation(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440

        uniform vec2 _translate;
        uniform mat4 _transform;

        in vec2 inPosition;
        in vec4 inColor0;
        in vec2 inTexCoord0;

        out vec2 fragTexCoord;
        out vec4 fragColor;

        void main() {
            fragTexCoord = inTexCoord0;
            fragColor = inColor0;

            vec2 translatedPos = inPosition + _translate;
            vec4 outPos = _transform * vec4(translatedPos, 0.0, 1.0);

            gl_Position = outPos;
        }
    )";

    const char *fragmentShader = R"(
        #version 440
       
        uniform float _value;
        uniform vec2 _dimensions;

        in vec2 fragTexCoord;
        in vec4 fragColor;
        out vec4 finalColor;

        void main() {
            float t = _value;
            vec3 c;
            float l;
            for (int i = 0; i < 3; i++) {
                vec2 p = fragTexCoord;
                vec2 uv = p;
                p -= .5;
                p.x *= _dimensions.x / _dimensions.y;
                float z = t + float(i) * .07;
                l = length(p);
                uv += p / l * (sin(z) + 1.) * abs(sin(l * 9. - z - z));
                c[i] = .01 / length(mod(uv, 1.) - .5);
            }
            finalColor = vec4(c / l, fragColor.a);
        }
    )";

    auto &shaderManager = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
    ES::Plugin::OpenGL::Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"RmlCreation"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::UI::System::LoadShaderPassthrough(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440

        in vec2 inPosition;
        in vec2 inTexCoord0;

        out vec2 fragTexCoord;

        void main() {
            fragTexCoord = inTexCoord0;
            gl_Position = vec4(inPosition, 0.0, 1.0);
        }
    )";

    const char *fragmentShader = R"(
        #version 440

        uniform sampler2D _tex;
        in vec2 fragTexCoord;
        out vec4 finalColor;

        void main() {
            finalColor = texture(_tex, fragTexCoord);
        }
    )";

    auto &shaderManager = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
    ES::Plugin::OpenGL::Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"RmlPassthrough"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::UI::System::LoadShaderColorMatrix(ES::Engine::Core &core)
{
    const char *vertexShader = R"(  
        #version 440
      
        in vec2 inPosition;
        in vec2 inTexCoord0;

        out vec2 fragTexCoord;

        void main() {
            fragTexCoord = inTexCoord0;
            gl_Position = vec4(inPosition, 0.0, 1.0);
        }
    )";

    const char *fragmentShader = R"( 
        #version 440       
        
        uniform sampler2D _tex;
        uniform mat4 _color_matrix;

        in vec2 fragTexCoord;
        out vec4 finalColor;

        void main() {
            // The general case uses a 4x5 color matrix for full rgba transformation, plus a constant term with the last column.
            // However, we only consider the case of rgb transformations. Thus, we could in principle use a 3x4 matrix, but we
            // keep the alpha row for simplicity.
            // In the general case we should do the matrix transformation in non-premultiplied space. However, without alpha
            // transformations, we can do it directly in premultiplied space to avoid the extra division and multiplication
            // steps. In this space, the constant term needs to be multiplied by the alpha value, instead of unity.
            vec4 texColor = texture(_tex, fragTexCoord);
            vec3 transformedColor = vec3(_color_matrix * texColor);
            finalColor = vec4(transformedColor, texColor.a);
        }
    )";

    auto &shaderManager = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
    ES::Plugin::OpenGL::Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"RmlColorMatrix"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::UI::System::LoadShaderBlendMatrix(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440
        
        in vec2 inPosition;
        in vec2 inTexCoord0;

        out vec2 fragTexCoord;

        void main() {
            fragTexCoord = inTexCoord0;
            gl_Position = vec4(inPosition, 0.0, 1.0);
        }
    )";

    const char *fragmentShader = R"(
        #version 440
        
        uniform sampler2D _tex;
        uniform sampler2D _texMask;

        in vec2 fragTexCoord;
        out vec4 finalColor;

        void main() {
            vec4 texColor = texture(_tex, fragTexCoord);
            float maskAlpha = texture(_texMask, fragTexCoord).a;
            finalColor = texColor * maskAlpha;
        }
    )";

    auto &shaderManager = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
    ES::Plugin::OpenGL::Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"RmlBlendMatrix"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::UI::System::LoadShaderBlur(ES::Engine::Core &core)
{
    const int blurSize = 7;
    const int blurNumWeights = (blurSize + 1) / 2;

    std::string shaderHeader = fmt::format("#version 440\n#define BLUR_SIZE {}\n#define BLUR_NUM_WEIGHTS {}\n",
                                           std::to_string(blurSize), std::to_string(blurNumWeights));

    const std::string vertexShader = shaderHeader +
                                     R"(
        uniform vec2 _texelOffset;

        in vec3 inPosition;
        in vec2 inTexCoord0;

        out vec2 fragTexCoord[BLUR_SIZE];

        void main() {
            for(int i = 0; i < BLUR_SIZE; i++)
                fragTexCoord[i] = inTexCoord0 - float(i - BLUR_NUM_WEIGHTS + 1) * _texelOffset;
            gl_Position = vec4(inPosition, 1.0);
        }
    )";

    const std::string fragmentShader = shaderHeader +
                                       R"(
        uniform sampler2D _tex;
        uniform float _weights[BLUR_NUM_WEIGHTS];
        uniform vec2 _texCoordMin;
        uniform vec2 _texCoordMax;

        in vec2 fragTexCoord[BLUR_SIZE];
        out vec4 finalColor;

        void main() {
            vec4 color = vec4(0.0);
            for(int i = 0; i < BLUR_SIZE; i++)
            {
                vec2 in_region = step(_texCoordMin, fragTexCoord[i]) * step(fragTexCoord[i], _texCoordMax);
                color += texture(_tex, fragTexCoord[i]) * in_region.x * in_region.y * _weights[abs(i - BLUR_NUM_WEIGHTS + 1)];
            }
            finalColor = color;
        }
    )";

    auto &shaderManager = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
    auto &sp = shaderManager.Add(entt::hashed_string{"RmlBlur"});
    sp.Create();
    sp.InitFromStrings(vertexShader.c_str(), fragmentShader.c_str());
}

void ES::Plugin::UI::System::LoadShaderDropShadow(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440
    
        in vec2 inPosition;
        in vec2 inTexCoord0;

        out vec2 fragTexCoord;

        void main() {
            fragTexCoord = inTexCoord0;
            gl_Position = vec4(inPosition, 0.0, 1.0);
        }
    )";

    const char *fragmentShader = R"(
        #version 440

        uniform sampler2D _tex;
        uniform vec2 _texCoordMin;
        uniform vec2 _texCoordMax;
        uniform vec4 _color;

        in vec2 fragTexCoord;
        out vec4 finalColor;

        void main() {
            vec2 in_region = step(_texCoordMin, fragTexCoord) * step(fragTexCoord, _texCoordMax);
            finalColor = texture(_tex, fragTexCoord).a * in_region.x * in_region.y * _color;
        }
    )";

    auto &shaderManager = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
    ES::Plugin::OpenGL::Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"RmlDropShadow"});
    sp.Create();
    sp.InitFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::UI::System::SetupShaderVertColorUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram =
        core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"RmlVertexColor"});

    m_shaderProgram.AddUniform("_translate");
    m_shaderProgram.AddUniform("_transform");
}

void ES::Plugin::UI::System::SetupShaderVertTextureUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram =
        core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"RmlVertexTexture"});

    m_shaderProgram.AddUniform("_translate");
    m_shaderProgram.AddUniform("_transform");
    m_shaderProgram.AddUniform("_tex");
}

void ES::Plugin::UI::System::SetupShaderVertGradientUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram =
        core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"RmlVertexGradient"});

    m_shaderProgram.AddUniform("_translate");
    m_shaderProgram.AddUniform("_transform");
    m_shaderProgram.AddUniform("_func");
    m_shaderProgram.AddUniform("_p");
    m_shaderProgram.AddUniform("_v");
    m_shaderProgram.AddUniform("_stop_colors");
    m_shaderProgram.AddUniform("_stop_positions");
    m_shaderProgram.AddUniform("_num_stops");
}

void ES::Plugin::UI::System::SetupShaderCreationUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram =
        core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"RmlCreation"});

    m_shaderProgram.AddUniform("_translate");
    m_shaderProgram.AddUniform("_transform");
    m_shaderProgram.AddUniform("_value");
    m_shaderProgram.AddUniform("_dimensions");
}

void ES::Plugin::UI::System::SetupShaderPassthroughUniform(ES::Engine::Core &core)
{
    auto &m_shaderProgram =
        core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"RmlPassthrough"});

    m_shaderProgram.AddUniform("_tex");
}

void ES::Plugin::UI::System::SetupShaderColorMatrix(ES::Engine::Core &core)
{
    auto &m_shaderProgram =
        core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"RmlColorMatrix"});

    m_shaderProgram.AddUniform("_tex");
    m_shaderProgram.AddUniform("_color_matrix");
}

void ES::Plugin::UI::System::SetupShaderBlendMatrix(ES::Engine::Core &core)
{
    auto &m_shaderProgram =
        core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"RmlBlendMatrix"});

    m_shaderProgram.AddUniform("_tex");
    m_shaderProgram.AddUniform("_texMask");
}

void ES::Plugin::UI::System::SetupShaderBlur(ES::Engine::Core &core)
{
    auto &m_shaderProgram =
        core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"RmlBlur"});

    m_shaderProgram.AddUniform("_texelOffset");
    m_shaderProgram.AddUniform("_weights");
    m_shaderProgram.AddUniform("_texCoordMin");
    m_shaderProgram.AddUniform("_texCoordMax");
    m_shaderProgram.AddUniform("_tex");
}

void ES::Plugin::UI::System::SetupShaderDropShadow(ES::Engine::Core &core)
{
    auto &m_shaderProgram =
        core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"RmlDropShadow"});

    m_shaderProgram.AddUniform("_tex");
    m_shaderProgram.AddUniform("_texCoordMin");
    m_shaderProgram.AddUniform("_texCoordMax");
    m_shaderProgram.AddUniform("_color");
}
