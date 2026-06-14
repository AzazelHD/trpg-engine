#pragma once

// Texture wraps a GPU texture resource.
// Loads from file via engine renderer backend.
// Stores width/height for fast access.
//
// Design rule:
// - SDL is NOT exposed in this API
// - Texture is backend-agnostic
// - Only renderer implementation may access GPU handle
//
// Lifetime:
// - created by engine resource system or renderer
// - destroyed automatically
//
// [x] Define constructor (filePath-based loading via backend renderer)
// [x] Implement destructor (free backend GPU resource)
// [x] Store width/height after loading

struct SDL_Renderer;

class Texture
{
public:
    explicit Texture(SDL_Renderer *renderer, const char *filePath);
    ~Texture();

    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    int getWidth() const;
    int getHeight() const;

private:
    friend class SpriteBatch;

    void *getNativeHandle() const { return m_texture; }

    void *m_texture = nullptr;
    int m_width = 0;
    int m_height = 0;
};