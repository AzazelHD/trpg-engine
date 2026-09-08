#pragma once

// Base interface for states owned by engine/animation/Animator.h.
//
// A state is a self-contained playback behavior: an enter hook, a per-frame
// update, an optional exit hook, and a completion signal. The Animator owns
// transitions between named states (Unity-style Animator: named states +
// trigger transitions + exit-time transitions).
//
// States never render directly — the game layer queries
// Animator::currentStateName() and draws the matching frame/pose. This keeps
// the engine gameplay-agnostic and leaves sprite/frame choice to the game.
class AnimationState
{
public:
    virtual ~AnimationState() = default;

    virtual void onEnter() {}
    virtual void update(float /*dt*/) {}

    // Reports whether this state finished playing. A finished state drives an
    // exit-time (auto) transition declared in Animator::addTransition with an
    // empty trigger. Loop animations simply never finish.
    [[nodiscard]] virtual bool isFinished() const { return false; }

    virtual void onExit() {}
};