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
#include <algorithm> 

SpriteRenderer *WallRenderer;
SpriteRenderer *FloorRenderer;
SpriteRenderer *SpRenderer;
SpriteRenderer *MapRenderer;


// Player stats
PlayerObject *Player;

GameObject  *look;
GameObject  *wallObj;
GameObject  *floorObj;
GameObject  *spriteObj;
Texture2D   *floorTexture;

//Scale of the level map in the grid size
float mapScale;


// Map size in grid units
unsigned int mapSizeGridX;
unsigned int mapSizeGridY;


// RayDensity = How thick is each wall slice
unsigned int rayDensity = 1;


Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete WallRenderer;
    delete FloorRenderer;
    delete SpRenderer;
    delete Player;
    delete look;
    delete wallObj;
    delete floorObj;
    delete spriteObj;

    delete floorTexture;
    floorTexture = nullptr;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("Shaders/shaderCoordinate.vs", "Shaders/shaderWall.fs", nullptr, "wall");
    ResourceManager::LoadShader("Shaders/shaderCoordinate.vs", "Shaders/shaderFloor.fs", nullptr, "floor");
    ResourceManager::LoadShader("Shaders/shaderText.vs", "Shaders/shaderText.fs", nullptr, "text");
    ResourceManager::LoadShader("Shaders/shaderSprite.vs", "Shaders/shaderSprite.fs", nullptr, "sprite");
    ResourceManager::LoadShader("Shaders/shaderCoordinate.vs", "Shaders/shaderCoordinate.fs", nullptr, "map");

   // Define the View Matrix - Game is oriented from top to bottom
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
   // The text Coordinate is defined from bottom to top
    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(this->Width), 0.0f, static_cast<float>(this->Height));
    
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
   
   
   // Set the uniform values on each shader    
   ResourceManager::GetShader("wall").Use().SetInt("image", 0);
   ResourceManager::GetShader("wall").SetMat4("projection", projection);
   ResourceManager::GetShader("floor").Use().SetInt("image", 0);
   ResourceManager::GetShader("floor").SetMat4("projection", projection);
   ResourceManager::GetShader("sprite").Use().SetInt("image", 0);
   ResourceManager::GetShader("sprite").SetMat4("projection", projection);
   ResourceManager::GetShader("sprite").SetFloat("screenWidth", static_cast<float>(this->Width)); // Pass the screen width to the sprite fragment shader
   ResourceManager::GetShader("text").Use().SetMat4("text", 0);
   ResourceManager::GetShader("text").SetMat4("projection", textProjection);
   ResourceManager::GetShader("map").Use().SetInt("image", 0);
   ResourceManager::GetShader("map").SetMat4("projection", projection);
   
   // Set render-specific controls
   Shader Shader = ResourceManager::GetShader("wall");
   WallRenderer = new SpriteRenderer(Shader);
   
   Shader = ResourceManager::GetShader("floor");
   FloorRenderer = new SpriteRenderer(Shader);

   Shader = ResourceManager::GetShader("sprite");
   SpRenderer = new SpriteRenderer(Shader);

   Shader = ResourceManager::GetShader("map");
   MapRenderer = new SpriteRenderer(Shader);

   // ========================= Buffers =======================================
   
   // Z Buffer to handle sprite depth
   // Each position contains the distance of each vertical stripe from the horizontal wall
    this->ZBuffer.resize(static_cast<int>(Width/2));
   
   // =================== Load textures ========================================
   
   ResourceManager::LoadTextures("Textures/");
   
   floorTexture = new Texture2D();
   
   // Initialize GameObjects
   wallObj = new GameObject();
   floorObj = new GameObject();
   spriteObj = new GameObject();
   
   // load levels
   GameLevel one; 
   one.Load("Levels/one.lvl", "Levels/one.flo", "Levels/one.cel", "Levels/one.ele",  this->Width/2, this->Height);
   this->Levels.push_back(one);
   this->Level = 0;
   
   // Set the map Scale
    mapScale = this->Levels[this->Level].tileSize;
    mapSizeGridX = this->Levels[this->Level].tileData[0].size();
    mapSizeGridY = this->Levels[this->Level].tileData.size();

    // Resize the spriteDistance based on the numbers of sprites avaiable
    this->numSprites = Levels[Level].elementsInfo.size();
    this->spriteDistance.resize(numSprites);
    this->spriteOrder.resize(numSprites);
    
    // Creates player instance                                                              Plane is perpendicular to the direction/ (0.66f) => FOV is 2 * atan(0.66/1.0)= 66° 
    Player = new PlayerObject(one.PlayerPosition, one.PlayerSize, ResourceManager::GetTexture(7), glm::vec3(1.0f, 1.0f, 0.0f), mapScale, 5.0f, glm::vec2(-1.0f, 0.0f), glm::vec2(0.0f, 0.66f), 0.2f);



    // creating a visual arrow to show the players direction
    glm::vec2 lookPos = glm::vec2(one.PlayerPosition.x + one.PlayerSize.x/4, one.PlayerPosition.y - one.PlayerSize.y*2);
    look = new GameObject(lookPos, glm::vec2(one.PlayerSize.x/2,  (one.PlayerSize.y*5)/2), ResourceManager::GetTexture(7), glm::vec3(1.0f, 1.0f, 0.0f));
    look->Pivot = glm::vec2(0.5f, 1.0f);
    look->Rotation = atan2(Player->direction.y, Player->direction.x) *  (180.0f / M_PI) + 90.0f;

    printf("DirX: %.2f  DirY: %.2f\n", Player->direction.x, Player->direction.y );

    // ============== INSTANCE FROM THE RAYCASTER =================
    RayCaster = new RayCasting(
    //==========================
    // Map Dimensions
        this->Width,
        this->Height,
    //==========================
    // Graphics options
        rayDensity,
    //==========================
    // Player Reference
        Player,
    //==========================
    // Level Reference
        &this->Levels[this->Level],
    //==========================
    // Renderers
        WallRenderer,
        FloorRenderer,
        SpRenderer,
    //==========================
    // Game Objects
        wallObj,
        floorObj,
        spriteObj,
    //==========================
    // Textures
        floorTexture
    );

}

void Game::Update(float dt)
{
    
}

void Game::ProcessInput(float dt)
{
    float velocity = Player->velocity *dt;
    float rotSpeed = Player->rotSpeed *dt;

// ========== MOVING FORWARD =============================

    if (this -> Keys[GLFW_KEY_W]) { 
        
    // Predict next position
    glm::vec2 nextPosition = glm::vec2(Player->Position.x + velocity * Player->direction.x,
                                       Player->Position.y + velocity * Player->direction.y);
    

    // Convert to map grid coords
    glm::vec2 playerGrid = glm::vec2(Player->Position.x / mapScale,
                                     Player->Position.y / mapScale);


    // Vector that checks the hit box position in the grid for the nextMove
    glm::vec2 checkPos = glm::vec2((nextPosition.x/mapScale) + (Player->direction.x > 0 ? Player->hitbox : -Player->hitbox),
                                   (nextPosition.y/mapScale) + (Player->direction.y > 0 ? Player->hitbox : -Player->hitbox));

    
    // Check on each axis if the player can move. If does, than change the player position to that axis
    // ---- X AXIS --------
    if(Levels[Level].tileData[static_cast<int>(playerGrid.y)][static_cast<int>(checkPos.x)] == 0) {
        // Apply translation based on the position of the player
        Player->Position.x = nextPosition.x;
        //Debug - Apply changes to the direction arrow 
        look->Position.x = Player->Position.x + Player->Size.x/4;    
    }

    // ---- Y AXIS --------
    if(Levels[Level].tileData[static_cast<int>(checkPos.y)][static_cast<int>(playerGrid.x)] == 0) {
        // Apply translation based on the position of the player
        Player->Position.y = nextPosition.y;
        // Debug - Apply changes to the direction arrow 
        look->Position.y = Player->Position.y - Player->Size.y*2;
   
    }
   
        //printf("X: %.2f , Y: %.2f\n", Player->Position.x/mapScale, Player->Position.y/mapScale);
        //printf("mapX: %d , mapY: %d\n", mapx, mapy);
    }

// ========== MOVING BACKWARS =============================
    if (this -> Keys[GLFW_KEY_S]) { 

    // Predict next position
        glm::vec2 nextPosition = glm::vec2(Player->Position.x - velocity * Player->direction.x,
                                           Player->Position.y - velocity * Player->direction.y);


        // Convert to map grid coords
        glm::vec2 playerGrid = glm::vec2(Player->Position.x / mapScale,
                                         Player->Position.y / mapScale);


        // Vector that checks the hit box position in the grid for the nextMove
        glm::vec2 checkPos = glm::vec2((nextPosition.x/mapScale) + (Player->direction.x < 0 ? Player->hitbox : -Player->hitbox),
                                       (nextPosition.y/mapScale) + (Player->direction.y < 0 ? Player->hitbox : -Player->hitbox));


        // Check on each axis if the player can move. If does, than change the player position to that axis
        // ---- X AXIS --------
        if(Levels[Level].tileData[static_cast<int>(playerGrid.y)][static_cast<int>(checkPos.x)] == 0) {
            // Apply translation based on the position of the player
            Player->Position.x = nextPosition.x;
            //Debug - Apply changes to the direction arrow 
            look->Position.x = Player->Position.x + Player->Size.x/4;    
        }

        // ---- Y AXIS --------
        if(Levels[Level].tileData[static_cast<int>(checkPos.y)][static_cast<int>(playerGrid.x)] == 0) {
            // Apply translation based on the position of the player
            Player->Position.y = nextPosition.y;
            // Debug - Apply changes to the direction arrow 
            look->Position.y = Player->Position.y - Player->Size.y*2;
        }

        //printf("X: %.2f , Y: %.2f\n", Player->Position.x/mapScale, Player->Position.y/mapScale);
        //printf("mapX: %d , mapY: %d\n", mapx, mapy);
       
    }
    if (this -> Keys[GLFW_KEY_A]) { 
        
        float oldDirX = Player->direction.x;
        float oldPlaneX = Player->plane.x;

      
        // Applies the rotation matrix to rotate the character
        Player->direction.x = Player->direction.x * cos(rotSpeed) -  Player->direction.y * sin(rotSpeed);
        Player->direction.y = oldDirX * sin(rotSpeed) + Player->direction.y * cos(rotSpeed);

        // Applies rotation matrix to rotate the player plane
        Player->plane.x = Player->plane.x * cos(rotSpeed) - Player->plane.y * sin(rotSpeed);
        Player->plane.y = oldPlaneX * sin(rotSpeed) + Player->plane.y * cos(rotSpeed);
        
        //printf("DirX: %.2f  DirY: %.2f\n", Player->direction.x, Player->direction.y );
        //printf(" PlaneX: %.2f  PlaneY: %.2f", Player->plane.x, Player->plane.y );
       
        // ---------- Debug - Apply changes to the direction arrow ----------------
        // It has to add 90 at the end cuz the plane is flipped
        look->Rotation = atan2(Player->direction.y, Player->direction.x) *  (180.0f / M_PI) + 90.0f;

      // printf(" angle: %.2f\n",look->Rotation);
    }
    if (this -> Keys[GLFW_KEY_D]) { // Apply rotation
        
        float oldDirX = Player->direction.x;
        float oldPlaneX = Player->plane.x;


         // Applies the rotation matrix to rotate the character
         Player->direction.x = Player->direction.x * cos(-rotSpeed) -  Player->direction.y * sin(-rotSpeed);
         Player->direction.y = oldDirX * sin(-rotSpeed) + Player->direction.y * cos(-rotSpeed);
 
         // Applies rotation matrix to rotate the player plane
         Player->plane.x = Player->plane.x * cos(-rotSpeed) - Player->plane.y * sin(-rotSpeed);
         Player->plane.y = oldPlaneX * sin(-rotSpeed) + Player->plane.y * cos(-rotSpeed);
        
        
         //printf("DirX: %.2f  DirY: %.2f\n", Player->direction.x, Player->direction.y );
        //printf(" PlaneX: %.2f  PlaneY: %.2f", Player->plane.x, Player->plane.y );

        // ---------- Debug - Apply changes to the direction arrow ----------------
        // It has to add 90 at the end cuz the plane is flipped
        look->Rotation = atan2(Player->direction.y, Player->direction.x) *  (180.0f / M_PI) + 90.0f;

       // printf(" angle: %.2f\n",look->Rotation);
    }

    // Sprint Key
    if(this->Keys[GLFW_KEY_LEFT_SHIFT]) {
        // Sprints when the key is pressed
        if(!Player->isRunning) {

            //printf("Pressionou\n");
            Player->velocity = Player->velocity * 2.0f;
            Player->isRunning = true;
        }
    }

    // Stops to sprint when the key is released once
    if(!this->Keys[GLFW_KEY_LEFT_SHIFT] && Player->isRunning) {
      // printf("Soltou\n");
       Player->velocity = Player->velocity/2.0f;
       Player->isRunning = false;
    }

    // Show the Key Chart
    if(this->Keys[GLFW_KEY_TAB]) {

        if(!this->keyChartOn) keyChartOn = true;
        std::cout << keyChartOn << std::endl;

    }
    // Hide Key Chart when the key is released
    if(!this->Keys[GLFW_KEY_TAB] && keyChartOn) {
        keyChartOn = false;
        std::cout << keyChartOn << std::endl;
    }

}

void Game::Render()
{
   
    // Draw Level Map in the first half of the screen

    RayCaster->FloorCeilingCasting();
    RayCaster->WallCasting(this->ZBuffer);
    RayCaster->SpriteCasting(this->ZBuffer);
    this->Levels[this->Level].DrawMap(*MapRenderer);
    Player->Draw(*MapRenderer);
    look->Draw(*MapRenderer);
    
}


