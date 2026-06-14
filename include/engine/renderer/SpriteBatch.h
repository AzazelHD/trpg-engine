#pragma once
#include <SDL3/SDL.h>
#include <vector>

class Texture;

// SpriteBatch collects draw calls and sends them to SDL in one pass per frame.
// It is intentionally dumb — it knows nothing about Camera or world space.
//
// Caller contract:
//   The caller (game state) must apply camera.tileToScreen() BEFORE calling draw().
//   SpriteBatch receives screen-ready SDL_Rects and just stores + flushes them.
//
// Checklist — updated status:
//
// [x] Define DrawCommand struct:
//       Texture* texture   — pointer to the engine Texture wrapper
//       SDL_Rect src       — sprite-sheet crop in texture/pixel space
//       SDL_Rect dst       — screen-space destination (already projected by caller)
//       bool flipH         — horizontal flip flag
//
// [x] draw(Texture*, SDL_Rect src, SDL_Rect dst, bool flipH)
//       Push a DrawCommand onto m_commands.
//
// [x] flush(SDL_Renderer*)
//       For each command:
//         SDL_RenderTextureRotated / SDL_RenderCopyEx equivalent backend call
//       Then call clear()
//
// [x] clear()
//       m_commands.clear()
//
// Coordinate contract:
//   src  — texture/pixel space (SDL_Rect)
//   dst  — screen space (SDL_Rect) — caller ran camera.tileToScreen() before draw()

struct DrawCommand
{
    const Texture *texture;
    SDL_Rect src;
    SDL_Rect dst;
    bool flipH;
};

class SpriteBatch
{
public:
    void draw(const Texture *texture, SDL_Rect src, SDL_Rect dst, bool flipH = false);

    void flush(SDL_Renderer *renderer);

    void clear();

private:
    std::vector<DrawCommand> m_commands;
};