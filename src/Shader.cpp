#include "Shader.hpp"

std::string getType(GLint type);

Shader::Shader()
{
    _id = glCreateProgram();
}

Shader::~Shader()
{
    glDeleteProgram(_id);
}

void Shader::add(GLint type, std::string& filename)
{
    std::string path = PROJECT_SOURCE_DIR "/shaders/";
    fprintf(stderr, "Adding %s to the program\n", (path + filename).c_str());
    std::ifstream stream(path + filename);

    std::string shaderSource = std::string(std::istreambuf_iterator<char>(stream),
                                          (std::istreambuf_iterator<char>()));
    //fprintf(stderr, "Shader source: %s\n", shaderSource.c_str());
    const char* code = shaderSource.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    checkErrors(shader, getType(type));

    glAttachShader(_id, shader);
    glDeleteShader(shader);
}

void Shader::link()
{
    glLinkProgram(_id);
    checkErrors(_id, "PROGRAM");
}

void Shader::use()
{
    glUseProgram(_id);
}

std::string getType(GLint type)
{
    if (type == GL_COMPUTE_SHADER) return "COMPUTE";
    if (type == GL_FRAGMENT_SHADER) return "FRAGMENT";
    if (type == GL_GEOMETRY_SHADER) return "GEOMETRY";
    if (type == GL_VERTEX_SHADER) return "VERTEX";
}

void Shader::checkErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            fprintf(stderr, "ERROR::SHADER::%s::COMPILE\n%s", type.c_str(), infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            fprintf(stderr, "ERROR::SHADER::LINK::%s\n%s", type.c_str(), infoLog);
        }
    }
    assert(success);
}

void Shader::setBool(const std::string& name, bool value) const
{
    GLint location = glGetUniformLocation(_id, name.c_str());
    if (location != -1) 
        glUniform1i(location, value);
}

void Shader::setInt(const std::string& name, int value) const
{
    GLint location = glGetUniformLocation(_id, name.c_str());
    if (location != -1) 
        glUniform1i(location, value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    GLint location = glGetUniformLocation(_id, name.c_str());
    if (location != -1) 
        glUniform1f(location, value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    GLint location = glGetUniformLocation(_id, name.c_str());
    if (location != -1) 
        glUniform2fv(location, 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    GLint location = glGetUniformLocation(_id, name.c_str());
    if (location != -1) 
        glUniform3fv(location, 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    GLint location = glGetUniformLocation(_id, name.c_str());
    if (location != -1) 
        glUniform4fv(location, 1, &value[0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& value) const
{
    GLint location = glGetUniformLocation(_id, name.c_str());
    if (location != -1) 
        glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    GLint location = glGetUniformLocation(_id, name.c_str());
    if (location != -1)
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}