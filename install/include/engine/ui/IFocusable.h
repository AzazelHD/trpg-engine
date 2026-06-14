#pragma once

class IFocusable
{
public:
    virtual ~IFocusable() = default;

    virtual bool activate() const = 0;
    virtual void setSelected(bool selected) = 0;
    virtual bool isEnabled() const = 0;
};