
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
#include "OpenGLError.hpp"

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

    // Has this shader program been initialised?

    // ---------- PRIVATE METHODS ----------

    // Private method to compile a shader of a given type
    GLuint compileShader(const std::string &shaderSource, GLenum shaderType) const
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
                              getInfoLog(ObjectType::SHADER, shaderId));
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
                fmt::format("{} compilation failed: {}", shaderTypeString, getInfoLog(ObjectType::SHADER, shaderId)));
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

    // Private method to compile/attach/link/verify the shaders.
    // Note: Rather than returning a boolean as a success/fail status we'll just consider
    // a failure here to be an unrecoverable error and throw a OpenGLError.
    void initialise(const std::string &vertexShaderSource, const std::string &fragmentShaderSource)
    {
        // Compile the shaders and return their id values
        vertexShaderId = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
        fragmentShaderId = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

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
                fmt::format("Shader program link failed: {}", getInfoLog(ObjectType::PROGRAM, programId)));
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
                fmt::format("Shader program validation failed: {}", getInfoLog(ObjectType::PROGRAM, programId)));
        }

        // Finally, the shader program is initialised
        initialised = true;
    }

    // Private method to load the shader source code from a file
    std::string loadShaderFromFile(const std::string &filename) const
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

    // Private method to return the current shader program info log as a string
    std::string getInfoLog(ObjectType type, int id) const
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

    void Create()
    {
        programId = glCreateProgram();
        if (programId == 0)
        {
            ES::Utils::Log::Error("Failed to generate shader program Id.");
        }
        glUseProgram(programId);
    }

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

    // Method to initialise a shader program from shaders provided as files
    void initFromFiles(const std::string &vertexShaderFilename, const std::string &fragmentShaderFilename)
    {
        // Get the shader file contents as strings
        std::string vertexShaderSource = loadShaderFromFile(vertexShaderFilename);
        std::string fragmentShaderSource = loadShaderFromFile(fragmentShaderFilename);

        initialise(vertexShaderSource, fragmentShaderSource);
    }

    // Method to initialise a shader program from shaders provided as strings
    void initFromStrings(const std::string &vertexShaderSource, const std::string &fragmentShaderSource)
    {
        initialise(vertexShaderSource, fragmentShaderSource);
    }

    // Method to enable the shader program - we'll suggest this for inlining
    inline void use() const
    {
        // Santity check that we're initialised and ready to go...
        if (initialised)
        {
            glUseProgram(programId);
        }
        else
        {
            std::string msg = "Shader program " + programId;
            msg += " not initialised - aborting.";
            throw OpenGLError(msg);
        }
    }

    // Method to disable the shader - we'll also suggest this for inlining
    inline void disable() const { glUseProgram(0); }

    // Method to return the bound location of a named attribute, or -1 if the attribute was not found
    GLuint attribute(const std::string &attributeName)
    {
        // You could do this method with the single line:
        //
        //		return attributeMap[attribute];
        //
        // BUT, if you did, and you asked it for a named attribute which didn't exist
        // like: attributeMap["FakeAttrib"] then the method would return an invalid
        // value which will likely cause the program to segfault. So we're making sure
        // the attribute asked for exists, and if it doesn't then we alert the user & bail.

        // Create an iterator to look through our attribute map (only create iterator on first run -
        // reuse it for all further calls).
        static std::map<std::string, int, std::less<>>::const_iterator attributeIter;

        // Try to find the named attribute
        attributeIter = attributeMap.find(attributeName);

        // Not found? Bail.
        if (attributeIter == attributeMap.end())
        {
            ES::Utils::Log::Error(fmt::format("Could not find attribute in shader program: {}", attributeName));
        }

        // Otherwise return the attribute location from the attribute map
        return attributeMap[attributeName];
    }

    // Method to returns the bound location of a named uniform
    GLuint uniform(const std::string &uniformName)
    {
        // Note: You could do this method with the single line:
        //
        // 		return uniformLocList[uniform];
        //
        // But we're not doing that. Explanation in the attribute() method above.

        // Create an iterator to look through our uniform map (only create iterator on first run -
        // reuse it for all further calls).
        static std::map<std::string, int, std::less<>>::const_iterator uniformIter;

        // Try to find the named uniform
        uniformIter = uniformMap.find(uniformName);

        // Found it? Great - pass it back! Didn't find it? Alert user and halt.
        if (uniformIter == uniformMap.end())
        {
            ES::Utils::Log::Error(fmt::format("Could not find uniform in shader program: {}", uniformName));
        }

        // Otherwise return the attribute location from the uniform map
        return uniformMap[uniformName];
    }

    // Method to add an attribute to the shader and return the bound location
    int addAttribute(const std::string &attributeName)
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

    // Method to add a uniform to the shader and return the bound location
    int addUniform(const std::string &uniformName)
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

}; // End of class
} // namespace ES::Plugin::OpenGL::Utils
#endif // SHADER_PROGRAM_HPP
