#pragma once

// Scene is the generic lifecycle contract owned by the engine.
// Games define concrete scenes such as boot, menus, battle, or credits.
class Scene
{
public:
    virtual ~Scene() = default;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;

    // Hook for discrete UI/Game input processing. Runs exactly once per frame.
    // Optional to implement: scenes with no input can just ignore it.
    virtual void handleInput() {}

    virtual void update(float dt) = 0;
    virtual void render(float alpha) = 0;
};