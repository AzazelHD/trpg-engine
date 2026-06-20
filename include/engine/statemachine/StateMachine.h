#pragma once
#include <stack>
#include <memory>
#include <vector>
#include <typeinfo>

#include "engine/core/Log.h"

// StateMachine<T> manages a stack of scenes or states.
// T must expose the Scene lifecycle (onEnter/onExit/update/render).
// Being a template means the same machine can manage engine-level states
// AND unit-level animation states — just instantiate with a different T.
//
// [x]: Implement the following methods:
//   - push(std::unique_ptr<T> state)   : push a new state. Calls newState->onEnter().
//   - pop()                            : remove the top state. Calls top->onExit().
//   - replace(std::unique_ptr<T> state): pop current + push new in one call.
//   - update(float dt)                 : forward to top state's update().
//   - render()                         : forward to top state's render().
//   - isEmpty() const                  : true if the stack is empty (game should exit).
//
// Implementation note: because this is a template, the implementation must live
// in this header file (or in a .inl file #included at the bottom of this header).
// You cannot put template method bodies in a .cpp file — the compiler needs to see
// them when it instantiates the template.
template <typename T>
class StateMachine
{
public:
    void push(std::unique_ptr<T> state)
    {
        if (m_updating)
        {
            m_pendingOps.emplace_back(OpType::Push, std::move(state));
            return;
        }
        applyPush(std::move(state));
    }

    void pop()
    {
        if (m_updating)
        {
            m_pendingOps.emplace_back(OpType::Pop);
            return;
        }
        applyPop();
    }

    void replace(std::unique_ptr<T> state)
    {
        if (m_updating)
        {
            m_pendingOps.emplace_back(OpType::Replace, std::move(state));
            return;
        }
        applyReplace(std::move(state));
    }

    void update(float dt)
    {
        if (m_states.empty())
            return;

        m_updating = true;
        m_states.top()->update(dt);
        m_updating = false;

        applyPending();
    }

    void handleInput()
    {
        if (m_states.empty())
            return;

        // Prevent re-entrant calls (safety).
        if (m_updating)
            return;

        // Guard against self-destruction: just like update(), we set
        // m_updating so that any push/pop/replace requested by the scene
        // is deferred until after handleInput() returns.
        m_updating = true;
        m_states.top()->handleInput();
        m_updating = false;

        applyPending();
    }

    void render(float alpha) const
    {
        if (!m_states.empty())
            m_states.top()->render(alpha);
    }

    bool isEmpty() const { return m_states.empty(); }

    const char *currentStateDebugName() const
    {
        if (m_states.empty() || !m_states.top())
            return "<none>";
        return typeid(*m_states.top()).name();
    }

private:
    enum class OpType
    {
        Push,
        Pop,
        Replace
    };

    struct PendingOp
    {
        OpType type;
        std::unique_ptr<T> state;

        explicit PendingOp(OpType t, std::unique_ptr<T> s = nullptr)
            : type(t), state(std::move(s)) {}
    };

    void logTransition(const char *op, const T *state, bool isReplace = false)
    {
        const char *suffix = isReplace ? " (replace)" : "";
        LOG_INFO("StateMachine", "%s -> %s%s", op, state ? typeid(*state).name() : "<null>", suffix);
        LOG_INFO("StateMachine", "TOP  -> %s", currentStateDebugName());
    }

    void applyPush(std::unique_ptr<T> state)
    {
        if (!state)
            return;
        state->onEnter();
        m_states.push(std::move(state));
        logTransition("PUSH", m_states.top().get());
    }

    void applyPop()
    {
        if (m_states.empty())
            return;
        logTransition("POP ", m_states.top().get());
        m_states.top()->onExit();
        m_states.pop();
    }

    // Does NOT touch m_pendingOps — performs the pop+push directly.
    void applyReplace(std::unique_ptr<T> state)
    {
        if (!state)
            return;

        if (!m_states.empty())
        {
            logTransition("POP ", m_states.top().get(), true);
            m_states.top()->onExit();
            m_states.pop();
        }

        state->onEnter();
        m_states.push(std::move(state));
        logTransition("PUSH", m_states.top().get(), true);
    }

    void applyPending()
    {
        for (auto &op : m_pendingOps)
        {
            switch (op.type)
            {
            case OpType::Push:
                applyPush(std::move(op.state));
                break;
            case OpType::Pop:
                applyPop();
                break;
            case OpType::Replace:
                applyReplace(std::move(op.state));
                break;
            }
        }
        m_pendingOps.clear();
    }

    std::stack<std::unique_ptr<T>> m_states;
    std::vector<PendingOp> m_pendingOps;
    bool m_updating = false;
};