#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "gameObject.h"
#include "spriteRenderer.h"
#include "resourceManager.h"


/// GameLevel holds all Tiles as part of a Breakout level and 
/// hosts functionality to Load/render levels from the harddisk.
class GameLevel
{
public:

    // level map data
    std::vector<std::vector<unsigned int>> tileData;
    // Matrix that contains the tiles gameObject information
    std::vector<std::vector<GameObject>> tileInfo;

    // floor map data
    std::vector<std::vector<unsigned int>> floorData;
    // Matrix that contains the floor tiles gameObject information
    std::vector<std::vector<GameObject>> floorInfo;

    // ceiling map data
    std::vector<std::vector<unsigned int>> ceilingData;
    // Matrix that contains the ceiling tiles gameObject information
    std::vector<std::vector<GameObject>> ceilingInfo;

    // elements
    std::vector<GameObject> Elements;

    // player initial position
    glm::vec2 PlayerPosition, PlayerSize;

    // Tile Size
    float tileSize;

    // constructor
    GameLevel() { }
    // loads level from file
    void Load(const char *mapFile, const char* floorFile, const char* ceilingFile, const char *elementFile, unsigned int levelWidth, unsigned int levelHeight);

    // render the level map
    void DrawMap(SpriteRenderer &renderer);
    /*
    // check if the level is completed (all non-solid tiles are destroyed)
    bool IsCompleted();
    */
private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> elementData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif