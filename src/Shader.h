#ifndef SHADER
#define SHADER

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
    unsigned int ID;

    static void checkCompilingErrors(unsigned int &shader)
    {
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "ERROR: Shader compilation failed\n" << infoLog << std::endl;
        }
    }
    static void checkLinkingErrors(unsigned int &program)
    {
        int success;
        char infoLog[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cout << "ERROR: Shader linking failed\n" << infoLog << std::endl;
        }
    }
public:
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "Error: Shader file was not successfully read" << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
        glCompileShader(vertexShader);
        checkCompilingErrors(vertexShader);

        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
        glCompileShader(fragmentShader);
        checkCompilingErrors(fragmentShader);

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);
        checkLinkingErrors(ID);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void use() const
    {
        glUseProgram(ID);
    }
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setMat4(const std::string &name, glm::mat4 value) const
    {
        unsigned int transformLoc = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
    }
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
};

#endif // SHADER
