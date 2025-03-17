#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <math.h>

#include <iostream>

#include "shader.h"

// Tell GLFW we want to call this function on every window resize by registering it
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window); // Declaring function to process the inputs

    
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

    Shader ourShader("Shaders/shaderMovingTriangle.vs", "Shaders/shaderOrange.fs");
    
    //===================================================================================================

    // Define the vertices coodinates of the triangles
    float vertices[] = {
        //First Triangle
        // positions          
        -0.5f, -0.5f, 0.0f, 
         0.5f, -0.5f, 0.0f,  
         0.0f,  0.5f, 0.0f, 

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
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0); // Stride value of 6 times the size of a float in bytes (= 24 bytes) 
    glEnableVertexAttribArray(0); // Enable the vertex attribute at location 0

    
    // Render loop
    while (!glfwWindowShouldClose(window)) { // Check if the window should close
        
        processInput(window); // Check if the user has pressed any key

        // Rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set the color that OpenGL uses to clear the screen
        glClear(GL_COLOR_BUFFER_BIT); // Clear the screen with the color set by glClearColor
    
        ourShader.use(); // Activate the shader program object

        float timeValue = glfwGetTime(); // Get the time as the number of seconds since GLFW started
        
        float offset = sin(timeValue) / 2.0f; // Change the offset value over time
        ourShader.setFloat("offset", offset); // Set the uniform value in the OpenGL code
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

