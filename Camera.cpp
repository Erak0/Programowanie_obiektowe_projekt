// Camaera odopawiada za poruszanie siê ekranu za graczem, w odpowiednim zakresie

#include "Camera.hpp"
#include "Settings.hpp"

Camera::Camera(float x, float y, float width, float height) // Konstruktor kamery, pozycja x,y oraz d³ugoœæ i szerokoœæ
    : x(x), y(y), width(width), height(height), thresholdX(width / 4), thresholdY(height / 4) 
{
}

void Camera::Update(float playerX, float playerY)
{
	if (playerX > x + width - thresholdX - playerW / 2) // Jeœli gracz przekroczy prawy próg kamery
    {
		x = playerX - (width - thresholdX - playerW / 2); // Przesuñ kamerê w prawo
    }
	else if (playerX < x + thresholdX) // Jeœli gracz przekroczy lewy próg kamery
    {
		x = playerX - thresholdX; // Przesuñ kamerê w lewo
    }

	if (playerY > y + height - thresholdY - playerH / 2) // Jeœli gracz przekroczy dolny próg kamery
    {
		y = playerY - (height - thresholdY - playerH / 2); // Przesuñ kamerê w dó³
    }
	else if (playerY < y + thresholdY) // Jeœli gracz przekroczy górny próg kamery
    {
		y = playerY - thresholdY; // Przesuñ kamerê w górê
    }

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + width > mapWidth) x = mapWidth - width;
    if (y + height > mapHeight) y = mapHeight - height;
}

void Camera::SetViewportSize(float newWidth, float newHeight) 
{
    width = newWidth;
    height = newHeight;
    thresholdX = width / 2.0f;
    thresholdY = height / 2.0f;
}

float Camera::GetX() const
{
    return x;
}

float Camera::GetY() const
{
    return y;
}
