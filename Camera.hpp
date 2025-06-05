#pragma once

class Camera
{
public:
    Camera(float x, float y, float width, float height); // Konstruktor
    void Update(float playerX, float playerY); // Aktualizacja pozycji kamery
    float GetX() const; // pobierz x
    float GetY() const; // pobierz y
	void SetViewportSize(float newWidth, float newHeight); // Ustawienie rozmiaru widoku

    float width, height; // szerokoœæ i wysokoœæ ekranu
    float thresholdX, thresholdY; // próg zakresu kamery

private:
    float x, y; // x y
};