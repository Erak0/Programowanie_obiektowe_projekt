#pragma once

#include <SDL3/SDL.h>
#include "Map.hpp"
#include "Camera.hpp"
#include <memory>
#include "GameState.hpp"

class Player;

class Enemy
{
public:
    Enemy(Player* player, Map* map, Camera* camera, SDL_Renderer* renderer); // Konstruktor przeciwnika, gracz, mapa, kamera, renderer
    ~Enemy(); // Destruktor

    void LoadTexture(SDL_Renderer* renderer, const char* pathFile); // Za³adowanie tekstury przeciwnika
    void Render(SDL_Renderer* renderer); // Renderowanie przeciwnika
	void BigRender(SDL_Renderer* renderer); // Renderowanie du¿ch przeciwników
    void SetPosition(float x, float y); // Ustwienie pozycji przeciwnika
    void Update(float deltaTime, GameStateRunning currentState); // Aktualizacja przeciwnika
    void SetVelocity(float x, float y); // Ustwienie prêdkoœci przeciwnika
    float GetX() const; // Pobranie pozycji x
    float GetY() const; // Pobranie pozycji y
    float GetVelocityX() const; // Pobranie prêdkoœci x
    float GetVelocityY() const; // Pobranie prêdkoœci y
    void SetCurrentRow(int row); // Ustwienie aktualnego wiersza dla animacji

    SDL_FRect GetCollisionRect() const; // Pobranie prostok¹ta kolizji

    Player* player; // Gracz
    Map* map; // Mapa
    Camera* camera; // Kamera

    double health; // Zdrowie
    bool isAlive; // Czy ¿yje

private:
    void UpdateAnimation(); // Aktualizacja animacji
    void renderHealthBar(double healthValue, SDL_Renderer* renderer); // Renderowanie paska zdrowia

    float x; // Pozycja x
    float y; // Pozycja y
    float velocityX; // Prêdkoœæ x
    float velocityY; // Prêdkoœæ y
    float directionX; // Kierunek x
    float directionY; // Kierunek y
    float speed; // Prêdkoœæ
    SDL_Texture* playerTexture; // Tekstura przeciwnika
	SDL_Renderer* renderer; // Renderer
    int frameWidth; // Szerokoœæ klatki
    int frameHeight; // Wysokoœæ klatki
    int currentFrame; // Aktualna klatka
    int currentRow; // Aktualny wiersz
    int totalFrames; // Ca³kowita liczba klatek
    Uint64 lastFrameTime; // Ostatni czas klatki
    Uint64 frameDuration; // Czas trwania klatki
};
