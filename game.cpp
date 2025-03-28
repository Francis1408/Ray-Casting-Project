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



// Player stats
PlayerObject *Player;

GameObject  *look;
GameObject  *wall;

//Scale of the level map in the grid size
float mapScale; 

// RayDensity = How thick is each wall slice
int rayDensity = 1;


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
    GameLevel one; one.Load("Levels/two.lvl", "Levels/one.ele", this->Width/2, this->Height);
    this->Levels.push_back(one);
    this->Level = 0;

    // Creates player instance                                                              Plane is perpendicular to the direction/ (0.66f) => FOV is 2 * atan(0.66/1.0)= 66° 
    Player = new PlayerObject(one.PlayerPosition, one.PlayerSize, glm::vec3(1.0f, 1.0f, 0.0f), 50.0f, 5.0f, glm::vec2(-1.0f, 0.0f), glm::vec2(0.0f, 0.66f));

    // Set the map Scale
    mapScale = this->Levels[this->Level].tileSize;


    // creating a visual arrow to show the players direction
    glm::vec2 lookPos = glm::vec2(one.PlayerPosition.x + one.PlayerSize.x/4, one.PlayerPosition.y - one.PlayerSize.y*2);
    look = new GameObject(lookPos, glm::vec2(one.PlayerSize.x/2,  (one.PlayerSize.y*5)/2),glm::vec3(1.0f, 1.0f, 0.0f));
    look->Pivot = glm::vec2(0.5f, 1.0f);
    look->Rotation = atan2(Player->direction.y, Player->direction.x) *  (180.0f / M_PI) + 90.0f;

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
        
        
    // Apply translation based on the position of the player
        Player->Position.x += velocity * Player->direction.x;
        Player->Position.y += velocity * Player->direction.y;

        
    // ---------- Debug - Apply changes to the direction arrow ----------------
        look->Position.x = Player->Position.x + Player->Size.x/4;
        look->Position.y = Player->Position.y - Player->Size.y*2;


        //printf("X: %.2f , Y: %.2f\n", Player->Position.x, Player->Position.y);
        //printf("mapX: %d , mapY: %d\n", mapx, mapy);
    }
    if (this -> Keys[GLFW_KEY_S]) { 
        
    // Apply translation based on the position of the player
        Player->Position.x -= velocity * Player->direction.x;
        Player->Position.y -= velocity * Player->direction.y;


    // ---------- Debug - Apply changes to the direction arrow ----------------
        look->Position.x = Player->Position.x + Player->Size.x/4;
        look->Position.y = Player->Position.y - Player->Size.y*2;
        

       // printf("X: %.2f , Y: %.2f\n", Player->Position.x, Player->Position.y);
       // printf("mapX: %d, mapY: %d\n", mapx, mapy);
       
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
        
       // printf("DirX: %.2f  DirY: %.2f", Player->direction.x, Player->direction.y );
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
        
        
        //printf("DirX: %.2f  DirY: %.2f", Player->direction.x, Player->direction.y );
        //printf(" PlaneX: %.2f  PlaneY: %.2f", Player->plane.x, Player->plane.y );

        // ---------- Debug - Apply changes to the direction arrow ----------------
        // It has to add 90 at the end cuz the plane is flipped
        look->Rotation = atan2(Player->direction.y, Player->direction.x) *  (180.0f / M_PI) + 90.0f;

       // printf(" angle: %.2f\n",look->Rotation);
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
    // Each interation creates a ray which are distributed throught the plane(screen) space;
    // Our screen is split in half
    for(int x = 0; x < Width/2; x+= rayDensity) {
        // calculate ray position and direction
        
        /*
        cameraX is the x-coordinate on the camera plane that the current x-coordinate of the screen represents, 
        done this way so that the right side of the screen will get coordinate 1, the center of the screen gets coordinate 0, 
        and the left side of the screen gets coordinate -1.
        */

        float cameraX = 2 * x / static_cast<float>(Width/2) - 1; //gets the x-coordinate of the ray in the
        glm::vec2 rayDir = glm::vec2(Player->direction.x + Player->plane.x * cameraX, Player->direction.y + Player->plane.y * cameraX);
        
        // The amount of units to the ray hit the next tile
        glm::vec2 deltaDist = glm::vec2(std::abs(1/rayDir.x), std::abs(1/rayDir.y));


        //Which box of the map we're in
        int mapx = ((static_cast<int>(Player->Position.x/mapScale)));
        int mapy = ((static_cast<int>(Player->Position.y/mapScale)));
       
        //length of ray from current position to next x or y-side
        float sideDistX;
        float sideDistY;

        /*
        length of ray from one x or y-side to next x or y-side
        these are derived as:
        deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX))
        deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY))
        which can be simplified to abs(|rayDir| / rayDirX) and abs(|rayDir| / rayDirY)
        where |rayDir| is the length of the vector (rayDirX, rayDirY). Its length,
        unlike (dirX, dirY) is not 1, however this does not matter, only the
        ratio between deltaDistX and deltaDistY matters, due to the way the DDA
        stepping further below works
        */
        float deltaDistX = std::abs(1/rayDir.x);
        float deltaDistY = std::abs(1/rayDir.y);

        //what direction to step in x or y-direction (either +1 or -1)
        /*
        stepX: 
            - +1 if the ray is moving right (positive X direction).
            - -1 if the ray is moving left (negative X direction).

        stepY:
            - +1 if the ray is moving down (positive Y direction).
            - -1 if the ray is moving up (negative Y direction).

        */
        int stepX;
        int stepY;
        
        // calculate step and initial sideDist
        if(rayDir.x < 0) { // negative
            stepX = -1; // Moving left
            sideDistX = ((Player->Position.x/mapScale) - mapx) * deltaDistX; // The difference gives the distance to the previous vertical grid line.

        } else { // positive
            stepX = 1; // Moving right
            sideDistX = (mapx + 1 - (Player->Position.x/mapScale)) * deltaDistX; // mapx + 1.0 is the next vertical grid line on the right
        }

        if(rayDir.y < 0) { // negative
            stepY = -1;
            sideDistY = ((Player->Position.y/mapScale) - mapy) * deltaDistY; // The difference gives the distance to the previous grid line
        }
        else { // positive
            stepY = 1;
            sideDistY = (mapy + 1 - (Player->Position.y/mapScale)) * deltaDistY; // mapY + 1.0 is the next horizontal grid line below
        }

        int hit = 0; // Was there a wall hit?
        int side; // It hitted vertically or horizontally? - Used to apply the shadowing


        // Peforms de DDA
        while(hit == 0) {
            //jump to next map square, either in x-direction, or in y-direction
            if(sideDistX < sideDistY) {

                sideDistX += deltaDistX;
                mapx += stepX; // Moves to the following tile
                side = 0; // It hitted vertically
            }
            else {
                sideDistY += deltaDistY;
                mapy += stepY;
                side = 1;
            }
            
            //Check if ray has hit a wall
            if(this->Levels[this->Level].tileData[mapx][mapy]) hit = 1;
        }

        /*
        Calculate distance projected on camera direction. This is the shortest distance from the point where the wall is
        hit to the camera plane. Euclidean to center camera point would give fisheye effect!
        This can be computed as (mapX - posX + (1 - stepX) / 2) / rayDirX for side == 0, or same formula with Y
        for size == 1, but can be simplified to the code below thanks to how sideDist and deltaDist are computed:
        because they were left scaled to |rayDir|. sideDist is the entire length of the ray above after the multiple
        steps, but we subtract deltaDist once because one step more into the wall was taken above.
        */

        float perpWallDistance;

        if(side == 0) perpWallDistance = (sideDistX - deltaDistX) * mapScale;  // Goes one step back
        else          perpWallDistance = (sideDistY - deltaDistY) * mapScale; // Goes one step back

        //calculate lowest and highest pixel to fill in current stripe
        float lineHeight = (Height/perpWallDistance) * mapScale;

        //calculate lowest and highest pixel to fill in current stripe
        float drawStart = -lineHeight / 2 + Height / 2;
        if(drawStart < 0) drawStart = 0;
        float drawEnd = lineHeight / 2 + Height / 2;
        if(drawEnd >= Height) drawEnd = Height - 1;

        // Pick the wall color
        glm::vec3 color;

        switch (this->Levels[this->Level].tileData[mapx][mapy])
        {
        case 1: color = glm::vec3(1.0f, 1.0f, 1.0f); break; // White
        case 2: color = glm::vec3(0.0f, 0.0f, 1.0f); break; // Blue
        case 3: color = glm::vec3(1.0f, 0.0f, 0.0f); break; // Red
        case 4: color = glm::vec3(0.0f, 1.0f, 0.0f); break; // Green
        case 5: color = glm::vec3(1.0f, 1.0f, 0.0f); break; // Yellow
        
        default:  color = glm::vec3(1.0f, 1.0f, 1.0f); break; // White
        }

        // Aplying shading for the walls
        if(side == 1) color = color * glm::vec3(0.5f, 0.5f, 0.5f);

        // Create a gameObject to draw on the screen

        // X + Width/2 = Starting X-coordinate
        // drawStart = Y Starting coordinate 
        // Size = (Density of the ray = 1 pixel, drawEnd - drawStart)

       wall = new GameObject(glm::vec2(x+ Width/2, drawStart), glm::vec2(rayDensity, drawEnd - drawStart),color);

        // Draw wall slice
       wall->Draw(*Renderer);

    }
}
    
