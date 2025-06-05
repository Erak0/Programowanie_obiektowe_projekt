#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Player;
class Map;
class Camera;
class EnemyManager;

SDL_AppResult InitEverything(SDL_Renderer*& renderer, SDL_Window*& window, Player*& player, Map*& map, Camera*& camera, EnemyManager*& enemyManager, TTF_Font*& font, TTF_Font*& bigFont, Uint64& startTime, Uint64& lastTime, void** appstate);