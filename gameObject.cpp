#include "gameObject.h"
#include "resourceManager.h"

/*
GameObject::GameObject() 
: Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity) 
: Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), IsSolid(false), Destroyed(false) { }
*/

GameObject::GameObject() 
: Position(0.0f, 0.0f), Size(1.0f, 1.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite ,glm::vec3 color, glm::vec2 velocity, glm::vec2 pivot) 
: Position(pos), Pivot(pivot), Size(size), Sprite(sprite), Color(color), Rotation(0.0f), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(SpriteRenderer &renderer)
{   

    //std::cout << this->Color.r << this->Color.g <<this->Color.b << std::endl;
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color, this->Pivot);
}