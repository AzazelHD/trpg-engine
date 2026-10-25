
#pragma once

#include <array>
#include <cstddef>
#include <string>

class Font;
class Renderer;

enum class FontRole
{
    Body,        // Buttons, menu items, unit portrait stats and descriptions
    Heading,     // Settings rows and menu subsections
    Title,       // Main menu title and victory/defeat banners
    Placeholder, // Large single-glyph placeholder art
    Count
};

// Owns the game's shared fonts.
//
// Role-based fonts are loaded once at boot.
// Dynamically sized fonts can be requested by path and size.
// FontManager owns all returned Font objects; callers must not delete them.
class FontManager
{
public:
    static FontManager &instance();

    // Loads the predefined role fonts. Subsequent calls are no-ops
    // while the manager remains loaded.
    void loadAll(Renderer *renderer);

    // Returns the font for a predefined role, or nullptr if unavailable.
    Font *get(FontRole role) const;

    // Returns a shared font for the requested file and point size.
    // The returned pointer is owned by FontManager.
    // Implementation will be added in FontManager.cpp.
    Font *getSized(Renderer *renderer, const std::string &filePath, float size);

    // Releases every font owned by this manager.
    void clear();

    FontManager(const FontManager &) = delete;
    FontManager &operator=(const FontManager &) = delete;
    FontManager(FontManager &&) = delete;
    FontManager &operator=(FontManager &&) = delete;

private:
    FontManager() = default;
    ~FontManager();

    std::array<Font *, static_cast<std::size_t>(FontRole::Count)> m_fonts{};

    // Dynamic fonts are cached by path and size in the implementation.
    // Using an opaque implementation keeps container details out of this header.
    struct DynamicFontCache;
    DynamicFontCache *m_dynamicCache = nullptr;

    bool m_loaded = false;
};
