#pragma once
#include <string>
#include <vector>
#include "engine/renderer/Font.h"
#include "engine/renderer/Color.h"
#include "engine/math/Vec2.h"
#include "engine/math/Rect.h"

class Font;
class Texture;
struct SDL_Renderer;

// Renderer wraps SDL_Renderer and is the ONLY place draw calls + SDL color/blend
// types are translated to/from engine types (Color, FColor, Rectf, Recti, Vec2f).
//
// Consumers (game states, UI widgets) never see SDL types: they pass Color/FColor/
// Rectf/Vec2f and get back the same.
//
// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------
//
// [x] Default ctor: leaves m_renderer null; Window assigns the real one
//       after SDL_CreateRenderer succeeds.
// [x] Constructor: Renderer(SDL_Renderer* renderer) - non-owning, App/Window owns it.
//
// -----------------------------------------------------------------------------
// Frame
// -----------------------------------------------------------------------------
//
// [x] clear(Color) - set draw color then SDL_RenderClear.
// [x] present() - SDL_RenderPresent.
//
// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------
//
// [x] setLogicalPresentation(width, height, mode) - SDL_SetRenderLogicalPresentation.
//       PresentationMode{Letterbox} covers current usage; extend if a state needs
//       Stretch/Overscan/IntegerScale later.
//
// -----------------------------------------------------------------------------
// Render state
// -----------------------------------------------------------------------------
//
// [x] setDrawColor(Color) / getDrawColor() - SDL_SetRenderDrawColor /
//       SDL_GetRenderDrawColor.
//
// [x] setBlendMode(BlendMode) / getBlendMode() - SDL_SetRenderDrawBlendMode /
//       SDL_GetRenderDrawBlendMode. BlendMode{None, Blend} covers current usage;
//       extend if a state needs Add/Mod later.
//
// -----------------------------------------------------------------------------
// Geometry / primitives
// -----------------------------------------------------------------------------
//
// [x] fillRect(Rectf) - SDL_RenderFillRect.
// [x] drawRect(Rectf) - SDL_RenderRect (outline).
// [x] drawLine(Vec2f, Vec2f) - SDL_RenderLine.
//
// [x] drawGeometry(vertices, indices) - generic textured/colored triangle list via
//       SDL_RenderGeometry. Vertex{position: Vec2f, color: FColor}. Covers both the
//       4-vert gradient quad (background) and 3-vert cursor highlight triangles
//       currently hand-rolled in BattleState.
//
// -----------------------------------------------------------------------------
// Textures
// -----------------------------------------------------------------------------
//
// [x] loadTexture(filePath) - IMG_Load + SDL_CreateTextureFromSurface using
//       m_renderer directly. Returns nullptr on failure. Caller owns the
//       returned Texture* (delete when done).
//
// [x] getTextureSize(Texture*) - SDL_GetTextureSize, returns Vec2f.
//
// [x] setTextureAlphaMod(Texture*, float 0..1) - SDL_SetTextureAlphaModFloat.
//
// [x] setTextureScaleMode(Texture*, ScaleMode) - SDL_SetTextureScaleMode.
//
// [x] drawTexture(Texture*, src: Recti, dst: Rectf, flipH = false) -
//       SDL_RenderTexture / SDL_RenderTextureRotated(angle=0, flip=H)
//       when flipH is true.
//
// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------
//
// [x] drawDebugText(Vec2f pos, text) - SDL_RenderDebugText.
class Renderer
{
public:
    enum class BlendMode
    {
        None,
        Blend
    };

    enum class ScaleMode
    {
        Nearest,
        Linear
    };

    struct Vertex
    {
        Vec2f position;
        FColor color;
    };

    // --- Construction ---
    Renderer() = default;
    explicit Renderer(SDL_Renderer *renderer);

    // --- Frame ---
    void clear(Color color);
    void present();

    // --- Configuration ---
    enum class PresentationMode
    {
        Letterbox
    };

    void setLogicalPresentation(int width, int height, PresentationMode mode);

    // --- Render state ---
    void setDrawColor(Color color);
    Color getDrawColor() const;

    void setBlendMode(BlendMode mode);
    BlendMode getBlendMode() const;

    // --- Geometry / primitives ---
    void fillRect(Rectf rect);
    void drawRect(Rectf rect);
    void drawLine(Vec2f a, Vec2f b);
    void drawGeometry(const std::vector<Vertex> &vertices,
                      const std::vector<int> &indices);

    // --- Textures ---
    Font *loadFont(const char *filePath, float size);

    Texture *loadTexture(const char *filePath);

    Vec2f getTextureSize(const Texture *texture) const;

    void setTextureAlphaMod(const Texture *texture, float alpha);
    void setTextureScaleMode(const Texture *texture, ScaleMode mode);

    // Renders text using a font in a single immediate-mode call.
    // Internally: TTF_RenderText_Blended → SDL_CreateTextureFromSurface → SDL_RenderTexture.
    // No caching or batching; texture is recreated every call.
    //
    // Parameters:
    // - font: font resource to use
    // - text: UTF-8 string to render
    // - pos: screen position
    // - color: text color
    // - bold/italic/underline: style flags applied per call
    void renderText(const Font *font, const std::string &text, Vec2f pos, Color color,
                    bool bold = false, bool italic = false, bool underline = false);

    void drawTexture(const Texture *texture,
                     Recti src,
                     Rectf dst,
                     bool flipH = false);

    // --- Debug ---
    void drawDebugText(Vec2f pos, const std::string &text);

private:
    SDL_Renderer *m_renderer = nullptr;
};