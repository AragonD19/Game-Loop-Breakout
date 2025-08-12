// src/Game.cpp
#include "Game.h"
#include <iostream>
#include "print.h"
#include <rlImGui.h>

Game::Game(const char* title, int width, int height) 
    : screen_width(width), screen_height(height), isRunning(true), editor(ecs, paused) {
    InitWindow(screen_width, screen_height, title);
    SetTargetFPS(60);
    rlImGuiSetup(true);
}

Game::~Game() {
    
}

void Game::setup() {
    // Paddle
    paddle = ecs.createEntity();
    ecs.addComponent(paddle, Position{screen_width / 2.f - 50.f, screen_height - 40.f});
    ecs.addComponent(paddle, Size{100.f, 20.f});
    ecs.addComponent(paddle, Velocity{0.f, 0.f});
    ecs.addComponent(paddle, PaddleControlled{});

    // Ball
    ball = ecs.createEntity();
    ecs.addComponent(ball, Position{screen_width / 2.f - 7.5f, screen_height / 2.f - 7.5f});
    ecs.addComponent(ball, Size{15.f, 15.f});
    ecs.addComponent(ball, Velocity{0.f, -150.f});  // Hacia arriba inicialmente
    ecs.addComponent(ball, Ball{});

    // Blocks
    const int cols = 4;
    const int rows = 2;
    const float blockWidth = screen_width / (float)cols;
    const float blockHeight = 20.f;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            Entity block = ecs.createEntity();
            ecs.addComponent(block, Position{x * blockWidth + 2, y * (blockHeight + 5) + 40});
            ecs.addComponent(block, Size{blockWidth - 4, blockHeight});
            ecs.addComponent(block, Block{});
        }
    }
}

void Game::frame_start() {
}

void Game::handle_events() {
}

void Game::update(float dt) {
    if (paused) return;

    systemPaddleControl(ecs, dt, screen_width);
    systemBallMovement(ecs, dt, screen_width, screen_height, isRunning);
    systemBallPaddleCollision(ecs);
    systemBallBlockCollision(ecs);

    if (ecs.getComponentMap<Block>().empty()) {
        std::cout << "You Win!" << std::endl;
        isRunning = false;
    }

#ifdef DEBUG
    auto* ballPos = ecs.getComponent<Position>(ball);
    if (ballPos) vprint(*ballPos);
#endif
}

void Game::render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    systemRender(ecs);
    DrawFPS(10, 10);

    rlImGuiBegin();
    editor.renderGUI();
    rlImGuiEnd();

    EndDrawing();
}

void Game::frame_end() {
}

void Game::clean() {
    if (cleaned) return; 
    cleaned = true;

    rlImGuiShutdown();
    CloseWindow();
}

bool Game::running() {
    return isRunning && !WindowShouldClose();
}