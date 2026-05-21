Proyecto reorganizado: estructura LOR/

- LOR/src -> todos los .cpp
- LOR/include -> todos los .h
- LOR/main.cpp
- LOR/CMakeLists.txt

Compila desde la raíz de `LOR` con:

cmake -S . -B build && cmake --build build --config Release
