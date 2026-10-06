#pragma once
#include <vector>
#include "engine/math/Rect.h"
#include "engine/renderer/Color.h"
#include "engine/renderer/Renderer.h"

class Texture;

// SpriteBatch queues draw calls and flushes them through engine::Renderer.
// It is intentionally dumb - it knows nothing about Camera or world space.
//
// Caller contract:
//   The caller (game state) must apply camera.tileToScreen() BEFORE calling draw().
//   SpriteBatch receives screen-ready Rectf and just stores + flushes them.
//
// [x] DrawCommand{texture, src: Recti (sheet crop, pixel space),
//       dst: Rectf (screen space, already projected), flipH, tint, blend}
// [x] draw(Texture*, Recti src, Rectf dst, bool flipH = false,
//       Color tint = white, Renderer::BlendMode blend = Blend) - push command.
// [x] flush(Renderer&) - calls renderer.drawTexture() per command, then clear().
// [x] clear() - m_commands.clear()
struct DrawCommand
{
    const Texture *texture;
    Recti src;
    Rectf dst;
    bool flipH;
    Color tint;
    Renderer::BlendMode blend;
};

class SpriteBatch
{
public:
    void draw(const Texture *texture, Recti src, Rectf dst, bool flipH = false,
              Color tint = Color::white(),
              Renderer::BlendMode blend = Renderer::BlendMode::Blend);
    void flush(Renderer &renderer);
    void clear();

private:
    std::vector<DrawCommand> m_commands;
};