#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

class Player;
class Map;
class Camera;
class EnemyManager;

enum class GameStateRunning
{
    MENU,
    GAME,
    GAMEOVER,
    PAUSE,
	STATS,
    SETTINGS,
    LEVELUP,
    EXIT
};

void LoadAnimationFrames(SDL_Renderer* renderer);
void Cleanup();

SDL_AppResult gameRunning(SDL_Renderer* renderer, Player* player, Map* map, Camera* camera, EnemyManager* enemyManager,
    const Uint64& startTime, Uint64& lastTime, SDL_Event& event, TTF_Font* font, void* appstate, GameStateRunning currentState);

void GameOver(SDL_Renderer* renderer, TTF_Font* font, Player* player, Uint64& endTime, Uint64& startTime);

void gameMenu(SDL_Renderer* renderer, SDL_Event& event, TTF_Font* font, Player* player, Map* map, EnemyManager* enemyManager);

void gamePause(SDL_Renderer* renderer, TTF_Font* font);

void gameSettings();

void gameCurrentStats(SDL_Renderer* renderer, TTF_Font* font, Player* player);

void levelUp(SDL_Renderer* renderer, TTF_Font* font, Player*& player, SDL_Event& event, GameStateRunning& currentState, EnemyManager* enemyManager);

void saveGameState(Player* player, Map* map, EnemyManager* enemyManager);
void loadGameState(Player* player, Map* map, EnemyManager* enemyManager, SDL_Renderer* renderer);
void savePlayerStats(Player* player);
void loadPlayerStats(Player* player);
void resetLastTime();
void gameStats(SDL_Renderer* renderer, TTF_Font* font, Player* player);
void updateButtonPositions();

void statTemplate(SDL_Renderer* renderer, TTF_Font* font, Player* player, std::string statText, float offSet, long int value);
void statTemplate(SDL_Renderer* renderer, TTF_Font* font, Player* player, std::string statText, float offSet, Uint64 value);

extern bool isFullscreen;