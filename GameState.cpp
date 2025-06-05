#include "GameState.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "Settings.hpp" // Postawowe ustwienia
#include "Player.hpp" // Gracz
#include "PlayerEventHandling.hpp" // Przechwytywanie przycisków
#include "Map.hpp" // Mapa
#include "Camera.hpp" // Kamera
#include "Enemy.hpp" // Przeciwnik
#include "EnemyManager.hpp" // Menedżer przeciwników (Dodawnie konkretnej iloci)
#include "OtherFunctions.hpp" // Inne funkcje
#include "Buttons.hpp"

#include <vector>
#include <memory>
#include <iostream>
#include <fstream>

extern GameStateRunning gameState;
extern Uint64 lastTime;
extern void resetLastTime();

float yOffSet = 100.0f;
bool dropdownOpen = false;
bool vSyncOpen = false;
bool isLevelUp = true;
bool isFullscreen = false;
int selectedResolutionIndex = 3; 
int selectedFPSIndex = 1;

std::vector<SDL_Texture*> animationFrames;
int currentFrame = 0;
Uint64 lastFrameTime = 0;
const int frameDuration = 500;

Button startButton(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT / 8, 200, 80, {255, 0, 0, 255}, []()
{
    SDL_Log("Start Game!");
    gameState = GameStateRunning::GAME;
    resetLastTime(); // Resetowanie lastTime poprzez wywołanie funkcji
});

Button exitButton(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT / 24, 200, 80, { 255, 0, 0, 255 }, []()
{
    SDL_Log("Exit Game!");
    gameState = GameStateRunning::EXIT;
});

Button statsButton(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT / 24, 200, 80, { 255, 0, 0, 255 }, []()
{
    SDL_Log("Stats Game!");
	gameState = GameStateRunning::STATS;
});

Button settingsButton(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT / 24, 200, 80, { 255, 0, 0, 255 }, []()
{
    SDL_Log("Stats Game!");
    gameState = GameStateRunning::SETTINGS;
});

SDL_AppResult gameRunning(SDL_Renderer* renderer, Player* player, Map* map, Camera* camera, EnemyManager* enemyManager,
    const Uint64& startTime, Uint64& lastTime, SDL_Event& event, TTF_Font* font, void* appstate, GameStateRunning currentState)
{
    player->isGameStart = true;

    Uint64 currentTime = SDL_GetTicks();

    if (!player->isGameOver && currentState == GameStateRunning::GAME)
    {
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        enemyManager->Update(deltaTime, currentState, font);
        player->Update(deltaTime, currentState);

        camera->Update(player->GetX(), player->GetY());

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.w = WINDOW_WIDTH;
        viewport.h = WINDOW_HEIGHT;
        SDL_SetRenderViewport(renderer, &viewport);

        map->Render(renderer, camera->GetX(), camera->GetY());
        enemyManager->Render(renderer, currentState);
        map->RenderObjects(renderer, camera->GetX(), camera->GetY(), player, font);
        player->UpdateKillsTexture(renderer);

        SDL_FPoint playerPosition = { player->GetX(), player->GetY() };
        int playerX = static_cast<int>(playerPosition.x);
        int playerY = static_cast<int>(playerPosition.y);

        if (!map->IsPixelTransparent(playerX, playerY))
        {
            player->Update(-deltaTime, currentState);
        }

		gameCurrentStats(renderer, font, player);
    }

    return SDL_APP_CONTINUE;
}

void GameOver(SDL_Renderer* renderer, TTF_Font* font, Player* player, Uint64& endTime, Uint64& startTime) // Funkcja wywietlajšca napis Game Over
{
    player->isGameStart = false;

    SDL_Color textColor = { 255, 255, 255, 255 };

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Game Over", 0, textColor);
    if (!textSurface)
    {
        SDL_Log("Unable to render text surface: %s", SDL_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!texture)
    {
        SDL_Log("Unable to create texture from rendered text: %s", SDL_GetError());
    }

    SDL_FRect dst;
    dst.w = textSurface->w;
    dst.h = textSurface->h;
    dst.x = (WINDOW_WIDTH - dst.w) / 2;
    dst.y = (WINDOW_HEIGHT - dst.h) / 2;
    SDL_RenderTexture(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(textSurface);

    player->isGameOver = true;

    RenderGameOverScreen(renderer, player, endTime, startTime); // Renderowanie ekranu "Game Over"
}

void gameMenu(SDL_Renderer* renderer, SDL_Event& event, TTF_Font* bigFont, Player* player, Map* map, EnemyManager* enemyManager)
{
    SDL_RenderClear(renderer);

    if (player->isGameStart)
    {
        gamePause(renderer, bigFont);
    }

    float OffSet = 48.0f;
    float OffSetW = 64.0f;

    // Renderowanie animowanego tła
    Uint64 currentTime = SDL_GetTicks();
    if (currentTime > lastFrameTime + frameDuration)
    {
        currentFrame = (currentFrame + 1) % animationFrames.size();
        lastFrameTime = currentTime;
    }

    if (!animationFrames.empty())
    {
        SDL_Texture* currentTexture = animationFrames[currentFrame];
        float texW = 0, texH = 0;
        SDL_GetTextureSize(currentTexture, &texW, &texH);
        SDL_FRect dstRect = { WINDOW_WIDTH - texW, WINDOW_HEIGHT - texH, texW, texH };
        SDL_RenderTexture(renderer, currentTexture, NULL, &dstRect);
    }

    // Załaduj teksturę przycisku
    SDL_Texture* buttonTexture = IMG_LoadTexture(renderer, "Images/srodek_1.png");
    if (!buttonTexture)
    {
        SDL_Log("Failed to load button texture: %s", SDL_GetError());
    }

    SDL_Texture* leftButtonTexture = IMG_LoadTexture(renderer, "Images/lewo.png");
    if (!leftButtonTexture)
    {
        SDL_Log("Failed to load button texture: %s", SDL_GetError());
    }

    SDL_Texture* rightButtonTexture = IMG_LoadTexture(renderer, "Images/prawo.png");
    if (!rightButtonTexture)
    {
        SDL_Log("Failed to load button texture: %s", SDL_GetError());
    }

    SDL_Texture* badylTexture = IMG_LoadTexture(renderer, "Images/badyl.png");
    if (!badylTexture)
    {
        SDL_Log("Failed to load background texture: %s", SDL_GetError());
    }

    if (badylTexture)
    {
        float texW = 0;
        float texH = 0;
        float sizeScale = 2.0f; // Skala 2.0f
        SDL_GetTextureSize(badylTexture, &texW, &texH);

        // Poprawione obliczenia dla dstRect
        SDL_FRect dstRect = {
            (WINDOW_WIDTH - texW / sizeScale) / 2.0f, // x
            (WINDOW_HEIGHT - texH / sizeScale) + 16.0f, // y (wyrodkowane w pionie)
            texW / sizeScale, // szerokoć
            texH / sizeScale // wysokoć
        };

        SDL_SetTextureBlendMode(badylTexture, SDL_BLENDMODE_BLEND);
        SDL_RenderTexture(renderer, badylTexture, NULL, &dstRect);
    }

    SDL_Color black = { 0, 0, 0, 0 };

    // Ustaw teksturę dla przycisków
    startButton.SetTexture(rightButtonTexture);
    startButton.SetScale(2.0f); // Przykładowa skala 1.5x

    statsButton.SetTexture(buttonTexture);
    statsButton.SetScale(2.0f); // Przykładowa skala 1.5x

    settingsButton.SetTexture(buttonTexture);
    settingsButton.SetScale(2.0f); // Przykładowa skala 1.5x

    exitButton.SetTexture(leftButtonTexture);
    exitButton.SetScale(2.0f); // Przykładowa skala 1.5x

    // Renderowanie przycisku start
    SDL_Surface* startTextSurface = TTF_RenderText_Solid(bigFont, "start", 0, black);
    if (startTextSurface)
    {
        int startTextWidth = startTextSurface->w;
        SDL_DestroySurface(startTextSurface);
        startButton.SetPosition(WINDOW_WIDTH / 2 - startButton.GetFRect().h - OffSet - 48.0f, WINDOW_HEIGHT / 2 - startButton.GetFRect().h - OffSet * 2);
    }
    startButton.Render(renderer, bigFont, "start", player, map, enemyManager);
    startButton.handleClick(event);
    startButton.SetTextOffset(24.0f, 0.0f);

    // Renderowanie przycisku stats
    SDL_Surface* statsTextSurface = TTF_RenderText_Solid(bigFont, "stats", 0, black);
    if (statsTextSurface)
    {
        int statsTextWidth = statsTextSurface->w;
        SDL_DestroySurface(statsTextSurface);
        statsButton.SetPosition(WINDOW_WIDTH / 2 - statsButton.GetFRect().h - OffSet - OffSetW, WINDOW_HEIGHT / 2 - OffSet);
    }
    statsButton.Render(renderer, bigFont, "stats", player, map, enemyManager);
    statsButton.handleClick(event);

    // Renderowanei przycisku settings
    SDL_Surface* settingsTextSurface = TTF_RenderText_Solid(bigFont, "settings", 0, black);
    if (settingsTextSurface)
    {
        SDL_DestroySurface(settingsTextSurface);
        settingsButton.SetPosition(WINDOW_WIDTH / 2 - settingsButton.GetFRect().h - OffSet - OffSetW, WINDOW_HEIGHT / 2 + settingsButton.GetFRect().h);
    }
    settingsButton.Render(renderer, bigFont, "settings", player, map, enemyManager);
    settingsButton.handleClick(event);

    // Renderowanie przycisku exit
    SDL_Surface* exitTextSurface = TTF_RenderText_Solid(bigFont, "exit", 0, black);
    if (exitTextSurface)
    {
        int exitTextWidth = exitTextSurface->w;
        SDL_DestroySurface(exitTextSurface);
        exitButton.SetPosition(WINDOW_WIDTH / 2 - exitButton.GetFRect().h - OffSet - 64.0f, WINDOW_HEIGHT / 2 + exitButton.GetFRect().h + OffSet * 3.5f + 16.0f);
    }
    exitButton.Render(renderer, bigFont, "exit", player, map, enemyManager);
    exitButton.handleClick(event);
    startButton.SetTextOffset(-24.0f, 0.0f);

    // Zwolnij teksturę przycisku
    if (buttonTexture)
    {
        SDL_DestroyTexture(buttonTexture);
    }
    if (leftButtonTexture)
    {
        SDL_DestroyTexture(leftButtonTexture);
    }
    if (rightButtonTexture)
    {
        SDL_DestroyTexture(rightButtonTexture);
    }
    if (badylTexture)
    {
        SDL_DestroyTexture(badylTexture);
    }
}

void gamePause(SDL_Renderer* renderer, TTF_Font* font)
{
    // Renderowanie tła pauzy
    SDL_FRect pauseRect = { WINDOW_WIDTH / 2 - 100, 100, 200, 100 };
    SDL_RenderFillRect(renderer, &pauseRect);

    // Renderowanie napisu "PAUZA"
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "PAUZA", 0, white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FRect textRect = { pauseRect.x + 50, pauseRect.y + 25, 100, 50 };
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void gameStats(SDL_Renderer* renderer, TTF_Font* font, Player* player)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128); // Ustawienie koloru tła na czarny
    SDL_RenderClear(renderer);
    SDL_FRect statsRect = { WINDOW_WIDTH / 2 - 100, 100, 200, 100 };
    SDL_RenderFillRect(renderer, &statsRect);

    SDL_Color white = { 255, 255, 255, 255 };
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, "STATS", 0, white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FRect textRect = { statsRect.x + 50, statsRect.y + 25, 100, 50 };
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);

    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);

    statTemplate(renderer, font, player, "Total Kills", 0, player->totalKills);
    statTemplate(renderer, font, player, "Total Deaths", 2, player->totalDeaths);
	statTemplate(renderer, font, player, "Total Time", -2, player->totalTime / 100);
}

void statTemplate(SDL_Renderer* renderer, TTF_Font* font, Player* player, std::string statText, float offSet, long int value)
{
    SDL_FRect rect = { WINDOW_WIDTH / 2 - 100, 100, 200, 100 };

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, statText.c_str(), 0, white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FRect textRect = { rect.x + 50, rect.y + 25, 100, 50 };

    float textWidth = textSurface->w;
    float textHeight = textSurface->h;

    std::string deathsText = statText + ": " + std::to_string(value);
    textSurface = TTF_RenderText_Solid(font, deathsText.c_str(), 0, white);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    textRect = { (WINDOW_WIDTH - textWidth) / 2, WINDOW_HEIGHT / 2 - textHeight * offSet, textWidth, textHeight };
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

void statTemplate(SDL_Renderer* renderer, TTF_Font* font, Player* player, std::string statText, float offSet, Uint64 value)
{
    SDL_FRect rect = { WINDOW_WIDTH / 2 - 100, 100, 200, 100 };

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, statText.c_str(), 0, white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FRect textRect = { rect.x + 50, rect.y + 25, 100, 50 };

    float textWidth = textSurface->w;
    float textHeight = textSurface->h;

    std::string deathsText = statText + ": " + std::to_string(value) + " s.";
    textSurface = TTF_RenderText_Solid(font, deathsText.c_str(), 0, white);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    textRect = { (WINDOW_WIDTH - textWidth) / 2, WINDOW_HEIGHT / 2 - textHeight * offSet, textWidth, textHeight };
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

// Dodanie funkcji gameSettings
void gameSettings()
{

}

void gameCurrentStats(SDL_Renderer* renderer, TTF_Font* font, Player* player)
{
    float size = 1.5f;

	SDL_Color black = { 0, 0, 0, 255 };

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Current Stats", 0, {0, 0, 0, 255} );
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FRect textRect = { 5, 50, static_cast<float>(textSurface->w), static_cast<float>(textSurface->h) };
	SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
	SDL_DestroySurface(textSurface);

	std::string hpText = "Max Health: " + std::to_string(static_cast<int>(player->maxHealth));
	SDL_Surface* hpSurface = TTF_RenderText_Solid(font, hpText.c_str(), 0, black);
	SDL_Texture* hpTexture = SDL_CreateTextureFromSurface(renderer, hpSurface);
    SDL_FRect hpRect = { 5, 100, static_cast<float>(hpSurface->w) / size, static_cast<float>(hpSurface->h) / size };
	SDL_RenderTexture(renderer, hpTexture, NULL, &hpRect);
	SDL_DestroySurface(hpSurface);

	std::string rangeText = "Range: " + std::to_string(static_cast<int>(player->attackRange));
	SDL_Surface* rangeSurface = TTF_RenderText_Solid(font, rangeText.c_str(), 0, black);
	SDL_Texture* rangeTexture = SDL_CreateTextureFromSurface(renderer, rangeSurface);
    SDL_FRect rangeRect = { 5, 150, static_cast<float>(rangeSurface->w) / size, static_cast<float>(rangeSurface->h) / size };
	SDL_RenderTexture(renderer, rangeTexture, NULL, &rangeRect);
	SDL_DestroySurface(rangeSurface);

	std::string speedText = "Speed: " + std::to_string(static_cast<int>(player->speed));
	SDL_Surface* speedSurface = TTF_RenderText_Solid(font, speedText.c_str(), 0, black);
	SDL_Texture* speedTexture = SDL_CreateTextureFromSurface(renderer, speedSurface);
	SDL_FRect speedRect = { 5, 200, static_cast<float>(speedSurface->w) / size, static_cast<float>(speedSurface->h) / size };
	SDL_RenderTexture(renderer, speedTexture, NULL, &speedRect);
	SDL_DestroySurface(speedSurface);

	std::string damageText = "Damage: " + std::to_string(static_cast<int>(player->damage));
	SDL_Surface* damageSurface = TTF_RenderText_Solid(font, damageText.c_str(), 0, black);
	SDL_Texture* damageTexture = SDL_CreateTextureFromSurface(renderer, damageSurface);
	SDL_FRect damageRect = { 5, 250, static_cast<float>(damageSurface->w) / size, static_cast<float>(damageSurface->h) / size };
	SDL_RenderTexture(renderer, damageTexture, NULL, &damageRect);
	SDL_DestroySurface(damageSurface);
}

void updateButtonPositions()
{
    float OffSet = 24.0f;

    startButton.SetPosition(WINDOW_WIDTH / 2 - startButton.GetFRect().w / 2, WINDOW_HEIGHT / 2 - startButton.GetFRect().h - OffSet);
    statsButton.SetPosition(WINDOW_WIDTH / 2 - statsButton.GetFRect().w / 2, WINDOW_HEIGHT / 2);
    settingsButton.SetPosition(WINDOW_WIDTH / 2 - settingsButton.GetFRect().w / 2, WINDOW_HEIGHT / 2 + settingsButton.GetFRect().h + OffSet);
    exitButton.SetPosition(WINDOW_WIDTH / 2 - exitButton.GetFRect().w / 2, WINDOW_HEIGHT / 2 + exitButton.GetFRect().h + OffSet * 5 + 8.0f);
}

void levelUp(SDL_Renderer* renderer, TTF_Font* font, Player*& player, SDL_Event& event, GameStateRunning& currentState, EnemyManager* enemyManager)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_FRect levelUpRect = { WINDOW_WIDTH / 2 - 150, 100, 300, 100 };
    SDL_RenderFillRect(renderer, &levelUpRect);

    // Renderowanie napisu "PAUZA"
	SDL_Color gold = { 255, 215, 0, 255 };
    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "LEVEL UP!", 0, gold);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FRect textRect = { levelUpRect.x, levelUpRect.y, 300, 50 };
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Więcej zdrowia
    SDL_FRect healthBoostRect = { WINDOW_WIDTH / 4 - WINDOW_WIDTH / 6, WINDOW_HEIGHT / 2, 180, 180 };
    SDL_RenderFillRect(renderer, &healthBoostRect);

	// Większy zasięg
    SDL_FRect rangeBoostRect = { WINDOW_WIDTH / 4 * 2 - WINDOW_WIDTH / 6, WINDOW_HEIGHT / 2, 180, 180 };
    SDL_RenderFillRect(renderer, &rangeBoostRect);

	// Większa prędkoć
    SDL_FRect speedBoostRect = { WINDOW_WIDTH / 4 * 3 - WINDOW_WIDTH / 6, WINDOW_HEIGHT / 2, 180, 180 };
    SDL_RenderFillRect(renderer, &speedBoostRect);

	// Większe obrażenia
    SDL_FRect damageBoostRect = { WINDOW_WIDTH / 4  * 4 - WINDOW_WIDTH / 6, WINDOW_HEIGHT / 2, 180, 180 };
    SDL_RenderFillRect(renderer, &damageBoostRect);

	// Renderowanie upgrade zdrowia
	SDL_Surface* healthTextSurface = TTF_RenderText_Solid(font, "Health Boost", 0, black);
	SDL_Texture* healthTexture = SDL_CreateTextureFromSurface(renderer, healthTextSurface);
	SDL_FRect healthTextRect = { healthBoostRect.x, healthBoostRect.y + 180, 180, 50 };

	SDL_RenderTexture(renderer, healthTexture, NULL, &healthTextRect);
	SDL_DestroySurface(healthTextSurface);
	SDL_DestroyTexture(healthTexture);

	// Renderowanie upgrade zasięgu
	SDL_Surface* rangeTextSurface = TTF_RenderText_Solid(font, "Range Boost", 0, black);
	SDL_Texture* rangeTexture = SDL_CreateTextureFromSurface(renderer, rangeTextSurface);
	SDL_FRect rangeTextRect = { rangeBoostRect.x, rangeBoostRect.y + 180, 180, 50 };

	SDL_RenderTexture(renderer, rangeTexture, NULL, &rangeTextRect);
	SDL_DestroySurface(rangeTextSurface);
	SDL_DestroyTexture(rangeTexture);

	// Renderowanie upgrade prędkoci
	SDL_Surface* speedTextSurface = TTF_RenderText_Solid(font, "Speed Boost", 0, black);
	SDL_Texture* speedTexture = SDL_CreateTextureFromSurface(renderer, speedTextSurface);
	SDL_FRect speedTextRect = { speedBoostRect.x, speedBoostRect.y + 180, 180, 50 };

	SDL_RenderTexture(renderer, speedTexture, NULL, &speedTextRect);
	SDL_DestroySurface(speedTextSurface);
	SDL_DestroyTexture(speedTexture);

	// Renderowanie upgrade obrażeń
	SDL_Surface* damageTextSurface = TTF_RenderText_Solid(font, "Damage Boost", 0, black);
	SDL_Texture* damageTexture = SDL_CreateTextureFromSurface(renderer, damageTextSurface);
	SDL_FRect damageTextRect = { damageBoostRect.x, damageBoostRect.y + 180, 180, 50 };

	SDL_RenderTexture(renderer, damageTexture, NULL, &damageTextRect);
	SDL_DestroySurface(damageTextSurface);
	SDL_DestroyTexture(damageTexture);

    int mouseX = event.button.x;
    int mouseY = event.button.y;
        
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (mouseX >= healthBoostRect.x && mouseX <= healthBoostRect.x + healthBoostRect.w &&
            mouseY >= healthBoostRect.y && mouseY <= healthBoostRect.y + healthBoostRect.h)
        {
            player->maxHealth *= 1.15f;
			SDL_Log("Health Boost! %f", player->health);
            isLevelUp = false;
        }

        if (mouseX >= rangeBoostRect.x && mouseX <= rangeBoostRect.x + rangeBoostRect.w &&
            mouseY >= rangeBoostRect.y && mouseY <= rangeBoostRect.y + rangeBoostRect.h)
        {
            player->attackRange *= 1.1f;
			SDL_Log("Range Boost!");
            isLevelUp = false;
        }

        if (mouseX >= speedBoostRect.x && mouseX <= speedBoostRect.x + speedBoostRect.w &&
            mouseY >= speedBoostRect.y && mouseY <= speedBoostRect.y + speedBoostRect.h)
        {
			player->speed *= 1.05f;
			SDL_Log("Speed Boost!");
            isLevelUp = false;
        }

        if (mouseX >= damageBoostRect.x && mouseX <= damageBoostRect.x + damageBoostRect.w &&
            mouseY >= damageBoostRect.y && mouseY <= damageBoostRect.y + damageBoostRect.h)
        {
            player->damage *= 1.15f;
			SDL_Log("Damage Boost!");
            isLevelUp = false;
        }
    }

    if (!isLevelUp)
    {
        player->health = player->maxHealth;
        enemyManager->lastCollisionTime = 0;
        isLevelUp = true;
        currentState = GameStateRunning::GAME;
    }
}

void LoadAnimationFrames(SDL_Renderer* renderer)
{
    for (int i = 1; i <= 31; ++i)
    {
        std::string filePath = "Images/Animacja/" + std::to_string(i) + ".png";
        SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
        if (!texture)
        {
            SDL_Log("Failed to load animation frame: %s", SDL_GetError());
        }
        else
        {
            animationFrames.push_back(texture);
        }
    }
}

void Cleanup()
{
    for (auto texture : animationFrames)
    {
        SDL_DestroyTexture(texture);
    }
    animationFrames.clear();
}

void savePlayerStats(Player* player)
{
	std::ofstream saveFile("playerstats.dat", std::ios::binary);
	if (saveFile.is_open())
	{
        saveFile.write(reinterpret_cast<char*>(&player->totalKills), sizeof(player->totalKills));
        saveFile.write(reinterpret_cast<char*>(&player->totalDeaths), sizeof(player->totalDeaths));
		saveFile.close();
	}
}

void loadPlayerStats(Player* player)
{
	std::ifstream loadFile("playerstats.dat", std::ios::binary);
	if (loadFile.is_open())
	{
		loadFile.read(reinterpret_cast<char*>(&player->totalKills), sizeof(player->totalKills));
        loadFile.read(reinterpret_cast<char*>(&player->totalDeaths), sizeof(player->totalDeaths));
		loadFile.close();
	}
}

void saveGameState(Player* player, Map* map, EnemyManager* enemyManager)
{
    std::ofstream saveFile("savegame.dat", std::ios::binary);

    float playerX = player->GetX();
    float playerY = player->GetY();

    if (saveFile.is_open())
    {
        // Zapisz stan gracza
        saveFile.write(reinterpret_cast<char*>(&playerX), sizeof(playerX));
        saveFile.write(reinterpret_cast<char*>(&playerY), sizeof(playerY));
        saveFile.write(reinterpret_cast<char*>(&player->health), sizeof(player->health));
        saveFile.write(reinterpret_cast<char*>(&player->kills), sizeof(player->kills));

        // Zapisz stan przeciwników
        int numEnemies = enemyManager->enemies.size();
        saveFile.write(reinterpret_cast<char*>(&numEnemies), sizeof(numEnemies));

        for (const auto& enemy : enemyManager->enemies)
        {
            float enemyX = enemy->GetX();
            float enemyY = enemy->GetY();
            saveFile.write(reinterpret_cast<const char*>(&enemyX), sizeof(enemyX));
            saveFile.write(reinterpret_cast<const char*>(&enemyY), sizeof(enemyY));
        }

        saveFile.close();
    }
}

void loadGameState(Player* player, Map* map, EnemyManager* enemyManager, SDL_Renderer* renderer)
{
    std::ifstream loadFile("savegame.dat", std::ios::binary);

    if (loadFile.is_open())
    {
        // Wczytaj stan gracza
        loadFile.read(reinterpret_cast<char*>(&player->x), sizeof(player->x));
        loadFile.read(reinterpret_cast<char*>(&player->y), sizeof(player->y));
        loadFile.read(reinterpret_cast<char*>(&player->health), sizeof(player->health));
        loadFile.read(reinterpret_cast<char*>(&player->kills), sizeof(player->kills));

        // Wczytaj stan przeciwników
        int numEnemies;
        loadFile.read(reinterpret_cast<char*>(&numEnemies), sizeof(numEnemies));

        enemyManager->enemies.clear();

        for (int i = 0; i < numEnemies; ++i) 
        {
            float enemyX, enemyY;
            loadFile.read(reinterpret_cast<char*>(&enemyX), sizeof(enemyX));
            loadFile.read(reinterpret_cast<char*>(&enemyY), sizeof(enemyY));
            auto enemy = std::make_unique<Enemy>(player, map, enemyManager->camera, enemyManager->renderer);
            enemy->SetPosition(enemyX, enemyY);
            enemyManager->enemies.push_back(std::move(enemy));
        }

        loadFile.close();
    }
}
