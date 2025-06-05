#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

//SDL
#include "Settings.hpp" // Postawowe ustwienia
#include "Player.hpp" // Gracz
#include "PlayerEventHandling.hpp" // Przechwytywanie przycisków
#include "Map.hpp" // Mapa
#include "Camera.hpp" // Kamera
#include "Enemy.hpp" // Przeciwnik
#include "EnemyManager.hpp" // Menedżer przeciwników (Dodawnie konkretnej iloci)
#include "OtherFunctions.hpp" // Inne funkcje
#include "GameState.hpp"
#include "Init.hpp"

//OpenGL

#include <vector>
#include <memory>
#include <iostream>

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static SDL_Event lastEvent;

Map* map = nullptr; // wskanik mapa
Camera* camera = nullptr; // wskanik kamera
Player* player = nullptr; // wskanik gracz
EnemyManager* enemyManager = nullptr; // wskanik przeciwnik
TTF_Font* font = nullptr; // wskanik czcionka
TTF_Font* bigFont = nullptr; // wskanik czcionka

Uint64 lastTime = 0;
Uint64 startTime = 0;
Uint64 lastUpdateTime = 0;

Uint64 lastFrameTimeFPS = SDL_GetTicks();
Uint64 frameCountFPS = 0;
double fps = 0.0;

GameStateRunning gameState = GameStateRunning::MENU;
GameStateRunning previousState = gameState; // Dodanie zmiennej previousState

void resetLastTime() 
{
    lastTime = SDL_GetTicks();
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    InitEverything(renderer, window, player, map, camera, enemyManager, font, bigFont, startTime, lastTime, appstate);
    loadPlayerStats(player);
    loadGameTime(player);

	return SDL_APP_CONTINUE;  /* continue running the program. */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    lastEvent = *event; // Zapisujemy event

    if (event->type == SDL_EVENT_QUIT)  // Zamknięcie po nacinięciu przycisku 'X'
    {
        savePlayerStats(player);
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

	if (event->type == SDL_EVENT_KEY_DOWN) // Zamknięcie po przycinięciu na klawiaturze ESCAPE
	{
        if (event->key.key == SDLK_ESCAPE)
        {
            if (gameState != GameStateRunning::MENU)
            {
                gameState = GameStateRunning::MENU;
            }
            else if (gameState != GameStateRunning::GAME)
            {
                gameState = GameStateRunning::GAME;
                SDL_Log("Wznowiono gre (GAME)");
                resetLastTime(); // Resetowanie lastTime przy przejciu do GAME
            }
        }
	}

    if (gameState == GameStateRunning::GAME && !player->isGameOver)
    {
        PlayerEventHandling(event, player, enemyManager->enemies, renderer); // Funkcja zawierajšca przechwytywanie WASD do poruszania się gracza
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    Uint32 frameStart = SDL_GetTicks();

    if (gameState != previousState)
    {
        if (gameState == GameStateRunning::GAME)
        {
            resetLastTime(); // Resetowanie lastTime przy przejciu do GAME
        }
        previousState = gameState;
    }

    if (gameState == GameStateRunning::GAME)
    {
        Uint64 currentTime = SDL_GetTicks();
        Uint64 deltaTime = (currentTime - lastUpdateTime) / 1000; // Oblicz różnicę czasu w sekundach
        player->totalTime += deltaTime; // Dodaj różnicę czasu do totalTime
        lastUpdateTime = currentTime; // Zaktualizuj lastUpdateTime
    }

    switch (gameState)
    {
    case GameStateRunning::GAME:
        gameRunning(renderer, player, map, camera, enemyManager, startTime, lastTime, lastEvent, font, appstate, gameState);
        showFPS(renderer, lastFrameTimeFPS, frameCountFPS, fps, font);
        break;

    case GameStateRunning::MENU:
        gameMenu(renderer, lastEvent, bigFont, player, map, enemyManager);
        break;

    case GameStateRunning::GAMEOVER:
        GameOver(renderer, font, player, lastTime, startTime);
        savePlayerStats(player);
        break;

    case GameStateRunning::STATS:
        gameStats(renderer, font, player);
        break;

    case GameStateRunning::LEVELUP:
        levelUp(renderer, font, player, lastEvent, gameState, enemyManager);
        break;

    case GameStateRunning::EXIT:
        saveGameTime(player, startTime);
        savePlayerStats(player);
        return SDL_APP_SUCCESS;
        break;

    default:
        return SDL_APP_FAILURE;
        break;
    }

    SDL_RenderPresent(renderer); // Renderowanie całoci

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    // Zwolnienie wskaników
    SDL_Log("Cleaning up...");
    delete player;
    delete map;
    delete camera;
    delete enemyManager;

    Cleanup();
}
