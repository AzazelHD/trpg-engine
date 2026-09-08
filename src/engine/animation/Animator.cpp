#include "engine/animation/Animator.h"

#include "engine/core/Log.h"

void Animator::addState(std::string name, std::unique_ptr<AnimationState> state)
{
    if (!state)
        return;

    if (m_states.find(name) != m_states.end())
    {
        LOG_ERROR("Animator", "State '%s' already registered", name.c_str());
        return;
    }

    m_states.emplace(std::move(name), std::move(state));
}

void Animator::addTransition(std::string from, std::string to, std::string trigger)
{
    m_transitions.push_back(Transition{std::move(from), std::move(to), std::move(trigger)});
}

void Animator::play(const std::string &stateName)
{
    enterState(stateName);
}

void Animator::trigger(const std::string &triggerName)
{
    if (m_currentName.empty())
        return;

    for (const Transition &transition : m_transitions)
    {
        if (transition.from == m_currentName && transition.trigger == triggerName)
        {
            enterState(transition.to);
            return;
        }
    }
}

void Animator::update(float dt)
{
    if (m_currentName.empty())
        return;

    const auto current = m_states.find(m_currentName);
    if (current == m_states.end())
    {
        LOG_ERROR("Animator", "Current state '%s' is no longer registered; stopping", m_currentName.c_str());
        m_currentName.clear();
        return;
    }

    AnimationState *state = current->second.get();
    state->update(dt);

    if (!state->isFinished())
        return;

    const std::string finished = m_currentName;
    for (const Transition &transition : m_transitions)
    {
        if (transition.from == finished && transition.trigger.empty())
        {
            enterState(transition.to);
            return;
        }
    }

    exitState();
}

void Animator::stop()
{
    exitState();
}

void Animator::enterState(const std::string &name)
{
    if (m_currentName == name)
        return;

    const auto existing = m_states.find(name);
    if (existing == m_states.end())
    {
        LOG_ERROR("Animator", "Cannot enter unknown animation state '%s'", name.c_str());
        return;
    }

    exitState();

    const auto it = m_states.find(name);
    if (it == m_states.end())
    {
        LOG_ERROR("Animator", "State '%s' vanished during exit; stopping", name.c_str());
        return;
    }

    m_currentName = name;
    it->second->onEnter();
}

void Animator::exitState()
{
    if (m_currentName.empty())
        return;

    const auto it = m_states.find(m_currentName);
    if (it != m_states.end())
        it->second->onExit();
    m_currentName.clear();
}