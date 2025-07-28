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
├── include/         # Archivos .h (ECS, Game, Systems)
├── src/             # Archivos .cpp
├── build/           # Directorio de build
├── run.sh           # Script para compilar y ejecutar
├── CMakeLists.txt   # Configuración de CMake
└── README.md
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

![Demo del juego]((https://youtu.be/zHB_1_eOeiU))

---
