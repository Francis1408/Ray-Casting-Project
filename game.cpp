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

SpriteRenderer *WallRenderer;
SpriteRenderer *FloorRenderer;


// Player stats
PlayerObject *Player;

GameObject  *look;
GameObject  *wallObj;
GameObject  *floorObj;
Texture2D   *floorTexture;

//Scale of the level map in the grid size
float mapScale; 

// RayDensity = How thick is each wall slice
int rayDensity = 2;


Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete WallRenderer;
    delete FloorRenderer;
    delete Player;
    delete look;
    delete wallObj;
    delete floorObj;

    delete floorTexture;
    floorTexture = nullptr;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("Shaders/shaderCoordinate.vs", "Shaders/shaderWall.fs", nullptr, "wall");
    ResourceManager::LoadShader("Shaders/shaderCoordinate.vs", "Shaders/shaderFloor.fs", nullptr, "floor");
    
   // Define the View Matrix
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
   
   
   // Set the uniform values on each shader    
   ResourceManager::GetShader("wall").Use().SetInt("image", 0);
   ResourceManager::GetShader("wall").SetMat4("projection", projection);
   ResourceManager::GetShader("floor").Use().SetInt("image", 0);
   ResourceManager::GetShader("floor").SetMat4("projection", projection);
   
   // Set render-specific controls
   Shader Shader = ResourceManager::GetShader("wall");
   WallRenderer = new SpriteRenderer(Shader);
   
   Shader = ResourceManager::GetShader("floor");
   FloorRenderer = new SpriteRenderer(Shader);
   
   
   // =================== Load textures ========================================
   
   ResourceManager::LoadTextures("Textures/");
   
   floorTexture = new Texture2D();
   
   // Initialize GameObjects
   wallObj = new GameObject();
   floorObj = new GameObject();
   
   // load levels
   GameLevel one; one.Load("Levels/two.lvl", "Levels/one.flo", "Levels/one.cel", "Levels/one.ele",  this->Width/2, this->Height);
   this->Levels.push_back(one);
   this->Level = 0;
   
   // Set the map Scale
    mapScale = this->Levels[this->Level].tileSize;
    
    // Creates player instance                                                              Plane is perpendicular to the direction/ (0.66f) => FOV is 2 * atan(0.66/1.0)= 66° 
    Player = new PlayerObject(one.PlayerPosition, one.PlayerSize, ResourceManager::GetTexture(7), glm::vec3(1.0f, 1.0f, 0.0f), mapScale, 5.0f, glm::vec2(-1.0f, 0.0f), glm::vec2(0.0f, 0.66f));



    // creating a visual arrow to show the players direction
    glm::vec2 lookPos = glm::vec2(one.PlayerPosition.x + one.PlayerSize.x/4, one.PlayerPosition.y - one.PlayerSize.y*2);
    look = new GameObject(lookPos, glm::vec2(one.PlayerSize.x/2,  (one.PlayerSize.y*5)/2), ResourceManager::GetTexture(7), glm::vec3(1.0f, 1.0f, 0.0f));
    look->Pivot = glm::vec2(0.5f, 1.0f);
    look->Rotation = atan2(Player->direction.y, Player->direction.x) *  (180.0f / M_PI) + 90.0f;

    printf("DirX: %.2f  DirY: %.2f\n", Player->direction.x, Player->direction.y );

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

}

void Game::Render()
{
   
    // Draw Level Map in the first half of the screen


   // Texture2D myTexture;
   // myTexture = ResourceManager::GetTexture("eagle");
   // Renderer->DrawSprite(myTexture,glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
   // this->Levels[this->Level].Draw(*Renderer);
   // Player->Draw(*Renderer);
   // look->Draw(*Renderer);
    FloorCasting();
    RayCasting();

   
    
}


 void Game::RayCasting() {
// ===================== RAYCASTING ALGORRITHM =====================

    
    // Set a variable to store the texture
    Texture2D currentTexture;
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
            
            // THE ORIGINAL COORDINATES ARE FLIPPED, SO THE Y-AXIS IS IN THE TILE DATA WIDTH AND MAPX IN THE TILE DATA HEIGHT
            //Check if ray has hit a wall
            if(this->Levels[this->Level].tileData[mapy][mapx]) hit = 1; 
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

        if(side == 0) perpWallDistance = (sideDistX - deltaDistX);  // Goes one step back
        else          perpWallDistance = (sideDistY - deltaDistY); // Goes one step back

        //calculate lowest and highest pixel to fill in current stripe
        float lineHeight = (Height/(perpWallDistance));

        //calculate lowest and highest pixel to fill in current stripe
        float drawStart = -lineHeight / 2 + Height / 2;
        float drawEnd = lineHeight / 2 + Height / 2;

        // By commenting these two lines, the wall will be drawn outside the screen
        // However was the fastest way that I found to fix the texture Y coordinate problem
        // I Hope that this doesn't cause any problems in the future
        // if(drawStart < 0) drawStart = 0;
        // if(drawEnd >= Height) drawEnd = Height - 1;

        
        // =============== TEXTURING HANDLING ==================
        
        // Load the texture from the tile
        currentTexture = this->Levels[this->Level].tileInfo[mapy][mapx].Sprite;

        
        float step = 1.0f * currentTexture.Height / lineHeight; // The step to take in the texture
        // Pick the wall color
        glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);


        // Calculate the position of the ray referenced to the wall (player position + raydist*distance offset)]
        float wallX; // where exactly the wall was hit
        if(side == 0) wallX = (Player->Position.y/mapScale) + perpWallDistance * rayDir.y;
        else          wallX = (Player->Position.x/mapScale) + perpWallDistance * rayDir.x;

        wallX -= floor(wallX); // Lower approx of the wall position

        // x coordinate on the texture
        float texX = wallX * static_cast<float>(currentTexture.Width);

        // Corrects the flipping textures
        //if(side == 0 && rayDir.x > 0) texX = static_cast<float>(mytexture.Width) - texX - 1.0f;
        //if(side == 1 && rayDir.x < 0) texX = static_cast<float>(mytexture.Width) - texX - 1.0f;

        float texXNormalized = texX/static_cast<float>(currentTexture.Width);


        // Sets the uniform to draw only the pre defined slice
        ResourceManager::GetShader("wall").Use().SetFloat("texXOffset", texXNormalized);

        // Create shading
        if(side == 1) color = glm::vec3(0.5f, 0.5f, 0.5f);

   
    
        // Create a gameObject to draw on the screen

        // X + Width/2 = Starting X-coordinate
        // drawStart = Y Starting coordinate 
        // Size = (Density of the ray = 1 pixel, drawEnd - drawStart)

        
        wallObj->Position = glm::vec2(x+ Width/2, drawStart);
        wallObj->Size = glm::vec2(rayDensity, drawEnd - drawStart);
        wallObj->Sprite = currentTexture;
        wallObj->Color = color;
        

        // Draw wall slice
      wallObj->Draw(*WallRenderer);
        
    }
    
}

void Game::FloorCasting() {

    Texture2D myTexture = ResourceManager::GetTexture(4);
    Texture2D myTexture2 = ResourceManager::GetTexture(7);

    // Buffer that will store the custom texture for the floor
    std::vector<unsigned char> pixelBuffer(Width * Height * 3); // RGB

    for(int y = Height/2; y < Height; y+= rayDensity) { // Mid to bottom of the screen

        // Calculate the directions from the extreme rays
        // Leftmost ray (x = 0) and rightmost ray ( x = w/2)
        glm::vec2 rayDirLeft = glm::vec2(Player->direction.x - Player->plane.x, Player->direction.y - Player->plane.y);
        glm::vec2 rayDirRight = glm::vec2(Player->direction.x + Player->plane.x, Player->direction.y + Player->plane.y);

        // Current Y position of the ray compared to the center of the screen
        int p = y - Height/2;

        // Vertical position of the camera in pixels
        float posZ = 0.5 * Height; // Camera on the center of the screen
        /*
        Horizontal distance from the camera to the floor. Based on triangle equivalence:
        rowDistance/1 = posZ/p;  Since the distance from the camera to the screen is always 1
        */
        float rowDistance = posZ/p;

       // std::cout << rowDistance << std::endl;

        // calculate the real world step vector we have to add for each x (parallel to camera plane)
        // By multiplying them by the rendering range (screen size), we get the rightmost coordinate from the slice
        //Apllies the same for y-axis when the player POV is tilted sideways
        glm::vec2 floorStep = glm::vec2(rowDistance * (rayDirRight.x - rayDirLeft.x) / (Width/2), rowDistance * (rayDirRight.y - rayDirLeft.y) / (Width/2));

        glm::vec2 mapPosition = glm::vec2(Player->Position.x/mapScale, Player->Position.y/mapScale); 
        // Real world coordinates of the leftmost column. This will be updated as we step to the right.
        glm::vec2 floor = glm::vec2(mapPosition.x + rowDistance * rayDirLeft.x, mapPosition.y + rowDistance * rayDirLeft.y);
        
        // Builds the buffer that will carry the new texture to render
        for(int x = 0; x < Width/2; x++) {
            
            // Integer parts of the floor current position
            int cellX = (int)(floor.x); 
            int cellY = (int)(floor.y);



            // .f part of the floor current position
            glm::vec2 fractional = glm::vec2(floor.x - cellX, floor.y - cellY);

            // Gets the exact pixel coodinate in the texture based on the floor position
            int texX = (int)(myTexture.Width * fractional.x) & (myTexture.Width -1); // Bitmask when texture width is power of two
            int texY = (int)(myTexture.Height * fractional.y) & (myTexture.Height - 1); // Bitmask when texture heght is power of two

            //std::cout << texX << " " << texY << std::endl;
            
            // Convert the pixel cordinate into the pixel position in the buffer array
            int texIndex = (texY * myTexture.Width + texX) * 3; // 3 bytes per pixel (RGB)

            // Gets the index of the pixel based on the screen coodinate
            int screenIndexFloor = (y * (Width/2) + x) * 3;
            int screenIndexCeiling = ((Height - y) * (Width/2) + x ) * 3;

           // this->Levels[this->Level].tileInfo[mapy][mapx].Sprite;

           // myTexture = this->Levels[]

            // Write each pixel to the buffer -> Each pixel contains an RGB value
            // Buffer part for the floor
            pixelBuffer[screenIndexFloor + 0] = myTexture.PixelBuffer[texIndex + 0]; // Red
            pixelBuffer[screenIndexFloor + 1] = myTexture.PixelBuffer[texIndex + 1]; // Green
            pixelBuffer[screenIndexFloor + 2] = myTexture.PixelBuffer[texIndex + 2]; // Blue

            // Buffer part for the ceiling
            pixelBuffer[screenIndexCeiling + 0] = myTexture2.PixelBuffer[texIndex + 0]; // Red
            pixelBuffer[screenIndexCeiling + 1] = myTexture2.PixelBuffer[texIndex + 1]; // Green    
            pixelBuffer[screenIndexCeiling + 2] = myTexture2.PixelBuffer[texIndex + 2]; // Blue
            

            // Goes to the next corresponding pixel based on the player's POV positions
            floor.x += floorStep.x;
            floor.y += floorStep.y;
        }


    }

    // Calls Generate only once in order to avoid memory leaking
    if(!floorTexture->IsInitialized) {

        floorTexture->Generate(Width, Height, pixelBuffer.data());

    // From this point and then, only updates the buffer inside the floorTexture pointer
    } else {
        floorTexture->Update(pixelBuffer.data());
    }
    

    floorObj->Position = glm::vec2(Width/2, 0);
    floorObj->Size = glm::vec2(Width, 2*Height);
    floorObj->Sprite = *floorTexture;
    floorObj->Color = glm::vec3(0.5f, 0.5f, 0.5f);
    
    floorObj->Draw(*FloorRenderer);
}
