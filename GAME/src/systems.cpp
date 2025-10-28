// src/systems.cpp
#include "systems.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <utility>
#include <cstdlib> 
#include <cstdint>


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


// systemInput: Limita a left/right anims
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

        // Set estado: Solo anima left/right; up/down puro va a idle
        if (vel->vel.x > 0) anim->currentState = "walk_right";
        else if (vel->vel.x < 0) anim->currentState = "walk_left";
        else anim->currentState = "idle";  // Incluye si solo up/down o parado
    }
}


void systemMovement(ECS& ecs, float dt) {
    for (auto& [entity, pos] : ecs.getComponentMap<Position>()) {
        auto* vel = ecs.getComponent<Velocity>(entity);
        if (!vel) continue;

        Vector2 newPos = { pos.pos.x + vel->vel.x * dt, pos.pos.y + vel->vel.y * dt };

        bool canMove = true;
        for (auto& [mapEnt, tilemap] : ecs.getComponentMap<TileMap>()) {
            int tx = (int)floor((newPos.x + 8.0f * tilemap.scale) / (tilemap.tileSize * tilemap.scale));
            int ty = (int)floor((newPos.y + 8.0f * tilemap.scale) / (tilemap.tileSize * tilemap.scale));
            if (tx < 0 || tx >= tilemap.width || ty < 0 || ty >= tilemap.height) {
                canMove = false;
                break;
            }

            int index = ty * tilemap.width + tx;
            if (tilemap.tiles[index].value == IntGridValue::NON_WALKABLE) {
                canMove = false;
                break;
            }
        }

        if (canMove) {
            pos.pos = newPos;
        } else {
            vel->vel = {0, 0};  // Stop
        }

    }
}

void systemAI(ECS& ecs, float dt) {
    for (auto& [entity, patrol] : ecs.getComponentMap<AIPatrol>()) {
        auto* vel = ecs.getComponent<Velocity>(entity);
        auto* anim = ecs.getComponent<Animation>(entity);
        if (!vel || !anim) continue;

        vel->vel.x = patrol.speed * patrol.direction;
        vel->vel.y = 0;  // Solo horizontal por simplicidad

        patrol.traveled += fabs(vel->vel.x) * dt;
        if (patrol.traveled >= patrol.patrolDistance) {
            patrol.direction = -patrol.direction;
            patrol.traveled = 0;
        }

        // Set estado basado en dirección
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
            Rectangle destRec = {drawPos.x, drawPos.y, 
                                 sprite.frameRec.width * sprite.scale.x, 
                                 sprite.frameRec.height * sprite.scale.y};
            DrawTexturePro(sprite.texture, sprite.frameRec, destRec, {0,0}, 0.0f, sprite.tint);
        } else {
            Rectangle destRec = {drawPos.x, drawPos.y, 
                                 (float)sprite.texture.width * sprite.scale.x, 
                                 (float)sprite.texture.height * sprite.scale.y};
            Rectangle srcRec = {0, 0, (float)sprite.texture.width, (float)sprite.texture.height};
            DrawTexturePro(sprite.texture, srcRec, destRec, {0,0}, 0.0f, sprite.tint);
        }
    }
}



// Mappings de bitmask a frames (adaptado de tu código, asume tileset layout compatible)
std::map<uint8_t, std::vector<std::pair<int, int>>> tileMappings = {
    {  2, {{   0,  80 }} }, // north
    {  8, {{  48,  96 }} },
    { 10, {{  80, 112 }} },
    { 11, {{  48,  80 }} },
    { 16, {{   0,  96 }} },
    { 18, {{  64, 112 }} },
    { 22, {{  16,  80 }} },
    { 24, {{  16,  96 }, {  32,  96 }} },
    { 26, {{ 144,  32 }} },
    { 27, {{ 144,  80 }} },
    { 30, {{  96,  80 }} },
    { 31, {{  32,  80 }} },
    { 64, {{   0,  32 }} },
    { 66, {{   0,  48 }, {   0,  64 }} },
    { 72, {{  80,  96 }} },
    { 74, {{ 128,  32 }} },
    { 75, {{ 112,  80 }} },
    { 80, {{  64,  96 }} },
    { 82, {{ 144,  48 }} },
    { 86, {{ 128,  80 }} },
    { 88, {{ 128,  48 }} },
    { 90, {{   0, 112 }, {  16, 112 }} },
    { 91, {{  32, 112 }} },
    { 94, {{  96,  48 }} },
    { 95, {{  96, 112 }} },
    {104, {{  48,  48 }} },
    {106, {{ 144,  64 }} },
    {107, {{  48,  64 }} },
    {120, {{ 112,  64 }} },
    {122, {{  48, 112 }} },
    {123, {{ 112, 112 }} },
    {126, {{  48, 112 }} },
    {127, {{  64,  64 }} },
    {208, {{  16,  48 }} },
    {210, {{  96,  64 }} },
    {214, {{  16,  64 }} },
    {216, {{ 128,  64 }} },
    {218, {{  96,  32 }} },
    {219, {{  32, 112 }} },
    {222, {{  96,  96 }} },
    {223, {{  80,  64 }} },
    {248, {{  32,  48 }} },
    {250, {{ 112,  96 }} },
    {251, {{  64,  80 }} },
    {254, {{  80,  80 }} },
    {255, {
        {   0,   0 }, {  16,   0 }, {  32,   0 }, {  48,   0 }, {  64,   0 }, {  80,   0 },
        {   0,  16 }, {  16,  16 }, {  32,  16 }, {  48,  16 }, {  64,  16 }, {  80,  16 },
        {  32,  64 }
    }},
    {  0, {
        { 16,  32 }, {  32,  32 }, {  48,  32 },
        { 64,  32 }, {  80,  32 }, {  64,  48 }, {  80,  48 }
    }}
};


// Directions para neighbors (8 dirs)
const int dx[8] = { -1,  0,  1, -1, 1, -1, 0, 1 }; 
const int dy[8] = { -1, -1, -1,  0, 0,  1, 1, 1 };


// Para corner validation (adaptado)
std::map<uint8_t, std::pair<int, int>> d_corner = {
    {0, {  1,  1 }},  // Nota: Ajusté indices a 0-based para dirs (0=NE? Ajusta si needed)
    {2, { -1,  1 }},
    {5, {  1, -1 }},
    {7, { -1, -1 }},
};


void systemAutoTiling(ECS& ecs) {
    for (auto& [entity, tilemap] : ecs.getComponentMap<TileMap>()) {
        if (tilemap.tiles.size() != tilemap.width * tilemap.height) continue;

        for (int y = 0; y < tilemap.height; ++y) {
            for (int x = 0; x < tilemap.width; ++x) {
                int index = y * tilemap.width + x;
                Tile& tile = tilemap.tiles[index];
                if (tile.value == IntGridValue::HAZARD) {
                    tile.specialTex = tilemap.hazardTex;
                    tile.frame = {0, 0};  // No usa tileset
                    continue;
                } else if (tile.value == IntGridValue::PICKUP) {
                    tile.specialTex = tilemap.pickupTex;
                    tile.frame = {0, 0};
                    continue;
                } else if (tile.value == IntGridValue::NON_WALKABLE) {
                    tile.specialTex = tilemap.wallTex;  // Optional: Si quieres specials para walls, sino set {0}
                    if (tile.specialTex.id != 0) continue;  // Skip bitmask si special
                }

                if (tile.value == IntGridValue::WALKABLE) {
                    std::vector<std::pair<int, int>> groundVariants = {
                        {0, 0}, {16, 0}, {0, 16}, {16, 16}
                    };
                    int randIdx = GetRandomValue(0, groundVariants.size() - 1);
                    tile.frame.x = static_cast<float>(groundVariants[randIdx].first);
                    tile.frame.y = static_cast<float>(groundVariants[randIdx].second);
                    std::cout << "Walkable frame: (" << tile.frame.x << "," << tile.frame.y << ")" << std::endl;  // Debug qué pickea
                    continue;
                }

                if (!tile.needsAutoTiling) {
                    tile.frame = {0.0f, 0.0f};
                    continue;
                }

                uint8_t bitmask = 0;
                IntGridValue type = tile.value;

                for (int i = 0; i < 8; ++i) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (nx < 0 || nx >= tilemap.width || ny < 0 || ny >= tilemap.height) continue;

                    // Corner validation para diagonals (0,2,5,7)
                    if (i == 0 || i == 2 || i == 5 || i == 7) {
                        auto it = d_corner.find(i);
                        if (it != d_corner.end()) {
                            int cx = it->second.first;
                            int cy = it->second.second;
                            int nx1 = x + cx;  // Desde x original
                            int ny1 = y;
                            int nx2 = x;
                            int ny2 = y + cy;

                            if (nx1 >= 0 && nx1 < tilemap.width && ny1 >= 0 && ny1 < tilemap.height &&
                                nx2 >= 0 && nx2 < tilemap.width && ny2 >= 0 && ny2 < tilemap.height) {
                                int idx1 = ny1 * tilemap.width + nx1;
                                int idx2 = ny2 * tilemap.width + nx2;
                                if (tilemap.tiles[idx1].value != type || tilemap.tiles[idx2].value != type) {
                                    continue;  // Skip diagonal si adjacents no match
                                }
                            } else {
                                continue;
                            }
                        }
                    }

                    int nIdx = ny * tilemap.width + nx;
                    if (tilemap.tiles[nIdx].value == type) bitmask |= (1 << i);
                }

                auto it = tileMappings.find(bitmask);
                if (it == tileMappings.end()) {
                    tile.frame = {0.0f, 0.0f};  // Fallback
                    std::cerr << "Bitmask not found: " << static_cast<int>(bitmask) << std::endl;
                } else {
                    auto& variants = it->second;
                    int randIdx = GetRandomValue(0, variants.size() - 1);
                    tile.frame.x = static_cast<float>(variants[randIdx].first);
                    tile.frame.y = static_cast<float>(variants[randIdx].second);
                }
            }
        }
    }
}



void systemRenderTileMap(ECS& ecs) {
    for (auto& [entity, tilemap] : ecs.getComponentMap<TileMap>()) {
        // Get camera para culling (asumiendo una—itera si multi)
        CameraComp* camComp = nullptr;
        for (auto& [camEnt, comp] : ecs.getComponentMap<CameraComp>()) {
            camComp = &comp;
            break;
        }
        if (!camComp) {
            // Fallback no culling
            // ... draw full como antes
            return;
        }

        // Precompute scaled tile size
        const float scaledTile = tilemap.tileSize * tilemap.scale;

        // Camera view bounds in world space
        Vector2 camMin = { camComp->cam.target.x - (GetScreenWidth() / 2.0f) / camComp->cam.zoom, 
                           camComp->cam.target.y - (GetScreenHeight() / 2.0f) / camComp->cam.zoom };
        Vector2 camMax = { camMin.x + GetScreenWidth() / camComp->cam.zoom, 
                           camMin.y + GetScreenHeight() / camComp->cam.zoom };

        // Tile range visible
        int startX = std::max(0, (int)floor(camMin.x / scaledTile));
        int startY = std::max(0, (int)floor(camMin.y / scaledTile));
        int endX = std::min(tilemap.width - 1, (int)ceil(camMax.x / scaledTile));
        int endY = std::min(tilemap.height - 1, (int)ceil(camMax.y / scaledTile));

        for (int y = startY; y <= endY; ++y) {
            for (int x = startX; x <= endX; ++x) {
                int index = y * tilemap.width + x;
                // ... en loop
                Tile& tile = tilemap.tiles[index];

                Rectangle dest = { x * scaledTile, y * scaledTile, scaledTile, scaledTile };

                if (tile.specialTex.id != 0) {  // Prioridad: Usa separate tex full (no src sub-rect)
                    Rectangle src = {0, 0, (float)tile.specialTex.width, (float)tile.specialTex.height};  // Full tex
                    DrawTexturePro(tile.specialTex, src, dest, {0,0}, 0.0f, WHITE);
                #ifdef DEBUG
                    std::cout << "Rendering special at (" << x << "," << y << ") with tex ID " << tile.specialTex.id << std::endl;
                #endif
                } else {  // Fallback a tileset sub-rect
                    Rectangle src = { tile.frame.x, tile.frame.y, (float)tilemap.tileSize, (float)tilemap.tileSize };
                    DrawTexturePro(tilemap.tileset, src, dest, {0,0}, 0.0f, WHITE);
                }
            }
        }
    }
}



void systemTileInteractions(ECS& ecs, float dt) {
    for (auto& [entity, pos] : ecs.getComponentMap<Position>()) {
        if (!ecs.hasComponent<InputControlled>(entity)) continue;  // Solo player

        auto* health = ecs.getComponent<Health>(entity);
        auto* score = ecs.getComponent<Score>(entity);
        if (!health || !score) continue;

        for (auto& [mapEnt, tilemap] : ecs.getComponentMap<TileMap>()) {
            int tx = (int)floor((pos.pos.x + 8.0f * tilemap.scale) / (tilemap.tileSize * tilemap.scale));  // Center offset
            int ty = (int)floor((pos.pos.y + 8.0f * tilemap.scale) / (tilemap.tileSize * tilemap.scale));
            if (tx < 0 || tx >= tilemap.width || ty < 0 || ty >= tilemap.height) continue;

            int index = ty * tilemap.width + tx;
            Tile& tile = tilemap.tiles[index];

            switch (tile.value) {
                case IntGridValue::HAZARD:
                    health->value -= 10.0f * dt;  // Daño continuo
                    if (health->value <= 0) { std::cout << "Game Over!" << std::endl; }  // Placeholder
                    std::cout << "Damage! Health now: " << health->value << std::endl;  // Debug
                    break;
                case IntGridValue::PICKUP:
                    score->value += 10;
                    tile.value = IntGridValue::WALKABLE;  // Recolectar
                    std::cout << "Pickup! Score now: " << score->value << std::endl;  // Debug
                    break;
                default: break;
            }
        }
    }
}


void systemDebugIntGrid(ECS& ecs) {
    for (auto& [entity, tilemap] : ecs.getComponentMap<TileMap>()) {
        for (int y = 0; y < tilemap.height; ++y) {
            for (int x = 0; x < tilemap.width; ++x) {
                int index = y * tilemap.width + x;
                Tile& tile = tilemap.tiles[index];

                Rectangle dest = {
                    static_cast<float>(x * tilemap.tileSize) * tilemap.scale,
                    static_cast<float>(y * tilemap.tileSize) * tilemap.scale,
                    static_cast<float>(tilemap.tileSize) * tilemap.scale,
                    static_cast<float>(tilemap.tileSize) * tilemap.scale
                };

                Color color = {0,0,0,0};
                switch (tile.value) {
                    case IntGridValue::NON_WALKABLE: color = {255,0,0,128}; break;
                    case IntGridValue::HAZARD: color = {255,165,0,128}; break;
                    case IntGridValue::PICKUP: color = {0,255,0,128}; break;
                    default: continue;
                }
                DrawRectangleRec(dest, color);
            }
        }
    }
}



void systemCameraUpdate(ECS& ecs, float dt) {
    for (auto& [entity, camComp] : ecs.getComponentMap<CameraComp>()) {
        if (camComp.target == (Entity)-1) continue;

        auto* targetPos = ecs.getComponent<Position>(camComp.target);
        if (!targetPos) continue;

        // Simple follow: Center target
        Vector2 target = targetPos->pos;
        // Smooth lerp (opcional—quita para instant)
        camComp.cam.target.x = camComp.cam.target.x + (target.x - camComp.cam.target.x) * camComp.smoothSpeed;
        camComp.cam.target.y = camComp.cam.target.y + (target.y - camComp.cam.target.y) * camComp.smoothSpeed;

        // Bounds (opcional: clamp camera a map edges)
        // auto* tilemap = ecs.getComponent<TileMap>(someMapEnt); // Si tienes global tilemapEnt
        // if (tilemap) {
        //     float mapW = tilemap->width * tilemap->tileSize * tilemap->scale;
        //     float mapH = tilemap->height * tilemap->tileSize * tilemap->scale;
        //     float screenW = GetScreenWidth() / camComp.cam.zoom;
        //     float screenH = GetScreenHeight() / camComp.cam.zoom;
        //     camComp.cam.target.x = fmax(screenW / 2, fmin(camComp.cam.target.x, mapW - screenW / 2));
        //     camComp.cam.target.y = fmax(screenH / 2, fmin(camComp.cam.target.y, mapH - screenH / 2));
        // }
    }
}


void systemAutoTilingChunk(ECS& ecs, int startX, int startY, int endX, int endY) {
    for (auto& [entity, tilemap] : ecs.getComponentMap<TileMap>()) {
        // Clamp ranges to grid
        startX = std::max(0, startX);
        startY = std::max(0, startY);
        endX = std::min(tilemap.width - 1, endX);
        endY = std::min(tilemap.height - 1, endY);

        for (int y = startY; y <= endY; ++y) {
            for (int x = startX; x <= endX; ++x) {
                int index = y * tilemap.width + x;
                Tile& tile = tilemap.tiles[index];

                if (tile.value == IntGridValue::HAZARD) {
                    tile.specialTex = tilemap.hazardTex;
                    tile.frame = {0, 0};  // No usa tileset
                    continue;
                } else if (tile.value == IntGridValue::PICKUP) {
                    tile.specialTex = tilemap.pickupTex;
                    tile.frame = {0, 0};
                    continue;
                } else if (tile.value == IntGridValue::NON_WALKABLE) {
                    tile.specialTex = tilemap.wallTex;  // Optional: Si quieres specials para walls, sino set {0}
                    if (tile.specialTex.id != 0) continue;  // Skip bitmask si special
                }

                if (tile.value == IntGridValue::WALKABLE) {
                    std::vector<std::pair<int, int>> groundVariants = {
                        {0, 0}, {16, 0}, {0, 16}, {16, 16}
                    };
                    int randIdx = GetRandomValue(0, groundVariants.size() - 1);
                    tile.frame.x = static_cast<float>(groundVariants[randIdx].first);
                    tile.frame.y = static_cast<float>(groundVariants[randIdx].second);
                    std::cout << "Walkable frame: (" << tile.frame.x << "," << tile.frame.y << ")" << std::endl;  // Debug qué pickea
                    continue;
                }

                if (!tile.needsAutoTiling) {
                    tile.frame = {0.0f, 0.0f};
                    continue;
                }


                uint8_t bitmask = 0;
                IntGridValue type = tile.value;

                for (int i = 0; i < 8; ++i) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (nx < 0 || nx >= tilemap.width || ny < 0 || ny >= tilemap.height) continue;

                    // Corner validation para diagonals (0,2,5,7)
                    if (i == 0 || i == 2 || i == 5 || i == 7) {
                        auto it = d_corner.find(i);
                        if (it != d_corner.end()) {
                            int cx = it->second.first;
                            int cy = it->second.second;
                            int nx1 = x + cx;  // Desde x original
                            int ny1 = y;
                            int nx2 = x;
                            int ny2 = y + cy;

                            if (nx1 >= 0 && nx1 < tilemap.width && ny1 >= 0 && ny1 < tilemap.height &&
                                nx2 >= 0 && nx2 < tilemap.width && ny2 >= 0 && ny2 < tilemap.height) {
                                int idx1 = ny1 * tilemap.width + nx1;
                                int idx2 = ny2 * tilemap.width + nx2;
                                if (tilemap.tiles[idx1].value != type || tilemap.tiles[idx2].value != type) {
                                    continue;  // Skip diagonal si adjacents no match
                                }
                            } else {
                                continue;
                            }
                        }
                    }

                    int nIdx = ny * tilemap.width + nx;
                    if (tilemap.tiles[nIdx].value == type) bitmask |= (1 << i);
                }

                auto it = tileMappings.find(bitmask);
                if (it == tileMappings.end()) {
                    tile.frame = {0.0f, 0.0f};  // Fallback
                    std::cerr << "Bitmask not found: " << static_cast<int>(bitmask) << std::endl;
                } else {
                    auto& variants = it->second;
                    int randIdx = GetRandomValue(0, variants.size() - 1);
                    tile.frame.x = static_cast<float>(variants[randIdx].first);
                    tile.frame.y = static_cast<float>(variants[randIdx].second);
                }


            }
        }
    }
}
