#include "engine/renderer/Aligment.h"
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

    SDL_BlendMode toSDL(Renderer::BlendMode mode)
    {
        switch (mode)
        {
        case Renderer::BlendMode::None:
            return SDL_BLENDMODE_NONE;
        case Renderer::BlendMode::Add:
            return SDL_BLENDMODE_ADD;
        case Renderer::BlendMode::Mod:
            return SDL_BLENDMODE_MOD;
        case Renderer::BlendMode::Blend:
        default:
            return SDL_BLENDMODE_BLEND;
        }
    }

    Renderer::BlendMode fromSDL(SDL_BlendMode mode)
    {
        switch (mode)
        {
        case SDL_BLENDMODE_NONE:
            return Renderer::BlendMode::None;
        case SDL_BLENDMODE_ADD:
            return Renderer::BlendMode::Add;
        case SDL_BLENDMODE_MOD:
            return Renderer::BlendMode::Mod;
        case SDL_BLENDMODE_BLEND:
        default:
            return Renderer::BlendMode::Blend;
        }
    }

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
    m_presentationMode = mode;
    m_logicalW = width;
    m_logicalH = height;

    if (m_logicalTarget)
    {
        SDL_DestroyTexture(m_logicalTarget);
        m_logicalTarget = nullptr;
    }

    m_logicalTarget = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888,
                                        SDL_TEXTUREACCESS_TARGET, width, height);
    m_targetW = width;
    m_targetH = height;
}

Vec2i Renderer::getOutputSize() const
{
    int outW = 0, outH = 0;
    if (m_renderer)
        SDL_GetRenderOutputSize(m_renderer, &outW, &outH);
    return Vec2i{outW, outH};
}

void Renderer::setWorldBufferSize(int width, int height)
{
    if (!m_renderer || width <= 0 || height <= 0)
        return;
    if (m_logicalTarget && width == m_targetW && height == m_targetH)
        return;

    m_targetW = width;
    m_targetH = height;

    if (m_logicalTarget)
        SDL_DestroyTexture(m_logicalTarget);
    m_logicalTarget = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888,
                                        SDL_TEXTUREACCESS_TARGET, width, height);
}

void Renderer::beginNativeOverlay()
{
    m_inNativeOverlay = true;
}

void Renderer::endNativeOverlay()
{
    m_inNativeOverlay = false;
}

void Renderer::setPresentationMode(PresentationMode mode)
{
    m_presentationMode = mode;
}

void Renderer::setLogicalScaleMode(ScaleMode mode)
{
    m_logicalScaleMode = mode;
}

void Renderer::setLetterboxColor(Color color)
{
    m_letterboxColor = color;
}

Renderer::LetterboxTransform Renderer::computeLetterboxTransform() const
{
    int outW = 0, outH = 0;
    SDL_GetRenderOutputSize(m_renderer, &outW, &outH);
    if (m_logicalW <= 0 || m_logicalH <= 0 || outW <= 0 || outH <= 0)
        return LetterboxTransform{1.0f, 1.0f, 0.0f, 0.0f};

    const float scaleX = static_cast<float>(outW) / static_cast<float>(m_logicalW);
    const float scaleY = static_cast<float>(outH) / static_cast<float>(m_logicalH);

    const float uniformScale = std::min(scaleX, scaleY);
    const float scaledW = static_cast<float>(m_logicalW) * uniformScale;
    const float scaledH = static_cast<float>(m_logicalH) * uniformScale;

    return LetterboxTransform{
        uniformScale,
        uniformScale,
        (static_cast<float>(outW) - scaledW) * 0.5f,
        (static_cast<float>(outH) - scaledH) * 0.5f,
    };
}

Rectf Renderer::targetBlitRect() const
{
    int outW = 0, outH = 0;
    SDL_GetRenderOutputSize(m_renderer, &outW, &outH);
    if (m_targetW <= 0 || m_targetH <= 0 || outW <= 0 || outH <= 0)
        return Rectf{0.0f, 0.0f, static_cast<float>(outW), static_cast<float>(outH)};

    const float scaleX = static_cast<float>(outW) / static_cast<float>(m_targetW);
    const float scaleY = static_cast<float>(outH) / static_cast<float>(m_targetH);
    const float uniformScale = std::min(scaleX, scaleY);
    const float scaledW = static_cast<float>(m_targetW) * uniformScale;
    const float scaledH = static_cast<float>(m_targetH) * uniformScale;

    return Rectf{(static_cast<float>(outW) - scaledW) * 0.5f,
                 (static_cast<float>(outH) - scaledH) * 0.5f,
                 scaledW,
                 scaledH};
}

Rectf Renderer::toNativeRect(Rectf r) const
{
    if (inNativeSpace())
        return r;
    const LetterboxTransform t = computeLetterboxTransform();
    return Rectf{r.x * t.scaleX + t.offsetX, r.y * t.scaleY + t.offsetY, r.w * t.scaleX, r.h * t.scaleY};
}

Vec2f Renderer::toNativePos(Vec2f p) const
{
    if (inNativeSpace())
        return p;
    const LetterboxTransform t = computeLetterboxTransform();
    return Vec2f{p.x * t.scaleX + t.offsetX, p.y * t.scaleY + t.offsetY};
}

void Renderer::beginLogicalPass()
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
    // beginLogicalPass() clears the buffer to (0,0,0,0), so blend it over
    // whatever backdrop the caller draws before blitWorldBuffer().
    SDL_SetTextureBlendMode(m_logicalTarget, SDL_BLENDMODE_BLEND);

    // Paint the full native output first, so on non-16:9 displays (e.g.
    // ultrawide) the area outside the letterboxed game rect ends up in
    // m_letterboxColor rather than whatever a prior clear() call left behind.
    // This makes the bars independent of call order elsewhere in the frame.
    //
    // This is also the backdrop seam: between this clear and blitWorldBuffer()
    // a caller may draw a full-window backdrop natively (sky gradient,
    // parallax art) for the world to composite over. To extend later:
    //   - Gradient: a drawGeometry() strip of Vertices whose FColor varies
    //     top-to-bottom (or left-right for side bars), instead of a flat fill.
    //   - Blurred game content: capture/downsample m_logicalTarget's edge
    //     columns/rows into a small texture once per frame (or every N frames
    //     for perf), then drawTexture() it stretched over the full native rect
    //     before the sharp blit — cheap "extended ambient" look without a
    //     real blur shader.
    //   - Static art / parallax: drawTexture() background textures stretched to
    //     the full native output rect before the sharp blit below.
    SDL_SetRenderDrawColor(m_renderer, m_letterboxColor.r, m_letterboxColor.g, m_letterboxColor.b, m_letterboxColor.a);
    SDL_RenderClear(m_renderer);
}

void Renderer::blitWorldBuffer()
{
    if (!m_logicalTarget)
        return;
    const Rectf dst = targetBlitRect();
    SDL_FRect sdlDst{dst.x, dst.y, dst.w, dst.h};
    SDL_RenderTexture(m_renderer, m_logicalTarget, nullptr, &sdlDst);
}

void Renderer::endLogicalPass()
{
    endWorldPass();
    blitWorldBuffer();
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
      m_targetW(other.m_targetW),
      m_targetH(other.m_targetH),
      m_logicalScaleMode(other.m_logicalScaleMode),
      m_inWorldPass(other.m_inWorldPass),
      m_inNativeOverlay(other.m_inNativeOverlay)
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
        m_targetW = other.m_targetW;
        m_targetH = other.m_targetH;
        m_logicalScaleMode = other.m_logicalScaleMode;
        m_inWorldPass = other.m_inWorldPass;
        m_inNativeOverlay = other.m_inNativeOverlay;

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
    SDL_SetRenderDrawBlendMode(m_renderer, toSDL(mode));
}

// [x] getBlendMode(): maps SDL_BlendMode -> BlendMode.
Renderer::BlendMode Renderer::getBlendMode() const
{
    SDL_BlendMode mode;
    SDL_GetRenderDrawBlendMode(m_renderer, &mode);
    return fromSDL(mode);
}

// -----------------------------------------------------------------------------
// Geometry / primitives
// -----------------------------------------------------------------------------

void Renderer::fillRect(Rectf rect)
{
    SDL_FRect r = toSDL(toNativeRect(rect));
    SDL_RenderFillRect(m_renderer, &r);
}

// [x] fillCircle(): filled disc (current draw color) sent through
//       drawGeometry as a 32-segment triangle fan — the rim is smooth enough
//       for both 2 px dial dots and large placeholder sprites, with no
//       scanline stair-stepping.
void Renderer::fillCircle(Vec2f center, float radius)
{
    if (radius <= 0.0f)
        return;

    constexpr int kSegments = 32;
    constexpr float kTwoPi = 6.28318530718f;

    const FColor fill{getDrawColor()};
    std::vector<Vertex> vertices;
    vertices.reserve(kSegments + 1);
    vertices.push_back({center, fill});
    for (int i = 0; i < kSegments; ++i)
    {
        const float a = kTwoPi * static_cast<float>(i + 1) / static_cast<float>(kSegments);
        vertices.push_back({center + Vec2f{std::sin(a), std::cos(a)} * radius, fill});
    }

    std::vector<int> indices;
    indices.reserve(kSegments * 3);
    for (int i = 0; i < kSegments; ++i)
    {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i + 1 == kSegments ? 1 : i + 2);
    }

    drawGeometry(vertices, indices);
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
        const Vec2f nativePos = inNativeSpace() ? vertices[i].position : toNativePos(vertices[i].position);
        sdlVerts[i].position = {nativePos.x, nativePos.y};

        sdlVerts[i].color = {
            vertices[i].color.r,
            vertices[i].color.g,
            vertices[i].color.b,
            vertices[i].color.a};

        sdlVerts[i].tex_coord = {0.0f, 0.0f};
    }

    SDL_RenderGeometry(m_renderer, nullptr, sdlVerts.data(), (int)sdlVerts.size(), indices.data(), (int)indices.size());
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

    // Enable alpha blending up front (SDL textures default to
    // SDL_BLENDMODE_NONE), so per-pixel alpha and setTextureAlphaMod() work.
    SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);

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

// [x] calculateTextScale(): Given a Font and a scale factor, compute the effective font size to use for rendering text, taking into account the current letterbox transform and resolution scaling. Returns true if the calculation was successful, false otherwise. The result is stored in the provided TextScale struct.
bool Renderer::calculateTextScale(
    const Font *font, float scale, TextScale &result) const
{
    if (!font || !font->m_font || !std::isfinite(scale) || scale <= 0.0f)
        return false;

    TTF_Font *ttfFont = static_cast<TTF_Font *>(font->m_font);
    const float originalFontSize = TTF_GetFontSize(ttfFont);

    const LetterboxTransform t = inNativeSpace()
                                     ? LetterboxTransform{1.0f, 1.0f, 0.0f, 0.0f}
                                     : computeLetterboxTransform();

    const float resolutionScale = (!m_inWorldPass && !m_inNativeOverlay)
                                      ? std::min(t.scaleX, t.scaleY)
                                      : 1.0f;

    const float effectiveScale = resolutionScale * scale;

    if (!std::isfinite(effectiveScale) || effectiveScale <= 0.0f)
        return false;

    const float scaledFontSize = originalFontSize * effectiveScale;

    if (!std::isfinite(scaledFontSize) || scaledFontSize <= 0.0f)
        return false;

    result = TextScale{
        originalFontSize,
        scaledFontSize,
        resolutionScale};
    return true;
}

// [x] renderText(): Renders styled text with resolution-aware scaling and opacity.
void Renderer::renderText(const Font *font, const std::string &text, Vec2f pos, Color color,
                          bool bold, bool italic, bool underline, float scale)
{
    if (!font || !font->m_font || text.empty())
        return;

    // Nothing would be visible — skip rasterizing a texture drawn at alpha 0.
    if (color.a == 0)
        return;

    TTF_Font *ttfFont = static_cast<TTF_Font *>(font->m_font);
    ScopedFontStyle styleGuard(ttfFont, toTTFStyle(bold, italic, underline));

    // Rasterize at native pixel resolution for sharpness when scaled up.
    // Only the position needs the letterbox transform below, never its size.
    TextScale textScale;

    if (!calculateTextScale(font, scale, textScale))
        return;

    std::unique_ptr<ScopedFontSize> sizeGuard;

    if (textScale.scaledFontSize != textScale.originalFontSize)
    {
        sizeGuard = std::make_unique<ScopedFontSize>(
            ttfFont, textScale.scaledFontSize);
    }

    SDL_Color sdlColor{color.r, color.g, color.b, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(ttfFont, text.c_str(), 0, sdlColor);
    if (!surface)
        return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture)
        return;

    // Preserve per-pixel alpha from the rendered glyphs.
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // Apply the requested opacity through the texture alpha modulation.
    SDL_SetTextureAlphaMod(texture, color.a);

    float w = 0.0f, h = 0.0f;
    SDL_GetTextureSize(texture, &w, &h);

    const Vec2f nativePos = inNativeSpace() ? pos : toNativePos(pos);
    SDL_FRect dst{nativePos.x, nativePos.y, w, h};
    SDL_RenderTexture(m_renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
}

// [x] measureText(): Measures styled text using the same resolution-aware scaling as renderText().
Vec2f Renderer::measureText(const Font *font, const std::string &text,
                            bool bold, bool italic, bool underline, float scale) const
{
    if (!font || !font->m_font || text.empty())
        return Vec2f{0.0f, 0.0f};

    TTF_Font *ttfFont = static_cast<TTF_Font *>(font->m_font);
    ScopedFontStyle styleGuard(ttfFont, toTTFStyle(bold, italic, underline));

    TextScale textScale;

    if (!calculateTextScale(font, scale, textScale))
        return Vec2f{0.0f, 0.0f};

    std::unique_ptr<ScopedFontSize> sizeGuard;

    if (textScale.scaledFontSize != textScale.originalFontSize)
    {
        sizeGuard = std::make_unique<ScopedFontSize>(
            ttfFont, textScale.scaledFontSize);
    }

    int w = 0;
    int h = 0;

    if (!TTF_GetStringSize(ttfFont, text.c_str(), 0, &w, &h))
        return Vec2f{0.0f, 0.0f};

    return Vec2f{
        static_cast<float>(w) / textScale.resolutionScale,
        static_cast<float>(h) / textScale.resolutionScale};
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

HorizontalAlign Renderer::getFontWrapAlignment(const Font *font) const
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
                                bool underline,
                                float scale)
{
    const Vec2f textSize = measureText(font, text, bold, italic, underline, scale);
    const Vec2f pos = alignInRect(rect, textSize, hAlign, vAlign);
    renderText(font, text, pos, color, bold, italic, underline, scale);
}

// [x] drawTexture(): tint (texture color mod) + per-draw blend override, then
//       SDL_RenderTexture, or SDL_RenderTextureRotated(angle=0, flip=H) when
//       flipH is true. SDL_RenderTexture() uses the TEXTURE's blend mode, not
//       the renderer's draw blend mode, so the override is applied to the
//       texture and restored here.
void Renderer::drawTexture(const Texture *texture, Recti src, Rectf dst, bool flipH,
                           Color tint, BlendMode blend)
{
    auto *sdlTex = static_cast<SDL_Texture *>(texture->m_texture);

    const bool tinted = tint.r != 255 || tint.g != 255 || tint.b != 255;
    if (tinted)
        SDL_SetTextureColorMod(sdlTex, tint.r, tint.g, tint.b);

    SDL_BlendMode previousBlend = SDL_BLENDMODE_BLEND;
    SDL_GetTextureBlendMode(sdlTex, &previousBlend);
    const SDL_BlendMode desiredBlend = toSDL(blend);
    if (desiredBlend != previousBlend)
        SDL_SetTextureBlendMode(sdlTex, desiredBlend);

    SDL_FRect srcRect = toSDL(src);
    const Rectf nativeDst = inNativeSpace() ? dst : toNativeRect(dst);
    SDL_FRect dstRect = toSDL(nativeDst);

    if (flipH)
    {
        SDL_RenderTextureRotated(m_renderer, sdlTex, &srcRect, &dstRect, 0.0, nullptr, SDL_FLIP_HORIZONTAL);
    }
    else
    {
        SDL_RenderTexture(m_renderer, sdlTex, &srcRect, &dstRect);
    }

    if (desiredBlend != previousBlend)
        SDL_SetTextureBlendMode(sdlTex, previousBlend);
    if (tinted)
        SDL_SetTextureColorMod(sdlTex, 255, 255, 255);
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