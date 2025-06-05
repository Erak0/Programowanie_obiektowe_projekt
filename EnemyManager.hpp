#pragma once

// EnemyManager odpowiada za zarzšdzanie przeciwnikami

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>
#include <vector>
#include <random>
#include "Player.hpp"
#include "Enemy.hpp"
#include "Settings.hpp"
#include "GameState.hpp"

class EnemyManager
{
public:
	EnemyManager(Player* player, Map* map, Camera* camera, SDL_Renderer* renderer); // Konstrutkor przeciwnika, gracz, mapa, kamera, renderer

	void AddEnemy(); // Dodanie przeciwnika
	void AddBigEnemy(); // Dodanie bossa

	void Update(float deltaTime, GameStateRunning currentState, TTF_Font* font); // Aktualizacja przeciwnika
	void Render(SDL_Renderer* renderer, GameStateRunning currentState); // Renderowanie przeciwnika

	void Reset();
	void AdjustSpawnTime(Uint64 pauseDuration); // Nowa metoda

	std::vector<std::unique_ptr<Enemy>> enemies; // Przeciwnicy
	std::vector<std::unique_ptr<Enemy>> bigEnemies; // Przeciwnicy

	Player* player; // Gracz
	Map* map; // Mapa
	Camera* camera; // Kamera
	SDL_Renderer* renderer; // Renderer

	Uint64 lastCollisionTime = 0; // Ostatni czas spawnu przeciwnika

private:
	bool CheckCollision(const SDL_FRect& rect1, const SDL_FRect& rect2) const; // Sprawdzenie kolizji
	bool IsPlayerInCollision() const; // Czy gracz jest w kolizji

	float GetRandomFloat(float min, float max); // Pobranie losowej liczby zmiennoprzecinkowej

	Uint64 lastSpawnTime = 0; // Ostatni czas spawnu przeciwnika
	Uint64 lastBigSpawnTime = 0; // Ostatni czas spawnu bossa
};
