#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> // Include SDL_image for image loading

class TextureManager {
public:
    // Load a texture from a file
    static SDL_Texture* LoadTexture(const char* texture);

    // Draw a texture to the screen
    static void Draw(SDL_Texture* texture, SDL_Rect sourceRect, SDL_Rect destRect, SDL_RendererFlip flip = SDL_FLIP_NONE);

};

