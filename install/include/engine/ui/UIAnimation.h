#pragma once

#include <memory>
#include <vector>

class UIAnimation
{
public:
    virtual ~UIAnimation() = default;

    virtual void start() {}
    virtual void update(float /*dt*/) {}
    virtual bool isFinished() const { return true; }
};

class UIAnimationTrack
{
public:
    void startAnimation(std::unique_ptr<UIAnimation> animation)
    {
        if (!animation)
            return;
        animation->start();
        m_animations.push_back(std::move(animation));
    }

    void updateAnimations(float dt)
    {
        for (std::size_t i = 0; i < m_animations.size();)
        {
            UIAnimation *anim = m_animations[i].get();
            if (!anim)
            {
                m_animations.erase(m_animations.begin() + static_cast<long long>(i));
                continue;
            }

            anim->update(dt);
            if (anim->isFinished())
            {
                m_animations.erase(m_animations.begin() + static_cast<long long>(i));
                continue;
            }
            ++i;
        }
    }

private:
    std::vector<std::unique_ptr<UIAnimation>> m_animations;
};
