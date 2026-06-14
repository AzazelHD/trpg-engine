#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

// Texture wraps an SDL_Texture. Loads from file, exposes dimensions.
// Textures are created once and reused — never load the same file twice.
//
// [x]: Declare the class with:
//   - Constructor: Texture(SDL_Renderer* renderer, const char* filePath)
//                  Use IMG_Load + SDL_CreateTextureFromSurface, then free the surface.
//   - Destructor: SDL_DestroyTexture
//   - getWidth(), getHeight() — read from SDL_QueryTexture in the constructor and cache.
//   - getSDLTexture() — returns the raw SDL_Texture* (used internally by SpriteBatch).
//
// Design note: do NOT forward-declare SDL_Renderer/SDL_Texture in this header —
// they are C structs and forward-declaration is messy. Just include SDL.h and SDL_image.h
// only in Texture.cpp, and use void* or a pimpl if you want zero SDL in the header.
// For simplicity at this stage, it's acceptable to include SDL here.
class Texture
{
private:
    SDL_Texture *m_texture;
    int m_width, m_height;

public:
    Texture(SDL_Renderer *renderer, const char *filePath);
    ~Texture();
    int getWidth() const;
    int getHeight() const;
    SDL_Texture *getSDL_Texture() const;
};
