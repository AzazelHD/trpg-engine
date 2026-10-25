
#pragma once

// Font wraps a TTF_Font resource.
// Loaded exclusively through Renderer::loadFont(filePath, size).
//
// Design rules:
// - SDL_ttf is not exposed through this public API.
// - Only Renderer can construct a Font.
// - The destructor releases the backend font resource.
// - Font is non-copyable and non-movable to prevent unsafe ownership
//   of the opaque backend handle.

class Renderer;

class Font
{
public:
    ~Font();

    Font(const Font &) = delete;
    Font &operator=(const Font &) = delete;

    // Prevent accidental transfer of the owned backend handle.
    Font(Font &&) = delete;
    Font &operator=(Font &&) = delete;

    // Returns the point size used when this font was loaded.
    float getSize() const { return m_size; }

private:
    friend class Renderer;

    Font() = default;

    void *m_font = nullptr;
    float m_size = 0.0f;
};
