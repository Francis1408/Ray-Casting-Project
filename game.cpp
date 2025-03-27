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
unsigned int NUM_OF_RAYS = 1;

float planeX = 0, planeY = 0.66; // the 2d raycaster version of camera plane


// Player stats
PlayerObject *Player;

GameObject  *look;


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
    
   // 
    
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);  
    

    /*
    ------------------------------------
    | (-1,-1)         |          (1,-1) |
    |                 |                 |
    |                 |                 |
    |                 |                 |
    |                 |                 |
    |                 |                 |
    |                 |                 |
    -------------------------------------
    |                 |                 |
    |                 |                 |
    |                 |                 |
    |                 |                 |
    |                 |                 |
    |                 |                 |
    | (-1,1)                       (1,1)|              
    -------------------------------------
    
    */
    
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

    Player = new PlayerObject(one.PlayerPosition, one.PlayerSize, glm::vec3(1.0f, 1.0f, 0.0f), 50.0f, 5.0f, glm::vec2(0.0f, -1.0f));

    // creating a visual look for the player
    glm::vec2 lookPos = glm::vec2(one.PlayerPosition.x + one.PlayerSize.x/4, one.PlayerPosition.y - one.PlayerSize.y*2);
    look = new GameObject(lookPos, glm::vec2(one.PlayerSize.x/2, + one.PlayerSize.y*2),glm::vec3(1.0f, 1.0f, 0.0f));
    look->Pivot = glm::vec2(0.5f, 1.0f);

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

    float mapScale = this->Levels[this->Level].tileSize;

    int mapx, mapy;

    if (this -> Keys[GLFW_KEY_W]) { // Apply translation
        
        

        Player->Position.x += velocity * Player->direction.x;
        Player->Position.y += velocity * Player->direction.y;

        look->Position.x = Player->Position.x + Player->Size.x/4;
        look->Position.y = Player->Position.y - Player->Size.y*2;

        mapx = ((static_cast<int>(Player->Position.x/mapScale)));
        mapy = ((static_cast<int>(Player->Position.y/mapScale)));

        //printf("X: %.2f , Y: %.2f\n", Player->Position.x, Player->Position.y);
        //printf("mapX: %d , mapY: %d\n", mapx, mapy);
    }
    if (this -> Keys[GLFW_KEY_S]) { // Apply rotation
        
        Player->Position.x -= velocity * Player->direction.x;
        Player->Position.y -= velocity * Player->direction.y;

        look->Position.x = Player->Position.x + Player->Size.x/4;
        look->Position.y = Player->Position.y - Player->Size.y*2;
        
        mapx = ((static_cast<int>(Player->Position.x/mapScale)));
        mapy = ((static_cast<int>(Player->Position.y/mapScale)));

       // printf("X: %.2f , Y: %.2f\n", Player->Position.x, Player->Position.y);
       // printf("mapX: %d, mapY: %d\n", mapx, mapy);
       
    }
    if (this -> Keys[GLFW_KEY_A]) { //Apply translation
        
        float oldDirX = Player->direction.x;
        
        Player->direction.x = Player->direction.x * cos(rotSpeed) -  Player->direction.y * sin(rotSpeed);
        Player->direction.y = oldDirX * sin(rotSpeed) + Player->direction.y * cos(rotSpeed);
        
       
       // Debug 
        look->Rotation = atan2(Player->direction.y, Player->direction.x) *  (180.0f / M_PI);

        printf("angle: %.2f\n",look->Rotation);
    }
    if (this -> Keys[GLFW_KEY_D]) { // Apply rotation
        
        float oldDirX = Player->direction.x;

        Player->direction.x = Player->direction.x * cos(-rotSpeed) -  Player->direction.y * sin(-rotSpeed);
        Player->direction.y = oldDirX * sin(-rotSpeed) + Player->direction.y * cos(-rotSpeed);

    
        look->Rotation = atan2(Player->direction.y, Player->direction.x) *  (180.0f / M_PI);

        printf("angle: %.2f\n",look->Rotation);
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
    look->Draw(*Renderer);


    // RAYCASTING ALGORRITHM
    /*
    // Each interation creates a ray which are distributed throught the plane(screen) space;
    for(int x = 0; x < NUM_OF_RAYS; x++) {
        // calculate ray position and direction
        /*
        cameraX is the x-coordinate on the camera plane that the current x-coordinate of the screen represents, 
        done this way so that the right side of the screen will get coordinate 1, the center of the screen gets coordinate 0, 
        and the left side of the screen gets coordinate -1.
        float cameraX = 2 * x / static_cast<float>(NUM_OF_RAYS) - 1; // Distribute symetrically the number of rays on the screen
        glm::vec2 rayDir = glm::vec2(Player->direction.x + planeX * cameraX,Player->direction.y + planeX * cameraX);
        
        // The amount of units to the ray hit the next tile
        glm::vec2 deltaDist = glm::vec2(std::abs(1/rayDir.x), std::abs(1/rayDir.y));
        */
       
       
    }
    
