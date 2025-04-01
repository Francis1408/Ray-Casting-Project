

#include "gameLevel.h"

#include <fstream>
#include <sstream>


void GameLevel::Load(const char *mapFile, const char  *elementFile, unsigned int screenWidth, unsigned int screenHeight)
{
    // clear old data
    this->Tiles.clear();

    // load from map mapFile
    unsigned int tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(mapFile);
    this->tileData;
    if (fstream)
    {
        while (std::getline(fstream, line)) // read each line from level mapFile
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            while (sstream >> tileCode) // read each word separated by spaces
                row.push_back(tileCode);
            this->tileData.push_back(row);
        }
    }


    // clear old data
    this->Elements.clear();

    // load from element file
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

    if (this->tileData.size() > 0 && elementData.size() > 0)
        this->init(elementData, screenWidth, screenHeight);

}

void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Tiles)
        if (!tile.Destroyed)
            tile.Draw(renderer);
}

/*
bool GameLevel::IsCompleted()
{
    for (GameObject &tile : this->Tiles)
    if (!tile.IsSolid && !tile.Destroyed)
    return false;
    return true;
}
*/

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
            if(this->tileData[i][j] >= 1)
            {
                glm::vec2 pos(unit_width * j, unit_height * i);
                glm::vec2 size(unit_width, unit_height);

                Texture2D pickedTexture;

                switch(this->tileData[i][j]) {
                    case 1: pickedTexture = ResourceManager::GetTexture("bluestone");  break;
                    case 2: pickedTexture = ResourceManager::GetTexture("colorstone"); break;
                    case 3: pickedTexture = ResourceManager::GetTexture("eagle");      break;
                    case 4: pickedTexture = ResourceManager::GetTexture("greystone");  break;
                    case 5: pickedTexture = ResourceManager::GetTexture("mossy");      break;
                    case 6: pickedTexture = ResourceManager::GetTexture("purplestone");break;
                    case 7: pickedTexture = ResourceManager::GetTexture("redbrick");   break;
                    case 8: pickedTexture = ResourceManager::GetTexture("wood");       break;
                    default:pickedTexture = ResourceManager::GetTexture("bluestone");  break;
                }

                GameObject obj(pos, size, pickedTexture, glm::vec3(1.0f, 1.0f, 1.0f));
                obj.IsSolid = true;
                this->Tiles.push_back(obj);
            }

            printf("%d ", this->tileData[i][j]); // Print map on the terminal
        }
            printf("\n");
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



