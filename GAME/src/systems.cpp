#include "systems.h"
#include <iostream>
#include <vector>


bool checkCollision(const Position& aPos, const Size& aSize, const Position& bPos, const Size& bSize) {
    return !(aPos.x > bPos.x + bSize.w ||
             aPos.x + aSize.w < bPos.x ||
             aPos.y > bPos.y + bSize.h ||
             aPos.y + aSize.h < bPos.y);
}

void systemPaddleControl(ECS& ecs, float dt, int screenWidth) {
    for (auto& [entity, _] : ecs.getComponentMap<PaddleControlled>()) {
        auto* pos = ecs.getComponent<Position>(entity);
        auto* vel = ecs.getComponent<Velocity>(entity);
        auto* size = ecs.getComponent<Size>(entity);
        if (!pos || !vel || !size) continue;

        vel->vx = 0;
        if (IsKeyDown(KEY_LEFT)) vel->vx = -300;
        if (IsKeyDown(KEY_RIGHT)) vel->vx = 300;

        pos->x += vel->vx * dt;
        if (pos->x < 0) pos->x = 0;
        if (pos->x + size->w > screenWidth) pos->x = screenWidth - size->w;
    }
}

void systemBallMovement(ECS& ecs, float dt, int screenWidth, int screenHeight, bool& isRunning) {
    for (auto& [entity, _] : ecs.getComponentMap<Ball>()) {
        auto* pos = ecs.getComponent<Position>(entity);
        auto* vel = ecs.getComponent<Velocity>(entity);
        auto* size = ecs.getComponent<Size>(entity);
        if (!pos || !vel || !size) continue;

        pos->x += vel->vx * dt;
        pos->y += vel->vy * dt;

        if (pos->x <= 0 || pos->x + size->w >= screenWidth) {
            std::cout << "*****Pelota tocó borde lateral. Cerrando juego.*****" << std::endl;
            isRunning = false;
        }


        if (pos->y + size->h >= screenHeight) {
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
                bVel->vy = -bVel->vy * 1.05f;
                float diff = (bPos->x + bSize->w / 2.0f) - (pPos->x + pSize->w / 2.0f);
                bVel->vx = diff * 5.0f;
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
                bVel->vy = -bVel->vy;
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
            DrawRectangle((int)pos->x, (int)pos->y, (int)size->w, (int)size->h, DARKBLUE);
    }

    for (auto& [entity, _] : ecs.getComponentMap<Ball>()) {
        auto* pos = ecs.getComponent<Position>(entity);
        auto* size = ecs.getComponent<Size>(entity);
        if (pos && size)
            DrawRectangle((int)pos->x, (int)pos->y, (int)size->w, (int)size->h, BLACK);
    }

    for (auto& [entity, _] : ecs.getComponentMap<Block>()) {
        auto* pos = ecs.getComponent<Position>(entity);
        auto* size = ecs.getComponent<Size>(entity);
        if (pos && size)
            DrawRectangle((int)pos->x, (int)pos->y, (int)size->w, (int)size->h, RED);
    }
}
