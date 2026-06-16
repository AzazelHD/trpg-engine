#include "engine/renderer/Font.h"
#include <SDL3_ttf/SDL_ttf.h>

// [x] Destructor: TTF_CloseFont on the opaque handle.
Font::~Font()
{
    if (m_font)
        TTF_CloseFont(static_cast<TTF_Font *>(m_font));
}