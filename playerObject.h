#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "gameObject.h"
#include "texture.h"

class PlayerObject : public GameObject {

    public:

    float velocity;
    float rotSpeed;
    glm::vec2 direction,plane; // The plane must be pependicular to the direction
    bool isRunning;

    // Constructor
    PlayerObject();

    PlayerObject(glm::vec2 pos, glm::vec2 size, glm::vec3 color, float velocity, float rotSpeed, glm::vec2 direction, glm::vec2 plane);


};


#endif



