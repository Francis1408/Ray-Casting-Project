#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>

// Tell GLFW we want to call this function on every window resize by registering it
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window); // Declaring function to process the inputs

// Source code from the vertex shader
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// Source code from the fragment shader
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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



    //PIPELINE PART 5
    // Create a fragment shader object with ID and attach the shader source code to it
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader object
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // Attach the shader source code to the shader object
    glCompileShader(fragmentShader); // Compile the shader


    // Create a shader program object to link the shaders created previously
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram(); // Final linked version of multiple shaders combined
    // Linking: Links the output of the vertex shader to the input of the fragment shader
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); // Link the shaders
    //Result: Program object that we can activate with glUseProgram and use to render objects
    
    // Delete the shader objects after linking them to the program object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //===================================================================================================

    // Define the vertices coodinates of the triangles
    float vertices1[] = {
        //First Triangle
        -0.5f, -0.5f, 0.0f, // Left
         0.5f, -0.5f, 0.0f, // Right
         0.0f,  0.5f, 0.0f,  // Top
    };

    float vertices2[] ={
        //Second Triangle
        0.5f, -0.5f, 0.0f, // Left
        1.0f, -0.5f, 0.0f, // Right
        0.75f, 0.5f, 0.0f // Top
    };

    // Declare a Vertex Array Object (VAO) to store the vertex attribute configuration
    // Does not need to manually set the vertex attribute pointers again on every draw call
    unsigned int VAOs[2]; // Declare multiple VAOs at the same time
    // Allocate a buffer on the GPU to store the vertex data (optimization purposes)
    unsigned int VBOs[2]; // Declare multiple VBOs at the same time

    glGenVertexArrays(2, VAOs); // Generate a vertex array object
    glGenBuffers(2, VBOs); // Generate a buffer object name
    
    glBindVertexArray(VAOs[0]); // Bind the first vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); // Bind the buffer object to the target GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW); // Copy the vertex data into the buffer's memory
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // No need to unbind the VAO or VBO, it will stay bound just like the shader program
    
    glBindVertexArray(VAOs[1]); // Bind the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]); // Bind the buffer object to the target GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW); // Copy the vertex data into the buffer's memory
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Enable the vertex attribute at location 0
     // No need to unbind the VAO or VBO, it will stay bound just like the shader program
    
    
    
    // Render loop
    while (!glfwWindowShouldClose(window)) { // Check if the window should close
        
        processInput(window); // Check if the user has pressed any key

        // Rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set the color that OpenGL uses to clear the screen
        glClear(GL_COLOR_BUFFER_BIT); // Clear the screen with the color set by glClearColor
        
        // Drawing commands here
        glUseProgram(shaderProgram); // Activate the shader program object
        // NOTE: Every shader and rendering call after glUseProgram will now use this program object
        glBindVertexArray(VAOs[0]); // Bind the vertex array object
        glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the triangles
        glBindVertexArray(VAOs[1]); // Bind the vertex array object
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

