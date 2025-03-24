#ifndef GAME_H
#define GAME_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game {

    public:
    // game state
    GameState State;
    bool Keys[1024]; // Array of keys
    unsigned int Width, Height; // Width and Height of the window

    // Constructor
    Game(unsigned int width, unsigned int height);

    // Destructor
    ~Game();

    // Initialize game state (load all shaders/textures/levels)
    void Init();

    // Game Loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();


};

#endif