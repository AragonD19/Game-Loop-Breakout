## 🕹️ Características

- Paddle controlado con teclado (izquierda y derecha).
- Movimiento basado en tiempo (delta time).
- Pelota que rebota en el paddle, paredes y techo.
- Aumento progresivo de la velocidad en cada rebote.
- Colisiones con bloques destructibles.
- Fin del juego si la pelota toca el límite derecho, izquierdo o inferior.

---

## 📁 Estructura del Proyecto

```
GAME/
    src/
        main.cpp
        systems.cpp
        Game.cpp
    src/editor/
        Editor.cpp  
    include/
        Game.h
        ecs.h
        systems.h
        components.h
        print.h
    include/editor/
        Editor.h   
    external/
        imgui/      # Clonado de repo
        rlImGui/    # Clonado de repo
    build/
    CMakeLists.txt
```

---

## 🚀 Compilación y Ejecución

Requisitos:
- CMake
- raylib (puedes instalarlo con tu gestor de paquetes o desde [raylib.com](https://www.raylib.com/))

Para compilar y ejecutar:

```bash
chmod +x run.sh
./run.sh
```

---

## 📹 Demostración

**Video del juego:**

[![Watch the video](https://img.youtube.com/vi/zHB_1_eOeiU/0.jpg)](https://youtu.be/zHB_1_eOeiU)

---
