// src/scenes/AdventureScene.cpp

#include "scenes/AdventureScene.h"
#include <raylib.h>
#include <iostream>
#include "../perlin.h"
#include <cstring>  // Para memmove
#include <thread>   // Para std::thread
#include <mutex>    // Para std::mutex

AdventureScene::AdventureScene(int width, int height) : screen_width(width), screen_height(height) {}

Entity tilemapEnt;
Entity cameraEnt;

void AdventureScene::setup() {

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
    playerSprite.isSheet = false;  // Separate mode
    ecs.addComponent(player, playerSprite);
    Animation playerAnim;
    playerAnim.mode = AnimationMode::Separate;
    playerAnim.frameTime = 0.15f;
    playerAnim.texStates["idle"] = {playerIdle1,playerIdle2,playerIdle3,playerIdle4};
    playerAnim.texStates["walk_left"] = {playerLeft1, playerLeft2};
    playerAnim.texStates["walk_right"] = {playerRight1, playerRight2};
    ecs.addComponent(player, playerAnim);
    ecs.addComponent(player, InputControlled{});

    ecs.getComponent<Sprite>(player)->scale = {4.0f, 4.0f};  // 16x16 -> 64x64, visible en 800x600

    
    // Después de player setup
    tilemapEnt = ecs.createEntity();
    TileMap tilemap;
    tilemap.tileset = LoadTexture("assets/tileset.png");  // Asume existe
    if (tilemap.tileset.id == 0) {
        std::cerr << "Error: Tileset load failed! Check path 'assets/tileset.png'" << std::endl;
    } else {
        std::cout << "Tileset loaded: width=" << tilemap.tileset.width << ", height=" << tilemap.tileset.height << std::endl;
    }

    tilemap.wallTex = LoadTexture("assets/wall.png");  // Ajusta path—tu sprite para NON_WALKABLE specials
    tilemap.hazardTex = LoadTexture("assets/hazard.png");  // Para HAZARD
    tilemap.pickupTex = LoadTexture("assets/pickup.png");   // Para PICKUP

    if (tilemap.wallTex.id == 0) std::cerr << "Wall tex load failed!" << std::endl;
    if (tilemap.hazardTex.id == 0) std::cerr << "Hazard tex load failed!" << std::endl;
    if (tilemap.pickupTex.id == 0) std::cerr << "Pickup tex load failed!" << std::endl;

    // Procedural gen inicial: Un chunk central
    // unsigned int seed = 12345;
    unsigned int seed = time(NULL);  // Fijo para test (cambia a time(NULL) para random)
    PerlinNoise perlin(seed);

    // Inicial gen: Un chunk central
    tilemap.tiles.reserve(tilemap.maxWidth * tilemap.maxHeight);  // Pre-allocate max para evitar reallocs
    tilemap.width = tilemap.chunkSize * 2;  // Start 40x40
    tilemap.height = tilemap.chunkSize * 2;
    tilemap.tiles.resize(tilemap.width * tilemap.height);

    // Gen con offset world inicial (0,0 para center)
    int worldOffsetX = 0;
    int worldOffsetY = 0;
    generateChunk(tilemap, worldOffsetX, worldOffsetY, perlin);

    ecs.addComponent(tilemapEnt, tilemap);

    // Autotiling después de gen
    systemAutoTiling(ecs);

    // Añadir Health y Score a player
    ecs.addComponent(player, Health{});
    ecs.addComponent(player, Score{});


    cameraEnt = ecs.createEntity();
    CameraComp camComp;
    camComp.cam.offset = { (float)screen_width / 2.0f, (float)screen_height / 2.0f };  // Center screen
    camComp.cam.target = {0, 0};  // Inicial
    camComp.cam.rotation = 0.0f;
    camComp.cam.zoom = 1.0f;  // 1x, ajusta para zoom out
    camComp.target = player;  // Follow player
    ecs.addComponent(cameraEnt, camComp);


}



void AdventureScene::generateChunk(TileMap& tilemap, int offsetX, int offsetY, PerlinNoise& perlin) {
    const int chunkSize = tilemap.chunkSize;  // Precompute const
    for (int y = 0; y < chunkSize; ++y) {
        for (int x = 0; x < chunkSize; ++x) {
            int globalX = x + offsetX * chunkSize;
            int globalY = y + offsetY * chunkSize;
            int localY = y + (offsetY >= 0 ? offsetY * chunkSize : 0);  // Ajuste para negative offsets
            int localX = x + (offsetX >= 0 ? offsetX * chunkSize : 0);
            int index = localY * tilemap.width + localX;

            double noiseVal = perlin.noise(globalX * frequency, globalY * frequency);

            if (noiseVal > thresholdWall) {
                tilemap.tiles[index].value = IntGridValue::NON_WALKABLE;
            } else if (noiseVal < thresholdHazard) {
                tilemap.tiles[index].value = IntGridValue::HAZARD;
            } else {
                tilemap.tiles[index].value = IntGridValue::WALKABLE;
            }

            // Random pickups en walkable (raro, ~2%)
            if (tilemap.tiles[index].value == IntGridValue::WALKABLE && GetRandomValue(0, 100) < 2) {
                tilemap.tiles[index].value = IntGridValue::PICKUP;
            }
        }
    }
}



void AdventureScene::update(float dt) {
    systemInput(ecs);
    systemAI(ecs, dt);
    systemTileInteractions(ecs, dt);
    systemMovement(ecs, dt);
    systemAnimationUpdate(ecs, dt);
    // Agrega lógica de juego, e.g., colisiones si expandes
    systemCameraUpdate(ecs, dt);

    // Opt: Cooldown para check expansión (e.g., cada 0.1s)
    static float expansionCooldown = 0.0f;
    expansionCooldown -= dt;
    if (expansionCooldown > 0.0f) return;
    expansionCooldown = 0.1f;  // Reset

    // Después de systemMovement
    auto* pos = ecs.getComponent<Position>(player);
    if (pos) {
        auto* tilemap = ecs.getComponent<TileMap>(tilemapEnt);
        if (tilemap) {
            const float edgeThreshold = tilemap->chunkSize * tilemap->tileSize * tilemap->scale / 2.0f;  // Precompute
            bool dirty = false;  // Flag para autotile solo si expanded

            // Right edge (append)
            if (pos->pos.x > (tilemap->width * tilemap->tileSize * tilemap->scale) - edgeThreshold && tilemap->width < tilemap->maxWidth) {
                int oldWidth = tilemap->width;
                tilemap->width += tilemap->chunkSize;
                tilemap->tiles.resize(tilemap->width * tilemap->height);
                std::thread genThread([this, tilemap, oldWidth] {
                    tileMutex.lock();
                    generateChunk(*tilemap, oldWidth / tilemap->chunkSize, 0, perlin);
                    tileMutex.unlock();
                });
                genThread.join();
                systemAutoTilingChunk(ecs, oldWidth - 1, 0, tilemap->width - 1, tilemap->height - 1);
                dirty = true;
            }

            // Bottom (append)
            if (pos->pos.y > (tilemap->height * tilemap->tileSize * tilemap->scale) - edgeThreshold && tilemap->height < tilemap->maxHeight) {
                int oldHeight = tilemap->height;
                tilemap->height += tilemap->chunkSize;
                tilemap->tiles.resize(tilemap->width * tilemap->height);
                std::thread genThread([this, tilemap, oldHeight] {
                    tileMutex.lock();
                    generateChunk(*tilemap, 0, oldHeight / tilemap->chunkSize, perlin);
                    tileMutex.unlock();
                });
                genThread.join();
                systemAutoTilingChunk(ecs, 0, oldHeight - 1, tilemap->width - 1, tilemap->height - 1);
                dirty = true;
            }

            // Left edge (insert at front, shift right)
            if (pos->pos.x < edgeThreshold && tilemap->width < tilemap->maxWidth) {
                int oldWidth = tilemap->width;
                tilemap->width += tilemap->chunkSize;
                tilemap->tiles.resize(tilemap->width * tilemap->height);

                // Shift existing tiles right by chunkSize
                for (int y = 0; y < tilemap->height; ++y) {
                    std::memmove(&tilemap->tiles[y * tilemap->width + tilemap->chunkSize], &tilemap->tiles[y * oldWidth], sizeof(Tile) * oldWidth);
                }

                std::thread genThread([this, tilemap] {
                    tileMutex.lock();
                    generateChunk(*tilemap, -1, 0, perlin);
                    tileMutex.unlock();
                });
                genThread.join();
                systemAutoTilingChunk(ecs, 0, 0, tilemap->chunkSize, tilemap->height - 1);
                // Adjust player pos (shift right)
                pos->pos.x += tilemap->chunkSize * tilemap->tileSize * tilemap->scale;
                dirty = true;
            }

            // Top edge (insert at front, shift down)
            if (pos->pos.y < edgeThreshold && tilemap->height < tilemap->maxHeight) {
                int oldHeight = tilemap->height;
                tilemap->height += tilemap->chunkSize;
                tilemap->tiles.resize(tilemap->width * tilemap->height);

                // Shift existing tiles down by chunkSize rows
                for (int y = oldHeight - 1; y >= 0; --y) {
                    std::memmove(&tilemap->tiles[(y + tilemap->chunkSize) * tilemap->width], &tilemap->tiles[y * tilemap->width], sizeof(Tile) * tilemap->width);
                }

                std::thread genThread([this, tilemap] {
                    tileMutex.lock();
                    generateChunk(*tilemap, 0, -1, perlin);
                    tileMutex.unlock();
                });
                genThread.join();
                systemAutoTilingChunk(ecs, 0, 0, tilemap->width - 1, tilemap->chunkSize);
                // Adjust player pos (shift down)
                pos->pos.y += tilemap->chunkSize * tilemap->tileSize * tilemap->scale;
                dirty = true;
            }

            if (dirty) {
                // No needed con chunk autotile
            }
        }
    }

}



void AdventureScene::render() {
    // Get camera
    auto* camComp = ecs.getComponent<CameraComp>(cameraEnt);
    if (camComp) {
        BeginMode2D(camComp->cam);
    }

    // Render map y entities
    systemRenderTileMap(ecs);
    systemRenderSprites(ecs);

    if (camComp) {
        EndMode2D();
    }

    // Health/Score (fixed UI, fuera de camera para no scroll)
    if (auto* health = ecs.getComponent<Health>(player)) {
        DrawText(TextFormat("Health: %.0f", health->value), 10, 30, 20, RED);
    }
    if (auto* score = ecs.getComponent<Score>(player)) {
        DrawText(TextFormat("Score: %d", score->value), 10, 50, 20, GREEN);
    }


}

void AdventureScene::clean() {
    
    // Similar para player (repíte código o haz función helper)
    if (auto* playerAnim = ecs.getComponent<Animation>(player)) {
        if (playerAnim->mode == AnimationMode::Sheet) {
            UnloadTexture(ecs.getComponent<Sprite>(player)->texture);
        } else {
            for (auto& [state, frames] : playerAnim->texStates) {
                for (auto& tex : frames) UnloadTexture(tex);
            }
        }
    }

    if (auto* tm = ecs.getComponent<TileMap>(tilemapEnt)) {
        UnloadTexture(tm->tileset);
    }


    if (auto* tm = ecs.getComponent<TileMap>(tilemapEnt)) {
        UnloadTexture(tm->tileset);
        UnloadTexture(tm->wallTex);
        UnloadTexture(tm->hazardTex);
        UnloadTexture(tm->pickupTex);
    }

}