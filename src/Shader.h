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

class Shader
{
public:
    Shader() = default;
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    Shader(unsigned int vertexId, unsigned int fragmentId);

    void init(const std::string& vertexPath, const std::string& fragmentPath);

    unsigned int getVertexShaderId() const;
    unsigned int getFragmentShaderId() const;

    void use() const;
    void setInt(const std::string& name, int value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;

private:
    unsigned int programId;
    unsigned int vertexId;
    unsigned int fragmentId;

    static void checkCompilingErrors(unsigned int& shader);
    static void checkLinkingErrors(unsigned int& program);

    void compileShaders(const char* vertexCode, const char* fragmentCode);
    void createProgram();
};

#endif // SHADER
