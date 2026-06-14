#pragma once
#include <stack>
#include <memory>
#include <vector>

// StateMachine<T> manages a stack of scenes or states.
// T must expose the Scene lifecycle (onEnter/onExit/update/render).
// Being a template means the same machine can manage engine-level states
// AND unit-level animation states — just instantiate with a different T.
// New code may use the SceneStack<T> alias in engine/scene/SceneStack.h.
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
        if (m_inDispatch)
        {
            m_pendingOps.push_back(PendingOp{OpType::Push, std::move(state)});
            return;
        }

        applyPush(std::move(state));
    }

    void pop()
    {
        if (m_inDispatch)
        {
            m_pendingOps.push_back(PendingOp{OpType::Pop, nullptr});
            return;
        }

        applyPop();
    }

    void replace(std::unique_ptr<T> state)
    {
        if (m_inDispatch)
        {
            m_pendingOps.push_back(PendingOp{OpType::Replace, std::move(state)});
            return;
        }

        applyReplace(std::move(state));
    }

    void update(float dt)
    {
        if (!m_states.empty())
        {
            m_inDispatch = true;
            m_states.top()->update(dt);
            m_inDispatch = false;
            applyPending();
        }
    }

    void render() const
    {
        if (!m_states.empty())
        {
            m_states.top()->render();
        }
    }

    void render(float alpha) const
    {
        (void)alpha;
        render();
    }

    bool isEmpty() const
    {
        return m_states.empty();
    }

private:
    enum class OpType
    {
        Push,
        Pop,
        Replace,
    };

    struct PendingOp
    {
        OpType type;
        std::unique_ptr<T> state;
    };

    void applyPush(std::unique_ptr<T> state)
    {
        if (state)
        {
            state->onEnter();
            m_states.push(std::move(state));
        }
    }

    void applyPop()
    {
        if (!m_states.empty())
        {
            m_states.top()->onExit();
            m_states.pop();
        }
    }

    void applyReplace(std::unique_ptr<T> state)
    {
        applyPop();
        applyPush(std::move(state));
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
    bool m_inDispatch = false;
};
