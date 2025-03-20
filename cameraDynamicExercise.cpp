
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

// Tell GLFW we want to call this function on every mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Texture mixing value
float mixValue = 0.2f;


// Initial paosition/ direction and up vector from the camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // The position of the camera
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // The point the camera is looking at
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // The up vector of the camera
    
 //================ FRANE RATE VARIABLES ================================================
 float lastFrame = 0.0f; // Time between current frame and last frame
 float deltaTime = 0.0f; // Time of last frame


 // Bool to control mouse sudden jump
 bool firstMouse = true;

 // Initial mouse position
 float lastX = 400, lastY = 300; // Middle of the screen size
 // Initial yaw and pitch
    float yaw = -90.0f, pitch = 0.0f; // Initial yaw is -90 degrees because the camera is looking at the negative z-axis

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

    // Enable Z buffer
    glEnable(GL_DEPTH_TEST);

    // Tell GLFW to capture the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Tell GLFW we want to call this function on every mouse movement
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

    //=============== SHADERS ================================================

    Shader ourShader("Shaders/shaderCoordinateSystem.vs", "Shaders/shaderCoordinateSystem.fs");
    
    //===================================================================================================

    // Define the vertices and texture coodinates of the rectangle
    float vertices[] = {
        // positions          // Texture
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // Cube Positions

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
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
    

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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
    
    // ================== CREATING COORDINATE SYSTEM WITH TRANFORMATION MATRICES ================================================
    // The model matrix is used to move/rotate/scale the object (model) in the world space
    // The view matrix is used to move the entire scene around to give the impression of moving the camera
    // The projection matrix is used to project the 3D scene to the 2D screen
    // The model, view and projection matrices are multiplied to create the final matrix that is used to transform the vertices


    // The projection matrix
    glm::mat4 projection = glm::mat4(1.0f); // Initialize the projection matrix to the identity matrix
    projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f); // Set the projection matrix to a perspective projection matrix
    ourShader.setMat4("projection", projection); // Set the value of the projection matrix uniform
    
    
    // ============================================================================================

    //================ RENDER LOOP ================================================
    while (!glfwWindowShouldClose(window)) { // Check if the window should close
        
        // Defines the constant speed based on the frame rate
        // The time difference between the current frame and the last frame
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Inputs
        processInput(window); // Check if the user has pressed any key
        
        // Rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set the color that OpenGL uses to clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen with the color set by glClearColor and buffer

        //Bind the textures
        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        ourShader.setFloat("mixValue", mixValue);

        // Set the coordinate system
        ourShader.use(); // Activate the shader program object
        
        // The view matrix => Used to define the camera
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // look at = cross prodeuct of the camera's direction (camerapos - target) and the up vector 
        ourShader.setMat4("view", view); // Set the value of the view matrix uniform
        
        // Drawing commands here
        glBindVertexArray(VAO); // Bind the vertex array object
    
        // Draw the boxes
        for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i; 
            if(i%3 == 0 || i == 0) {
                model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(1.0f, 0.3f, 0.5f));
            }
            else {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            }
            ourShader.setMat4("model", model); // Set the value of the model matrix uniform  

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    

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

// Callback function to each time that the mouse moves
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX; // The offset of the mouse position in Xaxis
    float yoffset = lastY - ypos; // The offset of the mouse position in Yaxis
    lastX = xpos; // Refresh lastX
    lastY = ypos; // Refresh lastY

    float sensitivity = 0.1f; // The sensitivity of the mouse
    xoffset *= sensitivity; // Apply the sensitivity to the Xoffset
    yoffset *= sensitivity; // Apply the sensitivity to the Yoffset

    // Update the yaw and pitchd
    // Yaw = rotation in the Yaxis
    // Pitch = rotation in the Xaxis
    yaw += xoffset;
    pitch += yoffset;

    // Clamp the pitch to avoid flipping the screen
    if(pitch > 89.0f) {
        pitch = 89.0f;
    }
    if(pitch < -89.0f) {
        pitch = -89.0f;
    }

    // Calculate the direction of the camera
    // The direction of the camera is calculated using the yaw and pitch
    // The direction is calculated using the spherical coordinates
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

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

    // Camera controls
    float cameraSpeed = static_cast<float>(2.5f * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;


}

