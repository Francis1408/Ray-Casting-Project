
#include "gameLevel.h"
#include <fstream>
#include <sstream>
#include <iostream>

// The size proportion from the elements compared to the walls
const int PLAYER_SIZE_OFFSET = 8; // The player is 8x smaller than the walls
const int ELEMENTS_SIZE_OFFSET = 1; // The elements are 8x smaller than the walls


void GameLevel::Load(const char *mapFile, const char *floorFile, const char *ceilingFile, const char  *elementFile, 
                     unsigned int screenWidth, unsigned int screenHeight)
{
    // clear old data
    this->tileInfo.clear();
    this->elementsInfo.clear();
    this->floorInfo.clear();
    this->ceilingInfo.clear();

    // ========================= LOAD MAP FILE =======================
    unsigned int tileCode;
    std::string line;
    std::ifstream fstream(mapFile);
    this->tileData;
    this->tileInfo;

    if (fstream)
    {
        while (std::getline(fstream, line)) // read each line from level mapFile
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            std::vector<GameObject> tileRow;
            while (sstream >> tileCode) {
                row.push_back(tileCode);
                tileRow.push_back(GameObject()); // Create empty gameObjects
            } // read each word separated by spaces
            this->tileData.push_back(row);
            this->tileInfo.push_back(tileRow);
        }
    }

    // ================= LOAD ELEMENT FILE =================================
    // IN THE ELEMENT FILE, THE FIRST LINE IS ALWAYS THE PLAYER
    // THE FOLLOWING LINES ARE THE SPRITES DISTRIBUTED AROUND THE MAP
    unsigned int elementCode;
    std::string line2;
    std::ifstream fstream2(elementFile);
    if (fstream2)
    {
        while (std::getline(fstream2, line2)) // read each line from level elementFile
        {
            std::istringstream sstream(line2);
            std::vector<unsigned int> row;
            while (sstream >> elementCode) // read each word separated by spaces
                row.push_back(elementCode);
            this->elementData.push_back(row); // Gets the position and sprite type information from each element
            this->elementsInfo.push_back(GameObject()); // Create an empty gameObject for each element
        }

        // Remove one GameObject since the first line (player) does not count as an sprite element
        this->elementsInfo.pop_back();

    }

    // ================= LOAD FLOOR FILE =================================
    unsigned int floorCode;
    std::string line3;
    std::ifstream fstream3(floorFile);
    if (fstream3)
    {
        while (std::getline(fstream3, line3)) // read each line from level floorFile
        {
            std::istringstream sstream(line3);
            std::vector<unsigned int> row;
            std::vector<GameObject> floorRow;
            while (sstream >> floorCode) {
                row.push_back(floorCode);
                floorRow.push_back(GameObject()); // Create empty gameObjects
            } // read each word separated by spaces
            this->floorData.push_back(row);
            this->floorInfo.push_back(floorRow);
        }
    }

     // ================= LOAD CEILING FILE =================================
     unsigned int ceilingCode;
     std::string line4;
     std::ifstream fstream4(ceilingFile);
     if (fstream4)
     {
         while (std::getline(fstream4, line4)) // read each line from level ceilingFile
         {
             std::istringstream sstream(line4);
             std::vector<unsigned int> row;
             std::vector<GameObject> ceilingRow;
             while (sstream >> floorCode) {
                 row.push_back(floorCode);
                 ceilingRow.push_back(GameObject()); // Create empty gameObjects
             } // read each word separated by spaces
             this->ceilingData.push_back(row);
             this->ceilingInfo.push_back(ceilingRow);
         }
     }




    if (this->tileData.size() > 0 && this->elementData.size() > 0 && 
        this->floorData.size() == this->tileData.size() && this->ceilingData.size() == this->floorData.size()) // All the 3 map builder files must be the same size
    {

        this->init(screenWidth, screenHeight);
    }
    else 
    {
        throw std::runtime_error("Map data size mismatch: tileData, floorData, and ceilingData must all have the same size");
    }

     

}

void GameLevel::DrawMap(SpriteRenderer &renderer)
{
    // Draw the map once at the left
    for (auto &row : this->tileInfo) {
        for(GameObject& tile : row) {
            tile.Draw(renderer);
        }
    }
}


void GameLevel::init(unsigned int screenWidth, unsigned int screenHeight)
{

   
    // The map is a square, so the width is the same as the height
    unsigned int mapWidth  = this->tileData[0].size();
    unsigned int mapHeight = this->tileData.size();


    // Size of the walls in the map
    float unit_width = screenWidth / static_cast<float>(mapWidth);
    float unit_height = screenHeight / static_cast<float>(mapHeight);

    this->tileSize = unit_width; // Set the proportion size of the walls

    //printf("%f", unit_width);
    //printf("%f", unit_height);

    // read throught the array of tile data
    for( int i = 0;i < mapHeight; i++)
    {
        for(int j = 0; j < mapWidth; j++)
        {

            // Assign the floor/ceiling tiles with the matching texture
                Texture2D pickedTexture;
                // Define the floor texture
                pickedTexture = ResourceManager::GetTexture(this->floorData[i][j]);
                this->floorInfo[i][j].Sprite = pickedTexture;
                this->floorInfo[i][j].IsSolid = true; // Save the floor info
                   
                   
                // Define the ceiling texure
                pickedTexture = ResourceManager::GetTexture(this->ceilingData[i][j]);
                this->ceilingInfo[i][j].Sprite = pickedTexture;
                this->ceilingInfo[i][j].IsSolid = true; // Save the ceiling info
                   
                if(this->tileData[i][j] >= 1)
                {
                  
                    if(this->tileData[i][j] <= ResourceManager::Textures.size()) {
                           
                        glm::vec2 pos(unit_width * j, unit_height * i);
                        glm::vec2 size(unit_width, unit_height);
                           
                           
                        // Define the wall object
                        pickedTexture = ResourceManager::GetTexture(this->tileData[i][j]);
                        GameObject wallObj(pos, size, pickedTexture, glm::vec3(1.0f));
                        wallObj.IsSolid = true;
                        this->tileInfo[i][j] = wallObj; // Save the tile info
                           
                    }
                }
                else {
                    this->tileInfo[i][j].IsSolid = false; // Set the tile as not solid
                }
        }
           
    }

    // LOADS THE PLAYER POSITION
    // The player is 8x smaller than the walls
    float player_width = unit_width/PLAYER_SIZE_OFFSET;
    float player_height = unit_height/PLAYER_SIZE_OFFSET;

    // Player position offset
    float player_pos_x = this->elementData[0][0] * unit_width;
    float player_pos_y = this->elementData[0][1] * unit_height;

    this->PlayerPosition = glm::vec2(player_pos_x, player_pos_y);
    this->PlayerSize = glm::vec2(player_width, player_height);

    // LOOP TO READ THE ELEMENTS STARTING FROM THE 2nd POSITION

    // Get the size of the elements
    float element_width = unit_width/ELEMENTS_SIZE_OFFSET;
    float element_height = unit_height/ELEMENTS_SIZE_OFFSET;
    
    // Assign attributes of each element
    for(int i = 1; i < this->elementData.size(); i++) {

        // Assign position
        this->elementsInfo[i - 1].Position = glm::vec2(this->elementData[i][0] * unit_width, this->elementData[i][1] * unit_height);
        // Assign size
        this->elementsInfo[i - 1].Size = glm::vec2(element_width, element_height);

        Texture2D pickedTexture;
        // Define the floor texture
        pickedTexture = ResourceManager::GetTexture(this->elementData[i][2]);
        // Assign sprite
        this->elementsInfo[i - 1].Sprite = pickedTexture;
        // Assign solid
        this->elementsInfo[i - 1].IsSolid = true;
    }
    

}



