#include "Bullet.h"
#include "TextureManager.h"
#include "Game.h" // Include Game.h for map and renderer access
#include "Player.h"
#include <iostream>

using namespace std;

const int Bullet::SPRITE_WIDTH = 16;
const int Bullet::SPRITE_HEIGHT = 16;
const int Bullet::SPRITES_PER_ROW = 4;

Bullet::Bullet(int startX, int startY, int directionX, int directionY, const char* texturePath)
    : x(startX), y(startY), velocityX(directionX * 10.0f), velocityY(directionY * 10.0f), active(true), isBuffBullet(false) {
    texture = TextureManager::LoadTexture(texturePath);
    if (!texture) {
        cout << "Failed to load bullet texture!" << endl;
    }
    rect.x = x;
    rect.y = y;
    rect.w = Bullet::SPRITE_WIDTH;
    rect.h = Bullet::SPRITE_HEIGHT;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = Bullet::SPRITE_WIDTH;
    srcRect.h = Bullet::SPRITE_HEIGHT;

    FacingDirection localFacingDirection = LEFT;

    if (directionX < 0) localFacingDirection = LEFT;
    else if (directionX > 0) localFacingDirection = RIGHT;
    else if (directionY < 0) localFacingDirection = UP;
    else if (directionY > 0) localFacingDirection = DOWN;

    // --- Sprite selection based on direction (example - adjust to your sprite sheet layout) ---
    switch (localFacingDirection) { // Use the LOCAL variable
    case UP:    srcRect.x = 0 * Bullet::SPRITE_WIDTH; break;     // Assuming UP is at index 0
    case RIGHT: srcRect.x = 1 * Bullet::SPRITE_WIDTH; break;     // Assuming RIGHT is at index 1
    case DOWN:  srcRect.x = 2 * Bullet::SPRITE_WIDTH; break;     // Assuming DOWN is at index 2
    case LEFT:  srcRect.x = 3 * Bullet::SPRITE_WIDTH; break;     // Assuming LEFT is at index 3
    default:    srcRect.x = 0 * Bullet::SPRITE_WIDTH; break;     // Default to UP if direction unknown
    }
}

Bullet::~Bullet() {
    SDL_DestroyTexture(texture);
}

// Bullet.cpp (modified update function)
void Bullet::update() {
    if (!active) return;

    x += static_cast<int>(velocityX);
    y += static_cast<int>(velocityY);
    rect.x = x;
    rect.y = y;

    if (rect.x < 0 || rect.x > SCREEN_WIDTH || rect.y < 0 || rect.y > SCREEN_HEIGHT) {
        active = false;
        return;
    }

    int mapWidth = Game::getMapWidth();
    int mapHeight = Game::getMapHeight();
    vector<vector<int>>& map = Game::getMap(); // Get a reference to the map (important for modification)

    if (mapWidth > 0 && mapHeight > 0 && !map.empty()) {
        bool collisionDetected = false; // Flag to track if any corner collided

        // --- 4-CORNER COLLISION CHECK ---
        SDL_Point corners[4];
        corners[0] = { rect.x, rect.y };                      // Top-left
        corners[1] = { rect.x + rect.w - 1, rect.y };          // Top-right
        corners[2] = { rect.x, rect.y + rect.h - 1 };          // Bottom-left
        corners[3] = { rect.x + rect.w - 1, rect.y + rect.h - 1 }; // Bottom-right

        for (const auto& corner : corners) {
            int tileRow = corner.y / TILE_SIZE;
            int tileCol = corner.x / TILE_SIZE;

            if (tileRow >= 0 && tileRow < mapHeight && tileCol >= 0 && tileCol < mapWidth) {
                int tileType = map[tileRow][tileCol];

                if (tileType == 1 || tileType == 2 || tileType == 3 || tileType == 4) { // Va ch?m v?i b?t k? t??ng nào
                    if (isBuffBullet) {
                        std::cout << "Buff Bullet Collision at (" << tileRow << ", " << tileCol << ")\n";
                        Game::destroy3x3Walls(tileCol, tileRow); // Trigger 3x3 area destruction
                        collisionDetected = true; // Deactivate bullet after explosion
                    }
                    else {

                        if (tileType == 1) { // Indestructible wall
                            collisionDetected = true;
                            cout << "  Bullet Corner Collision with Indestructible Wall! Deactivating bullet." << std::endl;
                            break; // No need to check other corners, collision found
                        }
                        else if (tileType == 2) { // Destructible Wall (3 hits)
                            map[tileRow][tileCol] = 3; // Change to 1-hit remaining wall (type 3)
                            collisionDetected = true;
                            cout << "  Bullet Corner Collision with Destructible Wall (3 hits -> 2 hit)! Deactivating bullet." << std::endl;
                            break;
                        }
                        else if (tileType == 3) { // Destructible Wall (2 hit remaining)
                            map[tileRow][tileCol] = 4; // Destroy the wall (set to empty)
                            collisionDetected = true;
                            cout << "  Bullet Corner Collision with Destructible Wall (2 hit -> 1 hit)! Deactivating bullet." << std::endl;
                            break;
                        }
                        else if (tileType == 4) { // Destructible Wall (1 hit remaining)
                            map[tileRow][tileCol] = 0; // Destroy the wall (set to empty)
                            collisionDetected = true;
                            cout << "  Bullet Corner Collision with Destructible Wall (1 hit -> Destroyed)! Deactivating bullet." << std::endl;
                            break;
                        }
                    }
                }
            }
        }

        if (collisionDetected) {
            active = false; // Deactivate bullet if any corner collided
        }
    }
}

void Bullet::render() {
    if (active) {
        SDL_RenderCopy(Game::renderer, texture, &srcRect , &rect);
    }
}
