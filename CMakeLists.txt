cmake_minimum_required(VERSION 3.10)
project(GameProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# SDL3 + SDL3_image + SDL3_ttf (poprawnie, przez find_package)
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

set(SOURCES
    main.cpp
    Init.cpp
    GameState.cpp
    Settings.cpp
    Map.cpp
    Player.cpp
    PlayerEventHandling.cpp
    Enemy.cpp
    EnemyManager.cpp
    Camera.cpp
    Buttons.cpp
    OtherFunctions.cpp
)

set(HEADERS
    Init.hpp
    GameState.hpp
    Settings.hpp
    Map.hpp
    Player.hpp
    PlayerEventHandling.hpp
    Enemy.hpp
    EnemyManager.hpp
    Camera.hpp
    Buttons.hpp
    OtherFunctions.hpp
)

add_executable(Game ${SOURCES} ${HEADERS})

target_link_libraries(Game
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
)

