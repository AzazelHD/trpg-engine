#include "engine/renderer/Texture.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdexcept>

// Texture loads an image file into a GPU texture using the SDL backend.
// It stores only width/height and an opaque backend handle.
//
// Design rules:
// - SDL is strictly an implementation detail of this file
// - m_texture is treated as opaque outside this file
// - no SDL types are exposed in Texture.h
// - renderer dependency exists only for creation time
//
// Lifetime:
// - created with a renderer + file path
// - destroyed automatically in destructor
//
// [x] Load surface from file (IMG_Load)
// [x] Create GPU texture from surface
// [x] Free surface after upload
// [x] Cache width/height from created texture
// [x] Destroy GPU texture in destructor

Texture::Texture(SDL_Renderer *renderer, const char *filePath)
{
    SDL_Surface *surface = IMG_Load(filePath);
    if (!surface)
        throw std::runtime_error(SDL_GetError());

    // FIX: must use provided renderer, not nullptr
    m_texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!m_texture)
    {
        SDL_DestroySurface(surface);
        throw std::runtime_error(SDL_GetError());
    }

    SDL_DestroySurface(surface);

    float w = 0.f, h = 0.f;
    SDL_GetTextureSize((SDL_Texture *)m_texture, &w, &h);

    m_width = (int)w;
    m_height = (int)h;
}

Texture::~Texture()
{
    if (m_texture)
        SDL_DestroyTexture((SDL_Texture *)m_texture);
}

int Texture::getWidth() const { return m_width; }
int Texture::getHeight() const { return m_height; }