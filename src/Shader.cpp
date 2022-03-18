#include "Shader.h"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    init(vertexPath, fragmentPath);
}

Shader::Shader(unsigned int vertexId, unsigned int fragmentId)
    : vertexId(vertexId)
    , fragmentId(fragmentId)
{
    createProgram();
}

void Shader::init(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    compileShaders(vertexCode.c_str(), fragmentCode.c_str());
    createProgram();

    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);
}

unsigned int Shader::getVertexShaderId() const
{
    return vertexId;
}

unsigned int Shader::getFragmentShaderId() const
{
    return fragmentId;
}

void Shader::use() const
{
    glUseProgram(programId);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    unsigned int transformLoc = glGetUniformLocation(programId, name.c_str());
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
}

void Shader::checkCompilingErrors(unsigned int& shader)
{
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR: Shader compilation failed\n" << infoLog << std::endl;
    }
}

void Shader::checkLinkingErrors(unsigned int& program)
{
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR: Shader linking failed\n" << infoLog << std::endl;
    }
}

void Shader::compileShaders(const char* vertexCode, const char* fragmentCode)
{
    vertexId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexId, 1, &vertexCode, nullptr);
    glCompileShader(vertexId);
    checkCompilingErrors(vertexId);

    fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentId, 1, &fragmentCode, nullptr);
    glCompileShader(fragmentId);
    checkCompilingErrors(fragmentId);
}

void Shader::createProgram()
{
    programId = glCreateProgram();
    glAttachShader(programId, vertexId);
    glAttachShader(programId, fragmentId);
    glLinkProgram(programId);
    checkLinkingErrors(programId);
}
