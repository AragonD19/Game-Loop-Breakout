#!/bin/bash

# Nombre del ejecutable
EXECUTABLE="GAME"

# Directorio donde se hará el build
BUILD_DIR="build"

# Crear carpeta de build si no existe
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

# Entrar a la carpeta de build
cd "$BUILD_DIR" || exit 1

# Ejecutar CMake y compilar con make
echo "==> Generando proyecto con CMake..."
cmake .. || { echo "CMake falló"; exit 1; }

echo "==> Compilando..."
make || { echo "Compilación falló"; exit 1; }

# Ejecutar el programa
echo "==> Ejecutando $EXECUTABLE..."
./$EXECUTABLE

