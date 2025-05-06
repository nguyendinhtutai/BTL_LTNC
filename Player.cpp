#include "Player.h"
#include "TextureManager.h"
#include "Game.h"
#include "Bullet.h"
#include <iostream>

const int SPEED = 2;

using namespace std;


Player::Player(int x, int y, const char* texturePath) : startX(x), startY(y), speed(SPEED), x(x), y(y), fireRate(500), facingDirection(UP), hasBuff(false) {
    playerTexture = TextureManager::LoadTexture(texturePath);
    if (!playerTexture) {
        cout << "Failed to load player texture!" << endl;
    }

    rect.x = x;
    rect.y = y;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;

	// Khoi tao srcRect
	srcRect.x = 0; // Ban dau xe tang huong len tren (x=0) (trong spritesheet)
    srcRect.y = 0;
    srcRect.w = TILE_SIZE;
    srcRect.h = TILE_SIZE;

}

Player::~Player() {
    SDL_DestroyTexture(playerTexture);
}

void Player::render() {
    switch (facingDirection) {
	    case UP:    srcRect.x = 0 * TILE_SIZE; break;     // Theo sprite sheet, UP la frame dau tien
		case RIGHT:  srcRect.x = 1 * TILE_SIZE; break;    // RIGHT la frame thu 2
		case DOWN:  srcRect.x = 2 * TILE_SIZE; break;     // DOWN la frame thu 3
		case LEFT: srcRect.x = 3 * TILE_SIZE; break;    // LEFT la frame cuoi cung
    }

    SDL_RenderCopy(Game::renderer, playerTexture, &srcRect, &rect);
}

void Player::move(int dx, int dy, const int mapWidth, const int mapHeight, vector<vector<int>> map, const Player* otherPlayer) {
	// Cap nhat vi tri moi cua nguoi choi
    int newX = x + dx * SPEED;
    int newY = y + dy * SPEED;


    SDL_Rect newRect;
    newRect.x = newX;
    newRect.y = newY;
    newRect.w = rect.w;
    newRect.h = rect.h;

    if (dx < 0) facingDirection = LEFT;
    else if (dx > 0) facingDirection = RIGHT;
    else if (dy < 0) facingDirection = UP;
    else if (dy > 0) facingDirection = DOWN;

    //// 1. Check for collisions with walls (tile type '1' or '2')

    bool collision = false;

	// Duyet cac vi tri co the di duoc cua nguoi choi
    int startTileRow = max(0, newY / TILE_SIZE - 1);  // Calculate starting tile row in pixel space
    int endTileRow = min(mapHeight / TILE_SIZE - 1, newY / TILE_SIZE + 1); // Calculate ending tile row in pixel space
    int startTileCol = max(0, newX / TILE_SIZE - 1);  // Calculate starting tile col in pixel space
    int endTileCol = min(mapWidth / TILE_SIZE - 1, newX / TILE_SIZE + 1); // Calculate ending tile col in pixel space

	// Tai sao lai co ham max va min o day?
    // -> Tranh duoc oob (out of bounds) khi nguoi choi o vi tri bien cua map

    for (int row = startTileRow; row <= endTileRow; ++row) { // Iterate through tile rows
        for (int col = startTileCol; col <= endTileCol; ++col) { // Iterate through tile cols
            if (map[row][col] == 1 || map[row][col] == 2 || map[row][col] == 3 || map[row][col] == 4) {
                SDL_Rect wallRect;
                wallRect.x = col * TILE_SIZE; // Wall rect in PIXEL coordinates
                wallRect.y = row * TILE_SIZE; // Wall rect in PIXEL coordinates
                wallRect.w = TILE_SIZE;
                wallRect.h = TILE_SIZE;

                if (newRect.x < wallRect.x + wallRect.w &&
                    newRect.x + newRect.w > wallRect.x &&
                    newRect.y < wallRect.y + wallRect.h &&
                    newRect.y + newRect.h > wallRect.y)
                {
                    collision = true;
                    break;
                }
            }
        }
        if (collision) {
            break;
        }
    }

    // 2. Check for collision with the other player (if otherPlayer is valid)
    if (!collision && otherPlayer != nullptr) {
        SDL_Rect otherRect = otherPlayer->getRect();
        if (newRect.x < otherRect.x + otherRect.w &&
            newRect.x + newRect.w > otherRect.x &&
            newRect.y < otherRect.y + otherRect.h &&
            newRect.y + newRect.h > otherRect.y)
        {
            collision = true; // Collision with other player!
        }
    }

    // 3. Only move the player if there was NO collision
    if (!collision) {
        x = newX;
        y = newY;
        rect.x = x;
        rect.y = y;
    }


}

void Player::respawn() {
    x = startX;
    y = startY;
    rect.x = x * TILE_SIZE;
    rect.y = y * TILE_SIZE;
}

