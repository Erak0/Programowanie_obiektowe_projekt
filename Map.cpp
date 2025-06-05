#include "Map.hpp"
#include "Player.hpp"
#include <algorithm>

struct Renderable 
{
    float depth; // np. dolna krawêdŸ obiektu
    enum Type { PLAYER, OBJECT } type;
    SDL_Texture* texture; // tylko dla obiektów
    SDL_FRect rect;       // tylko dla obiektów
    Player* player;       // tylko dla gracza
};

Map::Map(int width, int height) : width(width), height(height) // Konstruktor mapy, szerokoœæ, wysokoœæ
{
	CreateCollisionSurface(); // Utworzenie powierzchni kolizji
}

void Map::CreateCollisionSurface() // Utworzenie powierzchni kolizji
{
	collisionSurface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32); // Utworzenie powierzchni kolizji
	if (!collisionSurface) // Jeœli powierzchnia kolizji nie istnieje
    {
        SDL_Log("Unable to create collision surface! SDL Error: %s", SDL_GetError());
    }
}

bool Map::IsWithinBounds(float x, float y, float px, float py) const // Czy jest w granicach
{
    if (x < 64.0f || x >(width - px - 64.0f) || y < 0.0f || y >(height - py - 64.0f)) // Jeœli x jest mniejsze od 64, x jest wiêksze od szerokoœci - px - 64, y jest mniejsze od 0, y jest wiêksze od wysokoœci - py - 63
    {
        return false;
    }

    SDL_FRect playerRect = { x, y, px, py };

    for (const auto& objectCollision : objectCollisions)
    {
        for (const auto& rect : objectCollision.collisionRects)
        {
            if (SDL_HasRectIntersectionFloat(&playerRect, &rect)) // Jeœli jest kolizja
            {
                return false;
            }
        }
    }

    return true;
}

bool Map::LoadTexture(SDL_Renderer* renderer, const char* pathFile) // Wczytanie tekstury
{ 
	backgroundTexture = IMG_LoadTexture(renderer, pathFile); // Wczytanie tekstury
    SDL_SetTextureScaleMode(backgroundTexture, SDL_SCALEMODE_NEAREST);
	return backgroundTexture != nullptr; // Zwróæ czy tekstura istnieje
}

void Map::Render(SDL_Renderer* renderer, float cameraX, float cameraY) const
{
    if (backgroundTexture)
    {
        SDL_FRect srcRect = { 0, 0, width, height };
        SDL_FRect dstRect = { -cameraX, -cameraY, width, height };
        SDL_RenderTexture(renderer, backgroundTexture, &srcRect, &dstRect);
    }
}

void Map::RenderObjects(SDL_Renderer* renderer, float cameraX, float cameraY, Player* player, TTF_Font* font) const
{
    std::vector<Renderable> renderQueue;

    // Dodaj wszystkie obiekty (drzewa) do kolejki
    for (const auto& object : objectCollisions)
    {
        Renderable r;
        r.depth = object.collisionRects[0].y;
        r.type = Renderable::OBJECT;
        r.texture = object.texture;
        r.rect = object.rect;
        renderQueue.push_back(r);
    }

    // Dodaj gracza do kolejki
    Renderable p;
    p.depth = player->GetY() + player->GetHeight() + 32.0f;
    p.type = Renderable::PLAYER;
    p.player = player;
    renderQueue.push_back(p);

    // Sortuj kolejkê renderowania wed³ug wartoœci depth (od najmniejszej do najwiêkszej)
    std::sort(renderQueue.begin(), renderQueue.end(), [](const Renderable& a, const Renderable& b)
        {
            return a.depth < b.depth;
        });

    // Renderuj wszystkie elementy w kolejnoœci
    for (const auto& item : renderQueue)
    {
        if (item.type == Renderable::PLAYER)
        {
            item.player->Render(renderer);
        }
        else if (item.type == Renderable::OBJECT)
        {
            SDL_FRect dstRect = item.rect;
            dstRect.x -= cameraX;
            dstRect.y -= cameraY;
            SDL_RenderTexture(renderer, item.texture, nullptr, &dstRect);
        }
    }

    player->renderHealthBar(player->health, renderer, font); // Renderowanie paska jako pierwszego
	player->renderExpBar(player->exp, renderer, font); // Renderowanie paska expa jako pierwszego
}

bool Map::IsNodeWalkable(int x, int y) const // Czy wêze³ jest dostêpny
{
	if (x < 0 || x >= width / nodeSize || y < 0 || y >= height / nodeSize) // Jeœli x jest mniejsze od 0, x jest wiêksze od szerokoœci / rozmiar wêz³a, y jest mniejsze od 0, y jest wiêksze od wysokoœci / rozmiar wêz³a
    {
		return false; // Zwróæ fa³sz
    }

	return grid[y][x]; // Zwróæ siatkê
}

void Map::CreateGrid(int nodeSize) // Utworzenie siatki
{
	this->nodeSize = nodeSize; // Ustawienie rozmiaru wêz³a
	int rows = height / nodeSize; // Wiersze
    int cols = width / nodeSize; // Koluny
	grid.resize(rows, std::vector<bool>(cols, true)); // Zmiana rozmiaru siatki
}

void Map::AddCollisionRect(const SDL_FRect& rect) // Dodanie prostok¹ta kolizji
{
	collisionRects.push_back(rect); // Dodanie prostok¹ta kolizji
}

bool Map::LoadCollisionSurface(const char* pathFile) // Wczytanie powierzchni kolizji
{
	SDL_Surface* loadedSurface = IMG_Load(pathFile); // Wczytanie powierzchni kolizji
	if (!loadedSurface) // Jeœli powierzchnia kolizji nie istnieje
	{
		SDL_Log("Unable to load collision surface %s! SDL_image Error: %s", pathFile, SDL_GetError()); // Wyœwietlenie b³êdu
		return false; // Zwróæ fa³sz
	}

	SDL_Surface* scaledSurface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32); // Utworzenie powierzchni o szerokoœci, wysokoœci, formacie
	if (!scaledSurface) // Jeœli powierzchnia nie istnieje
    {
        SDL_Log("Unable to create scaled surface! SDL Error: %s", SDL_GetError());
		SDL_DestroySurface(loadedSurface); // Usuniêcie powierzchni
        return false;
    }

	SDL_BlitSurfaceScaled(loadedSurface, nullptr, scaledSurface, nullptr, SDL_SCALEMODE_NEAREST); // Skalowanie powierzchni
	SDL_DestroySurface(loadedSurface); // Usuniêcie powierzchni

	if (collisionSurface) // Jeœli powierzchnia kolizji istnieje
    {
		SDL_DestroySurface(collisionSurface); // Usuniêcie powierzchni kolizji
    }
	collisionSurface = scaledSurface; // Ustawienie powierzchni kolizji

    return true;
}

bool Map::IsPixelTransparent(int x, int y) const // Sprawdzenie czy pixel jest przezroczysty
{
    y += 128;
    x += 64;

	if (!collisionSurface || x < 0 || x >= collisionSurface->w || y < 0 || y >= collisionSurface->h) // Jeœli powierzchnia kolizji nie istnieje, x jest mniejsze od 0, x jest wiêksze od szerokoœci powierzchni kolizji, y jest mniejsze od 0, y jest wiêksze od wysokoœci powierzchni kolizji
    {
        return true;
    }

	Uint32* pixels = (Uint32*)collisionSurface->pixels; // Piksele
	Uint32 pixel = pixels[(y * collisionSurface->w) + x]; // Piksel
    

	Uint8 r, g, b, a; // Kana³y
	SDL_GetRGBA(pixel, SDL_GetPixelFormatDetails(collisionSurface->format), 0, &r, &g, &b, &a); // Ustawienie kolizji na podsatwie kana³ów

    return a == 0; 
}

void Map::AddObjectWithCollision(SDL_Renderer* renderer, const char* objectTexturePath, float posX, float posY, float objWidth, float objHeight)
{
    SDL_Texture* objectTexture = IMG_LoadTexture(renderer, objectTexturePath);
    if (!objectTexture)
    {
        SDL_Log("Nie mo¿na wczytaæ tekstury obiektu %s! SDL_image Error: %s", objectTexturePath, SDL_GetError());
        return;
    }

    SDL_SetTextureScaleMode(objectTexture, SDL_SCALEMODE_NEAREST);

    SDL_FRect dstRect = { posX, posY, objWidth, objHeight };

    // Definicja pojedynczego prostok¹ta kolizji na dole obiektu
    std::vector<SDL_FRect> collisionRects;

    // Wysokoœæ prostok¹ta kolizji (1.0f piksel)
    float collisionHeight = 1.0f;

    SDL_FRect collisionRect = {
        posX + objWidth - 126.0f,                   // X
        posY + objHeight - collisionHeight,         // Y
        objWidth - 252.0f,                          // Szerokoœæ
        collisionHeight                             // Wysokoœæ
    };

    collisionRects.push_back(collisionRect);

    ObjectCollision objectCollision = { objectTexture, dstRect, collisionRects };
    objectCollisions.push_back(objectCollision);
}

void mapCollisions(Map* map, SDL_Renderer* renderer) // Kolizje mapy
{
    map->AddObjectWithCollision(renderer, "Images/Trees/middle_lane_tree2.png", 512.0f, 512.0f, 256.0f, 256.0f);
    map->AddObjectWithCollision(renderer, "Images/Trees/middle_lane_tree3.png", 620.0f, 948.0f, 256.0f, 256.0f);
    map->AddObjectWithCollision(renderer, "Images/Trees/middle_lane_tree4.png", 2400.0f, 428.0f, 256.0f, 256.0f);
    map->AddObjectWithCollision(renderer, "Images/Trees/middle_lane_tree6.png", 2200.0f, 1852.0f, 256.0f, 256.0f);
    map->AddObjectWithCollision(renderer, "Images/Trees/middle_lane_tree7.png", 1982.0f, 2355.0f, 256.0f, 256.0f);
}

void Map::SortObjectsByY()
{
    std::sort(objectCollisions.begin(), objectCollisions.end(), [](const ObjectCollision& a, const ObjectCollision& b) 
    {
        return a.rect.y < b.rect.y;
    });
}

void Map::Reset(SDL_Renderer* renderer)
{
    // Resetuj mapê, np. usuñ obiekty kolizji
    objectCollisions.clear();
    // Ponownie utwórz siatkê kolizji
    CreateGrid(nodeSize);
    // Dodaj obiekty kolizji ponownie, jeœli to konieczne
    mapCollisions(this, renderer);
}