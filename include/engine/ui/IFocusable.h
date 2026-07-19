#pragma once

class IFocusable
{
public:
    virtual ~IFocusable() = default;

    virtual bool activate() const = 0;
    virtual void setSelected(bool selected) = 0;
    virtual bool isEnabled() const = 0;

    // Optional: controls that support left/right adjustment (sliders,
    // cycled values). Buttons and other simple focusables ignore this —
    // the default reports "not handled".
    virtual bool handleLeft() { return false; }
    virtual bool handleRight() { return false; }
};