#pragma once

#include "engine/animation/AnimationState.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Unity-style animation state machine: named states wrap AnimationState
// playback behaviors, and transitions are declared between them. Two kinds of
// transitions are supported:
//
//   - trigger transitions  (addTransition with a non-empty trigger) fire when
//     Animator::trigger() is called with the same trigger while in `from`.
//   - exit-time transitions (addTransition with an empty trigger) fire when
//     `from` reports AnimationState::isFinished() from within update().
//
// This is gameplay-agnostic: the engine only advances states and honors
// transitions. Which animations to play and when still belongs to the game.
//
// Example (game side, one-shot playback with a completion callback):
//   animator.addState("cast", std::make_unique<MyCastState>());
//   animator.addState("done", std::make_unique<MyDoneState>(onComplete));
//   animator.addTransition("cast", "done", "");          // when cast finishes
//   animator.addTransition("done", "idle", "finished");  // trigger reset
//   animator.play("cast");
class Animator
{
public:
    Animator() = default;
    ~Animator() = default;

    Animator(const Animator &) = delete;
    Animator &operator=(const Animator &) = delete;

    // Movable so an Animator can live inside STL containers (e.g. a per-effect
    // queue in the game layer). Each owned state keeps its identity across the
    // move because the map buckets move along with it.
    Animator(Animator &&) noexcept = default;
    Animator &operator=(Animator &&) noexcept = default;

    void addState(std::string name, std::unique_ptr<AnimationState> state);

    // Declares a transition out of `from`. A non-empty `trigger` makes it a
    // trigger transition (fired by trigger()); an empty `trigger` makes it an
    // exit-time transition (fired when `from` finishes).
    void addTransition(std::string from, std::string to, std::string trigger = {});

    // Forces an immediate switch to `stateName`, calling the previous state's
    // onExit() and the new state's onEnter().
    void play(const std::string &stateName);

    // Fires a trigger transition from the current state, if one is declared.
    void trigger(const std::string &triggerName);

    void update(float dt);

    // Stops playback; the animator becomes inactive until play() is called
    // again.
    void stop();

    [[nodiscard]] const std::string &currentStateName() const { return m_currentName; }
    [[nodiscard]] bool isActive() const { return !m_currentName.empty(); }

private:
    struct Transition
    {
        std::string from;
        std::string to;
        std::string trigger;
    };

    void enterState(const std::string &name);
    void exitState();

    std::unordered_map<std::string, std::unique_ptr<AnimationState>> m_states;
    std::vector<Transition> m_transitions;
    std::string m_currentName;
};