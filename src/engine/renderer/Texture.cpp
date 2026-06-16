#include "engine/renderer/Texture.h"
#include <SDL3/SDL.h>

// [x] Destructor: SDL_DestroyTexture on the opaque handle.
Texture::~Texture()
{
    if (m_texture)
        SDL_DestroyTexture(static_cast<SDL_Texture *>(m_texture));
}

int Texture::getWidth() const { return m_width; }
int Texture::getHeight() const { return m_height; }