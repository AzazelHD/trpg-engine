#pragma once

#include "engine/scene/Scene.h"

// Legacy compatibility wrapper for older code that still uses the "state" naming.
// New engine and game code should prefer Scene directly.
class IState : public Scene
{
public:
    ~IState() override = default;
};
