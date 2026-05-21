cmake_minimum_required(VERSION 3.10)
project(DungeonCrawler LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(DungeonCrawler
    main.cpp
    src/game.cpp
    src/map.cpp
    src/entity.cpp
)
