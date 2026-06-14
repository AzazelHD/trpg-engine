#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include "engine/data/PropertyId.h"

// [x]: Converts Tiled string property names into fast internal enums.
// Used ONLY during loading phase.

class PropertyRegistry
{
public:
    PropertyId get(std::string_view name) const noexcept
    {
        auto it = m_map.find(std::string(name));
        return it != m_map.end() ? it->second : PropertyId::Unknown;
    }

    void registerProperty(std::string_view name, PropertyId id) noexcept
    {
        m_map.emplace(std::string(name), id);
    }

private:
    std::unordered_map<std::string, PropertyId> m_map;
};