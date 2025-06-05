#include "PlayerEventHandling.hpp"
#include <SDL3/SDL.h>
#include "Player.hpp"
#include "Enemy.hpp"
#include <vector>
#include <memory>
#include <cmath>

#include <iostream> // Dodaj, aby zobaczyæ wyjœcie

void PlayerEventHandling(SDL_Event* event, Player* player, std::vector<std::unique_ptr<Enemy>>& enemies, SDL_Renderer* renderer)
{
    static bool upPressed = false;
    static bool downPressed = false;
    static bool leftPressed = false;
    static bool rightPressed = false;
    static float lastDirX = 1.0f;
    static float lastDirY = 0.0f;
    static bool isAttacking = false;

    if (event == nullptr || player == nullptr)
    {
        SDL_Log("Invalid event or player pointer.");
        return;
    }

    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        switch (event->key.scancode)
        {
        case SDL_SCANCODE_UP:
		case SDL_SCANCODE_W:
            upPressed = true;
            break;
        case SDL_SCANCODE_DOWN:
		case SDL_SCANCODE_S:
            downPressed = true;
            break;
        case SDL_SCANCODE_LEFT:
		case SDL_SCANCODE_A:
            leftPressed = true;
            break;
        case SDL_SCANCODE_RIGHT:
		case SDL_SCANCODE_D:
            rightPressed = true;
            break;
        case SDL_SCANCODE_Z:
        case SDL_SCANCODE_SPACE:
            if (!isAttacking)
            {
                isAttacking = true;
                float dirX = 0.0f;
                float dirY = 0.0f;

                if (upPressed && leftPressed)
                {
                    dirX = -1.0f;
                    dirY = -1.0f;
                }
                else if (upPressed && rightPressed)
                {
                    dirX = 1.0f;
                    dirY = -1.0f;
                }
                else if (downPressed && leftPressed)
                {
                    dirX = -1.0f;
                    dirY = 1.0f;
                }
                else if (downPressed && rightPressed)
                {
                    dirX = 1.0f;
                    dirY = 1.0f;
                }
                else if (upPressed)
                {
                    dirY = -1.0f;
                }
                else if (downPressed)
                {
                    dirY = 1.0f;
                }
                else if (leftPressed)
                {
                    dirX = -1.0f;
                }
                else if (rightPressed)
                {
                    dirX = 1.0f;
                }
                else
                {
                    dirX = lastDirX;
                    dirY = lastDirY;
                }

                if (dirX == 0.0f && dirY == 0.0f)
                {
                    dirX = -1.0f;
                    dirY = -1.0f;
                }

                player->attack(renderer, enemies, dirX, dirY);
            }
            break;
        default:
            break;
        }
    }
    else if (event->type == SDL_EVENT_KEY_UP)
    {
        switch (event->key.scancode)
        {
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_W:
            upPressed = false;
            break;
        case SDL_SCANCODE_DOWN:
        case SDL_SCANCODE_S:
            downPressed = false;
            break;
        case SDL_SCANCODE_LEFT:
		case SDL_SCANCODE_A:
            leftPressed = false;
            break;
        case SDL_SCANCODE_RIGHT:
		case SDL_SCANCODE_D:
            rightPressed = false;
            break;
        case SDL_SCANCODE_Z:
        case SDL_SCANCODE_SPACE:
            isAttacking = false;
            break;
        default:
            break;
        }
    }

    float velocityX = (rightPressed - leftPressed) * player->speed;
    float velocityY = (downPressed - upPressed) * player->speed;

    if (velocityX != 0 || velocityY != 0)
    {
        float length = std::sqrt(velocityX * velocityX + velocityY * velocityY);
        lastDirX = velocityX / length;
        lastDirY = velocityY / length;
    }

    player->SetVelocity(velocityX * player->velocityBoost, velocityY * player->velocityBoost);
}
