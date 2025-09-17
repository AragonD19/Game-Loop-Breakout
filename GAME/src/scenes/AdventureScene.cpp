// src/scenes/AdventureScene.cpp

#include "scenes/AdventureScene.h"
#include <raylib.h>

AdventureScene::AdventureScene(int width, int height) : screen_width(width), screen_height(height) {}

void AdventureScene::setup() {
    backgroundTex = LoadTexture("assets/BackGround.png");

    Texture2D playerIdle1 = LoadTexture("assets/MagoIdel1.png");
    Texture2D playerIdle2 = LoadTexture("assets/MagoIdel2.png");
    Texture2D playerIdle3 = LoadTexture("assets/MagoIdel3.png");
    Texture2D playerIdle4 = LoadTexture("assets/MagoIdel4.png");
    Texture2D playerLeft1 = LoadTexture("assets/MagoWalkL1.png");
    Texture2D playerLeft2 = LoadTexture("assets/MagoWalkL2.png");
    Texture2D playerRight1 = LoadTexture("assets/MagoWalkR1.png");
    Texture2D playerRight2 = LoadTexture("assets/MagoWalkR2.png");
    player = ecs.createEntity();
    ecs.addComponent(player, Position{{100.0f, 100.0f}});
    ecs.addComponent(player, Velocity{{0, 0}});
    Sprite playerSprite{playerIdle1};
    playerSprite.isSheet = false;  
    ecs.addComponent(player, playerSprite);
    Animation playerAnim;
    playerAnim.mode = AnimationMode::Separate;
    playerAnim.frameTime = 0.15f;
    playerAnim.texStates["idle"] = {playerIdle1,playerIdle2,playerIdle3,playerIdle4};
    playerAnim.texStates["walk_left"] = {playerLeft1, playerLeft2};
    playerAnim.texStates["walk_right"] = {playerRight1, playerRight2};
    ecs.addComponent(player, playerAnim);
    ecs.addComponent(player, InputControlled{});

    ecs.getComponent<Sprite>(player)->scale = {4.0f, 4.0f}; 
}

void AdventureScene::update(float dt) {
    systemInput(ecs);
    systemAI(ecs, dt);
    systemMovement(ecs, dt);
    systemAnimationUpdate(ecs, dt);

}

void AdventureScene::render() {

    DrawTextureEx(backgroundTex, {0,0}, 0, 1.0f, WHITE);  

    systemRenderSprites(ecs);
}

void AdventureScene::clean() {
    UnloadTexture(backgroundTex);

    if (auto* playerAnim = ecs.getComponent<Animation>(player)) {
        if (playerAnim->mode == AnimationMode::Sheet) {
            UnloadTexture(ecs.getComponent<Sprite>(player)->texture);  
        } else {
            for (auto& [state, frames] : playerAnim->texStates) {
                for (auto& tex : frames) UnloadTexture(tex);
            }
        }
    }

    if (auto* playerAnim = ecs.getComponent<Animation>(player)) {
        if (playerAnim->mode == AnimationMode::Sheet) {
            UnloadTexture(ecs.getComponent<Sprite>(player)->texture);
        } else {
            for (auto& [state, frames] : playerAnim->texStates) {
                for (auto& tex : frames) UnloadTexture(tex);
            }
        }
    }
}