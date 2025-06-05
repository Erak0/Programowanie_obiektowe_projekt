#pragma once

#include <vector>

struct Resolution
{
    int width;
    int height;
};

std::vector<Resolution> GetAvailableResolutions();
std::vector<float> GetAvailableFPS();
std::pair<float, float> GetWindowSize();
void InitializeSettings();

extern std::vector<Resolution> availableResolutions;
extern Resolution currentResolution;

extern float WINDOW_WIDTH;
extern float WINDOW_HEIGHT;

extern float DEFAULT_WIDTH;
extern float DEFAULT_HEIGHT;

extern float scaleX;
extern float scaleY;

static const float mapWidth = 4096;
static const float mapHeight = 4096;

static const float playerW = 128.0f; // Ustawienie szerokoœci gracza
static const float playerH = 128.0f; // Ustawienie wysokoœci gracza

static const float enemyW = 128.0f; // Ustawienie szerokoœci przeciwnika
static const float enemyH = 128.0f; // Ustawienie wysokoœci przeciwnika

extern std::vector<float> availableFPS;
extern float fps_t;
extern float fpsf;