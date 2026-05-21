Proyecto reorganizado: estructura LOR/

- LOR/src -> todos los .cpp
- LOR/include -> todos los .h
- LOR/main.cpp
- LOR/CMakeLists.txt

Compila desde la raíz de `LOR` con:

cmake -S . -B build && cmake --build build --config Release

# Desarrollos innovadores implementados

## 1. Sistema de guardado y carga de partida
Permite persistir el estado completo del juego utilizando archivos de texto.

### Funciones principales
- `saveGame()`
- `loadGame()`

### Características
- Guarda posición del jugador.
- Restaura enemigos y objetos.
- Conserva estados de visibilidad y progreso.

### Archivo
- `game.cpp`

---

## 2. Sistema de niebla de guerra
Implementa visibilidad limitada alrededor del jugador para aumentar la exploración.

### Funciones principales
- `revealArea()`
- Uso de `known`

### Características
- Visibilidad circular dinámica.
- Oculta zonas no exploradas.
- Revela áreas conforme el jugador avanza.

### Archivos
- `map.cpp`
- `game.h`

---

## 3. Minimapa dinámico
Se añadió un minimapa renderizado en tiempo real.

### Función principal
- `drawMinimap()`

### Características
- Vista resumida del mapa.
- Actualización dinámica.
- Posicionado al lado derecho de la interfaz.

### Archivo
- `game.cpp`

---

## 4. Interfaz avanzada de consola
Sistema de renderizado optimizado para mejorar estabilidad visual.

### Métodos utilizados
- `WriteConsoleOutputCharacterA`
- Render buffered `draw()`
- Render por línea `drawMap()`

### Características
- Paneles informativos.
- Instrucciones en pantalla.
- Reducción de parpadeo.
- Mejor experiencia visual.

### Archivo
- `game.cpp`

---

## 5. Sistema de sonidos y feedback audible
Se implementó retroalimentación sonora utilizando la consola de Windows.

### Funciones principales
- `beepShort()`
- `beepLong()`

### Características
- Sonidos de eventos importantes.
- Confirmaciones auditivas.
- Alertas para daño y acciones especiales.

### Archivo
- `game.cpp`

---

## 6. Sistema avanzado de ítems
Se añadieron múltiples tipos de objetos interactivos.

### Tipos implementados
- `$` → Llave
- `S` → Espada
- `T` → Libreta/Tesoro
- `P` → Poción

### Características
- Campos personalizados:
  - `name`
  - `special`
- Objetos con habilidades únicas.

### Archivos
- `entity.h`
- `game.cpp`

---

## 7. Sistema de enemigos extendido
Se implementaron múltiples tipos de enemigos con IA diferenciada.

### Tipos de enemigos
- `Chaser`
- `Stalker`
- `Shadow`
- `Boss`

### Características
- Persecución inteligente.
- Movimiento por patrones.
- Enemigos sombra.
- Jefe con comportamiento especial.

### Archivos
- `entity.h`
- `game.cpp`

---

## 8. Ataque de área con espada
La espada permite eliminar enemigos cercanos.

### Función principal
- `killNearbyEnemies()`

### Características
- Ataque en casillas adyacentes.
- Eliminación instantánea.
- Mejora del combate.

### Archivo
- `game.cpp`

---

## 9. Sistema de estados y mensajes dinámicos
Sistema textual para informar eventos importantes al jugador.

### Función principal
- `setStatus()`

### Características
- Mensajes contextuales.
- Avisos de daño.
- Notificaciones de victoria/derrota.
- Información al recoger objetos.

### Archivo
- `game.cpp`

---

## 10. Sistema de salas dinámicas
Detección automática de habitaciones y eventos asociados.

### Funciones principales
- `updateCurrentRoom()`
- `drawRoomTitle()`

### Características
- Títulos dinámicos.
- Detección de sala actual.
- Activación de eventos automáticos.

### Archivos
- `game.cpp`
- `map.cpp`

---

## 11. Sistema de puertas dinámicas
Permite desbloquear rutas dentro del mapa.

### Función principal
- `openDoor(int x, int y)`

### Características
- Conversión de muros en caminos transitables.
- Apertura automática mediante eventos.

### Archivo
- `map.cpp`

---

## 12. Sistema de salida y portal final
Implementación de condición de victoria mediante portal.

### Características
- Casilla especial `X`.
- Victoria condicionada al tesoro.
- Portal interactivo.

### Archivos
- `map.cpp`
- `game.cpp`

---

## 13. Configuración avanzada de consola
Ajustes automáticos del tamaño y buffer de consola.

### Funciones utilizadas
- `SetConsoleScreenBufferSize`
- `SetConsoleWindowInfo`

### Características
- Mejor adaptación visual.
- Mayor estabilidad gráfica.

### Archivo
- `game.cpp`

---

## 14. Compatibilidad multiplataforma
Soporte para diferentes compiladores y sistemas.

### Compatibilidad
- `g++`
- `cl`
- `CMake`

### Características
- Scripts de compilación.
- Instrucciones multiplataforma.

### Archivos
- `CMakeLists.txt`
- `README.md`

---

## 15. Uso de estructuras estáticas
Implementación sin memoria dinámica.

### Estructuras utilizadas
- `enemies[MAX_ENEMIES]`
- `items[MAX_ITEMS]`

### Características
- Evita `new/delete`.
- Mejor control de memoria.
- Cumplimiento de requisitos académicos.

### Archivo
- `game.h`

---

## 16. Menús e interacción de usuario
Sistema de navegación interactiva mediante consola.

### Funciones principales
- `showMenu()`
- `showInstructions()`

### Características
- Menú principal.
- Pantalla de ayuda.
- Lectura bloqueante de opciones.

### Archivo
- `game.cpp`

---

## 17. Soporte para entidades de ancho doble
Permite enemigos y objetos de mayor tamaño.

### Característica principal
- Uso de `isDouble`

### Características
- Entidades de 2 casillas horizontales.
- Renderizado especial.
- Mejor variedad visual.

### Archivos
- `entity.h`
- `game.cpp`
