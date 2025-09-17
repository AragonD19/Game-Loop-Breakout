// include/components.h

#pragma once
#include <raylib.h>
#include <string>
#include <vector>
#include <unordered_map>

enum class AnimationMode { Sheet, Separate };

struct Position { Vector2 pos; };  
struct Velocity { Vector2 vel; };  
struct Size { float w, h; }; 
struct PaddleControlled {}; 
struct Ball {};
struct Block {};


struct Sprite {
    Texture2D texture;  
    Rectangle frameRec = {0, 0, 16, 16}; 
    Vector2 origin = {0, 0};
    Color tint = WHITE;
    bool isSheet = true;  
    Vector2 scale = {1.0f, 1.0f}; 
};


struct Animation {
    std::string currentState = "idle";
    AnimationMode mode = AnimationMode::Sheet; 
    std::unordered_map<std::string, std::vector<Rectangle>> rectStates;  
    std::unordered_map<std::string, std::vector<Texture2D>> texStates;   
    int currentFrame = 0;
    float frameTime = 0.1f;
    float timer = 0.0f;
};


struct InputControlled {};


struct AIPatrol {
    float speed = 100.0f;
    float direction = 1.0f;  
    float patrolDistance = 200.0f;
    float traveled = 0.0f;
};
