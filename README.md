# Game-Loop-Breakout

Este es un proyecto en C++ usando la biblioteca [raylib](https://www.raylib.com/) que simula un juego clásico tipo **Breakout**. El jugador controla una paleta (paddle) para hacer rebotar una pelota y destruir todos los bloques de la pantalla.

## 🎮 Características

- Movimiento del paddle con teclado (izquierda/derecha).
- Pelota que rebota en los bordes, el paddle y los bloques.
- Eliminación de bloques al ser golpeados.
- Mensaje de victoria al eliminar todos los bloques.
- Mensaje de derrota si la pelota toca el fondo.
- Movimiento basado en tiempo (`delta time`).
- Paddle limitado a los bordes de la pantalla.

## 🧱 Estructura del proyecto

```
GAME/
├── include/
│   ├── Game.h
│   └── print.h
├── src/
│   └── main.cpp
├── CMakeLists.txt
├── run.sh
└── README.md
```

## ⚙️ Requisitos

- CMake (versión 3.0+)
- Compilador C++ con soporte para C++23
- [raylib](https://github.com/raysan5/raylib) instalado en el sistema

### En Linux

Puedes instalar raylib con:

```bash
sudo apt install libraylib-dev
```

## 🚀 Compilación y ejecución

Desde la raíz del proyecto:

```bash
./run.sh
```

Este script crea el directorio `build/`, ejecuta `cmake`, compila el proyecto y lanza el ejecutable.

## 🎮 Controles

- ⬅️ Flecha izquierda: mover el paddle a la izquierda  
- ➡️ Flecha derecha: mover el paddle a la derecha

## 🏁 Objetivo

Haz rebotar la pelota con el paddle para destruir todos los bloques.  
Pierdes si la pelota toca la parte inferior de la pantalla.  
¡Ganas si destruyes todos los bloques!

## 📄 Licencia

Este proyecto es educativo y no tiene licencia específica. Puedes modificarlo y usarlo libremente.
