#include "Game.h"

Game* game = nullptr;


int main(int argc, char* argv[]) {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;  // Milliseconds per frame

    Uint32 frameStart;
    int frameTime;

    game = new Game();

    game->init("Tank battle 2P", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, false);

    while (game->running()) {
        frameStart = SDL_GetTicks();

        game->handleEvents();
        game->update();
        game->render();

        frameTime = SDL_GetTicks() - frameStart;

        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    game->clean();
    delete game;
    return 0;
}

