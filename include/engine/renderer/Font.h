#pragma once
#include <string>

// Font wraps a TTF_Font resource.
// Loaded via Renderer::loadFont(filePath, size).
// Stores point size for reference.
//
// Design rule:
// - SDL_ttf is NOT exposed in this API.
// - Created exclusively via Renderer::loadFont() (friend access).
// - Destructor frees the backend font resource automatically.
//
// [x] Private default constructor - only Renderer (friend) can construct.
// [x] Destructor: TTF_CloseFont on the opaque handle.
// [x] getSize() - returns point size the font was loaded at.

class Renderer;

class Font
{
public:
    ~Font();

    Font(const Font &) = delete;
    Font &operator=(const Font &) = delete;

    float getSize() const { return m_size; }

private:
    friend class Renderer;

    Font() = default;

    void *m_font = nullptr;
    float m_size = 0.f;
};