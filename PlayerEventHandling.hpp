#pragma once

// Obs³uga zdarzeñ

#include <SDL3/SDL.h>
#include "Player.hpp"
#include "Enemy.hpp"
#include <vector>
#include <memory>

void PlayerEventHandling(SDL_Event* event, Player* player, std::vector<std::unique_ptr<Enemy>>& enemies, SDL_Renderer* renderer);
