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
    // level map state
    std::vector<GameObject> Tiles;

    // elements
    std::vector<GameObject> Elements;

    // player initial position
    glm::vec2 PlayerPosition, PlayerSize;

    // constructor
    GameLevel() { }
    // loads level from file
    void Load(const char *mapFile, const char *elementFile, unsigned int levelWidth, unsigned int levelHeight);
    // render level
    void Draw(SpriteRenderer &renderer);
    /*
    // check if the level is completed (all non-solid tiles are destroyed)
    bool IsCompleted();
    */
private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData, std::vector<std::vector<unsigned int>> elementData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif