#pragma once
#include <SDL2/SDL.h>

class Bullet {
public:
    Bullet(int startX, int startY, int directionX, int directionY, const char* texturePath);
    ~Bullet();

    void update();
    void render();

    SDL_Rect getRect() const { return rect; }
    bool isActive() const { return active; }
    void deactivate() { active = false; }


    bool isBuffBullet;

private:

    int x;
    int y;
    float velocityX; // Use float for velocity to allow finer movement control
    float velocityY;
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Rect srcRect;
    bool active; // Flag to indicate if the bullet is active (on screen)

    static const int SPRITE_WIDTH;   // Width of each bullet sprite
    static const int SPRITE_HEIGHT;  // Height of each bullet sprite
    static const int SPRITES_PER_ROW;
};
