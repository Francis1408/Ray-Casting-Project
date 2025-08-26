#ifndef RAYCASTING_H
#define RAYCASTING_H

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "glm/glm.hpp"
#include "gameLevel.h"
#include "playerObject.h"
#include "spriteRenderer.h"
#include "gameObject.h"
#include "texture.h"

class RayCasting  {

    public:

    // Constructor
    // Payload with all the variables nedded in game in order to the raycasting work
    RayCasting(
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
    );


    // Methods
    void WallCasting(std::vector<float>& zBuffer); // Wall rendering
    void FloorCeilingCasting(); // Floor and Ceiling rendering
    void SpriteCasting(std::vector<float>& zBuffer); // Sprite rendering
    void SortSprites(); // Method to sort sprites based on their distances

    private:
        // Measures from the game level
        unsigned int Width, Height;
        unsigned int rayDensity;

        // Player reference
        PlayerObject* Player;
        // Level reference
        GameLevel* Level;

        // Renderers references
        SpriteRenderer* WallRenderer;
        SpriteRenderer* FloorRenderer;
        SpriteRenderer* SpRenderer;

        // Objects to be drawn
        GameObject* wallObj;
        GameObject* floorObj;
        GameObject* spriteObj;

        // Textures
        Texture2D* floorTexture;

        float mapScale;

        unsigned int mapSizeGridX, mapSizeGridY;

        // Number of sprites avaiable
        unsigned int numSprites;

        // Array of sprite distances
        std::vector<float> spriteDistance;
        // Array to store the order of the sprites from fartest to the nearst
        std::vector<int> spriteOrder;


};


#endif
