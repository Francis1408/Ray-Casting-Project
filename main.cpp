
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "game.h"
#include "resourceManager.h"
#include "textRenderer.h"
#include "character.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <filesystem>

#include <iostream>

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Calculate FPS function
void showFPS(Shader &shader, float& fpsLastTime, unsigned int& fpsFrameCount);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 1024;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 512;
// Time to update the FPS value
const float ELAPSED_TIME = 0.5f;


Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);


TextRenderer *textRenderer;

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL configuration
    // --------------------
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initialize game
    // ---------------
    Breakout.Init();

    // Create the text shader
    Shader TextShader = ResourceManager::GetShader("text");

    // Instansiate the Text Renderer
    textRenderer = new TextRenderer(TextShader);

    // Load the character font
    textRenderer->LoadFont("Fonts/Antonio-Bold.ttf", 48);

    // deltaTime variables
    // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // Fps variables
    // -------------------
    float fpsLastTime = 0.0f;
    unsigned int fpsFrameCount = 0;

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
        Breakout.ProcessInput(deltaTime);
        
        // update game state
        // -----------------
        // Breakout.Update(deltaTime);
        
        // render
        // ------
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Breakout.Render();
        

        // FPS Counter
        showFPS(TextShader, fpsLastTime, fpsFrameCount);
        
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
            Breakout.Keys[key] = true;
        else if (action == GLFW_RELEASE)
            Breakout.Keys[key] = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// Function to calculate the FPS 
void showFPS(Shader &shader, float& fpsLastTime, unsigned int& fpsFrameCount) {
    
    static float FPS = 0.0f; // Persistent value between calls

    float currentTime = glfwGetTime();
    float elapsedTime = currentTime - fpsLastTime;
    fpsFrameCount++;

    // Update the FPS on every elapsed time
    if(elapsedTime >= ELAPSED_TIME) {
        
        FPS = fpsFrameCount/ elapsedTime;   
        fpsFrameCount = 0;
        fpsLastTime = currentTime;
    }
    // Show the FPS time on screen
    textRenderer->DrawText("FPS: " + std::to_string(static_cast<int>(FPS)),
        0.0f, 480.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));

}
