// include/Game.h
#pragma once
#include <raylib.h>
#include <vector>
#include "ecs.h"
#include "components.h"
#include "systems.h"
#include "editor/Editor.h"

class Game {
public:
    Game(const char* title, int width, int height);
    ~Game();  

    void setup();
    void frame_start();
    void handle_events();
    void update(float dt);
    void render();
    void frame_end();
    void clean();
    bool running();

private:
    int screen_width;
    int screen_height;
    bool isRunning;
    bool paused = false;
    bool cleaned = false;  
    ECS ecs;
    Entity paddle;
    Entity ball;
    Editor editor;
};