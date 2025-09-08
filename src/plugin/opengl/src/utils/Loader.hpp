// Original author: r3dux

#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "Exception.hpp"
#include "Logger.hpp"
#include "exception/OpenGLError.hpp"

#include <GL/glew.h>

namespace ES::Plugin::OpenGL::Utils {
class ShaderProgram {
  private:
    // static DEBUG flag - if set to false then, errors aside, we'll run completely silent
    static constexpr bool DEBUG_SHADER = true;

    // We'll use an enum to differentiate between shaders and shader programs when querying the info log
    enum class ObjectType {
        SHADER,
        PROGRAM
    };

    // Shader program and individual shader Ids
    GLuint programId = 0;
    GLuint vertexShaderId = 0;
    GLuint fragmentShaderId = 0;
    bool initialised = false;

    // Map of attributes and their binding locations
    std::map<std::string, int, std::less<>> attributeMap;

    // Map of uniforms and their binding locations
    std::map<std::string, int, std::less<>> uniformMap;

    // Map to store SSBO bindings
    std::map<std::string, std::pair<GLuint, GLsizeiptr>, std::less<>> ssboMap;

    // Has this shader program been initialised?

    // ---------- PRIVATE METHODS ----------

    /**
     * @brief Compiles a shader of a given type from its source code.
     *
     * @param shaderSource The source code of the shader.
     * @param shaderType The type of the shader (e.g., GL_VERTEX_SHADER, GL_FRAGMENT_SHADER).
     * @return GLuint The ID of the compiled shader.
     * @throws OpenGLError If the shader compilation fails.
     */
    GLuint CompileShader(const std::string &shaderSource, GLenum shaderType) const
    {
        std::string shaderTypeString;
        switch (shaderType)
        {
        case GL_VERTEX_SHADER: shaderTypeString = "GL_VERTEX_SHADER"; break;
        case GL_FRAGMENT_SHADER: shaderTypeString = "GL_FRAGMENT_SHADER"; break;
        case GL_GEOMETRY_SHADER: throw OpenGLError("Geometry shaders are unsupported at this time."); break;
        default: throw OpenGLError("Bad shader type enum in compileShader."); break;
        }

        // Generate a shader id
        // Note: Shader id will be non-zero if successfully created.
        GLuint shaderId = glCreateShader(shaderType);
        if (shaderId == 0)
        {
            // Display the shader log via a OpenGLError
            throw OpenGLError("Could not create shader of type " + shaderTypeString + ": " +
                              GetInfoLog(ObjectType::SHADER, shaderId));
        }

        // Get the source string as a pointer to an array of characters
        const char *shaderSourceChars = shaderSource.c_str();

        // Attach the GLSL source code to the shader
        // Params: GLuint shader, GLsizei count, const GLchar **string, const GLint *length
        // Note: The pointer to an array of source chars will be null terminated, so we don't need to specify the length
        // and can instead use NULL.
        glShaderSource(shaderId, 1, &shaderSourceChars, nullptr);

        // Compile the shader
        glCompileShader(shaderId);

        // Check the compilation status and throw a OpenGLError if shader compilation failed
        GLint shaderStatus;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shaderStatus);
        if (shaderStatus == GL_FALSE)
        {
            ES::Utils::Log::Error(
                fmt::format("{} compilation failed: {}", shaderTypeString, GetInfoLog(ObjectType::SHADER, shaderId)));
        }
        else
        {
            if (DEBUG_SHADER)
            {
                ES::Utils::Log::Info(fmt::format("{} shader compilation successful.", shaderTypeString));
            }
        }

        // If everything went well, return the shader id
        return shaderId;
    }

    /**
     * @brief Compiles, attaches, links, and validates shaders to initialise the shader program.
     *
     * @note Rather than returning a boolean success value, this method will throw a OpenGLError
     * considering the a failure to be an unrecoverable error.
     *
     * @param vertexShaderSource The source code of the vertex shader.
     * @param fragmentShaderSource The source code of the fragment shader.
     * @throws OpenGLError If any step of the initialisation fails.
     */
    void Initialise(const std::string &vertexShaderSource, const std::string &fragmentShaderSource)
    {
        // Compile the shaders and return their id values
        vertexShaderId = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
        fragmentShaderId = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

        // Attach the compiled shaders to the shader program
        glAttachShader(programId, vertexShaderId);
        glAttachShader(programId, fragmentShaderId);

        // Link the shader program - details are placed in the program info log
        glLinkProgram(programId);

        // Once the shader program has the shaders attached and linked, the shaders are no longer required.
        // If the linking failed, then we're going to abort anyway so we still detach the shaders.
        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);

        // Check the program link status and throw a OpenGLError if program linkage failed.
        GLint programLinkSuccess = GL_FALSE;
        glGetProgramiv(programId, GL_LINK_STATUS, &programLinkSuccess);
        if (programLinkSuccess == GL_TRUE)
        {
            if (DEBUG_SHADER)
            {
                ES::Utils::Log::Info("Shader program link successful.");
            }
        }
        else
        {
            ES::Utils::Log::Error(
                fmt::format("Shader program link failed: {}", GetInfoLog(ObjectType::PROGRAM, programId)));
        }

        // Validate the shader program
        glValidateProgram(programId);

        // Check the validation status and throw a OpenGLError if program validation failed
        GLint programValidatationStatus;
        glGetProgramiv(programId, GL_VALIDATE_STATUS, &programValidatationStatus);
        if (programValidatationStatus == GL_TRUE)
        {
            if (DEBUG_SHADER)
            {
                ES::Utils::Log::Info("Shader program validation successful.");
            }
        }
        else
        {
            ES::Utils::Log::Error(
                fmt::format("Shader program validation failed: {}", GetInfoLog(ObjectType::PROGRAM, programId)));
        }

        // Finally, the shader program is initialised
        initialised = true;
    }

    /**
     * @brief Loads the shader source code from a file.
     *
     * @param filename The path to the shader file.
     * @return std::string The contents of the shader file as a string.
     * @throws std::ios_base::failure If the file cannot be opened.
     */
    std::string LoadShaderFromFile(const std::string &filename) const
    {
        // Create an input filestream and attempt to open the specified file
        std::ifstream file(filename.c_str());

        // If we couldn't open the file we'll bail out
        if (!file.good())
        {
            ES::Utils::Log::Error(
                fmt::format("Failed to open file({}): {}", filename, ES::Utils::Log::GetErrnoMessage()));
        }

        // Otherwise, create a string stream...
        std::stringstream stream;

        // ...and dump the contents of the file into it.
        stream << file.rdbuf();

        // Now that we've read the file we can close it
        file.close();

        // Finally, convert the stringstream into a string and return it
        return stream.str();
    }

    /**
     * @brief Retrieves the info log for a shader or shader program.
     *
     * @param type The type of the object (SHADER or PROGRAM).
     * @param id The ID of the shader or program.
     * @return std::string The info log as a string.
     */
    std::string GetInfoLog(ObjectType type, int id) const
    {
        GLint infoLogLength;
        if (type == ObjectType::SHADER)
        {
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
        }
        else // type must be ObjectType::PROGRAM
        {
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
        }

        std::vector<GLchar> infoLog(infoLogLength + 1);

        if (type == ObjectType::SHADER)
        {
            glGetShaderInfoLog(id, infoLogLength, nullptr, infoLog.data());
        }
        else // type must be ObjectType::PROGRAM
        {
            glGetProgramInfoLog(id, infoLogLength, nullptr, infoLog.data());
        }

        // Convert the info log to a string
        std::string infoLogString(infoLog.data());

        // Finally, return the string version of the info log
        return infoLogString;
    }

  public:
    ShaderProgram() = default;
    ~ShaderProgram() = default;

    /**
     * @brief Creates a new shader program.
     *
     * @throws OpenGLError If the program creation fails.
     */
    void Create()
    {
        programId = glCreateProgram();
        if (programId == 0)
        {
            ES::Utils::Log::Error("Failed to generate shader program Id.");
        }
        glUseProgram(programId);
    }

    /**
     * @brief Deletes the shader program.
     */
    void Destroy() const
    {
        if (programId == 0)
        {
            ES::Utils::Log::Warn("No shader program Id to delete.");
        }
        else
        {
            glDeleteProgram(programId);
        }
    }

    /**
     * @brief Initialises the shader program using shader source files.
     *
     * @param vertexShaderFilename The path to the vertex shader file.
     * @param fragmentShaderFilename The path to the fragment shader file.
     */
    void InitFromFiles(const std::string &vertexShaderFilename, const std::string &fragmentShaderFilename)
    {
        // Get the shader file contents as strings
        std::string vertexShaderSource = LoadShaderFromFile(vertexShaderFilename);
        std::string fragmentShaderSource = LoadShaderFromFile(fragmentShaderFilename);

        Initialise(vertexShaderSource, fragmentShaderSource);
    }

    /**
     * @brief Initialises the shader program using shader source strings.
     *
     * @param vertexShaderSource The source code of the vertex shader.
     * @param fragmentShaderSource The source code of the fragment shader.
     */
    void InitFromStrings(const std::string &vertexShaderSource, const std::string &fragmentShaderSource)
    {
        Initialise(vertexShaderSource, fragmentShaderSource);
    }

    /**
     * @brief Activates the shader program for use.
     *
     * @throws OpenGLError If the shader program is not initialised.
     */
    inline void Use() const
    {
        // Santity check that we're initialised and ready to go...
        if (initialised)
        {
            glUseProgram(programId);
        }
        else
        {
            std::string msg = fmt::format("Shader program {} not initialised - aborting.", programId);
            throw OpenGLError(msg);
        }
    }

    /**
     * @brief Deactivates the shader program.
     */
    inline void Disable() const { glUseProgram(0); }

    /**
     * @brief Retrieves the location of a named attribute in the shader program.
     *
     * @param attributeName The name of the attribute.
     * @return GLuint The location of the attribute, or -1 if not found.
     */
    GLuint GetAttribute(const std::string &attributeName)
    {
        // You could do this method with the single line:
        //
        //		return attributeMap[attribute];
        //
        // BUT, if you did, and you asked it for a named attribute which didn't exist
        // like: attributeMap["FakeAttrib"] then the method would return an invalid
        // value which will likely cause the program to segfault. So we're making sure
        // the attribute asked for exists, and if it doesn't then we alert the user & bail.

        // Not found? Bail.
        if (!attributeMap.contains(attributeName))
        {
            ES::Utils::Log::Error(fmt::format("Could not find attribute in shader program: {}", attributeName));
        }

        // Otherwise return the attribute location from the attribute map
        return attributeMap[attributeName];
    }

    /**
     * @brief Retrieves the location of a named uniform in the shader program.
     *
     * @param uniformName The name of the uniform.
     * @return GLuint The location of the uniform, or -1 if not found.
     */
    GLuint GetUniform(const std::string &uniformName)
    {
        // Note: You could do this method with the single line:
        //
        // 		return uniformLocList[uniform];
        //
        // But we're not doing that. Explanation in the GetAttribute() method above.

        // Found it? Great - pass it back! Didn't find it? Alert user and halt.
        if (!uniformMap.contains(uniformName))
        {
            ES::Utils::Log::Error(fmt::format("Could not find uniform in shader program: {}", uniformName));
        }

        // Otherwise return the attribute location from the uniform map
        return uniformMap[uniformName];
    }

    /**
     * @brief Returns the bound location of a named Shader Storage Buffer Object (SSBO).
     *
     * @param ssboName The name of the SSBO to retrieve.
     * @return GLuint The binding point of the SSBO.
     */
    GLuint GetSSBO(const std::string &ssboName)
    {
        // Not found? Bail.
        if (!ssboMap.contains(ssboName))
        {
            ES::Utils::Log::Error(fmt::format("Could not find ssbo in shader program: {}", ssboName));
        }

        // Otherwise return the ssbo location from the ssbo map
        return ssboMap[ssboName].first;
    }

    /**
     * @brief Adds an attribute to the shader program and returns its bound location.
     *
     * @param attributeName The name of the attribute to add.
     * @return int The location of the attribute in the shader program.
     */
    int AddAttribute(const std::string &attributeName)
    {
        // Add the attribute location value for the attributeName key
        attributeMap[attributeName] = glGetAttribLocation(programId, attributeName.c_str());

        // Check to ensure that the shader contains an attribute with this name
        if (attributeMap[attributeName] == -1)
        {
            ES::Utils::Log::Error(fmt::format("Could not add attribute: {} - location returned -1.", attributeName));
        }
        else // Valid attribute location? Inform user if we're in debug mode.
        {
            if (DEBUG_SHADER)
            {
                ES::Utils::Log::Info(
                    fmt::format("Attribute {} bound to location: {}", attributeName, attributeMap[attributeName]));
            }
        }

        // Return the attribute location
        return attributeMap[attributeName];
    }

    /**
     * @brief Adds a uniform to the shader program and returns its bound location.
     *
     * @param uniformName The name of the uniform to add.
     * @return int The location of the uniform in the shader program.
     */
    int AddUniform(const std::string &uniformName)
    {
        // Add the uniform location value for the uniformName key
        uniformMap[uniformName] = glGetUniformLocation(programId, uniformName.c_str());

        // Check to ensure that the shader contains a uniform with this name
        if (uniformMap[uniformName] == -1)
        {
            ES::Utils::Log::Error(fmt::format("Could not add uniform: {} - location returned -1.", uniformName));
        }
        else // Valid uniform location? Inform user if we're in debug mode.
        {
            if (DEBUG_SHADER)
            {
                ES::Utils::Log::Info(
                    fmt::format("Uniform {} bound to location: {}", uniformName, uniformMap[uniformName]));
            }
        }

        // Return the uniform location
        return uniformMap[uniformName];
    }

    /**
     * @brief Adds a Shader Storage Buffer Object (SSBO) to the shader program.
     *
     * @param ssboName The name of the SSBO.
     * @param bindingPoint The binding point for the SSBO.
     * @param size The size of the SSBO in bytes.
     * @param data Optional pointer to the initial data for the SSBO. Defaults to nullptr.
     */
    void AddSSBO(const std::string &ssboName, GLuint bindingPoint, GLsizeiptr size, const GLvoid *data = nullptr)
    {
        GLuint ssbo;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssbo);

        ssboMap[ssboName] = {bindingPoint, size};

        if (DEBUG_SHADER)
        {
            ES::Utils::Log::Info(fmt::format("SSBO {} added and bound to binding point {}", ssboName, bindingPoint));
        }
    }

    /**
     * @brief Updates the data of an existing Shader Storage Buffer Object (SSBO).
     *
     * @note This method will resize the SSBO if the new size is larger than the current size.
     *
     * @param ssboName The name of the SSBO to update.
     * @param size The new size of the SSBO in bytes.
     * @param data Pointer to the new data for the SSBO.
     */
    void UpdateSSBO(const std::string &ssboName, GLsizeiptr size, const GLvoid *data)
    {
        auto it = ssboMap.find(ssboName);
        if (it == ssboMap.end())
        {
            ES::Utils::Log::Error(fmt::format("SSBO {} not found.", ssboName));
            return;
        }

        if (size > it->second.second)
        {
            AddSSBO(ssboName, it->second.first, size, data);
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, it->second.first);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);
    }

    /**
     * @brief Deletes a Shader Storage Buffer Object (SSBO) from the shader program.
     *
     * @param ssboName The name of the SSBO to delete.
     */
    void DeleteSSBO(const std::string &ssboName)
    {
        auto it = ssboMap.find(ssboName);
        if (it == ssboMap.end())
        {
            ES::Utils::Log::Error(fmt::format("SSBO {} not found.", ssboName));
            return;
        }

        glDeleteBuffers(1, &it->second.first);
        ssboMap.erase(it);

        if (DEBUG_SHADER)
        {
            ES::Utils::Log::Info(fmt::format("SSBO {} deleted.", ssboName));
        }
    }

}; // End of class
} // namespace ES::Plugin::OpenGL::Utils
#endif // SHADER_PROGRAM_HPP
