

#include "gameLevel.h"

#include <fstream>
#include <sstream>


void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
    // clear old data
    this->Tiles.clear();
    // load from file
    unsigned int tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    if (fstream)
    {
        while (std::getline(fstream, line)) // read each line from level file
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            while (sstream >> tileCode) // read each word separated by spaces
                row.push_back(tileCode);
            tileData.push_back(row);
        }
        if (tileData.size() > 0)
            this->init(tileData, levelWidth, levelHeight);
    }
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

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{

    // calculate dimensions
    unsigned int height = tileData.size();
    unsigned int width  = tileData[0].size();
    float unit_width = levelWidth / static_cast<float>(width);
    float unit_height = levelHeight / tileData.size();

    // read throught the array of tile data
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            if(tileData[i][j] == 1)
            {
                glm::vec2 pos(unit_width * j, unit_height * i);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, glm::vec3(1.0f, 1.0f, 1.0f));
                obj.IsSolid = true;
                this->Tiles.push_back(obj);
            }
            else if(tileData[i][j] > 1)
            {
                glm::vec3 color = glm::vec3(1.0f); // original: white
                if(tileData[i][j] == 2)
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                else if(tileData[i][j] == 3)
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                else if(tileData[i][j] == 4)
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
    
                glm::vec2 pos(unit_width * j, unit_height * i);
                glm::vec2 size(unit_width, unit_height);
                this->Tiles.push_back(GameObject(pos, size, color));
            }

            printf("%d ", tileData[i][j]); // Print map on the terminal
        }
            printf("\n");
        }
}



