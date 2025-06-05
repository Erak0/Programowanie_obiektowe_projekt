#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <functional>
#include <iostream>

#include "GameState.hpp"
#include "Settings.hpp"

class Player;
class Map;
class EnemyManager;

class Button
{
public:
    Button(float x, float y, float w, float h, SDL_Color color, std::function<void()> onClick)
        : rect{ x * scaleX, y * scaleY, w , h }, color(color), onClick(onClick), isPressed(false), texture(nullptr), scale(1.0f), textOffsetX(0), textOffsetY(0)
    {
    }

    void SetTexture(SDL_Texture* tex)
    {
        texture = tex;
    }

    void SetScale(float newScale)
    {
        scale = newScale;
    }

    void SetTextOffset(float offsetX, float offsetY)
    {
        textOffsetX = offsetX;
        textOffsetY = offsetY;
    }

    void Render(SDL_Renderer* renderer, TTF_Font* font, const char* name, Player* player, Map* map, EnemyManager* enemyManager)
    {
        SDL_FRect scaledRect = { rect.x, rect.y, rect.w * scaleX * scale, rect.h * scaleY * scale };

        if (texture)
        {
            SDL_RenderTexture(renderer, texture, NULL, &scaledRect);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &scaledRect);
        }

        SDL_Color textColor = { 0, 0, 0, 255 };

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, name, 0, textColor);
        if (!textSurface)
        {
            SDL_Log("Unable to render text surface: %s", SDL_GetError());
            return;
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (!textTexture)
        {
            SDL_Log("Unable to create texture from rendered text: %s", SDL_GetError());
        }

        SDL_FRect dst;
        dst.w = textSurface->w;
        dst.h = textSurface->h;
        dst.x = (scaledRect.x + (scaledRect.w - textSurface->w) / 2.0f) + textOffsetX;
        dst.y = (scaledRect.y + (scaledRect.h - textSurface->h) / 2.0f) + textOffsetY;

        SDL_RenderTexture(renderer, textTexture, NULL, &dst);
        SDL_DestroyTexture(textTexture);
        SDL_DestroySurface(textSurface);
    }

    void handleClick(const SDL_Event& event)
    {
        float mouseX = event.button.x;
        float mouseY = event.button.y;

        SDL_FRect scaledRect = { rect.x, rect.y, rect.w * scaleX * scale, rect.h * scaleY * scale };
        bool isInside = mouseX >= scaledRect.x && mouseX <= scaledRect.x + scaledRect.w &&
            mouseY >= scaledRect.y && mouseY <= scaledRect.y + scaledRect.h;

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && isInside)
        {
            isPressed = true;
        }

        else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && isPressed && isInside)
        {
            isPressed = false;
            if (onClick)
            {
                onClick();
            }
        }
    }

    void SetPosition(float x, float y)
    {
        rect.x = x * scaleX;
        rect.y = y * scaleY;
    }

    SDL_FRect GetFRect() const
    {
        return rect;
    }

private:
    SDL_FRect rect;
    SDL_Color color;
    std::function<void()> onClick;
    bool isPressed;
    SDL_Texture* texture;
    float scale;
    float textOffsetX;
    float textOffsetY;
};