#pragma once
#include <array>

class Font;
class Renderer;

enum class FontRole
{
    Body,        // buttons, menu items, unit portrait stats (name/HP/MP), skill descriptions
    Heading,     // slightly larger than Body — settings menu rows, main menu subsections
    Title,       // largest — main menu title, victory/defeat banners
    Placeholder, // large single-glyph placeholder art (unit circle+letter)
    Count
};

// FontManager owns every Font the game uses, keyed by role instead of by
// call site. Loaded once in BootState, consumed everywhere via get().
// Fonts are never reloaded per-scene — see BootState::onEnter() for the
// single loading point.
class FontManager
{
public:
    static FontManager &instance();

    // Loads every FontRole from the given path/size table. Safe to call
    // once at boot; subsequent calls are no-ops if already loaded.
    void loadAll(Renderer *renderer);

    // Returns the font for this role, or nullptr if loadAll() hasn't run
    // yet or that specific font failed to load.
    Font *get(FontRole role) const;

    void clear(); // deletes all loaded fonts — called from App's dtor

    FontManager(const FontManager &) = delete;
    FontManager &operator=(const FontManager &) = delete;

private:
    FontManager() = default;
    ~FontManager() = default;

    std::array<Font *, static_cast<std::size_t>(FontRole::Count)> m_fonts{};
    bool m_loaded = false;
};