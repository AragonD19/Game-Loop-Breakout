// inlcude/scenes/AdventureScene.h
#pragma once
#include "../Scene.h"
#include "../components.h"
#include "../systems.h"

class AdventureScene : public Scene {
public:
    AdventureScene(int width, int height);
    void setup() override;
    void update(float dt) override;
    void render() override;
    void clean() override;

private:
    int screen_width, screen_height;
    Texture2D backgroundTex; 
    Entity player, enemy;
};