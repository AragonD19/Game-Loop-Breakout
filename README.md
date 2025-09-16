✨ Características Principales
Core Engine

ECS Ligero: Templates para components dinámicos (add/remove/query en O(1)). Soporta queries genéricas para systems (e.g., getAllEntities() para editor).
Escenas Modulares: Switching dinámico via std::unique_ptr<Scene> (Menu → Breakout → Adventure). Cada escena maneja su ECS propio.
Editor ImGui Runtime: Pausa/resume, lista de entidades por escena (categorizada por components), inspector para tweak Position/Velocity/Scale (hot-reload vibes).
Input y Movimiento: Teclado (WASD/arrows), AI patrol simple, clamping borders. Delta-time para 60 FPS estables.

Breakout (Escena Clásica)

Paddle controlado con teclado (izquierda/derecha).
Movimiento basado en tiempo (delta time).
Pelota que rebota en paddle, paredes y techo.
Aumento progresivo de velocidad en rebotes (1.05x factor).
Colisiones AABB con bloques destructibles (remueve on-hit).
Win condition: Todos bloques destruidos. Game Over: Pelota toca bottom.

Sprites y Rendering (Engine Highlight)

Sprites Híbridos: Struct Sprite soporta sheets (DrawTextureRec con frames) y separate textures (DrawTextureEx). Incluye tint, origin, y escala Vector2 tweakable (default 1x, e.g., 16x16 → 64x64 para visibilidad en 800x600).
Animaciones Estatales: Component Animation con modos Sheet/Separate. Estados como "idle", "walk_left/right" via std::unordered_map<std::string, std::vector<Rectangle/Texture2D>>. Frame timing (0.15s default), loop suave.
Render System: systemRenderSprites usa Raylib's DrawTexturePro/Ex para scale/rotation (GPU-accelerated, zero overhead). Prioriza entities con Position + Sprite.
Carga Dinámica: Assets desde assets/ (copiados a build/ post-compile). Soporte para backgrounds tiled (e.g., DrawTextureEx full-screen).
Editor Integration: Tweak scale/tint/isSheet en inspector – ve cambios realtime (ideal para artist tweaks en proto).


📁 Estructura del Proyecto
text

## 📹 Demostración

**Video del juego:**

[![Watch the video](https://img.youtube.com/vi/zHB_1_eOeiU/0.jpg)](https://youtu.be/zHB_1_eOeiU)

---
