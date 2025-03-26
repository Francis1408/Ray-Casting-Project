#include "playerObject.h"

PlayerObject::PlayerObject()
    :GameObject() { }
    
PlayerObject::PlayerObject(glm::vec2 pos, glm::vec2 size, glm::vec3 color, float velocity, float rotSpeed, glm::vec2 direction) 
    :GameObject(pos, size, color, glm::vec2(velocity, velocity)), velocity(velocity), rotSpeed(rotSpeed), direction(direction) { }


