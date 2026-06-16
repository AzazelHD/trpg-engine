#include "engine/renderer/Texture.h"
#include "engine/renderer/Renderer.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

namespace
{
    SDL_FRect toSDL(Rectf r) { return {r.x, r.y, r.w, r.h}; }
    SDL_FRect toSDL(Recti r) { return {(float)r.x, (float)r.y, (float)r.w, (float)r.h}; }
}

// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------

// [x] Constructor: store the non-owning SDL_Renderer* (owned by Window).
Renderer::Renderer(SDL_Renderer *renderer) : m_renderer(renderer) {}

// -----------------------------------------------------------------------------
// Frame
// -----------------------------------------------------------------------------

// [x] clear(): set draw color then SDL_RenderClear.
void Renderer::clear(Color color)
{
    setDrawColor(color);
    SDL_RenderClear(m_renderer);
}

// [x] present(): SDL_RenderPresent.
void Renderer::present()
{
    SDL_RenderPresent(m_renderer);
}

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------

// [x] setLogicalPresentation(): maps PresentationMode -> SDL_RendererLogicalPresentation.
void Renderer::setLogicalPresentation(int width, int height, PresentationMode mode)
{
    SDL_RendererLogicalPresentation sdlMode = SDL_LOGICAL_PRESENTATION_LETTERBOX;

    switch (mode)
    {
    case PresentationMode::Letterbox:
        sdlMode = SDL_LOGICAL_PRESENTATION_LETTERBOX;
        break;
    }

    SDL_SetRenderLogicalPresentation(m_renderer, width, height, sdlMode);
}

// -----------------------------------------------------------------------------
// Render state
// -----------------------------------------------------------------------------

// [x] setDrawColor(): SDL_SetRenderDrawColor.
void Renderer::setDrawColor(Color color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
}

// [x] getDrawColor(): SDL_GetRenderDrawColor.
Color Renderer::getDrawColor() const
{
    Color c;
    SDL_GetRenderDrawColor(m_renderer, &c.r, &c.g, &c.b, &c.a);
    return c;
}

// [x] setBlendMode(): maps BlendMode -> SDL_BlendMode.
void Renderer::setBlendMode(BlendMode mode)
{
    SDL_SetRenderDrawBlendMode(
        m_renderer,
        mode == BlendMode::Blend ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
}

// [x] getBlendMode(): maps SDL_BlendMode -> BlendMode.
Renderer::BlendMode Renderer::getBlendMode() const
{
    SDL_BlendMode mode;
    SDL_GetRenderDrawBlendMode(m_renderer, &mode);
    return mode == SDL_BLENDMODE_BLEND ? BlendMode::Blend : BlendMode::None;
}

// -----------------------------------------------------------------------------
// Geometry / primitives
// -----------------------------------------------------------------------------

// [x] fillRect(): SDL_RenderFillRect.
void Renderer::fillRect(Rectf rect)
{
    SDL_FRect r = toSDL(rect);
    SDL_RenderFillRect(m_renderer, &r);
}

// [x] drawRect(): SDL_RenderRect (outline).
void Renderer::drawRect(Rectf rect)
{
    SDL_FRect r = toSDL(rect);
    SDL_RenderRect(m_renderer, &r);
}

// [x] drawLine(): SDL_RenderLine.
void Renderer::drawLine(Vec2f a, Vec2f b)
{
    SDL_RenderLine(m_renderer, a.x, a.y, b.x, b.y);
}

// [x] drawGeometry(): convert engine Vertex (Vec2f + FColor) to SDL_Vertex,
//       then SDL_RenderGeometry. tex_coord is unused (no textured geometry yet).
void Renderer::drawGeometry(const std::vector<Vertex> &vertices,
                            const std::vector<int> &indices)
{
    std::vector<SDL_Vertex> sdlVerts(vertices.size());

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        sdlVerts[i].position = {
            vertices[i].position.x,
            vertices[i].position.y};

        sdlVerts[i].color = {
            vertices[i].color.r,
            vertices[i].color.g,
            vertices[i].color.b,
            vertices[i].color.a};

        sdlVerts[i].tex_coord = {0.0f, 0.0f};
    }

    SDL_RenderGeometry(
        m_renderer,
        nullptr,
        sdlVerts.data(),
        (int)sdlVerts.size(),
        indices.data(),
        (int)indices.size());
}

// -----------------------------------------------------------------------------
// Textures
// -----------------------------------------------------------------------------

// [x] loadTexture(): IMG_Load + SDL_CreateTextureFromSurface using m_renderer.
//       Returns nullptr on failure. Caller owns the result.
Texture *Renderer::loadTexture(const char *filePath)
{
    SDL_Surface *surface = IMG_Load(filePath);
    if (!surface)
        return nullptr;

    SDL_Texture *sdlTexture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_DestroySurface(surface);

    if (!sdlTexture)
        return nullptr;

    auto *texture = new Texture();
    texture->m_texture = sdlTexture;

    float w = 0.0f;
    float h = 0.0f;
    SDL_GetTextureSize(sdlTexture, &w, &h);

    texture->m_width = static_cast<int>(w);
    texture->m_height = static_cast<int>(h);

    return texture;
}

// [x] getTextureSize(): SDL_GetTextureSize -> Vec2f.
Vec2f Renderer::getTextureSize(const Texture *texture) const
{
    float w = 0.0f;
    float h = 0.0f;

    SDL_GetTextureSize(
        static_cast<SDL_Texture *>(texture->m_texture),
        &w,
        &h);

    return {w, h};
}

// [x] setTextureAlphaMod(): SDL_SetTextureAlphaModFloat.
void Renderer::setTextureAlphaMod(const Texture *texture, float alpha)
{
    SDL_SetTextureAlphaModFloat(
        static_cast<SDL_Texture *>(texture->m_texture),
        alpha);
}

// [x] setTextureScaleMode(): maps ScaleMode -> SDL_ScaleMode.
void Renderer::setTextureScaleMode(const Texture *texture, ScaleMode mode)
{
    SDL_SetTextureScaleMode(
        static_cast<SDL_Texture *>(texture->m_texture),
        mode == ScaleMode::Linear
            ? SDL_SCALEMODE_LINEAR
            : SDL_SCALEMODE_NEAREST);
}

// [x] drawTexture(): SDL_RenderTexture, or SDL_RenderTextureRotated(angle=0, flip=H)
//       when flipH is true.
void Renderer::drawTexture(const Texture *texture, Recti src, Rectf dst, bool flipH)
{
    auto *sdlTex = static_cast<SDL_Texture *>(texture->m_texture);

    SDL_FRect srcRect = toSDL(src);
    SDL_FRect dstRect = toSDL(dst);

    if (flipH)
    {
        SDL_RenderTextureRotated(
            m_renderer,
            sdlTex,
            &srcRect,
            &dstRect,
            0.0,
            nullptr,
            SDL_FLIP_HORIZONTAL);
    }
    else
    {
        SDL_RenderTexture(
            m_renderer,
            sdlTex,
            &srcRect,
            &dstRect);
    }
}

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------

// [x] drawDebugText(): SDL_RenderDebugText.
void Renderer::drawDebugText(Vec2f pos, const std::string &text)
{
    SDL_RenderDebugText(
        m_renderer,
        pos.x,
        pos.y,
        text.c_str());
}