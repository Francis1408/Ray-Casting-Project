#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include <string>
#include <fstream>  
#include <sstream>
#include <iostream>

class Shader
{

public:

    // the program ID
    unsigned int ID;

    // construtor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);

    // use/activate the shader
    void use();

    //utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

    Shader(/* args */);
    ~Shader();
};

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. Retrieve the vertex/fragment source code from filePath

    std::string vertexCode; // Source code from the vertex shader
    std::string fragmentCode; // Source code from the fragment shader
    
    std::ifstream vShaderFile; // File that carries the vertz shader source code
    std::ifstream fShaderFile; // File that carries the fragment shader source code

    //ensure ifstream objects can throw exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {

        //open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        //read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // close file handlers
        vShaderFile.close();
        fShaderFile.close();   

        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str(); 
    }
    catch(std::ifstream::failure e) { // Error message if fails to read the file
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    // convert code into a const char* format
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();


    // 2. Compile the shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // Create a vertex shader object with ID and attach the shader source code to it
    vertex = glCreateShader(GL_VERTEX_SHADER); // Create a shader object
    // The second parameter is the number of strings in the third parameter (source code) of the shader object
    glShaderSource(vertex, 1, &vShaderCode, NULL); // Attach the shader source code to the shader object 
    glCompileShader(vertex); // Compile the shader

    // Check for shader compile errors
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Create a fragment shader object with ID and attach the shader source code to it
    fragment = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader object
    glShaderSource(fragment, 1, &fShaderCode, NULL); // Attach the shader source code to the shader object
    glCompileShader(fragment); // Compile the shader

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    if(!success)
    {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    ID = glCreateProgram(); // Final linked version of multiple shaders combined
    // Linking: Links the output of the vertex shader to the input of the fragment shader
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID); // Link the shaders
    //Result: Program object that we can activate with glUseProgram and use to render objects

    glGetShaderiv(ID, GL_COMPILE_STATUS, &success);

    if(!success)
    {
    glGetShaderInfoLog(ID, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Delete the shader objects after linking them to the program object
    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

// Method to activate the shader
void Shader::use()
{
    glUseProgram(ID);
}

// Utility uniform functions

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

Shader::~Shader()
{
}

#endif
