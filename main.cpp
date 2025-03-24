#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "game.h"
#include "resourceManager.h"

#include <iostream>

// Tell GLFW we want to call this function on every window resize by registering it
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode); // Declaring function to process the inputs

// The Width of the screen
const unsigned int SCREEN_WIDTH = 800;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 600;

Game RayCast(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[]) {

    // Instanciating a GLFWwindow object and setting its configuration
    glfwInit(); // Initialize the GLFW library
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // What version of OpenGL we want to use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Version 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creating a window object
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RayCast", NULL, NULL); // Width, Height, Title, Monitor, Share
    glfwMakeContextCurrent(window); // Make the window's context the current context


    // Load all OpenGL function pointers with GLAD
    // Necessary to do after creating the window and before calling any OpenGL function
    // This is because the function pointers are loaded based on the current context
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set the callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Enable Z buffer
    glEnable(GL_DEPTH_TEST);

    // initialize game
    // ---------------
    RayCast.Init();

    // deltaTime variables
    // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;


     //================ RENDER LOOP ================================================

     while (!glfwWindowShouldClose(window))
    {
        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        // manage user input
        // -----------------
        RayCast.ProcessInput(deltaTime);

        // update game state
        // -----------------
        RayCast.Update(deltaTime);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        RayCast.Render();

        glfwSwapBuffers(window);
    }

    // delete all resources as loaded using the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();

    glfwTerminate();
    return 0;

}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            RayCast.Keys[key] = true;
        else if (action == GLFW_RELEASE)
            RayCast.Keys[key] = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
