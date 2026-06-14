#include <SDL3/SDL.h>
#include "engine/renderer/Texture.h"
#include "engine/renderer/SpriteBatch.h"

// [x] draw(): push a DrawCommand onto m_commands.
//     Caller is responsible for passing screen-ready SDL_Rects.

void SpriteBatch::draw(const Texture *texture, SDL_Rect src, SDL_Rect dst, bool flipH)
{
    m_commands.push_back(DrawCommand{texture, src, dst, flipH});
}

// [x] flush(): send all queued draw commands to SDL, then clear.
//     No coordinate conversion happens here — rects are already screen-space.

void SpriteBatch::flush(SDL_Renderer *renderer)
{
    for (const DrawCommand &command : m_commands)
    {
        if (!command.texture)
            continue;

        SDL_FlipMode flip = command.flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

        SDL_FRect srcF{
            (float)command.src.x,
            (float)command.src.y,
            (float)command.src.w,
            (float)command.src.h};

        SDL_FRect dstF{
            (float)command.dst.x,
            (float)command.dst.y,
            (float)command.dst.w,
            (float)command.dst.h};

        SDL_RenderTextureRotated(
            renderer,
            (SDL_Texture *)command.texture->getNativeHandle(),
            &srcF,
            &dstF,
            0.0,
            nullptr,
            flip);
    }

    clear();
}

// [x] clear(): reset draw queue.

void SpriteBatch::clear()
{
    m_commands.clear();
}