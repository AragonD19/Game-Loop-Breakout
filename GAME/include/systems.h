#pragma once
#include "ecs.h"
#include "components.h"
#include <raylib.h>

bool checkCollision(const Position& aPos, const Size& aSize, const Position& bPos, const Size& bSize);

void systemPaddleControl(ECS& ecs, float dt, int screenWidth);
void systemBallMovement(ECS& ecs, float dt, int screenWidth, int screenHeight, bool& isRunning);
void systemBallPaddleCollision(ECS& ecs);
void systemBallBlockCollision(ECS& ecs);
void systemRender(ECS& ecs);
