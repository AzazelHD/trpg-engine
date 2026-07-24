#include "engine/renderer/Font.h"
#include "engine/renderer/Texture.h"
#include "engine/renderer/Renderer.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include <algorithm>
#include <memory>
#include <cmath>

namespace
{
    SDL_FRect toSDL(Rectf r) { return {r.x, r.y, r.w, r.h}; }
    SDL_FRect toSDL(Recti r) { return {(float)r.x, (float)r.y, (float)r.w, (float)r.h}; }

    int toTTFStyle(bool bold, bool italic, bool underline)
    {
        int style = TTF_STYLE_NORMAL;
        if (bold)
            style |= TTF_STYLE_BOLD;
        if (italic)
            style |= TTF_STYLE_ITALIC;
        if (underline)
            style |= TTF_STYLE_UNDERLINE;
        return style;
    }

    // Applies a temporary font style and restores the previous one on scope
    // exit (including early returns), so measureText/renderText never leave
    // the shared TTF_Font mutated after they return.
    class ScopedFontStyle
    {
    public:
        ScopedFontStyle(TTF_Font *font, int style)
            : m_font(font), m_previousStyle(TTF_GetFontStyle(font))
        {
            TTF_SetFontStyle(m_font, style);
        }

        ~ScopedFontStyle()
        {
            TTF_SetFontStyle(m_font, m_previousStyle);
        }

        ScopedFontStyle(const ScopedFontStyle &) = delete;
        ScopedFontStyle &operator=(const ScopedFontStyle &) = delete;

    private:
        TTF_Font *m_font;
        int m_previousStyle;
    };

    // Same RAII pattern as ScopedFontStyle, for TTF_SetFontSize instead of
    // TTF_SetFontStyle — restores the font's original point size on scope
    // exit so measureText() (which never bumps size) stays consistent.
    class ScopedFontSize
    {
    public:
        ScopedFontSize(TTF_Font *font, float newSize)
            : m_font(font), m_previousSize(TTF_GetFontSize(font))
        {
            TTF_SetFontSize(m_font, newSize);
        }

        ~ScopedFontSize()
        {
            TTF_SetFontSize(m_font, m_previousSize);
        }

        ScopedFontSize(const ScopedFontSize &) = delete;
        ScopedFontSize &operator=(const ScopedFontSize &) = delete;

    private:
        TTF_Font *m_font;
        float m_previousSize;
    };
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
    (void)mode; // manual compositing below replaces SDL's own logical-presentation scaling
    m_logicalW = width;
    m_logicalH = height;

    if (m_logicalTarget)
    {
        SDL_DestroyTexture(m_logicalTarget);
        m_logicalTarget = nullptr;
    }

    m_logicalTarget = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888,
                                        SDL_TEXTUREACCESS_TARGET, width, height);
}

void Renderer::setLogicalScaleMode(ScaleMode mode)
{
    m_logicalScaleMode = mode;
}

Renderer::LetterboxTransform Renderer::computeLetterboxTransform() const
{
    int outW = 0, outH = 0;
    SDL_GetRenderOutputSize(m_renderer, &outW, &outH);
    if (m_logicalW <= 0 || m_logicalH <= 0 || outW <= 0 || outH <= 0)
        return LetterboxTransform{1.0f, 0.0f, 0.0f};

    const float scaleX = static_cast<float>(outW) / static_cast<float>(m_logicalW);
    const float scaleY = static_cast<float>(outH) / static_cast<float>(m_logicalH);
    const float scale = std::min(scaleX, scaleY);

    const float scaledW = static_cast<float>(m_logicalW) * scale;
    const float scaledH = static_cast<float>(m_logicalH) * scale;

    return LetterboxTransform{
        scale,
        (static_cast<float>(outW) - scaledW) * 0.5f,
        (static_cast<float>(outH) - scaledH) * 0.5f,
    };
}

Rectf Renderer::toNativeRect(Rectf r) const
{
    if (m_inWorldPass)
        return r;
    const LetterboxTransform t = computeLetterboxTransform();
    return Rectf{r.x * t.scale + t.offsetX, r.y * t.scale + t.offsetY, r.w * t.scale, r.h * t.scale};
}

Vec2f Renderer::toNativePos(Vec2f p) const
{
    if (m_inWorldPass)
        return p;
    const LetterboxTransform t = computeLetterboxTransform();
    return Vec2f{p.x * t.scale + t.offsetX, p.y * t.scale + t.offsetY};
}

void Renderer::beginWorldPass()
{
    if (!m_logicalTarget)
        return;
    SDL_SetRenderTarget(m_renderer, m_logicalTarget);
    m_inWorldPass = true;
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
    SDL_RenderClear(m_renderer);
}

void Renderer::endWorldPass()
{
    if (!m_logicalTarget)
        return;
    SDL_SetRenderTarget(m_renderer, nullptr);
    m_inWorldPass = false;

    SDL_SetTextureScaleMode(m_logicalTarget,
                            m_logicalScaleMode == ScaleMode::Linear ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);

    const LetterboxTransform t = computeLetterboxTransform();
    SDL_FRect dst{t.offsetX, t.offsetY, static_cast<float>(m_logicalW) * t.scale, static_cast<float>(m_logicalH) * t.scale};
    SDL_RenderTexture(m_renderer, m_logicalTarget, nullptr, &dst);
}

Renderer::~Renderer()
{
    if (m_logicalTarget)
        SDL_DestroyTexture(m_logicalTarget);
}

Renderer::Renderer(Renderer &&other) noexcept
    : m_renderer(other.m_renderer),
      m_logicalTarget(other.m_logicalTarget),
      m_logicalW(other.m_logicalW),
      m_logicalH(other.m_logicalH),
      m_logicalScaleMode(other.m_logicalScaleMode),
      m_inWorldPass(other.m_inWorldPass)
{
    other.m_logicalTarget = nullptr;
}

Renderer &Renderer::operator=(Renderer &&other) noexcept
{
    if (this != &other)
    {
        if (m_logicalTarget)
            SDL_DestroyTexture(m_logicalTarget);

        m_renderer = other.m_renderer;
        m_logicalTarget = other.m_logicalTarget;
        m_logicalW = other.m_logicalW;
        m_logicalH = other.m_logicalH;
        m_logicalScaleMode = other.m_logicalScaleMode;
        m_inWorldPass = other.m_inWorldPass;

        other.m_logicalTarget = nullptr;
    }
    return *this;
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

void Renderer::fillRect(Rectf rect)
{
    SDL_FRect r = toSDL(toNativeRect(rect));
    SDL_RenderFillRect(m_renderer, &r);
}

void Renderer::drawRect(Rectf rect)
{
    SDL_FRect r = toSDL(toNativeRect(rect));
    SDL_RenderRect(m_renderer, &r);
}

void Renderer::drawLine(Vec2f a, Vec2f b)
{
    const Vec2f na = toNativePos(a);
    const Vec2f nb = toNativePos(b);
    SDL_RenderLine(m_renderer, na.x, na.y, nb.x, nb.y);
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

// [x] loadFont(): TTF_OpenFont wrapper. Returns nullptr on failure.
Font *Renderer::loadFont(const char *filePath, float size)
{
    TTF_Font *ttfFont = TTF_OpenFont(filePath, size);
    if (!ttfFont)
        return nullptr;

    auto *font = new Font();
    font->m_font = ttfFont;
    font->m_size = size;

    return font;
}

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

// [x] renderText(): TTF_RenderText_Blended -> surface -> texture -> render -> destroy.
void Renderer::renderText(const Font *font, const std::string &text, Vec2f pos, Color color,
                          bool bold, bool italic, bool underline)
{
    if (!font || !font->m_font || text.empty())
        return;

    TTF_Font *ttfFont = static_cast<TTF_Font *>(font->m_font);
    ScopedFontStyle styleGuard(ttfFont, toTTFStyle(bold, italic, underline));

    // Rasterize at native pixel resolution for sharpness when scaled up
    // (e.g. after a resolution change) — the resulting texture is then
    // drawn 1:1 in physical pixels, so only its POSITION needs the
    // letterbox transform below, never its size.
    const float previousSize = TTF_GetFontSize(ttfFont);
    const LetterboxTransform t = m_inWorldPass ? LetterboxTransform{1.0f, 0.0f, 0.0f} : computeLetterboxTransform();
    std::unique_ptr<ScopedFontSize> sizeGuard;
    if (!m_inWorldPass && t.scale > 0.0f && t.scale != 1.0f)
        sizeGuard = std::make_unique<ScopedFontSize>(ttfFont, previousSize * t.scale);

    SDL_Color sdlColor{color.r, color.g, color.b, color.a};
    SDL_Surface *surface = TTF_RenderText_Blended(ttfFont, text.c_str(), 0, sdlColor);
    if (!surface)
        return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture)
        return;

    float w = 0.f, h = 0.f;
    SDL_GetTextureSize(texture, &w, &h);

    const Vec2f nativePos = m_inWorldPass ? pos : toNativePos(pos);
    SDL_FRect dst{nativePos.x, nativePos.y, w, h};
    SDL_RenderTexture(m_renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
}

Vec2f Renderer::measureText(const Font *font, const std::string &text,
                            bool bold, bool italic, bool underline) const
{
    if (!font || !font->m_font || text.empty())
        return Vec2f{0.0f, 0.0f};

    TTF_Font *ttfFont = static_cast<TTF_Font *>(font->m_font);
    ScopedFontStyle styleGuard(ttfFont, toTTFStyle(bold, italic, underline));

    int w = 0, h = 0;
    if (!TTF_GetStringSize(ttfFont, text.c_str(), 0, &w, &h))
        return Vec2f{0.0f, 0.0f};

    return Vec2f{static_cast<float>(w), static_cast<float>(h)};
}

bool Renderer::setFontWrapAlignment(const Font *font, HorizontalAlign align) const
{
    if (!font || !font->m_font)
        return false;

    auto *ttfFont = static_cast<TTF_Font *>(font->m_font);
    TTF_HorizontalAlignment sdlAlign = TTF_HORIZONTAL_ALIGN_LEFT;
    switch (align)
    {
    case HorizontalAlign::Left:
        sdlAlign = TTF_HORIZONTAL_ALIGN_LEFT;
        break;
    case HorizontalAlign::Center:
        sdlAlign = TTF_HORIZONTAL_ALIGN_CENTER;
        break;
    case HorizontalAlign::Right:
        sdlAlign = TTF_HORIZONTAL_ALIGN_RIGHT;
        break;
    }

    TTF_SetFontWrapAlignment(ttfFont, sdlAlign);
    return true;
}

Renderer::HorizontalAlign Renderer::getFontWrapAlignment(const Font *font) const
{
    if (!font || !font->m_font)
        return HorizontalAlign::Left;

    auto *ttfFont = static_cast<TTF_Font *>(font->m_font);
    const TTF_HorizontalAlignment sdlAlign = TTF_GetFontWrapAlignment(ttfFont);
    switch (sdlAlign)
    {
    case TTF_HORIZONTAL_ALIGN_CENTER:
        return HorizontalAlign::Center;
    case TTF_HORIZONTAL_ALIGN_RIGHT:
        return HorizontalAlign::Right;
    case TTF_HORIZONTAL_ALIGN_LEFT:
    default:
        return HorizontalAlign::Left;
    }
}

Vec2f Renderer::alignInRect(Rectf rect, Vec2f contentSize,
                            HorizontalAlign hAlign,
                            VerticalAlign vAlign) const
{
    Vec2f out{rect.x, rect.y};

    if (hAlign == HorizontalAlign::Center)
        out.x = rect.x + (rect.w - contentSize.x) * 0.5f;
    else if (hAlign == HorizontalAlign::Right)
        out.x = rect.x + rect.w - contentSize.x;

    if (vAlign == VerticalAlign::Middle)
        out.y = rect.y + (rect.h - contentSize.y) * 0.5f;
    else if (vAlign == VerticalAlign::Bottom)
        out.y = rect.y + rect.h - contentSize.y;

    // Snap to integer pixel coordinates to avoid half-pixel drift and 1px bias.
    out.x = std::round(out.x);
    out.y = std::round(out.y);

    return out;
}

void Renderer::renderTextInRect(const Font *font,
                                const std::string &text,
                                Rectf rect,
                                Color color,
                                HorizontalAlign hAlign,
                                VerticalAlign vAlign,
                                bool bold,
                                bool italic,
                                bool underline)
{
    const Vec2f textSize = measureText(font, text, bold, italic, underline);
    const Vec2f pos = alignInRect(rect, textSize, hAlign, vAlign);
    renderText(font, text, pos, color, bold, italic, underline);
}

// [x] drawTexture(): SDL_RenderTexture, or SDL_RenderTextureRotated(angle=0, flip=H)
//       when flipH is true.
void Renderer::drawTexture(const Texture *texture, Recti src, Rectf dst, bool flipH)
{
    auto *sdlTex = static_cast<SDL_Texture *>(texture->m_texture);

    SDL_FRect srcRect = toSDL(src);
    const Rectf nativeDst = m_inWorldPass ? dst : toNativeRect(dst);
    SDL_FRect dstRect = toSDL(nativeDst);

    if (flipH)
    {
        SDL_RenderTextureRotated(m_renderer, sdlTex, &srcRect, &dstRect, 0.0, nullptr, SDL_FLIP_HORIZONTAL);
    }
    else
    {
        SDL_RenderTexture(m_renderer, sdlTex, &srcRect, &dstRect);
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