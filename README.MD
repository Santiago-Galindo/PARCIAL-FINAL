# Integrantes

- Daniel Santiago Galindo Velásquez
- Santiago Jimenez Toledo

---

# Desarrollos innovadores implementados

-Guardado/Carga: saveGame() / loadGame() 
-Niebla de guerra
-Archivo: map.cpp, game.h.
-Minimapa
-Interfaz consola avanzada
-Sonidos/beeps
-Archivo: game.cpp.
-Sistema de items variados
-Lógica de enemigos extendida
-Ataque de área al usar espada
-Archivo: game.cpp.
-Sistema de estado
-Archivo: game.cpp.
-Detección de sala
-Archivo: game.cpp, map.cpp.
-Puertas dinámicas: 
-Archivo: map.cpp.
-Salida/portal marcado: 
-Archivo: map.cpp y game.cpp.
-Ajustes de consola (tamaño/atributos): 
-Archivo: game.cpp.
-Soporte para múltiples compiladores en scripts de ayuda: CMakeLists.txt y README con instrucciones y comandos alternativos (g++, cl). Archivo: CMakeLists.txt y ---README.md.
-Estructura de datos estática
-Archivo: game.h.
-Mensajes y menú interactivo: showMenu() / showInstructions() y lectura bloqueante de entrada para opciones de usuario. Archivo: game.cpp.
-Soporte para items/entidades con ancho doble: Archivo: entity.h, game.cpp.

# Compilación

## Windows

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Linux

```bash
mkdir build
cd build
cmake ..
make
```

## MacOS

```bash
mkdir build
cd build
cmake ..
make
```
