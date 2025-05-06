#include "Game.h"
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include "Player.h"
#include "TextureManager.h"
#include <fstream>
#include "Bullet.h"
#include <cstdlib> // Cho rand() và srand()
#include <ctime>   // Cho time()
#include <string>

using namespace std;

SDL_Renderer* Game::renderer = nullptr;
vector<vector<int>> Game::map;      // Initialize static map
int Game::mapWidth = 0;                       // Initialize static mapWidth
int Game::mapHeight = 0;

GameState Game::gameState = MENU;
int Game::winner = 0; // Initialize static winner

Game::Game() {
    window = nullptr;
    font = nullptr;
    titlefont = nullptr;
    isRunning = false;

    gameState = MENU;
    font = nullptr;
    textColor = { 0, 0, 0, 255 }; // Black
    buttonTextColor = { 255, 255, 255, 255 }; // white

    // khoi tao cac bien
    playButtonRect.x = 0;
    playButtonRect.y = 0;
    playButtonRect.w = 0;
    playButtonRect.h = 0;
    highScoreButtonRect.x = 0;
    highScoreButtonRect.y = 0;
    highScoreButtonRect.w = 0;
    highScoreButtonRect.h = 0;
    creditsButtonRect.x = 0;
    creditsButtonRect.y = 0;
    creditsButtonRect.w = 0;
    creditsButtonRect.h = 0;
}

Game::~Game() {
    if (font) {
        TTF_CloseFont(font);
    }
    if (titlefont) {
        TTF_CloseFont(titlefont);
    }
}

bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    srand(static_cast<unsigned int>(time(0))); // Seed the random number generator with the current time

    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        cout << "Subsystems Initialized!..." << endl;

        window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if (window) {
            cout << "Window created!" << endl;
        }

        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // xanh mau pastel (179,235,242)
            cout << "Renderer created!" << endl;
            isRunning = true;
        }
        else {
            cout << "Renderer creation failed!" << endl;
            return false;
        }
    }
    else {
        cout << "SDL Init failed!" << endl;
        return false;
    }

    //initialize ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // Load font
    font = TTF_OpenFont("assets/PressStart2P-Regular.ttf", 24);
    titlefont = TTF_OpenFont("assets/PressStart2P-Regular.ttf", 30);
    if (!font || !titlefont) {
        cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << endl;
        return false;
    }

    // Load menu background
    menuBackgroundTexture = TextureManager::LoadTexture("assets/menu_background.jpg");
    if (!menuBackgroundTexture) {
        cout << "Failed to load menu background texture!" << endl;
        return false;
    }

    // Load button textures
    playButtonNormalTexture = TextureManager::LoadTexture("assets/button_texture.jpg");
    highScoreButtonNormalTexture = TextureManager::LoadTexture("assets/button_texture.jpg");
    creditsButtonNormalTexture = TextureManager::LoadTexture("assets/button_texture.jpg");

    if (!playButtonNormalTexture ||
        !highScoreButtonNormalTexture ||
        !creditsButtonNormalTexture)
    {
        cout << "Failed to load button textures!" << endl;
        return false;
    }

    // Load wall textures
    wallTexture = TextureManager::LoadTexture("assets/wall.png");
    if (!wallTexture) {
        cout << "Failed to load wall texture!" << endl;
        return false;
    }

    wall2Texture = TextureManager::LoadTexture("assets/wall2.png");
    if (!wall2Texture) {
        cout << "Failed to load wall2 texture!" << endl;
        return false;
    }

    wall2Texture_damaged1 = TextureManager::LoadTexture("assets/wall2_damaged1.png");
    if (!wall2Texture_damaged1) {
        cout << "Failed to load wall2_damaged1 texture!" << endl;
        return false;
    }

    wall2Texture_damaged2 = TextureManager::LoadTexture("assets/wall2_damaged2.png");
    if (!wall2Texture_damaged2) {
        cout << "Failed to load wall2_damaged2 texture!" << endl;
        return false;
    }

    targetP1Texture = TextureManager::LoadTexture("assets/targetP1.jpeg");
    if (!targetP1Texture) {
        cout << "Failed to load target 1 texture!" << endl;
        return false;
    }

    targetP2Texture = TextureManager::LoadTexture("assets/targetP2.jpeg");
    if (!targetP2Texture) {
        cout << "Failed to load target 2 texture!" << endl;
        return false;
    }

    buff3x3Texture = TextureManager::LoadTexture("assets/buff3x3.png");
    if (!buff3x3Texture) {
        cout << "Failed to load star3 texture!" << endl;
        return false;
    }

    bombTexture = TextureManager::LoadTexture("assets/bomb.png");
    if (!bombTexture) {
        cout << "Failed to load bomb texture!" << endl;
        return false;
    }


    // khoi tao nguoi choi
    player1 = new Player(175, 350, "assets/green_tank_spritesheet.png");
    player2 = new Player(1155, 350, "assets/red_tank_spritesheet.png");

    // khoi tao map
    Game::copyFileContent("assets/scr_map.txt", "assets/map.txt");
    loadMap("assets/map.txt");

	// Gan vi tri cua target
    targetP1TileCol = 37;
    targetP1TileRow = 10;

    targetP2TileCol = 1;
    targetP2TileRow = 10;

    // --- Initialize SDL_mixer ---
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0) {
        cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer could not open audio! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }
    cout << "SDL_mixer Initialized!..." << endl;

    // --- Load Sound Effects ---
    fireSound = Mix_LoadWAV("assets/sound/fire_sound.wav");
    if (!fireSound) {
        cout << "Failed to load fire sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    hitSound = Mix_LoadWAV("assets/sound/hit_sound.wav");
    if (!hitSound) {
        std::cerr << "Failed to load hit sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    winSound = Mix_LoadWAV("assets/sound/win_sound.wav");
    if (!winSound) {
        std::cerr << "Failed to load win sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    // --- Load Background Music ---
    music = Mix_LoadMUS("assets/sound/music.wav"); // Replace "assets/music.wav" with your actual music file path
    if (!music) {
        cout << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }
    cout << "Background Music Loaded!..." << std::endl;


    buffSound = Mix_LoadWAV("assets/sound/buff_powerup.wav");   // Replace "assets/win_sound.wav" with your actual file path
    if (!buffSound) {
        std::cerr << "Failed to load win sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    cout << "Sound Effects Loaded!..." << std::endl;

    // Cai dat buff time
    lastBuffSpawnTime = SDL_GetTicks(); // Initialize last spawn time to current time at game start
    buffSpawnInterval = 10000;         // 10 seconds in milliseconds
    buffDuration = 5000;             // 5 seconds buff duration
    isBuffActive = false;             // No buff active initially
    currentBuffLocation = { -1, -1 };    // Initialize to invalid location

    return true;
}


void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if (gameState == PLAYING) {
                handlePlayingEvents(event);
            }
            else if (gameState == MENU) {
                handleMenuEvents(event);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (gameState == MENU) {
                handleMenuEvents(event);
            }
            else if (gameState == CREDITS) {
                setGameState(MENU);
            }
            else if (gameState == GAME_OVER) {
                resetGame();
            }
            break;
        default:
            break;
        }
    }
}

void Game::update() {
	// Kiem tra neu nguoi choi co buff thi cap nhat buff
    int p1CenterX = player1->getRect().x + player1->getRect().w / 2;
    int p1CenterY = player1->getRect().y + player1->getRect().h / 2;
    int p1TileCol = p1CenterX / TILE_SIZE;
    int p1TileRow = p1CenterY / TILE_SIZE;
    if (p1TileRow >= 0 && p1TileRow < mapHeight && p1TileCol >= 0 && p1TileCol < mapWidth) {
        if (map[p1TileRow][p1TileCol] == 7 && !player1->hasBuff) {
            player1->hasBuff = true;
			map[p1TileRow][p1TileCol] = 0; // Xoa buff khoi map

            isBuffActive = false;
            currentBuffLocation = { -1, -1 };
            cout << "Player 1 picked up the 3x3 buff!" << endl;
            Mix_PlayChannel(-1, buffSound, 0);
        }
    }

	// Kiem tra neu nguoi choi co buff thi cap nhat buff
    int p2CenterX = player2->getRect().x + player2->getRect().w / 2;
    int p2CenterY = player2->getRect().y + player2->getRect().h / 2;
    int p2TileCol = p2CenterX / TILE_SIZE;
    int p2TileRow = p2CenterY / TILE_SIZE;
    if (p2TileRow >= 0 && p2TileRow < mapHeight && p2TileCol >= 0 && p2TileCol < mapWidth) {
        if (map[p2TileRow][p2TileCol] == 7 && !player2->hasBuff) {
            player2->hasBuff = true;
			map[p2TileRow][p2TileCol] = 0; // Xoa buff khoi map

            isBuffActive = false;
            currentBuffLocation = { -1, -1 };
            cout << "Player 2 picked up the 3x3 buff!" << endl;
            Mix_PlayChannel(-1, buffSound, 0);
        }
    }


    for (int i = 0; i < bullets.size(); ++i) {
        if (bullets[i]->isActive()) {
            bullets[i]->update(); // Call update on active bullets

            // --- Bullet-Player Collision Detection ---
            SDL_Rect bulletRect = bullets[i]->getRect();
            SDL_Rect player1Rect = player1->getRect();
            SDL_Rect player2Rect = player2->getRect();

            // Bullet vs Player 1
            if (bullets[i]->isActive() && // Check if bullet is still active after update()
                bulletRect.x < player1Rect.x + player1Rect.w &&
                bulletRect.x + bulletRect.w > player1Rect.x &&
                bulletRect.y < player1Rect.y + player1Rect.h &&
                bulletRect.y + bulletRect.h > player1Rect.y)
            {
                bullets[i]->deactivate(); // Deactivate bullet
                Mix_PlayChannel(-1, hitSound, 0); // Play hit sound
                player1->respawn();      // Player 1 respawns
                cout << "Player 1 hit!" << endl;
            }
            // Bullet vs Player 2
            if (bullets[i]->isActive() && // Check if bullet is still active after update()
                bulletRect.x < player2Rect.x + player2Rect.w &&
                bulletRect.x + bulletRect.w > player2Rect.x &&
                bulletRect.y < player2Rect.y + player2Rect.h &&
                bulletRect.y + bulletRect.h > player2Rect.y)
            {
                bullets[i]->deactivate(); // Deactivate bullet
                Mix_PlayChannel(-1, hitSound, 0); // Play hit sound
                player2->respawn();      // Player 2 respawns
                cout << "Player 2 hit!" << endl;
            }

        }
        else {
            // If bullet is not active, delete it and remove from vector
            delete bullets[i];
            bullets.erase(bullets.begin() + i);
            --i; // Adjust index after erasing
        }
    }

    // --- Win condition check based on TILE coordinates ---
    if (Game::gameState == PLAYING) {
        int p1TileCol = player1->getRect().x / TILE_SIZE; // Player 1 tile column
        int p1TileRow = player1->getRect().y / TILE_SIZE; // Player 1 tile row

        int p2TileCol = player2->getRect().x / TILE_SIZE; // Player 2 tile column
        int p2TileRow = player2->getRect().y / TILE_SIZE; // Player 2 tile row

        if (p1TileCol == targetP1TileCol && p1TileRow == targetP1TileRow) {
            cout << "Player 1 Wins! (Tile Coordinates Match)" << endl;
            Mix_PlayChannel(-1, winSound, 0); // Play win sound
            Game::setGameState(GAME_OVER);
            Game::setWinner(1);
        }
        else if (p2TileCol == targetP2TileCol && p2TileRow == targetP2TileRow) {
            cout << "Player 2 Wins! (Tile Coordinates Match)" << endl;
			Mix_PlayChannel(-1, winSound, 0);
            Game::setGameState(GAME_OVER);
            Game::setWinner(2);
        }
    }

    // --- Start Background Music (if not already playing and in PLAYING state) ---
    if (gameState == PLAYING && Mix_PlayingMusic() == 0) { // Check game state and if music is NOT already playing
        if (Mix_PlayMusic(music, -1) == -1) { // Start playing music, loop infinitely (-1)
            cout << "Failed to play background music! SDL_mixer Error: " << Mix_GetError() << endl;
        }
    }

	// Dung nhac khi game over
	if (gameState == GAME_OVER && Mix_PlayingMusic() == 1) { // Check game state and if music is playing
		Mix_HaltMusic(); // Stop playing music
	}

    if (gameState == PLAYING) {
        const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

        // Player 1 movement (WASD)
        if (keyboardState[SDL_SCANCODE_W]) {
            player1->move(0, -1, mapWidth * TILE_SIZE, mapHeight * TILE_SIZE, map, player2);
        }
        if (keyboardState[SDL_SCANCODE_S]) {
            player1->move(0, 1, mapWidth * TILE_SIZE, mapHeight * TILE_SIZE, map, player2);
        }
        if (keyboardState[SDL_SCANCODE_A]) {
            player1->move(-1, 0, mapWidth * TILE_SIZE, mapHeight * TILE_SIZE, map, player2);
        }
        if (keyboardState[SDL_SCANCODE_D]) {
            player1->move(1, 0, mapWidth * TILE_SIZE, mapHeight * TILE_SIZE, map, player2);
        }

        // Player 2 movement (Arrow Keys)
        if (keyboardState[SDL_SCANCODE_UP]) {
            player2->move(0, -1, mapWidth * TILE_SIZE, mapHeight * TILE_SIZE, map, player1);
        }
        if (keyboardState[SDL_SCANCODE_DOWN]) {
            player2->move(0, 1, mapWidth * TILE_SIZE, mapHeight * TILE_SIZE, map, player1);
        }
        if (keyboardState[SDL_SCANCODE_LEFT]) {
            player2->move(-1, 0, mapWidth * TILE_SIZE, mapHeight * TILE_SIZE, map, player1);
        }
        if (keyboardState[SDL_SCANCODE_RIGHT]) {
            player2->move(1, 0, mapWidth * TILE_SIZE, mapHeight * TILE_SIZE, map, player1);
        }
    }

    // --- Buff Spawning Logic ---
    Uint32 currentTime = SDL_GetTicks();

    if (!isBuffActive) {
        if (currentTime - lastBuffSpawnTime >= buffSpawnInterval) {
            lastBuffSpawnTime = currentTime;


            int spawnTileRow, spawnTileCol;
            bool foundSpawnPoint = false;
            int maxAttempts = 100;

            for (int attempt = 0; attempt < maxAttempts; ++attempt) {
                spawnTileRow = rand() % mapHeight;
                spawnTileCol = rand() % mapWidth;

                if (map[spawnTileRow][spawnTileCol] == 0) {
                    foundSpawnPoint = true;
                    break;
                }
            }

            if (foundSpawnPoint) {
                map[spawnTileRow][spawnTileCol] = 7;
                currentBuffLocation = { spawnTileCol, spawnTileRow };
                currentBuffSpawnTime = currentTime;
                isBuffActive = true;
                std::cout << "Spawned buff at (" << spawnTileRow << ", " << spawnTileCol << ")" << std::endl;
            }
            else {
                std::cout << "Failed to find empty tile to spawn buff after " << maxAttempts << " attempts." << std::endl;
            }
        }
    }
    else {
        if (currentTime - currentBuffSpawnTime >= buffDuration) {
            if (currentBuffLocation.x != -1 && currentBuffLocation.y != -1) {
                if (map[currentBuffLocation.y][currentBuffLocation.x] == 7) {
                    map[currentBuffLocation.y][currentBuffLocation.x] = 0;
                    cout << "Buff at (" << currentBuffLocation.y << ", " << currentBuffLocation.x << ") disappeared." << endl;
                }
            }
            isBuffActive = false;
            currentBuffLocation = { -1, -1 };

        }
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    // kiem tra trang thai game truoc khi render
    switch (gameState) {
    case MENU:
        renderMenu();
        break;
    case PLAYING:
        renderMap();
        player1->render();
        player2->render();
        for (Bullet* bullet : bullets) { // Loop through ALL bullets, update and render
            bullet->render(); //The bullet check if it is active.
        }
        break;
    case GAME_OVER:
        renderGameOver();
        break;
    case CREDITS:
        renderCredits();
        break;
    }

    SDL_RenderPresent(renderer);
}

void Game::renderMenu() {

    // back ground
    SDL_Rect backgroundRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderCopy(renderer, menuBackgroundTexture, NULL, &backgroundRect);

    // Title
    SDL_Surface* titleSurface = TTF_RenderText_Solid(titlefont, "Tank battle 2P", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect;
    titleRect.w = titleSurface->w;
    titleRect.h = titleSurface->h;
    titleRect.x = SCREEN_WIDTH / 2 - titleSurface->w / 2; // center to horizontal
    titleRect.y = 150; // set to 100 from top
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Button1: PLAY
    playButtonRect.x = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2; // horizontal center - half of button with
    playButtonRect.y = 300; // titleRect.y + BUTTON_SPACE + 50; // spawn cách title = 1 but_space + 50 (+ 50 vao cho dep)
    playButtonRect.w = BUTTON_WIDTH; // button width
    playButtonRect.h = BUTTON_HEIGHT; // button height
    SDL_RenderCopy(renderer, playButtonNormalTexture, NULL, &playButtonRect); // render texture

    //SDL_RenderFillRect(renderer, &playButtonRect);

    SDL_Surface* playSurface = TTF_RenderText_Solid(font, "PLAY", buttonTextColor);
    SDL_Texture* playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_Rect playTextRect;
    playTextRect.w = playSurface->w;
    playTextRect.h = playSurface->h;
    playTextRect.x = playButtonRect.x + (playButtonRect.w - playSurface->w) / 2; // Center text horizontally
    playTextRect.y = playButtonRect.y + (playButtonRect.h - playSurface->h) / 2; // Center text vertically
    SDL_RenderCopy(renderer, playTexture, NULL, &playTextRect);
    SDL_FreeSurface(playSurface);
    SDL_DestroyTexture(playTexture);




    // Button2: HIGH SCORE
    highScoreButtonRect.x = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2;
    highScoreButtonRect.y = 450; //playButtonRect.y + BUTTON_SPACE; // tinh theo playbutton
    highScoreButtonRect.w = BUTTON_WIDTH;
    highScoreButtonRect.h = BUTTON_HEIGHT;

    //SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for button
    SDL_RenderCopy(renderer, highScoreButtonNormalTexture, NULL, &highScoreButtonRect);
    //SDL_RenderFillRect(renderer, &highScoreButtonRect);

    SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, "CREDIT", buttonTextColor);
    SDL_Texture* highScoreTexture = SDL_CreateTextureFromSurface(renderer, highScoreSurface);
    SDL_Rect highScoreTextRect;
    highScoreTextRect.w = highScoreSurface->w;
    highScoreTextRect.h = highScoreSurface->h;
    highScoreTextRect.x = highScoreButtonRect.x + (highScoreButtonRect.w - highScoreSurface->w) / 2; // Center text horizontally
    highScoreTextRect.y = highScoreButtonRect.y + (highScoreButtonRect.h - highScoreSurface->h) / 2; // Center text vertically
    SDL_RenderCopy(renderer, highScoreTexture, NULL, &highScoreTextRect);
    SDL_FreeSurface(highScoreSurface);
    SDL_DestroyTexture(highScoreTexture);

    // Button3: CREDITS
    creditsButtonRect.x = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2;
    creditsButtonRect.y = 600; //highScoreButtonRect.y + BUTTON_SPACE;
    creditsButtonRect.w = BUTTON_WIDTH;
    creditsButtonRect.h = BUTTON_HEIGHT;
    // SDL_SetRenderDrawColor(renderer, 120, 0, 255, 255); // Blue color for button

    SDL_RenderCopy(renderer, creditsButtonNormalTexture, NULL, &creditsButtonRect);
    //SDL_RenderFillRect(renderer, &creditsButtonRect);

    SDL_Surface* creditsSurface = TTF_RenderText_Solid(font, "QUIT", buttonTextColor);
    SDL_Texture* creditsTexture = SDL_CreateTextureFromSurface(renderer, creditsSurface);
    SDL_Rect creditsTextRect;
    creditsTextRect.w = creditsSurface->w;
    creditsTextRect.h = creditsSurface->h;
    creditsTextRect.x = creditsButtonRect.x + (creditsButtonRect.w - creditsSurface->w) / 2; // Center text horizontally
    creditsTextRect.y = creditsButtonRect.y + (creditsButtonRect.h - creditsSurface->h) / 2; // Center text vertically
    SDL_RenderCopy(renderer, creditsTexture, NULL, &creditsTextRect);
    SDL_FreeSurface(creditsSurface);
    SDL_DestroyTexture(creditsTexture);

    // Initialize button rectangles *after* rendering them:
}

void Game::handleMenuEvents(const SDL_Event& event) {
    switch (event.type) {
    case SDL_QUIT:
        isRunning = false;
        break;
    case SDL_MOUSEBUTTONDOWN: {
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        cout << "Mouse click at: (" << mouseX << ", " << mouseY << ")" << endl;

        //Check if PLAY button was clicked
        if (mouseX >= playButtonRect.x && mouseX <= playButtonRect.x + playButtonRect.w &&
            mouseY >= playButtonRect.y && mouseY <= playButtonRect.y + playButtonRect.h) {
            gameState = PLAYING;
            cout << "PLAY button clicked!\n";
        }
        // Check if CREDIT button was clicked
        else if (mouseX >= highScoreButtonRect.x && mouseX <= highScoreButtonRect.x + highScoreButtonRect.w &&
            mouseY >= highScoreButtonRect.y && mouseY <= highScoreButtonRect.y + highScoreButtonRect.h) {
            gameState = CREDITS;
            cout << "CREDITS button clicked!\n";
        }
        // Check if CREDITS button was clicked
        else if (mouseX >= creditsButtonRect.x && mouseX <= creditsButtonRect.x + creditsButtonRect.w &&
            mouseY >= creditsButtonRect.y && mouseY <= creditsButtonRect.y + creditsButtonRect.h) {
            gameState = QUIT;
            cout << "QUIT button clicked!\n";
			isRunning = false;
        }
        break;
    }
    default:
        break;
    }
}

void Game::clean() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    if (menuBackgroundTexture) {
        SDL_DestroyTexture(menuBackgroundTexture);
        menuBackgroundTexture = nullptr;
    }
    if (playButtonNormalTexture) {
        SDL_DestroyTexture(playButtonNormalTexture);
        playButtonNormalTexture = nullptr;
    }
    if (highScoreButtonNormalTexture) {
        SDL_DestroyTexture(highScoreButtonNormalTexture);
        highScoreButtonNormalTexture = nullptr;
    }
    if (creditsButtonNormalTexture) {
        SDL_DestroyTexture(creditsButtonNormalTexture);
        creditsButtonNormalTexture = nullptr;
    }

	// --- SDL_ttf Cleanup ---
    if (font) { // Check if font is loaded before closing and quitting
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (titlefont) {
        TTF_CloseFont(titlefont);
        titlefont = nullptr;
    }
    TTF_Quit();

    for (Bullet* bullet : bullets) {
        delete bullet;
    }
    bullets.clear(); // Clear the bullets vector

    // --- SDL_mixer Cleanup ---
    if (fireSound) {
        Mix_FreeChunk(fireSound);
        fireSound = nullptr;
    }
    if (hitSound) {
        Mix_FreeChunk(hitSound);
        hitSound = nullptr;
    }
    if (winSound) {
        Mix_FreeChunk(winSound);
        winSound = nullptr;
    }
	if (music) {
		Mix_FreeMusic(music);
		music = nullptr;
	}

    Mix_CloseAudio(); // Close the audio device
    Mix_Quit();       // Quit SDL_mixer
    cout << "SDL_mixer Cleaned!..." << std::endl;

    SDL_Quit();
    cout << "Game Cleaned" << std::endl;
}


void Game::handlePlayingEvents(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_SPACE) { // Player 1 shoots with SPACE
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - player1->getLastShotTime() >= player1->getFireRate()) {
                player1->setLastShotTime(currentTime);
                FacingDirection p1Facing = player1->getFacingDirection();
                int bulletDirX = 0;
                int bulletDirY = 0;
                int spawnOffsetX = 0;
                int spawnOffsetY = 0;
                switch (p1Facing) {
                case UP:    bulletDirY = -1; spawnOffsetY = -30; break;
                case DOWN:  bulletDirY = 1;  spawnOffsetY = 30;  break;
                case LEFT:  bulletDirX = -1; spawnOffsetX = -30; break;
                case RIGHT: bulletDirX = 1;  spawnOffsetX = 30;  break;
                }
                Bullet* bullet = new Bullet(
                    player1->getRect().x + player1->getRect().w / 2 - 8 + spawnOffsetX,
                    player1->getRect().y + player1->getRect().h / 2 - 8 + spawnOffsetY,
                    bulletDirX, bulletDirY,
                    "assets/bullet_spritesheet.png"
                );
                bullets.push_back(bullet);
                Mix_PlayChannel(-1, fireSound, 0); \
            }
        }
        else if (event.key.keysym.sym == SDLK_SLASH) { // Player 2 shoots with '/'
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - player2->getLastShotTime() >= player2->getFireRate()) {
                player2->setLastShotTime(currentTime);
                FacingDirection p2Facing = player2->getFacingDirection();
                int bulletDirX = 0;
                int bulletDirY = 0;
                int spawnOffsetX = 0;
                int spawnOffsetY = 0;
                switch (p2Facing) {
                case UP:    bulletDirY = -1; spawnOffsetY = -30; break;
                case DOWN:  bulletDirY = 1;  spawnOffsetY = 30;  break;
                case LEFT:  bulletDirX = -1; spawnOffsetX = -30; break;
                case RIGHT: bulletDirX = 1;  spawnOffsetX = 30;  break;
                }
                Bullet* bullet = new Bullet(
                    player2->getRect().x + player2->getRect().w / 2 - 8 + spawnOffsetX,
                    player2->getRect().y + player2->getRect().h / 2 - 8 + spawnOffsetY,
                    bulletDirX, bulletDirY,
                    "assets/bullet_spritesheet.png"
                );
                bullets.push_back(bullet);
                Mix_PlayChannel(-1, fireSound, 0); // Play fire sound effect
            }
        }
        else if (event.key.keysym.sym == SDLK_e && player1->hasBuff) { // Player 1 ban dan buff
            player1->hasBuff = false; // Dung buff mot lan roi mat
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - player1->getLastShotTime() >= player1->getFireRate()) {
                player1->setLastShotTime(currentTime);
                FacingDirection p1Facing = player1->getFacingDirection();
                int bulletDirX = 0;
                int bulletDirY = 0;
                int spawnOffsetX = 0;
                int spawnOffsetY = 0;
                switch (p1Facing) {
                case UP:    bulletDirY = -1; spawnOffsetY = -30; break;
                case DOWN:  bulletDirY = 1;  spawnOffsetY = 30;  break;
                case LEFT:  bulletDirX = -1; spawnOffsetX = -30; break;
                case RIGHT: bulletDirX = 1;  spawnOffsetX = 30;  break;
                }

                Bullet* bullet = new Bullet(
                    player1->getRect().x + player1->getRect().w / 2 - 8 + spawnOffsetX,
                    player1->getRect().y + player1->getRect().h / 2 - 8 + spawnOffsetY,
                    bulletDirX, bulletDirY,
                    "assets/bomb.png" // (Tùy chọn) Dùng texture khác cho đạn buff
                );
				bullet->isBuffBullet = true; // Danh dau la dan buff
                bullets.push_back(bullet);
                Mix_PlayChannel(-1, fireSound, 0);
            }
        }
        else if (event.key.keysym.sym == SDLK_m && player2->hasBuff) { // Player 2 ban dan buff
			player2->hasBuff = false; // Dung buff mot lan roi mat
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - player2->getLastShotTime() >= player2->getFireRate()) {
                player2->setLastShotTime(currentTime);
                FacingDirection p2Facing = player2->getFacingDirection();
                int bulletDirX = 0;
                int bulletDirY = 0;
                int spawnOffsetX = 0;
                int spawnOffsetY = 0;
                switch (p2Facing) {
                case UP:    bulletDirY = -1; spawnOffsetY = -30; break;
                case DOWN:  bulletDirY = 1;  spawnOffsetY = 30;  break;
                case LEFT:  bulletDirX = -1; spawnOffsetX = -30; break;
                case RIGHT: bulletDirX = 1;  spawnOffsetX = 30;  break;
                }
				// Tao dan buff
                Bullet* bullet = new Bullet(
                    player2->getRect().x + player2->getRect().w / 2 - 8 + spawnOffsetX,
                    player2->getRect().y + player2->getRect().h / 2 - 8 + spawnOffsetY,
                    bulletDirX, bulletDirY,
                    "assets/bomb.png"
                );
				bullet->isBuffBullet = true; // Danh dau la dan buff
                bullets.push_back(bullet);
                Mix_PlayChannel(-1, fireSound, 0);
            }
        }
        else if (event.key.keysym.sym == SDLK_ESCAPE) {
            setGameState(MENU); // Return to menu on ESC
        }
    }
}

bool Game::loadMap(const char* filePath) {
    ifstream mapFile(filePath);
    if (!mapFile.is_open()) {
        cout << "Failed to open map file: " << filePath << endl;
        return false;
    }

    mapFile >> mapWidth >> mapHeight;  // Read width and height
    map.resize(mapHeight, vector<int>(mapWidth)); // Resize the map vector

    for (int row = 0; row < mapHeight; row++) {
        for (int col = 0; col < mapWidth; col++) {
            char tileChar;
            mapFile >> tileChar; // Read the tile as a character
            map[row][col] = tileChar - '0'; // Convert character '0', '1', etc., to integer 0, 1, etc.
        }
    }
    mapFile.close();
    return true;
}

void Game::renderMap() {
    for (int row = 0; row < mapHeight; ++row) {
        for (int col = 0; col < mapWidth; ++col) {
            int tileType = map[row][col];

            SDL_Rect destRect; // Destination rectangle (on the screen)
            destRect.x = col * TILE_SIZE;  // Calculate x position based on column and tile width
            destRect.y = row * TILE_SIZE; // Calculate y position based on row and tile height
            destRect.w = TILE_SIZE;
            destRect.h = TILE_SIZE;

            switch (tileType) {
            case 0: // Empty space - do nothing (just background will show)
                break;
            case 1: // Indestructible wall
                SDL_RenderCopy(renderer, wallTexture, NULL, &destRect); // Draw wall texture
                break;
            case 2:
                SDL_RenderCopy(renderer, wall2Texture, NULL, &destRect);  // Draw wall texture
                break;
            case 3:
                SDL_RenderCopy(renderer, wall2Texture_damaged1, NULL, &destRect);  // Draw wall texture
                break;
            case 4:
                SDL_RenderCopy(renderer, wall2Texture_damaged2, NULL, &destRect);  // Draw wall texture
                break;
            case 8:
                SDL_RenderCopy(renderer ,targetP1Texture, NULL, &destRect); // Draw target1 texture
                break;
            case 5:
                SDL_RenderCopy(renderer, targetP2Texture, NULL, &destRect); // Draw target2 texture
                break;
            case 7:
                SDL_RenderCopy(renderer, buff3x3Texture, NULL, &destRect); // Draw target2 texture
                break;
            }
        }
    }
}


void Game::copyFileContent(const std::string& sourceFilePath, const std::string& destinationFilePath) {
    ofstream destFileClear(destinationFilePath, ios::trunc);
    destFileClear.close();

    ifstream sourceFile(sourceFilePath);
    if (!sourceFile.is_open()) {
        cout << "Khong the mo tep nguon " << sourceFilePath << endl;
        return;
    }

    ofstream destFile(destinationFilePath, ios::app);
    if (!destFile.is_open()) {
        cout << "Khong the mo tep dich " << destinationFilePath << endl;
        sourceFile.close();
        return;
    }

    string line;
    while (std::getline(sourceFile, line)) {
        destFile << line << std::endl;
    }

    sourceFile.close();
    destFile.close();

    cout << "Noi dung cua '" << sourceFilePath << "' da duoc sao chep vao '" << destinationFilePath << "'" << endl;
}

void Game::renderGameOver() {
    // Black background for Game Over screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
    SDL_RenderClear(renderer);

    SDL_Color winTextColor = { 255, 255, 255, 255 }; // White text for win message
    SDL_Color menuTextColor = { 200, 200, 200, 255 }; // Grayish for "click to menu"

    // Determine win message based on winner
    string winMessage;
    if (Game::getWinner() == 1) {
        winMessage = "Player 1 Wins!";
    }
    else if (Game::getWinner() == 2) {
        winMessage = "Player 2 Wins!";
    }

    // 1. Render "Player X Wins!" Text
    SDL_Surface* winSurface = TTF_RenderText_Solid(titlefont, winMessage.c_str(), winTextColor);
    SDL_Texture* winTexture = SDL_CreateTextureFromSurface(renderer, winSurface);
    SDL_Rect winRect;
    winRect.w = winSurface->w;
    winRect.h = winSurface->h;
    winRect.x = SCREEN_WIDTH / 2 - winSurface->w / 2; // Center horizontally
    winRect.y = SCREEN_HEIGHT / 2 - winSurface->h / 2 - 50; // Position a bit above center
    SDL_RenderCopy(renderer, winTexture, NULL, &winRect);
    SDL_FreeSurface(winSurface);
    SDL_DestroyTexture(winTexture);

    // 2. Render "Click anywhere to return to menu" Text
    SDL_Surface* menuSurface = TTF_RenderText_Solid(font, "Click anywhere to return to menu", menuTextColor);
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);
    SDL_Rect menuRect;
    menuRect.w = menuSurface->w;
    menuRect.h = menuSurface->h;
    menuRect.x = SCREEN_WIDTH / 2 - menuSurface->w / 2; // Center horizontally
    menuRect.y = winRect.y + winRect.h + 30; // Position below win message
    SDL_RenderCopy(renderer, menuTexture, NULL, &menuRect);
    SDL_FreeSurface(menuSurface);
    SDL_DestroyTexture(menuTexture);
}

void Game::resetGame() {
    gameState = MENU; // Set game state back to MENU
    winner = 0;      // Reset winner

    player1->respawn(); // Respawn players to starting positions
    player2->respawn();

    bullets.clear();    // Clear all bullets

    Game::copyFileContent("assets/scr_map.txt", "assets/map.txt"); // Reload map from source
    loadMap("assets/map.txt");

    cout << "Game Reset and Back to Menu" << endl;
}

void Game::renderCredits() {
    // Black background for Game Over screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
    SDL_RenderClear(renderer);

    SDL_Color hovaten = { 255, 255, 255, 255 }; // White text for win message
    SDL_Color mssv = { 255, 255, 255, 255 }; // Grayish for "click to menu"
	SDL_Color lhp = { 255, 255, 255, 255 };


    // 1. Render hovaten
    SDL_Surface* tenSurface = TTF_RenderText_Solid(titlefont, "DO HOANG DAT", hovaten);
    SDL_Texture* tenTexture = SDL_CreateTextureFromSurface(renderer, tenSurface);
    SDL_Rect tenRect;
    tenRect.w = tenSurface->w;
    tenRect.h = tenSurface->h;
    tenRect.x = SCREEN_WIDTH / 2 - tenSurface->w / 2; // Center horizontally
    tenRect.y = SCREEN_HEIGHT / 2 - tenSurface->h / 2 - 50; // Position a bit above center
    SDL_RenderCopy(renderer, tenTexture, NULL, &tenRect);
    SDL_FreeSurface(tenSurface);
    SDL_DestroyTexture(tenTexture);

    // 2. Render mssv
    SDL_Surface* mssvSurface = TTF_RenderText_Solid(font, "24020060", mssv);
    SDL_Texture* mssvTexture = SDL_CreateTextureFromSurface(renderer, mssvSurface);
    SDL_Rect mssvRect;
    mssvRect.w = mssvSurface->w;
    mssvRect.h = mssvSurface->h;
    mssvRect.x = SCREEN_WIDTH / 2 - mssvSurface->w / 2; // Center horizontally
    mssvRect.y = tenRect.y + tenRect.h + 30; // Position below win message
    SDL_RenderCopy(renderer, mssvTexture, NULL, &mssvRect);
    SDL_FreeSurface(mssvSurface);
    SDL_DestroyTexture(mssvTexture);

    SDL_Surface* lhpSurface = TTF_RenderText_Solid(font, "INT2215-4 Nhom 2", lhp);
    SDL_Texture* lhpTexture = SDL_CreateTextureFromSurface(renderer, lhpSurface);
    SDL_Rect lhpRect;
    lhpRect.w = lhpSurface->w;
    lhpRect.h = lhpSurface->h;
    lhpRect.x = SCREEN_WIDTH / 2 - lhpSurface->w / 2; // Center horizontally
    lhpRect.y = mssvRect.y + mssvRect.h + 30; // Position below win message
    SDL_RenderCopy(renderer, lhpTexture, NULL, &lhpRect);
    SDL_FreeSurface(lhpSurface);
    SDL_DestroyTexture(lhpTexture);
}

void Game::destroy3x3Walls(int tileCol, int tileRow) {
    for (int r = tileRow - 1; r <= tileRow + 1; r++) { // Iterate over 3 rows
        for (int c = tileCol - 1; c <= tileCol + 1; c++) { // Iterate over 3 columns
            if (r >= 0 && r < mapHeight && c >= 0 && c < mapWidth) { // Check map boundaries
                int tileType = map[r][c];
                if (tileType == 2 || tileType == 3) { // Only affect destructible walls (types 2, 3, 4)
                    map[r][c] = 4; // Set to 0 to destroy the wall (empty space)
				}
				else if (tileType == 4) { // Player 2 target
					map[r][c] = 0;
				}
            }
        }
    }
}
