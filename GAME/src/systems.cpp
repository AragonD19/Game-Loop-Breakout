// src/systems.cpp
#include "systems.h"
#include <iostream>
#include <vector>
#include <cmath>


bool checkCollision(const Position& aPos, const Size& aSize, const Position& bPos, const Size& bSize) {
    return !(aPos.pos.x > bPos.pos.x + bSize.w ||
             aPos.pos.x + aSize.w < bPos.pos.x ||
             aPos.pos.y > bPos.pos.y + bSize.h ||
             aPos.pos.y + aSize.h < bPos.pos.y);
}

void systemPaddleControl(ECS& ecs, float dt, int screenWidth) {
    for (auto& [entity, _] : ecs.getComponentMap<PaddleControlled>()) {
        auto* pos = ecs.getComponent<Position>(entity);
        auto* vel = ecs.getComponent<Velocity>(entity);
        auto* size = ecs.getComponent<Size>(entity);
        if (!pos || !vel || !size) continue;

        vel->vel.x = 0;
        if (IsKeyDown(KEY_LEFT)) vel->vel.x = -300;
        if (IsKeyDown(KEY_RIGHT)) vel->vel.x = 300;

        pos->pos.x += vel->vel.x * dt;
        if (pos->pos.x < 0) pos->pos.x = 0;
        if (pos->pos.x + size->w > screenWidth) pos->pos.x = screenWidth - size->w;
    }
}

void systemBallMovement(ECS& ecs, float dt, int screenWidth, int screenHeight, bool& isRunning) {
    for (auto& [entity, _] : ecs.getComponentMap<Ball>()) {
        auto* pos = ecs.getComponent<Position>(entity);
        auto* vel = ecs.getComponent<Velocity>(entity);
        auto* size = ecs.getComponent<Size>(entity);
        if (!pos || !vel || !size) continue;

        pos->pos.x += vel->vel.x * dt;
        pos->pos.y += vel->vel.y * dt;

        if (pos->pos.x <= 0) {
            pos->pos.x = 0;
            vel->vel.x = -vel->vel.x;
        } else if (pos->pos.x + size->w >= screenWidth) {
            pos->pos.x = screenWidth - size->w;
            vel->vel.x = -vel->vel.x;
        }

        if (pos->pos.y <= 0) {
            pos->pos.y = 0;
            vel->vel.y = -vel->vel.y;
        }

        if (pos->pos.y + size->h >= screenHeight) {
            std::cout << "*****Game Over*****" << std::endl;
            isRunning = false;
        }
    }
}

void systemBallPaddleCollision(ECS& ecs) {
    for (auto& [ballEnt, _] : ecs.getComponentMap<Ball>()) {
        auto* bPos = ecs.getComponent<Position>(ballEnt);
        auto* bVel = ecs.getComponent<Velocity>(ballEnt);
        auto* bSize = ecs.getComponent<Size>(ballEnt);
        if (!bPos || !bVel || !bSize) continue;

        for (auto& [padEnt, _] : ecs.getComponentMap<PaddleControlled>()) {
            auto* pPos = ecs.getComponent<Position>(padEnt);
            auto* pSize = ecs.getComponent<Size>(padEnt);
            if (!pPos || !pSize) continue;

            if (checkCollision(*bPos, *bSize, *pPos, *pSize)) {
                bVel->vel.y = -bVel->vel.y * 1.05f;
                float diff = (bPos->pos.x + bSize->w / 2.0f) - (pPos->pos.x + pSize->w / 2.0f);
                bVel->vel.x = diff * 5.0f;
            }
        }
    }
}

void systemBallBlockCollision(ECS& ecs) {
    std::vector<Entity> toRemove;

    for (auto& [ballEnt, _] : ecs.getComponentMap<Ball>()) {
        auto* bPos = ecs.getComponent<Position>(ballEnt);
        auto* bVel = ecs.getComponent<Velocity>(ballEnt);
        auto* bSize = ecs.getComponent<Size>(ballEnt);
        if (!bPos || !bVel || !bSize) continue;

        for (auto& [blockEnt, _] : ecs.getComponentMap<Block>()) {
            auto* blkPos = ecs.getComponent<Position>(blockEnt);
            auto* blkSize = ecs.getComponent<Size>(blockEnt);
            if (!blkPos || !blkSize) continue;

            if (checkCollision(*bPos, *bSize, *blkPos, *blkSize)) {
                bVel->vel.y = -bVel->vel.y;
                toRemove.push_back(blockEnt);
                break;
            }
        }
    }

    for (Entity e : toRemove) {
        ecs.removeComponent<Block>(e);
        ecs.removeComponent<Position>(e);
        ecs.removeComponent<Size>(e);
    }
}


void systemRender(ECS& ecs) {
    for (auto& [entity, _] : ecs.getComponentMap<PaddleControlled>()) {
        auto* pos = ecs.getComponent<Position>(entity);
        auto* size = ecs.getComponent<Size>(entity);
        if (pos && size)
            DrawRectangle((int)pos->pos.x, (int)pos->pos.y, (int)size->w, (int)size->h, DARKBLUE);
    }

    for (auto& [entity, _] : ecs.getComponentMap<Ball>()) {
        auto* pos = ecs.getComponent<Position>(entity);
        auto* size = ecs.getComponent<Size>(entity);
        if (pos && size)
            DrawRectangle((int)pos->pos.x, (int)pos->pos.y, (int)size->w, (int)size->h, BLACK);
    }

    for (auto& [entity, _] : ecs.getComponentMap<Block>()) {
        auto* pos = ecs.getComponent<Position>(entity);
        auto* size = ecs.getComponent<Size>(entity);
        if (pos && size)
            DrawRectangle((int)pos->pos.x, (int)pos->pos.y, (int)size->w, (int)size->h, RED);
    }
}



void systemInput(ECS& ecs) {
    for (auto& [entity, _] : ecs.getComponentMap<InputControlled>()) {
        auto* vel = ecs.getComponent<Velocity>(entity);
        auto* anim = ecs.getComponent<Animation>(entity);
        if (!vel || !anim) continue;

        vel->vel = {0, 0};
        if (IsKeyDown(KEY_RIGHT)) vel->vel.x = 200.0f;
        if (IsKeyDown(KEY_LEFT)) vel->vel.x = -200.0f;
        if (IsKeyDown(KEY_DOWN)) vel->vel.y = 200.0f;
        if (IsKeyDown(KEY_UP)) vel->vel.y = -200.0f;

        
        if (vel->vel.x > 0) anim->currentState = "walk_right";
        else if (vel->vel.x < 0) anim->currentState = "walk_left";
        else anim->currentState = "idle"; 
    }
}


void systemMovement(ECS& ecs, float dt) {
    for (auto& [entity, pos] : ecs.getComponentMap<Position>()) {
        auto* vel = ecs.getComponent<Velocity>(entity);
        if (!vel) continue;
        pos.pos.x += vel->vel.x * dt;
        pos.pos.y += vel->vel.y * dt;
        
        if (pos.pos.x < 0) pos.pos.x = 0;
        if (pos.pos.x > GetScreenWidth() - 16) pos.pos.x = GetScreenWidth() - 16; 
        if (pos.pos.y < 0) pos.pos.y = 0;
        if (pos.pos.y > GetScreenHeight() - 16) pos.pos.y = GetScreenHeight() - 16; 
    }
}


void systemAI(ECS& ecs, float dt) {
    for (auto& [entity, patrol] : ecs.getComponentMap<AIPatrol>()) {
        auto* vel = ecs.getComponent<Velocity>(entity);
        auto* anim = ecs.getComponent<Animation>(entity);
        if (!vel || !anim) continue;

        vel->vel.x = patrol.speed * patrol.direction;
        vel->vel.y = 0;  

        patrol.traveled += fabs(vel->vel.x) * dt;
        if (patrol.traveled >= patrol.patrolDistance) {
            patrol.direction = -patrol.direction;
            patrol.traveled = 0;
        }


        if (patrol.direction > 0) anim->currentState = "walk_right";
        else anim->currentState = "walk_left";
    }
}

void systemAnimationUpdate(ECS& ecs, float dt) {
    for (auto& [entity, anim] : ecs.getComponentMap<Animation>()) {
        auto* sprite = ecs.getComponent<Sprite>(entity);
        if (!sprite) continue;

        std::string state = anim.currentState;
        if (anim.mode == AnimationMode::Sheet) {
            if (anim.rectStates.find(state) == anim.rectStates.end()) continue;
            auto& frames = anim.rectStates[state];
            if (frames.empty()) continue;
            anim.timer += dt;
            if (anim.timer >= anim.frameTime) {
                anim.currentFrame = (anim.currentFrame + 1) % frames.size();
                sprite->frameRec = frames[anim.currentFrame];
                anim.timer = 0.0f;
            }
        } else {  // Separate
            if (anim.texStates.find(state) == anim.texStates.end()) continue;
            auto& frames = anim.texStates[state];
            if (frames.empty()) continue;
            anim.timer += dt;
            if (anim.timer >= anim.frameTime) {
                anim.currentFrame = (anim.currentFrame + 1) % frames.size();
                sprite->texture = frames[anim.currentFrame];
                anim.timer = 0.0f;
            }
        }
    }
}


void systemRenderSprites(ECS& ecs) {
    for (auto& [entity, sprite] : ecs.getComponentMap<Sprite>()) {
        auto* pos = ecs.getComponent<Position>(entity);
        if (!pos) continue;
        
        Vector2 drawPos = {pos->pos.x - sprite.origin.x * sprite.scale.x, 
                           pos->pos.y - sprite.origin.y * sprite.scale.y};  
        
        if (sprite.isSheet) {
            Rectangle scaledRec = {0, 0, sprite.frameRec.width * sprite.scale.x, 
                                   sprite.frameRec.height * sprite.scale.y};
            DrawTexturePro(sprite.texture, sprite.frameRec, scaledRec, sprite.origin, 0.0f, sprite.tint);

        } else {
            DrawTextureEx(sprite.texture, drawPos, 0.0f, sprite.scale.x, sprite.tint); 
        }
    }
}
