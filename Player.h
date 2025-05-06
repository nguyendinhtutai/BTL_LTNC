#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "Bullet.h"
using namespace std;

enum FacingDirection {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Player {
public:
    Player(int x, int y, const char* texturePath);
    ~Player();

    void update(int mapWidth, int mapHeight, vector<vector<int>> map);
    void render();
    void move(int dx, int dy, const int mapWidth, const int mapHeight, vector<vector<int>> map, const Player* otherPlayer);
    void respawn();

    SDL_Rect getRect() const { return rect; }

    FacingDirection getFacingDirection() const { return facingDirection; }

    Uint32 getLastShotTime() const { return lastShotTime; }
    void setLastShotTime(Uint32 time) { lastShotTime = time; }
    Uint32 getFireRate() const { return fireRate; }

    bool hasBuff;
private:
    FacingDirection facingDirection;

    int startX;
    int startY;
    int x;
    int y;
    int speed;
    SDL_Texture* playerTexture;
    SDL_Rect rect;
    SDL_Rect srcRect;

    Uint32 lastShotTime;  // Time of the last shot (in milliseconds)
    const Uint32 fireRate; // Minimum time between shots (in milliseconds)

};
