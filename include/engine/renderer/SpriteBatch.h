#pragma once
#include <vector>
#include "engine/math/Rect.h"

class Texture;
class Renderer;

// SpriteBatch queues draw calls and flushes them through engine::Renderer.
// It is intentionally dumb - it knows nothing about Camera or world space.
//
// Caller contract:
//   The caller (game state) must apply camera.tileToScreen() BEFORE calling draw().
//   SpriteBatch receives screen-ready Rectf and just stores + flushes them.
//
// [x] DrawCommand{texture, src: Recti (sheet crop, pixel space),
//       dst: Rectf (screen space, already projected), flipH}
// [x] draw(Texture*, Recti src, Rectf dst, bool flipH = false) - push command.
// [x] flush(Renderer&) - calls renderer.drawTexture() per command, then clear().
// [x] clear() - m_commands.clear()
struct DrawCommand
{
    const Texture *texture;
    Recti src;
    Rectf dst;
    bool flipH;
};

class SpriteBatch
{
public:
    void draw(const Texture *texture, Recti src, Rectf dst, bool flipH = false);
    void flush(Renderer &renderer);
    void clear();

private:
    std::vector<DrawCommand> m_commands;
};