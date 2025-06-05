#define SDL_HINT_RENDER_VSYNC "SDL_RENDER_VSYNC"
#include "Init.hpp"
#include "Settings.hpp"
#include "Map.hpp"
#include "Camera.hpp"
#include "EnemyManager.hpp"

#include <iostream>

SDL_AppResult InitEverything(SDL_Renderer*& renderer, SDL_Window*& window, Player*& player, Map*& map, Camera*& camera, EnemyManager*& enemyManager, TTF_Font*& font, TTF_Font*& bigFont,
    Uint64& startTime, Uint64& lastTime, void** appstate)
{
    SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

    if (!SDL_Init(SDL_INIT_VIDEO)) // Inicjalizacja wideo SDL
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    InitializeSettings();

    window = SDL_CreateWindow("Game", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN);
    if (!window)
    {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
    {
        SDL_Log("Couldn't create renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Log("Creating map...");
    map = new Map(mapWidth, mapHeight); // Tworzenie mapy
    if (!map->LoadTexture(renderer, "Images/mapa.png"))
    {
        SDL_Log("Couldn't load map texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!map->LoadCollisionSurface("Images/mapa_.png")) // Nałorzenie kolizji za pomocš mapy
    {
        SDL_Log("Couldn't load collision surface");
        return SDL_APP_FAILURE;
    }

    SDL_Log("Creating camera...");
    camera = new Camera(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); // Tworzenie kamery względem okna
    if (!camera)
    {
        SDL_Log("Couldn't create camera");
        return SDL_APP_FAILURE;
    }
    SDL_Log("Creating player...");

    player = new Player(map, camera, renderer); // Tworzenie Gracza
    if (!player)
    {
        SDL_Log("Couldn't create player");
        return SDL_APP_FAILURE;
    }

    player->SetPosition(512.0f, 1024.0f); // Ustawienie pozycji gracza na mapie

    SDL_Log("Creating enemies...");
    enemyManager = new EnemyManager(player, map, camera, renderer); // Twrozenie wroga
    if (!enemyManager)
    {
        SDL_Log("Couldn't create enemy manager");
        return SDL_APP_FAILURE;
    }

    LoadAnimationFrames(renderer);

    SDL_Log("Initialization complete");

    lastTime = SDL_GetTicks(); // Pobranie ostatniego czasu
    startTime = SDL_GetTicks(); // Pobranie czasu rozpoczęcia gry

    mapCollisions(map, renderer); // Kolizja za pomocš kwadratów

    font = TTF_OpenFont("Poppins-Bold.ttf", 24);
    if(!font)
    {
        SDL_Log("Can't open font: %s",SDL_GetError());
        return SDL_APP_FAILURE;
    };
    
    bigFont = TTF_OpenFont("Poppins-Bold.ttf", 50);
    if(!bigFont)
    {
        SDL_Log("Can't open bigFont: %s",SDL_GetError());
        return SDL_APP_FAILURE;
    };

    return SDL_APP_CONTINUE;
}
