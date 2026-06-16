#include "engine/renderer/SpriteBatch.h"
#include "engine/renderer/Renderer.h"

// [x] draw(): push a DrawCommand onto m_commands.
//     Caller is responsible for passing screen-ready Rectf (already projected
//     by camera.tileToScreen()).
void SpriteBatch::draw(const Texture *texture, Recti src, Rectf dst, bool flipH)
{
    m_commands.push_back(DrawCommand{texture, src, dst, flipH});
}

// [x] flush(): send all queued draw commands through Renderer, then clear.
//     No coordinate conversion happens here - rects are already screen-space.
void SpriteBatch::flush(Renderer &renderer)
{
    for (const DrawCommand &command : m_commands)
    {
        if (!command.texture)
            continue;

        renderer.drawTexture(command.texture, command.src, command.dst, command.flipH);
    }

    clear();
}

// [x] clear(): reset draw queue.
void SpriteBatch::clear()
{
    m_commands.clear();
}