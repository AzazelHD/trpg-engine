#include "engine/renderer/FontManager.h"
#include "engine/renderer/Renderer.h"
#include "engine/core/Log.h"

namespace
{
    struct FontSpec
    {
        FontRole role;
        const char *path;
        float size;
    };

    constexpr FontSpec kFontTable[] = {
        {FontRole::Body, "assets/fonts/PixeloidSans.ttf", 24.0f},
        {FontRole::Heading, "assets/fonts/PixeloidSans.ttf", 32.0f},
        {FontRole::Title, "assets/fonts/PixeloidSans.ttf", 72.0f},
        {FontRole::Placeholder, "assets/fonts/PixeloidSans.ttf", 40.0f},
    };
}

FontManager &FontManager::instance()
{
    static FontManager manager;
    return manager;
}

void FontManager::loadAll(Renderer *renderer)
{
    if (m_loaded || !renderer)
        return;

    for (const FontSpec &spec : kFontTable)
    {
        Font *font = renderer->loadFont(spec.path, spec.size);
        if (!font)
            LOG_ERROR("FontManager", "Failed to load font for role %d: %s (size %.0f)",
                      static_cast<int>(spec.role), spec.path, spec.size);
        m_fonts[static_cast<std::size_t>(spec.role)] = font;
    }

    m_loaded = true;
}

Font *FontManager::get(FontRole role) const
{
    return m_fonts[static_cast<std::size_t>(role)];
}

void FontManager::clear()
{
    for (Font *&font : m_fonts)
    {
        delete font;
        font = nullptr;
    }
    m_loaded = false;
}