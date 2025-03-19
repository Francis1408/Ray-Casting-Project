
#include "glad/glad.h"
#include "stb_image.h"
#include "shader.h"

// GLM Mathematics Library headers
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <GLFW/glfw3.h>
#include <math.h>
#include <iostream>


// Tell GLFW we want to call this function on every window resize by registering it
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window); // Declaring function to process the inputs

// Texture mixing value
float mixValue = 0.2f;
    
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

    Shader ourShader("Shaders/shaderTransformation.vs", "Shaders/shaderTexture.fs");
    
    //===================================================================================================

    // Define the vertices, colors and texture coodinates of the rectangle
    float vertices[] = {
        // Rectangle
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
        
    };
    unsigned int indices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };
    
    // Declare a Vertex Array Object (VAO) to store the vertex attribute configuration
    // Does not need to manually set the vertex attribute pointers again on every draw call
    unsigned int VAO;
    // Allocate a buffer on the GPU to store the vertex data (optimization purposes)
    unsigned int VBO;
    // Create an element buffer object to store the indices
    unsigned int EBO;
    
    glGenVertexArrays(1,&VAO); // Generate a vertex array object
    glGenBuffers(1, &VBO); // Generate a buffer object name
    glGenBuffers(1, &EBO); // Generate a buffer object name
    
    glBindVertexArray(VAO); // Bind the vertex array object
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the buffer object to the target GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copy the vertex data into the buffer's memory
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Bind the buffer object to the target GL_ELEMENT_ARRAY_BUFFER
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Copy the indices data into the buffer's memory

    // Tell OpenGL how to interpret the vertex data (Per vertex attribute)
    

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //================ TEXTURES ================================================
    
    unsigned int texture1, texture2;

    // Binding the texture 1
    glGenTextures(1, &texture1); // Generate a texture object
    glBindTexture(GL_TEXTURE_2D, texture1); // Bind the texture object to any subsequent texture commands
    
    
    // Set the texture wrapping/filtering options for texture 1 (on the currently bound texture object)
    // Set the texture wrapping parameters for bopth the S and T axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Set the texture filtering parameters for the currently bound texture object
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    
    // Load and create a texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("Textures/container.jpg", &width, &height, &nrChannels, 0); //image's width, height and number of color channels
    
    if(data) {
        // Generating the Texture
        // Argument 1: The texture target
        // Argument 2: The mipmap level for which we want to create a texture for
        // Argument 3: The format of the texture we want to generate
        // Argument 4 and 5: The width and height of the resulting image
        // Argument 6: Always 0
        // Argument 7 and 8: The format and datatype of the source image
        // Argument 9: The actual image data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // Generate a 2D texture image
        glGenerateMipmap(GL_TEXTURE_2D); // Generate all the mipmap levels for the specified texture object
        
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    // free the image memory
    stbi_image_free(data);


    // Binding the texture 2
    glGenTextures(1, &texture2); // Generate a texture object
    glBindTexture(GL_TEXTURE_2D, texture2); // Bind the texture object to any subsequent texture commands
    
    
    // Set the texture wrapping/filtering options for texture 2 (on the currently bound texture object)
    // Set the texture wrapping parameters for bopth the S and T axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Set the texture filtering parameters for the currently bound texture object
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Load and create  texture 2
    data = stbi_load("Textures/awesomeface.png", &width, &height, &nrChannels, 0);
    
    if(data) {
        // Generating the Texture
        // Argument 1: The texture target
        // Argument 2: The mipmap level for which we want to create a texture for
        // Argument 3: The format of the texture we want to generate
        // Argument 4 and 5: The width and height of the resulting image
        // Argument 6: Always 0
        // Argument 7 and 8: The format and datatype of the source image
        // Argument 9: The actual image data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // Generate a 2D texture image
        glGenerateMipmap(GL_TEXTURE_2D); // Generate all the mipmap levels for the specified texture object
        
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    // free the image memory
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    // or set it via the texture class
    ourShader.setInt("texture2", 1);
    
    //===================================================================================================
    
    
    //================ RENDER LOOP ================================================
    while (!glfwWindowShouldClose(window)) { // Check if the window should close
        
        processInput(window); // Check if the user has pressed any key
        
        // Rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set the color that OpenGL uses to clear the screen
        glClear(GL_COLOR_BUFFER_BIT); // Clear the screen with the color set by glClearColor


         //==================== TRANSFORMATIONS ============================================
    
        glm::mat4 trans = glm::mat4(1.0f); // Create the identity matrix
        // OBS : The transformation sequence is reversed
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f)); // Apply a translation of 0.5 units in the x and -0.5 units in the y
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0)); // Apply a rotation of 90 degrees in the z axis

        // Apply the transformations to the shader
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform"); // Locate the uniform variable
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans)); // Set the uniform variable
       
        //Bind the textures
        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        
        ourShader.setFloat("mixValue", mixValue);
        
        ourShader.use(); // Activate the shader program object
        
        
        // Drawing commands here
        glBindVertexArray(VAO); // Bind the vertex array object
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw the rectangle
        // The first parameter is the mode, the second is the starting index, and the third is the number of vertices to draw
        

        // DOUBLE BUFFERING: The front buffer contains the final output image that is shown at the screen
        glfwSwapBuffers(window); // Swap the color buffer that is used to render
        glfwPollEvents(); // Check if any events are triggered
        



        //===================================================================================================
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

    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if(mixValue < 1.0f) {
            mixValue += 0.01f;
        }
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if(mixValue > 0.0f) {
            mixValue -= 0.01f;
        }
    }

}

