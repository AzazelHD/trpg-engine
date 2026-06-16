#pragma once

// Texture wraps a GPU texture resource (opaque handle) + cached width/height.
//
// Design rule:
// - SDL is NOT exposed in this API.
// - Created exclusively via Renderer::loadTexture() (friend access to the
//   private default constructor + members).
// - Destructor frees the backend GPU resource automatically.
//
// [x] Private default constructor - only Renderer (friend) can construct.
// [x] Destructor: free backend GPU resource.
// [x] getWidth()/getHeight().

class Texture
{
public:
    ~Texture();

    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    int getWidth() const;
    int getHeight() const;

private:
    friend class Renderer;

    Texture() = default;

    void *m_texture = nullptr;
    int m_width = 0;
    int m_height = 0;
};