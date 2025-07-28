#include "ecs.h"
#include "components.h"
#include "systems.h"
#include <raylib.h>
#include <iostream>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Breakout ECS");
    SetTargetFPS(60);

    ECS ecs;

    // Paddle
    Entity paddle = ecs.createEntity();
    ecs.addComponent(paddle, Position{screenWidth / 2.f - 50.f, screenHeight - 40.f});
    ecs.addComponent(paddle, Size{100.f, 20.f});
    ecs.addComponent(paddle, Velocity{0.f, 0.f});
    ecs.addComponent(paddle, PaddleControlled{});

    // Ball
    Entity ball = ecs.createEntity();
    ecs.addComponent(ball, Position{screenWidth / 2.f - 7.5f, screenHeight / 2.f - 7.5f});
    ecs.addComponent(ball, Size{15.f, 15.f});
    ecs.addComponent(ball, Velocity{0.f, 150.f});
    ecs.addComponent(ball, Ball{});

    // Blocks
    const int cols = 4;
    const int rows = 2;
    const float blockWidth = screenWidth / (float)cols;
    const float blockHeight = 20.f;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            Entity block = ecs.createEntity();
            ecs.addComponent(block, Position{x * blockWidth + 2, y * (blockHeight + 5) + 40});
            ecs.addComponent(block, Size{blockWidth - 4, blockHeight});
            ecs.addComponent(block, Block{});
        }
    }

    bool isRunning = true;

    while (isRunning && !WindowShouldClose()) {
        float dt = GetFrameTime();

        systemPaddleControl(ecs, dt, screenWidth);
        systemBallMovement(ecs, dt, screenWidth, screenHeight, isRunning);
        systemBallPaddleCollision(ecs);
        systemBallBlockCollision(ecs);

        if (ecs.getComponentMap<Block>().empty()) {
            std::cout << "You Win!" << std::endl;
            isRunning = false;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        systemRender(ecs);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
