#include "OtherFunctions.hpp"
#include "GameState.hpp"
#include "Settings.hpp"

#include <string>
#include <fstream>

//void gameTime(Uint64& startTime, TTF_Font* font, SDL_Texture* textTexture, SDL_Surface* textSurface, SDL_Renderer* renderer)
//{
//	Uint64 currentTime = SDL_GetTicks();
//	Uint64 elapsedTime = (currentTime - startTime) / 1000;
//	std::string timeText = "Time: " + std::to_string(elapsedTime) + " seconds";
//	textSurface = TTF_RenderText_Solid(font, timeText.c_str(), textColor);
//	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
//	SDL_DestroySurface(textSurface);
//}

extern GameStateRunning gameState;

void RenderGameOverScreen(SDL_Renderer* renderer, Player* player, const Uint64& gameOverTime, const Uint64& startTime)
{
    // Ustaw kolor tła na czarny
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Ustaw kolor tekstu na biały
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Renderowanie napisu "Game Over"
    TTF_Font* font = TTF_OpenFont("Poppins-Bold.ttf", 48);
    if (!font)
    {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return;
    }

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Game Over", 0, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    float textWidth = textSurface->w;
    float textHeight = textSurface->h;
    SDL_FRect textRect = { (WINDOW_WIDTH - textSurface->w) / 2, WINDOW_HEIGHT / 4, textWidth, textHeight };
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);

    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Renderowanie iloci zabójstw
    std::string killsText = "Kills: " + std::to_string(player->kills);
    textSurface = TTF_RenderText_Solid(font, killsText.c_str(), 0, textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    textRect = { (WINDOW_WIDTH - textWidth) / 2, WINDOW_HEIGHT / 2, textWidth, textHeight };
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);

    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Renderowanie czasu spędzonego w grze
    const Uint64 elapsedTime = (gameOverTime - startTime) / 1000;
    std::string timeText = "Time: " + std::to_string(elapsedTime) + " seconds";
    textSurface = TTF_RenderText_Solid(font, timeText.c_str(), 0, textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    textRect = { (WINDOW_WIDTH - textWidth) / 2, (WINDOW_HEIGHT * 3) / 4, textWidth, textHeight };
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);

    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);

    TTF_CloseFont(font);
}

void showFPS(SDL_Renderer* renderer, Uint64& lastFrameTime, Uint64& frameCount, double& fps, TTF_Font* font)
{
    static Uint64 lastTime = SDL_GetPerformanceCounter();
    static Uint64 frequency = SDL_GetPerformanceFrequency();

    Uint64 currentTime = SDL_GetPerformanceCounter();
    frameCount++;

    if (currentTime - lastTime >= frequency) // Aktualizacja co sekundę
    {
        fps = frameCount / ((currentTime - lastTime) / static_cast<double>(frequency));
        lastTime = currentTime;
        frameCount = 0;
    }

    std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, fpsText.c_str(), 0, textColor);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture)
    {
        SDL_DestroySurface(textSurface);
        return;
    }

    SDL_FRect textRect = { 10.0f, 10.0f, static_cast<float>(textSurface->w), static_cast<float>(textSurface->h) };
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);

    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void saveGameTime(Player*& player, const Uint64& startTime)
{
    Uint64 currentTime = SDL_GetTicks();
    Uint64 elapsedTime = (currentTime - startTime) / 1000;
    player->totalTime += elapsedTime; // Dodanie upływajšcego czasu do totalTime
    SDL_Log("Saving totalTime: %lu", player->totalTime);
    std::ofstream saveFile("game_time.dat", std::ios::binary);
    if (saveFile.is_open())
    {
        saveFile.write(reinterpret_cast<char*>(&player->totalTime), sizeof(player->totalTime));
        saveFile.close();
    }
}

void loadGameTime(Player*& player)
{
    std::ifstream loadFile("game_time.dat", std::ios::binary);
    if (loadFile.is_open())
    {
        loadFile.read(reinterpret_cast<char*>(&player->totalTime), sizeof(player->totalTime));
        loadFile.close();
        SDL_Log("Loaded totalTime: %lu", player->totalTime);
    }
    else
    {
        player->totalTime = 0; // Inicjalizacja totalTime na 0, jeli plik nie istnieje
        SDL_Log("No save file found. Initializing totalTime to 0.");
    }
}
