#include "game.h"
#include "resourceManager.h"
#include "spriteRenderer.h"
#include "gameLevel.h"

// GLM Mathematics Library headers
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>


SpriteRenderer *Renderer;
unsigned int NumOfRays = 320;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("Shaders/shaderCoordinate.vs", "Shaders/shaderCoordinate.fs", nullptr, "coordinate");

    // Initial Position
    glm::vec2 position = glm::vec2(22.0f, 12.0f); // x, y start position
    glm::vec2 direction = glm::vec2(-1.0f, 0.0f); // initial direction vector
    glm::vec2 plane = glm::vec2(0.0f, 0.66f); // the 2d raycaster version of camera plane


   //configure coordinate system
   // FOV => 2 * atan(0.5 * aspect ratio)
   //float fov = 2 * atan(0.5 * static_cast<float>(plane.y) / static_cast<float>(1)); 
   //printf("fov: %f\n", fov);


    glm::mat4 projection = glm::perspective(glm::radians(66.0f), static_cast<float>(this->Width) / static_cast<float>(this->Height), 0.1f, 100.0f);

    GameLevel one; one.Load("Levels/one.lvl", this->Width, this->Height / 2);
   // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
    //    static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    ResourceManager::GetShader("coordinate").Use().SetInt("image", 0);
    ResourceManager::GetShader("coordinate").SetMat4("projection", projection);
    ResourceManager::GetShader("coordinate").SetMat4("view", view);

    // set render-specific controls
    Shader coordinateShader = ResourceManager::GetShader("coordinate");
    Renderer = new SpriteRenderer(coordinateShader);

    // load textures
    //ResourceManager::LoadTexture("Textures/awesomeface.png", true, "face");
}

void Game::Update(float dt)
{
    
}

void Game::ProcessInput(float dt)
{
   
}

void Game::Render()
{

    //Texture2D myTexture;
   // myTexture = ResourceManager::GetTexture("face");
    Renderer->DrawSprite(glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}