

#include "gameLevel.h"
#include <fstream>
#include <sstream>


void GameLevel::Load(const char *mapFile, const char *floorFile, const char *ceilingFile, const char  *elementFile, 
                     unsigned int screenWidth, unsigned int screenHeight)
{
    // clear old data
    this->tileInfo.clear();
    this->Elements.clear();
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
    unsigned int elementCode;
    std::string line2;
    std::ifstream fstream2(elementFile);
    std::vector<std::vector<unsigned int>> elementData;
    if (fstream2)
    {
        while (std::getline(fstream2, line2)) // read each line from level elementFile
        {
            std::istringstream sstream(line2);
            std::vector<unsigned int> row;
            while (sstream >> elementCode) // read each word separated by spaces
                row.push_back(elementCode);
            elementData.push_back(row);
        }

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




    if (this->tileData.size() > 0 && elementData.size() > 0 && 
        this->floorData.size() == this->tileData.size() && this->ceilingData.size() == this->floorData.size()) // All the 3 map builder files must be the same size
    {

        this->init(elementData, screenWidth, screenHeight);
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


void GameLevel::init(std::vector<std::vector<unsigned int>> eleData, unsigned int screenWidth, unsigned int screenHeight)
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

            //if(this->tileData[i][j] <= ResourceManager::Textures.size() || this->floorData[i][j] <= ResourceManager::Textures.size()
            //   || this->ceilingData[i][j] <= ResourceManager::Textures.size()) {

                   
                Texture2D pickedTexture;
                // Define the floor texture
                //std::cout << this->floorData[i][j] <<std::endl;
                pickedTexture = ResourceManager::GetTexture(this->floorData[i][j]);
                this->floorInfo[i][j].Sprite = pickedTexture;
                //std::cout << this->floorInfo[i][j].Sprite.ID << std::endl;
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
           // }
          //  else {
            //    throw std::runtime_error("Index in a map file does not match the amount of textures avaiable");
                // Terminate the execution somehow
           // }
        }
           
    }

    // The player is 8x smaller than the walls
    float player_width = unit_width/8;
    float player_height = unit_height/8;

    // Player position offset
    float player_pos_x = eleData[0][0] * unit_width;
    float player_pos_y = eleData[0][1] * unit_height;

    this->PlayerPosition = glm::vec2(player_pos_x, player_pos_y);
    this->PlayerSize = glm::vec2(player_width, player_height);

    // LOOP TO READ THE ELEMENTS
    

}



