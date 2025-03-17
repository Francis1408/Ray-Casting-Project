#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <math.h>

#include <iostream>

// Tell GLFW we want to call this function on every window resize by registering it
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window); // Declaring function to process the inputs

// Source code from the vertex shader
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "out vec4 vertexColor;\n" // Output a color to the fragment shader
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n" // Set the output to the fragment shader to dark red
    "}\0";

// Source code from the fragment shader
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 vertexColor;\n" // The input variable from the vertex shader (same name and same type)
    "void main()\n"
    "{\n"
    "   FragColor = vertexColor;\n"
    "}\n\0";

// Source code from the fragment shader with uniform value
const char* fragmentShaderSourceUniform = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n" // Set this variable in the OpenGL code
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n"
    "}\n\0";


    
int main() {

    // Instanciating a GLFWwindow object and setting its configuration
    glfwInit(); // Initialize the GLFW library
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // What version of OpenGL we want to use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Version 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    

    // Creating a window object
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL); // Width, Height, Title, Monitor, Share

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // Terminate the GLFW library
        return -1;
    }
    glfwMakeContextCurrent(window); // Make the window's context the current context

    // Load all OpenGL function pointers with GLAD
    // Necessary to do after creating the window and before calling any OpenGL function
    // This is because the function pointers are loaded based on the current context
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Tell OpenGL the size of the rendering window
    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

    //=============== SHADERS ================================================

    // PIPELINE PART 1
    // Create a vertex shader object with ID and attach the shader source code to it
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER); // Create a shader object
    // The second parameter is the number of strings in the third parameter (source code) of the shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Attach the shader source code to the shader object 
    glCompileShader(vertexShader); // Compile the shader

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //PIPELINE PART 5
    // Create a fragment shader object with ID and attach the shader source code to it
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader object
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceUniform, NULL); // Attach the shader source code to the shader object
    glCompileShader(fragmentShader); // Compile the shader

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    // Create a shader program object to link the shaders created previously
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram(); // Final linked version of multiple shaders combined
    // Linking: Links the output of the vertex shader to the input of the fragment shader
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); // Link the shaders
    //Result: Program object that we can activate with glUseProgram and use to render objects

    glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &success);

    if(!success)
    {
    glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Delete the shader objects after linking them to the program object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //===================================================================================================

    // Define the vertices coodinates of the triangles
    float vertices[] = {
        //First Triangle
        -0.5f, -0.5f, 0.0f, // Left
         0.5f, -0.5f, 0.0f, // Right
         0.0f,  0.5f, 0.0f,  // Top

    };

    // Declare a Vertex Array Object (VAO) to store the vertex attribute configuration
    // Does not need to manually set the vertex attribute pointers again on every draw call
    unsigned int VAO;
    // Allocate a buffer on the GPU to store the vertex data (optimization purposes)
    unsigned int VBO;

    glGenVertexArrays(1,&VAO); // Generate a vertex array object
    glGenBuffers(1, &VBO); // Generate a buffer object name

    glBindVertexArray(VAO); // Bind the vertex array object

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the buffer object to the target GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copy the vertex data into the buffer's memory

    // Tell OpenGL how to interpret the vertex data (Per vertex attribute)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Enable the vertex attribute at location 0


    
    
    // Render loop
    while (!glfwWindowShouldClose(window)) { // Check if the window should close
        
        processInput(window); // Check if the user has pressed any key

        // Rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set the color that OpenGL uses to clear the screen
        glClear(GL_COLOR_BUFFER_BIT); // Clear the screen with the color set by glClearColor
        
        // Change the Uniform color value (which is universal for the pipeline) over time
        float timeValue = glfwGetTime(); // Get the time as the number of seconds since GLFW started
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f; // Change the green value over time
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor"); // Get the location of the uniform variable (Query)
        glUseProgram(shaderProgram); // Activate the shader program object
        // NOTE: Every shader and rendering call after glUseProgram will now use this program object
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); // Set the uniform value in the OpenGL code
        
        
        
        // Drawing commands here
        glBindVertexArray(VAO); // Bind the vertex array object
        glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the triangles
        // The first parameter is the mode, the second is the starting index, and the third is the number of vertices to draw
        


        // DOUBLE BUFFERING: The front buffer contains the final output image that is shown at the screen
        glfwSwapBuffers(window); // Swap the color buffer that is used to render
        glfwPollEvents(); // Check if any events are triggered
        
    }
    
    glfwTerminate(); // Terminate the GLFW library and clean all the resources allocated
    
    return 0;
}


// Callback funtion to each time that the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

    glViewport(0, 0, width, height);

}

// Function to deal with the inputs
void processInput(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { // If the key pressed is space
        glfwSetWindowShouldClose(window, true); // Set the window to close
    }

}

