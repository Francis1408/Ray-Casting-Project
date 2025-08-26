#include "rayCasting.h"
#include <algorithm>


RayCasting::RayCasting(
    unsigned int screenWidth,
    unsigned int screenHeight,
    unsigned int rayDensity,
    PlayerObject* player,
    GameLevel* level,
    SpriteRenderer* wallRenderer,
    SpriteRenderer* floorRenderer,
    SpriteRenderer* spriteRenderer,
    GameObject* wallObj,
    GameObject* floorObj,
    GameObject* spriteObj,
    Texture2D* floorTexture
)
: Width(screenWidth), Height(screenHeight), rayDensity(rayDensity),
  Player(player), Level(level),
  WallRenderer(wallRenderer), FloorRenderer(floorRenderer), SpRenderer(spriteRenderer),
  wallObj(wallObj), floorObj(floorObj), spriteObj(spriteObj), floorTexture(floorTexture)
{
    
    // Define the level scale based on the map size
    mapScale = Level->tileSize;
    mapSizeGridX = Level->tileData[0].size();
    mapSizeGridY = Level->tileData.size();

    // Resize the spriteDistance based on the numbers of sprites avaiable
    numSprites = Level->elementsInfo.size();
    spriteDistance.resize(numSprites);
    spriteOrder.resize(numSprites);
}

// ===================== WALL CASTING ALGORRITHM =====================

void RayCasting::WallCasting(std::vector<float>& zBuffer) {
 
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
            if(Level->tileData[mapy][mapx]) hit = 1; 
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
        currentTexture = Level->tileInfo[mapy][mapx].Sprite;

        
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

        // x + Width/2 = Starting X-coordinate
        // drawStart = Y Starting coordinate 
        // Size = (Density of the ray = 1 pixel, drawEnd - drawStart)

        
        wallObj->Position = glm::vec2(x+ Width/2, drawStart);
        wallObj->Size = glm::vec2(this->rayDensity, drawEnd - drawStart);
        wallObj->Sprite = currentTexture;
        wallObj->Color = color;
        

        // Draw wall slice
        wallObj->Draw(*WallRenderer);

        zBuffer[x] = perpWallDistance;
        //std::cout << perpWallDistance << std::endl;

    }

    ResourceManager::GetShader("sprite").Use().SetVec1("ZBuffer", zBuffer.data(), Width/2);
      

}

// ===================== FLOOR AND CEILING CASTING ALGORRITHM =====================
void RayCasting::FloorCeilingCasting() {

    // Texture2D floorBuffer = ResourceManager::GetTexture(7);
   //  Texture2D ceilingBuffer = ResourceManager::GetTexture(7);

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

        // Real world coordinates in the grid of the leftmost column. This will be updated as we step to the right.
        glm::vec2 floor = glm::vec2(mapPosition.x + rowDistance * rayDirLeft.x, mapPosition.y + rowDistance * rayDirLeft.y);
        
        // Builds the buffer that will carry the new texture to render
        for(int x = 0; x < Width/2; x++) {
            
            // Integer parts of the floor current position in the grid map
            int cellX = (int)(floor.x); 
            int cellY = (int)(floor.y);

            // Make a ground check because the ray can scan coordinates out of bounds
            if(cellX >= 0 && cellX < mapSizeGridX &&
            cellY >= 0 && cellY < mapSizeGridY) {
 

                Texture2D& floorTex = Level->floorInfo[cellY][cellX].Sprite;
                Texture2D& ceilTex  = Level->ceilingInfo[cellY][cellX].Sprite;
                
                
                // .f part of the floor current position
                glm::vec2 fractional = glm::vec2(floor.x - cellX, floor.y - cellY);
                
                // Gets the exact pixel coodinate in the texture based on the floor position
                int texX = (int)(floorTex.Width * fractional.x) & (floorTex.Width -1); // Bitmask when texture width is power of two
                int texY = (int)(floorTex.Height * fractional.y) & (floorTex.Height- 1); // Bitmask when texture heght is power of two
                
                // std::cout << cellX << " " << cellY << std::endl;
                
                // Convert the pixel cordinate into the pixel position in the buffer array
                int texIndex = (texY * floorTex.Width + texX) * 3; // 3 bytes per pixel (RGB)
                
                // Gets the index of the pixel based on the screen coodinate
                int screenIndexFloor = (y * (Width/2) + x) * 3;
                int screenIndexCeiling = ((Height - y) * (Width/2) + x ) * 3;


                // Write each pixel to the buffer -> Each pixel contains an RGB value
                // Buffer part for the floor
                pixelBuffer[screenIndexFloor + 0] = floorTex.PixelBuffer[texIndex + 0]; // Red
                pixelBuffer[screenIndexFloor + 1] = floorTex.PixelBuffer[texIndex + 1]; // Green
                pixelBuffer[screenIndexFloor + 2] = floorTex.PixelBuffer[texIndex + 2]; // Blue
                
                // Buffer part for the ceiling
                pixelBuffer[screenIndexCeiling + 0] = ceilTex.PixelBuffer[texIndex + 0]; // Red
                pixelBuffer[screenIndexCeiling + 1] = ceilTex.PixelBuffer[texIndex + 1]; // Green    
                pixelBuffer[screenIndexCeiling + 2] = ceilTex.PixelBuffer[texIndex + 2]; // Blue
                
                
            }
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

// ===================== SPRITE CASTING ALGORRITHM =====================
void RayCasting::SpriteCasting(std::vector<float>& zBuffer) {

    // Sort the sprites based on distance and save it on a distance array
    for(int i = 0; i < numSprites; i++) {

        glm::vec2 spritePos = glm::vec2(Level->elementsInfo[i].Position.x, Level->elementsInfo[i].Position.y);

        // Assing an ID to the sprite
        spriteOrder[i] = i;
        // Calculates sprite distance relative to the player
        spriteDistance[i] = ((Player->Position.x - spritePos.x) * (Player->Position.x - spritePos.x) +
                            (Player->Position.y - spritePos.y) * (Player->Position.y - spritePos.y));
                            
    }

    // Calls the sort sprite method
    SortSprites();

    
    // Converts the sprite coordinates in the view space (relative to the camera)
    for(int i = 0; i < numSprites; i++) {
        
    // Translate sprite position to relative to camera
    glm::vec2 spriteCoord = glm::vec2((Level->elementsInfo[spriteOrder[i]].Position.x/mapScale) - (Player->Position.x/mapScale),
    (Level->elementsInfo[spriteOrder[i]].Position.y/mapScale) - (Player->Position.y/mapScale));
    
    //transform sprite with the inverse camera matrix
    // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
    // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
    // [ planeY   dirY ]                                          [ -planeY  planeX ]
    float invDet = 1.0f / (Player->plane.x * Player->direction.y - Player->direction.x * Player->plane.y);
    
    
    // spriteTransform.x = Where the sprite appears horizontally relative to the camera(left or right)
    // spriteTransform.y = how far away the sprite is (depth)
    glm::vec2 spriteTransform = glm::vec2(invDet * (Player->direction.y * spriteCoord.x - Player->direction.x * spriteCoord.y),
                                          invDet * (-Player->plane.y * spriteCoord.x + Player->plane.x * spriteCoord.y));   
    
    // Computes the sprite's camera-space X coordinate to the 2D screen
    // The width is divided by 4 because we use only half of the screen
    int spriteScreenX = static_cast<int>( ((Width/4) * (1 + spriteTransform.x/ spriteTransform.y) + (Width/2)));
    //std::cout << spriteTransform.x << std::endl;
    // Calculates the height and width of the sprite on screen
    // As the transformY gets bigger, smaller will the the sprite
    int spriteHeight = abs(static_cast<int>(Height/(spriteTransform.y)));
    int spriteWidth = abs(static_cast<int>(Height/(spriteTransform.y)));
    // Gets the drawing coordinates
    
    // Original drawing coord without clamping
    float originalStartX = -spriteWidth/2 + spriteScreenX;
    float originalEndX = spriteWidth/2 + spriteScreenX;

    // Creates the drawing coord vectors
    glm::vec2 drawStart = glm::vec2(originalStartX, -spriteHeight/2 + Height/2);
    glm::vec2 drawEnd = glm::vec2(originalEndX, spriteHeight/2 + Height/2);
    
    // Clamp the horizontal drawing
    drawStart.x = std::max(drawStart.x, static_cast<float>(Width/2));
    drawEnd.x = std::min(drawEnd.x, static_cast<float>(Width)); 

    // ============ TEXTURE HANDLING ==============

    // Calculate the portion of the texture which need to be drawn
    // Normalized texX coordinate range
    // uv_coord_start = percentace of where the drawing tex should start
    // uv_coord_end = percentace of where the drawing tex should end
    float uv_coord_start = (drawStart.x - originalStartX) / (originalEndX - originalStartX);
    float uv_coord_end = (drawEnd.x - originalStartX) / (originalEndX - originalStartX); 
    
    // 
    ResourceManager::GetShader("sprite").Use().SetFloat("u_start", uv_coord_start);
    ResourceManager::GetShader("sprite").Use().SetFloat("u_end", uv_coord_end);


    // If the sprite is not behind the player
    if(spriteTransform.y > 0) {

        //std::cout << "Sprite: "<<spriteTransform.y << std::endl;

        // Pass the transformY to the fragment shader
        ResourceManager::GetShader("sprite").Use().SetFloat("spriteDepth", spriteTransform.y);
        
        // Set the Quad dimensions

        // Create a gameObject to draw on the screen
    
        // X + Width/2 = Starting X-coordinate
        // drawStart = Y Starting coordinate 
        // Size = (Density of the ray = 1 pixel, drawEnd - drawStart)
        spriteObj->Position = drawStart; 
        spriteObj->Size =  drawEnd - drawStart;
        spriteObj->Sprite = Level->elementsInfo[spriteOrder[i]].Sprite;
        spriteObj->Color = Level->elementsInfo[spriteOrder[i]].Color;
        
        spriteObj->Draw(*SpRenderer);
    }
        
    }

}

void RayCasting::SortSprites() {

    // Joins the sprite distance with its ID
    std::vector<std::pair<float, int>> sprites(numSprites);
    for(int i = 0; i < numSprites; i++) {
        sprites[i].first = spriteDistance[i];
        sprites[i].second = spriteOrder[i];
    }

    // Sorts the distances
    std::sort(sprites.begin(), sprites.end());

    // restore in reverse order to go from farthest to nearest
    for(int i = 0; i < numSprites; i++) {
        spriteDistance[i] = sprites[numSprites - i - 1].first;
        spriteOrder[i] = sprites[numSprites - i - 1].second;
      }

}