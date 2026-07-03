#include "Container.h"

#include "GDClass.h"
#include "Theme.h"

Control* Container::_globalFocus = nullptr;
const Container* Container::_focusOwner = nullptr;

Container::Container()
    : Control(),
      _childCount(0)
{
    for (uint8_t i = 0; i < MaxChildren; ++i)
    {
        _children[i] = nullptr;
    }
}

Container::Container(int16_t x, int16_t y, int16_t w, int16_t h)
    : Control(x, y, w, h),
      _childCount(0)
{
    for (uint8_t i = 0; i < MaxChildren; ++i)
    {
        _children[i] = nullptr;
    }
}

void Container::Draw()
{
    if (!_visible)
    {
        return;
    }

    for (uint8_t i = 0; i < _childCount; ++i)
    {
        Control* child = _children[i];
        if (child != nullptr && child->IsVisible())
        {
            child->Draw();
        }
    }

    DrawFocus();
}

void Container::TouchStart(int16_t x, int16_t y)
{
    if (!_enabled || !_visible)
    {
        return;
    }

    for (int i = _childCount - 1; i >= 0; --i)
    {
        Control* child = _children[i];
        if (child != nullptr &&
            child->IsVisible() &&
            child->IsEnabled() &&
            child->Contains(x, y))
        {
            if (child->IsFocusable())
            {
                SetFocus(child);
            }
            child->TouchStart(x, y);
            return;
        }
    }
}

void Container::TouchMove(int16_t x, int16_t y)
{
    if (!_enabled || !_visible)
    {
        return;
    }

    for (uint8_t i = 0; i < _childCount; ++i)
    {
        Control* child = _children[i];
        if (child != nullptr && child->IsVisible() && child->IsEnabled())
        {
            child->TouchMove(x, y);
        }
    }
}

void Container::TouchEnd(int16_t x, int16_t y)
{
    if (!_enabled || !_visible)
    {
        return;
    }

    for (uint8_t i = 0; i < _childCount; ++i)
    {
        Control* child = _children[i];
        if (child != nullptr && child->IsVisible() && child->IsEnabled())
        {
            child->TouchEnd(x, y);
        }
    }
}

bool Container::AddChild(Control* child)
{
    if (child == nullptr || _childCount >= MaxChildren)
    {
        return false;
    }

    _children[_childCount] = child;
    ++_childCount;
    return true;
}

void Container::SetFocus(Control* control)
{
    if (_globalFocus == control && _focusOwner == this)
    {
        return;
    }

    if (_globalFocus != nullptr)
    {
        _globalFocus->SetFocused(false);
    }

    _globalFocus = control;
    _focusOwner = (control != nullptr) ? this : nullptr;

    if (_globalFocus != nullptr)
    {
        _globalFocus->SetFocused(true);
    }
}

Control* Container::GetFocus() const
{
    return _globalFocus;
}

void Container::ClearFocus()
{
    if (_globalFocus != nullptr)
    {
        _globalFocus->SetFocused(false);
    }

    _globalFocus = nullptr;
    _focusOwner = nullptr;
}

void Container::ClearChildren()
{
    for (uint8_t i = 0; i < _childCount; ++i)
    {
        _children[i] = nullptr;
    }

    _childCount = 0;
}

uint8_t Container::GetChildCount() const
{
    return _childCount;
}

Control* Container::GetChild(uint8_t index) const
{
    if (index >= _childCount)
    {
        return nullptr;
    }

    return _children[index];
}

void Container::DrawFocus() const
{
    if (_focusOwner != this || _globalFocus == nullptr || !_globalFocus->IsVisible())
    {
        return;
    }

    const int16_t x = _globalFocus->GetX() + 1;
    const int16_t y = _globalFocus->GetY() + 1;
    const int16_t x1 = _globalFocus->GetX() + _globalFocus->GetWidth() - 1;
    const int16_t y1 = _globalFocus->GetY() + _globalFocus->GetHeight() - 1;

    GD.ColorRGB(Theme::FocusYellow);
    GD.LineWidth(16);
    GD.Begin(LINE_STRIP);
    GD.Vertex2f(x, y);
    GD.Vertex2f(x1, y);
    GD.Vertex2f(x1, y1);
    GD.Vertex2f(x, y1);
    GD.Vertex2f(x, y);
}
