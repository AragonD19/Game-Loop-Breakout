## Características Principales

### Core Engine
- **ECS Ligero**: Templates para components dinámicos (add/remove/query en O(1)). Soporta queries genéricas para systems (e.g., `getAllEntities()` para editor).
- **Escenas Modulares**: Switching dinámico via `std::unique_ptr<Scene>` (Menu → Breakout → Adventure). Cada escena maneja su ECS propio.
- **Editor ImGui Runtime**: Pausa/resume, lista de entidades por escena (categorizada por components), inspector para tweak Position/Velocity/Scale (hot-reload vibes).
- **Input y Movimiento**: Teclado (WASD/arrows), AI patrol simple, clamping borders. Delta-time para 60 FPS estables.

### Breakout
- Paddle controlado con teclado (izquierda/derecha).
- Movimiento basado en tiempo (delta time).
- Pelota que rebota en paddle, paredes y techo.
- Aumento progresivo de velocidad en rebotes (1.05x factor).
- Colisiones AABB con bloques destructibles (remueve on-hit).
- Win condition: Todos bloques destruidos. Game Over: Pelota toca bottom.

### Sprites y Rendering
- **Sprites Híbridos**: Struct `Sprite` soporta sheets (DrawTextureRec con frames) y separate textures (DrawTextureEx). Incluye tint, origin, y **escala Vector2 tweakable** (default 1x, e.g., 16x16 → 64x64 para visibilidad en 800x600).
- **Animaciones Estatales**: Component `Animation` con modos Sheet/Separate. Estados como "idle", "walk_left/right" via `std::unordered_map<std::string, std::vector<Rectangle/Texture2D>>`. Frame timing (0.15s default), loop suave.
- **Render System**: `systemRenderSprites` usa Raylib's `DrawTexturePro/Ex` para scale/rotation (GPU-accelerated, zero overhead). Prioriza entities con Position + Sprite.
- **Carga Dinámica**: Assets desde `assets/` (copiados a build/ post-compile). Soporte para backgrounds tiled (e.g., DrawTextureEx full-screen).
- **Editor Integration**: Tweak scale/tint/isSheet en inspector – ve cambios realtime (ideal para artist tweaks en proto).

---

## 📁 Estructura del Proyecto

```
GAME/
├── assets/                 # Texturas PNG (e.g., MagoIdle1.png, BackGround.png)
├── src/
│   ├── main.cpp            # Entry point: Game loop
│   ├── systems.cpp         # Systems ECS (input, movement, render, animation)
│   ├── Game.cpp            # Core: Scene switching, update/render
│   ├── scenes/             # Escenas modulares
│   │   ├── AdventureScene.cpp  # Player animado, AI patrol
│   │   ├── BreakoutScene.cpp   # Paddle/ball/blocks
│   │   └── MenuScene.cpp       # UI simple (SPACE to start)
│   └── editor/
│       └── Editor.cpp      # ImGui: Entities list, inspector
├── include/
│   ├── Game.h              # Game class
│   ├── ecs.h               # ECS templates (getAllEntities para editor)
│   ├── systems.h           # Decls systems (e.g., systemRenderSprites)
│   ├── components.h        # Structs: Sprite (scale), Animation (states)
│   ├── print.h             # Variadic debug print (Raylib overloads)
│   ├── Scene.h             # Base para escenas
│   ├── scenes/             # Headers escenas
│   │   ├── AdventureScene.h
│   │   ├── BreakoutScene.h
│   │   └── MenuScene.h
│   └── editor/
│       └── Editor.h        # GUI controls
├── external/
│   ├── imgui/              # Clonado: UI toolkit
│   └── rlImGui/            # Clonado: Raylib + ImGui bridge
├── build/                  # Generado: Exe + assets copy
├── run.sh                  # Script: CMake + make + run (con -c/-v flags)
└── CMakeLists.txt          # Build: GLOB sources, copy assets post-build
```

---

## Compilación y Ejecución

### Requisitos (Linux Mint)
- **CMake 3.28+**: `sudo apt install cmake`
- **Raylib**: `sudo apt install libraylib3-dev` (o build from [raylib.com](https://www.raylib.com/) para latest).
- **ImGui/rlImGui**: Clona repos en `external/` (git clone https://github.com/ocornut/imgui.git; git clone https://github.com/raylib-extras/rlImGui.git).
- **GCC 13+**: Default en Mint 22, con `-std=c++23` para variadics lambdas.

### Build y Run
Usa el script `run.sh` para full flow (crea build/, cmake, make -j, copy assets, run). Assets se copian auto a `build/assets/` para paths relativos.

```bash
chmod +x run.sh
./run.sh          # Build + run (fast iter)
./run.sh -c       # Clean rebuild (post-header changes)
./run.sh -v       # Verbose: Ve copy assets y logs
```

- **Output**: Ventana 800x600@60FPS. SPACE en menú → Adventure (WASD mueve mago animado, scale 4x). ImGui: Pausa, tweak entities.
- **Debug**: Valgrind para leaks (`valgrind --leak-check=full build/GAME`), o GDB (`gdb build/GAME`).

---

## Sprites y Animaciones: El Corazón del Rendering Engine

Para un engine 2D, sprites son el MVP: flexibles, performant (Raylib GPU batching), y tweakables. Aquí el breakdown técnico – expande con AssetManager singleton para cache/unload global.

### Components Clave (components.h)
```cpp
struct Sprite {
    Texture2D texture;
    Rectangle frameRec = {0, 0, 16, 16};  // Sheet frames
    Vector2 origin = {0, 0};
    Color tint = WHITE;
    bool isSheet = true;  // Sheet vs. separate
    Vector2 scale = {1.0f, 1.0f};  // Escala: {4,4} para 16x16 → 64x64
};

struct Animation {
    std::string currentState = "idle";  // e.g., "walk_left"
    AnimationMode mode = AnimationMode::Sheet;  // Sheet/Separate
    std::unordered_map<std::string, std::vector<Rectangle>> rectStates;  // Sheet
    std::unordered_map<std::string, std::vector<Texture2D>> texStates;   // Separate
    int currentFrame = 0;
    float frameTime = 0.15f;  // Segs por frame
    float timer = 0.0f;
};
```
- **Uso**: En setup() escena: `ecs.addComponent(player, Sprite{tex}); ecs.getComponent<Sprite>(player)->scale = {4.0f, 4.0f};`

### Systems (systems.cpp)
- **systemAnimationUpdate(ECS& ecs, float dt)**: Avanza timer, switch frame por state (loop % size). Set `sprite.frameRec` o `sprite.texture` basado en mode.
- **systemRenderSprites(ECS& ecs)**: Loop por Sprite + Position. Usa:
  - Sheet: `DrawTexturePro(texture, frameRec, scaledDest, origin, 0, tint);` (scale en dest rec).
  - Separate: `DrawTextureEx(texture, pos, 0, scale.x, tint);` (uniform scale).
- **Perf Tip**: Para 100+ entities, agrupa calls en Raylib batch (future: rlgl.h custom shader).

### Carga y Cleanup (AdventureScene.cpp)
- **Load**: `LoadTexture("assets/MagoIdle1.png")` – cachea en `texStates["idle"] = {idle1, idle2, ...};`.
- **Unload**: En clean(): Loop `texStates` y `UnloadTexture(tex)` (híbrido maneja sheet vs. separate). Editor tweaks no leak (ref-only).
- **Input Link**: `systemInput` setea state ("walk_right" si vel.x >0) → anima auto.

### Editor Tweaks (Editor.cpp)
- Lista entidades por component (e.g., "Players" via InputControlled).
- Inspector: `ImGui::InputFloat2("Scale", (float*)&sprite->scale);` – ve sprite resize realtime.

**Expansión Engine**: Agrega `AssetManager` singleton para paths dinámicos (`/proc/self/exe` en Mint) y cache (`std::unordered_map<std::string, Texture2D>`). Para parallax backgrounds o particle sprites, hookea en `systemRenderSprites`.

---

## 📹 Demostración

**Video del juego:**

[![Watch the video](https://img.youtube.com/vi/zHB_1_eOeiU/0.jpg)](https://youtu.be/zHB_1_eOeiU)

---
