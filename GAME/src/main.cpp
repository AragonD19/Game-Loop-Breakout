#include <raylib.h>
#include <string>    // para std::to_string

#include "Game.h"
#include "print.h"   // tu archivo de impresión personalizado

Rectangle ball;
Rectangle paddle;

int ball_sx = 0;
int ball_sy = 150;
int paddle_sx = 300;

int main() {
    Game game("Mi juego", 800, 600);
    game.setup();

    while (game.running()) {
        game.frame_start();
        game.handle_events();
        game.update();
        game.render();
        game.frame_end();
    }

    game.clean();
    return 0;
}

Game::Game(const char* title, int width, int height)
: screen_width(width), screen_height(height) {
  InitWindow(width, height, title);
  print("GAME STARTED");
  isRunning = true;
  counter = 0;
}

Game::~Game() {
}

void Game::setup() {
  SetTargetFPS(60);
  ball = Rectangle{
    (float)(screen_width / 2 - 7.5f), 
    (float)(screen_height / 2 - 7.5f),
    15, 15
  };

  paddle = Rectangle{(float)(screen_width / 2) - ball.width * 5, (float)screen_height - 15, ball.width * 10, 15};
  
  
  // Creacion de bloques
  int cols = 10;
  int rows = 4;
  float blockWidth = screen_width / (float)cols;
  float blockHeight = 20;

  for (int y = 0; y < rows; ++y) {
      for (int x = 0; x < cols; ++x) {
          Rectangle block = {
              x * blockWidth + 2, 
              y * (blockHeight + 5) + 40,
              blockWidth - 4,
              blockHeight
          };
          blocks.push_back(block);
      }
  }

}

void Game::frame_start() {
  print("==== FRAME", counter, "START ====");
  BeginDrawing();
}

void Game::frame_end() {
  print("==== FRAME END ====");
  EndDrawing();
  counter++;
}

void Game::handle_events() {
  float dT = GetFrameTime();
  if (WindowShouldClose()) {
    isRunning = false;
  }
  if (IsKeyDown(KEY_RIGHT)) {
      paddle.x += paddle_sx * dT;
  }
  if (IsKeyDown(KEY_LEFT)) {
      paddle.x -= paddle_sx * dT;
  }

  // Para que el paddle no salga de la pantalla
  if (paddle.x < 0) {
      paddle.x = 0;
  }
  if ((paddle.x + paddle.width) > screen_width) {
      paddle.x = screen_width - paddle.width;
  }

}

void Game::update() {
  float dT = GetFrameTime();

  if ((ball.x + ball.width) >= screen_width || ball.x <= 0) {
      ball_sx *= -1;
  }

  if ((ball.y + ball.height) >= screen_height) {
      print("Game Over");
      CloseWindow(); 
      exit(0);
  }

  if (CheckCollisionRecs(ball, paddle)) {
    ball_sy *= -1;
    
    float ballCenterX = ball.x + ball.width / 2;
    float paddleCenterX = paddle.x + paddle.width / 2;
    float diff = ballCenterX - paddleCenterX;

    ball_sx = diff * 5.0f; 

    ball_sy *= 1.05f;
    ball_sx *= 1.05f;
  }

  if (ball.y <= 0) {
      ball_sy *= -1;
  }

  ball.x += ball_sx * dT;
  ball.y += ball_sy * dT;

  // Colisión con bloques
  for (size_t i = 0; i < blocks.size(); ++i) {
      if (CheckCollisionRecs(ball, blocks[i])) {
          
          ball_sy *= -1;

          
          blocks.erase(blocks.begin() + i);
          break; 
      }
  }

  if (blocks.empty()) {
    print("You Win!");
    CloseWindow();
    exit(0);
  }


}

void Game::render() {
  ClearBackground(GRAY);
  DrawText(("FPS: " + std::to_string(GetFPS())).c_str(), 10, 10, 20, GRAY);
  DrawRectangleRec(ball, BLACK);
  DrawRectangleRec(paddle, BLACK);
  for (const Rectangle& block : blocks) {
    DrawRectangleRec(block, DARKBLUE);
  }

}

void Game::clean() {
  CloseWindow();
}

bool Game::running() {
  return isRunning;
}

