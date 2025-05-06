#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "Player.h"
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <SDL2/SDL_mixer.h>

const int TILE_SIZE = 35;

const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;
const int BUTTON_WIDTH = 350;
const int BUTTON_HEIGHT = 90;
const int BUTTON_SPACE = 100;


const int PLAYER1_START_POS_X = 0;
const int PLAYER1_START_POS_Y = 0;
const int PLAYER1_W = 35;
const int PLAYER1_H = 35;

const int PLAYER2_START_POS_X = 500;
const int PLAYER2_START_POS_Y = 0;
const int PLAYER2_W = 35;
const int PLAYER2_H = 35;


enum GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    QUIT,
    CREDITS
};

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    bool running() { return isRunning; }
    void clean();

    static SDL_Renderer* renderer;
    TTF_Font* titlefont;

	// Tra ve cac gia tri cua map
    static int getMapWidth() { return mapWidth; }
    static int getMapHeight() { return mapHeight; }
    static vector<vector<int>>& getMap() { return map; }

	// Tra ve trang thai cua game
    static GameState getGameState() { return gameState; }
    static void setGameState(GameState state) { gameState = state; }
    static int getWinner() { return winner; }
    static void setWinner(int playerNum) { winner = playerNum; }

    static void destroy3x3Walls(int tileCol, int tileRow); // Ham pha tuong

private:
    SDL_Window* window;
    bool isRunning;
    int countthu = 0;


    // Menu
    TTF_Font* font;

    SDL_Color textColor;
    SDL_Color buttonTextColor;
    SDL_Texture* menuBackgroundTexture; // background image

    SDL_Texture* playButtonNormalTexture; // texture cua button
    SDL_Texture* highScoreButtonNormalTexture;
    SDL_Texture* creditsButtonNormalTexture;
    SDL_Rect playButtonRect;
    SDL_Rect highScoreButtonRect;
    SDL_Rect creditsButtonRect;

    // khoi tao player pointer
    Player* player1;
    Player* player2;

    // texture cua map
    SDL_Texture* wallTexture;
    SDL_Texture* wall2Texture;
    SDL_Texture* wall2Texture_damaged1;
    SDL_Texture* wall2Texture_damaged2;

    static vector<vector<int>> map;
    static int mapWidth;
    static int mapHeight;

    SDL_Texture* bombTexture;

    SDL_Texture* targetP1Texture; // Texture for target 1
    SDL_Texture* targetP2Texture; // Texture for target 2
    SDL_Texture* buff3x3Texture; // Texture for 3x3 stars

    //bullet
    vector<Bullet*> bullets;

    static GameState gameState;
    static int winner;

	int targetP1TileCol, targetP1TileRow; // Toa do cua target P1 trong tile
	int targetP2TileCol, targetP2TileRow; // Toa do cua target P2 trong tile

    bool loadMap(const char* filePath);
    void renderMap();
    void renderMenu();
    void handleMenuEvents(const SDL_Event& event);
    void handlePlayingEvents(const SDL_Event& event);
    void copyFileContent(const string& sourceFilePath, const string& destinationFilePath);
	void resetGame();
	void renderGameOver();
    void renderCredits();

    Mix_Music* music;       // For background music (if you add music later)
    Mix_Chunk* fireSound;   // For player shooting sound
    Mix_Chunk* hitSound;    // For player hit sound
    Mix_Chunk* winSound;    // For win sound effect
    Mix_Chunk* buffSound;    // For win sound effect

    Uint32 lastBuffSpawnTime;     // Time of the last buff spawn
    Uint32 buffSpawnInterval; // Interval between buff spawns (10 seconds)
    Uint32 currentBuffSpawnTime;    // Time when the current buff spawned
    Uint32 buffDuration;      // Duration for which the buff stays on the map (e.g., 5 seconds)
    SDL_Point currentBuffLocation;  // Location of the currently spawned buff (tile coordinates)
    bool isBuffActive;              // Flag to track if a buff is currently active on the map

};

