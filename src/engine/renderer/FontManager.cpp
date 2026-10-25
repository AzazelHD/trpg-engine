
#include "engine/renderer/FontManager.h"

#include "engine/renderer/Font.h"
#include "engine/renderer/Renderer.h"
#include "engine/core/Log.h"

#include <cmath>
#include <unordered_map>
#include <utility>
#include <vector>

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

// Dynamic font cache implementation.
// Each font is owned by FontManager.
struct FontManager::DynamicFontCache
{
    std::unordered_map<std::string, std::vector<std::pair<float, Font *>>> fonts;
};

FontManager &FontManager::instance()
{
    static FontManager manager;
    return manager;
}

void FontManager::loadAll(Renderer *renderer)
{
    if (m_loaded || renderer == nullptr)
        return;

    for (const FontSpec &spec : kFontTable)
    {
        const std::size_t index = static_cast<std::size_t>(spec.role);

        Font *font = renderer->loadFont(spec.path, spec.size);

        if (font == nullptr)
        {
            LOG_ERROR(
                "FontManager",
                "Failed to load font for role %d: %s (size %.0f)",
                static_cast<int>(spec.role),
                spec.path,
                spec.size);
        }

        m_fonts[index] = font;
    }

    m_loaded = true;
}

Font *FontManager::get(FontRole role) const
{
    const std::size_t index = static_cast<std::size_t>(role);

    if (index >= m_fonts.size())
        return nullptr;

    return m_fonts[index];
}

Font *FontManager::getSized(
    Renderer *renderer,
    const std::string &filePath,
    float size)
{
    if (renderer == nullptr ||
        filePath.empty() ||
        !std::isfinite(size) ||
        size <= 0.0f)
    {
        return nullptr;
    }

    // Reuse a font if this exact path and size have already been requested.
    if (m_dynamicCache != nullptr)
    {
        const auto pathIt = m_dynamicCache->fonts.find(filePath);

        if (pathIt != m_dynamicCache->fonts.end())
        {
            for (const auto &entry : pathIt->second)
            {
                if (std::abs(entry.first - size) < 0.01f)
                    return entry.second;
            }
        }
    }

    // Load only when no matching cached font exists.
    Font *font = renderer->loadFont(filePath.c_str(), size);

    if (font == nullptr)
    {
        LOG_ERROR(
            "FontManager",
            "Failed to load dynamic font: %s (size %.2f)",
            filePath.c_str(),
            size);

        return nullptr;
    }

    if (m_dynamicCache == nullptr)
        m_dynamicCache = new DynamicFontCache();

    m_dynamicCache->fonts[filePath].emplace_back(size, font);

    return font;
}

FontManager::~FontManager()
{
    clear();
}

void FontManager::clear()
{
    // Release predefined role fonts.
    for (Font *&font : m_fonts)
    {
        delete font;
        font = nullptr;
    }

    // Release dynamically sized fonts.
    if (m_dynamicCache != nullptr)
    {
        for (auto &pathEntry : m_dynamicCache->fonts)
        {
            for (auto &fontEntry : pathEntry.second)
            {
                delete fontEntry.second;
                fontEntry.second = nullptr;
            }
        }

        delete m_dynamicCache;
        m_dynamicCache = nullptr;
    }

    m_loaded = false;
}
