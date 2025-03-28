
#include "shader.h"
#include <iostream>


Shader &Shader::Use() {

    glUseProgram(this->ID);
    return *this; 
}


void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource) {
        // compile shaders
        unsigned int vertex, fragment, geometry;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexSource, NULL);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");

        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentSource, NULL);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");

        // geometry shader
        if(geometrySource != nullptr) {
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &geometrySource, NULL);
            glCompileShader(geometry);
            CheckCompileErrors(geometry, "GEOMETRY");
        }

        // shader Program
        this->ID = glCreateProgram();
        glAttachShader(this->ID, vertex);
        glAttachShader(this->ID, fragment);
        if (geometrySource != nullptr)
            glAttachShader(this->ID, geometry);
        glLinkProgram(this->ID);
        CheckCompileErrors(this->ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometrySource != nullptr)
            glDeleteShader(geometry);

}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::SetBool(const std::string &name, bool value, bool useShader) 
{   
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
// ------------------------------------------------------------------------
void Shader::SetInt(const std::string &name, int value, bool useShader) 
{ 
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::SetFloat(const std::string &name, float value, bool useShader) 
{ 
    if (useShader)
        this->Use();
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::SetVec2(const std::string &name, const glm::vec2 &value, bool useShader) 
{ 
    if (useShader)
        this->Use();
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::SetVec2(const std::string &name, float x, float y,bool useShader) 
{ 
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
}
// ------------------------------------------------------------------------
void Shader::SetVec3(const std::string &name, const glm::vec3 &value, bool useShader) 
{ 
    if (useShader)
        this->Use();
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::SetVec3(const std::string &name, float x, float y, float z, bool useShader) 
{ 
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
}
// ------------------------------------------------------------------------
void Shader::SetVec4(const std::string &name, const glm::vec4 &value, bool useShader) 
{ 
    if (useShader)
        this->Use();
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::SetVec4(const std::string &name, float x, float y, float z, float w, bool useShader) 
{ 
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
}
// ------------------------------------------------------------------------
void Shader::SetMat2(const std::string &name, const glm::mat2 &mat, bool useShader) 
{
    if (useShader)
        this->Use();
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::SetMat3(const std::string &name, const glm::mat3 &mat, bool useShader) 
{
    if (useShader)
        this->Use();
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::SetMat4(const std::string &name, const glm::mat4 &mat, bool useShader) 
{
    if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::CheckCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
