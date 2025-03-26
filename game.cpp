#include "game.h"
#include "resourceManager.h"
#include "spriteRenderer.h"
#include "gameLevel.h"
#include "playerObject.h"

// GLM Mathematics Library headers
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <vector> 



SpriteRenderer *Renderer;
unsigned int NUM_OF_RAYS = 320;


// Player stats
PlayerObject *Player;


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
    /*
    glm::vec2 position = glm::vec2(22.0f, 12.0f); // x, y start position
    glm::vec2 direction = glm::vec2(-1.0f, 0.0f); // initial direction vector
    glm::vec2 plane = glm::vec2(0.0f, 0.66f); // the 2d raycaster version of camera plane
    */


    // Create player object
    
    //configure coordinate system
    // FOV => 2 * atan(0.5 * aspect ratio)
    //float fov = 2 * atan(0.5 * static_cast<float>(plane.y) / static_cast<float>(1)); 
    //printf("fov: %f\n", fov);
    
    // Setting up camera system
    
    
    //glm::mat4 projection = glm::perspective(glm::radians(66.0f), static_cast<float>(this->Width) / static_cast<float>(this->Height), 0.1f, 100.0f);
    
    
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);  
    
    
    // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
    //    static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    
    ResourceManager::GetShader("coordinate").Use().SetInt("image", 0);
    ResourceManager::GetShader("coordinate").SetMat4("projection", projection);
    
    // set render-specific controls
    Shader coordinateShader = ResourceManager::GetShader("coordinate");
    Renderer = new SpriteRenderer(coordinateShader);
    
    
    // load levels
    GameLevel one; one.Load("Levels/one.lvl", "Levels/one.ele", this->Width/2, this->Height);
    this->Levels.push_back(one);
    this->Level = 0;

    Player = new PlayerObject(one.PlayerPosition, one.PlayerSize, glm::vec3(1.0f, 1.0f, 0.0f), 50.0f, 5.0f, glm::vec2(-1.0f, 0.0f));
    // load textures
    //ResourceManager::LoadTexture("Textures/awesomeface.png", true, "face");
}

void Game::Update(float dt)
{
    
}

void Game::ProcessInput(float dt)
{
    float velocity = Player->velocity *dt;
    float rotSpeed = Player->rotSpeed *dt;

    if (this -> Keys[GLFW_KEY_W]) { // Apply translation
        
        Player->Position.x += velocity * Player->direction.x;
        Player->Position.y += velocity * Player->direction.y;
        
       // printf("X: %.2f , Y: %.2f\n", Player->Position.x, Player->Position.y);
    }
    if (this -> Keys[GLFW_KEY_S]) { // Apply rotation
        
        Player->Position.x -= velocity * Player->direction.x;
        Player->Position.y -= velocity * Player->direction.y;

       // printf("X: %.2f , Y: %.2f\n", Player->Position.x, Player->Position.y);
    }
    if (this -> Keys[GLFW_KEY_A]) { //Apply translation
        
        float oldDirX = Player->direction.x;

        Player->direction.x = Player->direction.x * cos(rotSpeed) -  Player->direction.y * sin(rotSpeed);
        Player->direction.y = oldDirX * sin(rotSpeed) + Player->direction.y * cos(rotSpeed);
    }
    if (this -> Keys[GLFW_KEY_D]) { // Apply rotation
        
        float oldDirX = Player->direction.x;

        Player->direction.x = Player->direction.x * cos(-rotSpeed) -  Player->direction.y * sin(-rotSpeed);
        Player->direction.y = oldDirX * sin(-rotSpeed) + Player->direction.y * cos(-rotSpeed);

        //printf("X: %.2f , Y: %.2f\n", Player->Position.x, Player->Position.y);
    }
   
}

void Game::Render()
{
   
    // Draw Level Map in the first half of the screen


    //Texture2D myTexture;
   // myTexture = ResourceManager::GetTexture("face");
    //Renderer->DrawSprite(glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    this->Levels[this->Level].Draw(*Renderer);
    Player->Draw(*Renderer);
    
}