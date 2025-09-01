#include "playerObject.h"

PlayerObject::PlayerObject()
    :GameObject() { }
    
PlayerObject::PlayerObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, float velocity, float rotSpeed, glm::vec2 direction, glm::vec2 plane, float hitbox) 
    :GameObject(pos, size, sprite, color, glm::vec2(velocity, velocity)), velocity(velocity), rotSpeed(rotSpeed), direction(direction), plane(plane), isRunning(false), hitbox(hitbox) { 

    }


