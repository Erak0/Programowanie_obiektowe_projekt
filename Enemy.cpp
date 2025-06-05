#include "Enemy.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Camera.hpp"
#include "Settings.hpp"
#include <SDL3_image/SDL_image.h>
#include <cmath>

extern GameStateRunning gameState;

Enemy::Enemy(Player* player, Map* map, Camera* camera, SDL_Renderer* renderer) // Konstrutkor przeciwnika, gracz, mapa, kamera, renderer
	: x(0), y(0), velocityX(0), velocityY(0), speed(150.0f), playerTexture(nullptr),
	frameWidth(0), frameHeight(0), currentFrame(0), currentRow(0),
	totalFrames(9), lastFrameTime(0), frameDuration(100), health(100), isAlive(true), player(player), map(map), camera(camera)
{
	LoadTexture(renderer, "spritesheet.png");
} // Przekazywanie prêdkoœci, tekstury, klatek, czasu trwania klatki, zdrowia

Enemy::~Enemy() // Destruktor
{
	if (playerTexture) // Jeœli tekstura gracza istnieje
	{
		SDL_DestroyTexture(playerTexture); // Usuñ teksturê gracza
		playerTexture = nullptr; // Ustaw teksturê gracza na nullptr
	}

	isAlive = false; // Ustawienie na false
}

void Enemy::LoadTexture(SDL_Renderer* renderer, const char* pathFile) // Za³adowanie tekstury przeciwnika
{
	SDL_Surface* surface = IMG_Load(pathFile); // Wczytanie tekstury przeciwnika
	if (!surface) // Jeœli tekstura nie istnieje
	{
		SDL_Log("Couldn't load enemy texture: %s", SDL_GetError()); // Wyœwietlenie b³êdu
		return; // Zwróæ
	}

	playerTexture = SDL_CreateTextureFromSurface(renderer, surface); // Utworzenie tekstury przeciwnika
	frameWidth = surface->w / 9; // Szerokoœæ klatki
	frameHeight = surface->h / 4; // Wysokoœæ klatki
	totalFrames = 4; // Ca³kowita liczba klatek

	SDL_DestroySurface(surface); // Usuniêcie powierzchni
}

void Enemy::Render(SDL_Renderer* renderer) // Renderowanie przeciwnika
{
	if (!isAlive) return; // Jeœli nie ¿yje

	if (camera == nullptr) // Jeœli kamera nie istnieje
	{
		SDL_Log("Enemy::Render() - camera is nullptr");
		return;
	}

	renderHealthBar(health, renderer); // Renderowanie paska zdrowia

	SDL_FRect srcRect = { currentFrame * frameWidth, currentRow * frameHeight, frameWidth, frameHeight }; // Ustawienie klatki
	SDL_FRect dstRect = { x - camera->GetX() + (playerW / 16), y - camera->GetY() + (playerH / 16), enemyW, enemyH }; // Ustawienie pozycji
	SDL_RenderTexture(renderer, playerTexture, &srcRect, &dstRect); // Renderowanie tekstury
}

void Enemy::BigRender(SDL_Renderer* renderer) // Renderowanie przeciwnika
{
	if (!isAlive) return; // Jeœli nie ¿yje

	if (camera == nullptr) // Jeœli kamera nie istnieje
	{
		SDL_Log("Enemy::Render() - camera is nullptr");
		return;
	}

	renderHealthBar(health, renderer); // Renderowanie paska zdrowia

	SDL_FRect srcRect = { currentFrame * frameWidth, currentRow * frameHeight, frameWidth, frameHeight }; // Ustawienie klatki
	SDL_FRect dstRect = { x - camera->GetX() + (playerW / 16), y - camera->GetY() + (playerH / 16), enemyW * 4, enemyH * 4}; // Ustawienie pozycji
	SDL_RenderTexture(renderer, playerTexture, &srcRect, &dstRect); // Renderowanie tekstury
}

void Enemy::SetPosition(float x, float y) // Ustawienie pozycji przeciwnika
{
	this->x = x; // Ustawienie pozycji x
	this->y = y; // Ustawienie pozycji y
}

void Enemy::Update(float deltaTime, GameStateRunning currentState) // Aktualizacja przeciwnika
{
	if (currentState != GameStateRunning::GAME)
	{
		return;
	}

	if (player == nullptr) // Jeœli gracz nie istnieje
	{
		SDL_Log("Enemy::Update() - player is nullptr");
		return;
	}

	if (map == nullptr) // Jeœli mapa nie istnieje
	{
		SDL_Log("Enemy::Update() - map is nullptr");
		return;
	}

	float targetX = player->GetX() - playerW / 16;	// Pozycja x gracza
	float targetY = player->GetY() - playerH / 16; // Pozycja y gracza

	float directionX = targetX - x; // Kierunek x
	float directionY = targetY - y; // Kierunek y
	float length = std::sqrt(directionX * directionX + directionY * directionY); // D³ugoœæ

	if (length != 0) // Jeœli d³ugoœæ nie jest równa 0
	{
		directionX /= length; // Podziel kierunek x przez d³ugoœæ
		directionY /= length; // Podziel kierunek y przez d³ugoœæ
	}

	float velocityX = directionX * speed; // Prêdkoœæ x
	float velocityY = directionY * speed; // Prêdkoœæ y

	float newX = x + velocityX * deltaTime; // Nowa pozycja x
	float newY = y + velocityY * deltaTime; // Nowa pozycja y

	if (velocityX == 0 && velocityY == 0) // Jeœli prêdkoœæ x i y s¹ równe 0
	{
		currentFrame = 0; // Ustaw klatkê na 0
	}
	else if (velocityX > 0) // Right
	{
		currentRow = 3; // Ustawk klatkê na 3
	}
	else if (velocityX < 0) // Left
	{
		currentRow = 1; // Ustaw klatkê na 1
	}
	else if (velocityY > 0) // Up
	{
		currentRow = 2; // Ustaw klatkê na 2
	}
	else if (velocityY < 0) // Down
	{
		currentRow = 0; // Ustaw klatkê na 0
	}

	x = newX; // Ustaw pozycjê x
	y = newY; // Ustaw pozycjê y

	UpdateAnimation(); // Aktualizacja animacji
}

void Enemy::renderHealthBar(double healthValue, SDL_Renderer* renderer) // Renderowanie paska 
{
	float barHeight = 5.0f; // Wysokoœæ paska
	float barWidth = enemyW / 2; // Szerokoœæ paska

	float healthPercentage = healthValue / 100.0f; // Procent zdrowia
	float currentBarWidth = barWidth * healthPercentage; // Aktualna szerokoœæ paska

	float renderX = x - camera->GetX() + (playerW / 2) - 32.0f;
	float renderY = y - camera->GetY() + (playerH / 8);

	SDL_FRect backgroundBar = { renderX, renderY, barWidth, barHeight }; // Prostok¹t t³a
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // kolor szary
	SDL_RenderFillRect(renderer, &backgroundBar); // Wype³nienie prostok¹ta

	SDL_FRect foregroundBar = { renderX, renderY, currentBarWidth, barHeight }; // Prostok¹t paska
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Kolor czerwony
	SDL_RenderFillRect(renderer, &foregroundBar); // Wype³nienie prostok¹ta
}


void Enemy::SetVelocity(float x, float y) // Ustawienie prêdkoœci przeciwnika
{
	velocityX = x; // Ustaw prêdkoœæ x
	velocityY = y; // Ustaw prêdkoœæ y
}

float Enemy::GetX() const // Pobranie pozycji x
{
	return x;
}

float Enemy::GetY() const // Pobranie pozycji y
{
	return y;
}

float Enemy::GetVelocityX() const // Pobranie prêdkoœci x
{
	return velocityX;
}

float Enemy::GetVelocityY() const // Pobranie prêdkoœci y
{
	return velocityY;
}

void Enemy::SetCurrentRow(int row) // Ustawienie aktualnego wiersza dla animacji
{
	currentRow = row;
}

void Enemy::UpdateAnimation() // Aktualizacja animacji
{
	if (velocityX == 0 && velocityY == 0) // Jeœli prêdkoœæ x i y s¹ równe 0
	{
		return;
	}

	Uint64 currentTime = SDL_GetTicks(); // Aktualny czas
	if (currentTime > lastFrameTime + frameDuration) // Jeœli aktualny czas jest wiêkszy od ostatniego czasu + czasu trwania klatki
	{
		currentFrame = (currentFrame + 1) % totalFrames; // Ustaw klatkê
		lastFrameTime = currentTime; // Ustaw ostatni czas
	}
}

SDL_FRect Enemy::GetCollisionRect() const // Pobranie prostok¹ta kolizji
{
	if (!isAlive || gameState != GameStateRunning::GAME)
	{
		return { 0,0,0,0 };
	}

	else
	{
		return SDL_FRect
		{
			x,
			y,
			enemyW / 2,
			enemyH / 2
		}; // Zwróæ prostok¹t kolizji
	}
}