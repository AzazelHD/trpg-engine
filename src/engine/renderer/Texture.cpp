#include "engine/renderer/Texture.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdexcept>

// [x]: Implement the constructor.
//   SDL_Surface* surface = IMG_Load(filePath);
//   if (!surface) throw std::runtime_error(IMG_GetError());
//   m_texture = SDL_CreateTextureFromSurface(renderer, surface);
//   SDL_DestroySurface(surface);
//   float w = 0.f, h = 0.f;
//   SDL_GetTextureSize(m_texture, &w, &h);
//   m_width = (int)w; m_height = (int)h;

// [x]: Implement the destructor.
//   SDL_DestroyTexture(m_texture);

Texture::Texture(SDL_Renderer *renderer, const char *filePath)
{
    SDL_Surface *surface = IMG_Load(filePath);
    if (!surface)
    {
        throw std::runtime_error(SDL_GetError());
    }
    m_texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!m_texture)
    {
        SDL_DestroySurface(surface);
        throw std::runtime_error(SDL_GetError());
    }
    SDL_DestroySurface(surface);
    float w = 0.f, h = 0.f;
    SDL_GetTextureSize(m_texture, &w, &h);
    m_width = static_cast<int>(w);
    m_height = static_cast<int>(h);
}

Texture::~Texture()
{
    if (m_texture)
        SDL_DestroyTexture(m_texture);
}

int Texture::getWidth() const
{
    return m_width;
}

int Texture::getHeight() const
{
    return m_height;
}

SDL_Texture *Texture::getSDL_Texture() const
{
    return m_texture;
}