#include <SDL3/SDL.h>
#include "engine/renderer/Texture.h"
#include "engine/renderer/SpriteBatch.h"

// [x] draw(): push a DrawCommand onto m_commands.
//     Caller is responsible for passing screen-ready SDL_Rects.
//     Typical caller pattern:
//         SDL_Rect dst = camera.tileToScreen(tilePos);
//         spriteBatch.draw(texture, src, dst, flipH);

void SpriteBatch::draw(const Texture *texture, SDL_Rect src, SDL_Rect dst, bool flipH)
{
    m_commands.push_back(DrawCommand{texture, src, dst, flipH});
}

// [x] flush(): send all queued draw commands to SDL, then clear.
//     Called once per frame after all render() calls have queued their draws.
//     No coordinate conversion happens here — rects are already SDL-ready.

void SpriteBatch::flush(SDL_Renderer *renderer)
{
    for (const DrawCommand &command : m_commands)
    {
        if (!command.texture)
        {
            continue;
        }

        const SDL_FlipMode flip =
            command.flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

        const SDL_FRect srcF{(float)command.src.x, (float)command.src.y, (float)command.src.w, (float)command.src.h};
        const SDL_FRect dstF{(float)command.dst.x, (float)command.dst.y, (float)command.dst.w, (float)command.dst.h};
        SDL_RenderTextureRotated(
            renderer,
            command.texture->getSDL_Texture(),
            &srcF,
            &dstF,
            0.0,
            nullptr,
            flip);
    }

    clear();
}

// [x] clear(): reset the draw queue.
//     Called automatically by flush(). Can also be called manually to discard queued draws.

void SpriteBatch::clear()
{
    m_commands.clear();
}