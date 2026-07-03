#include "SwipeController.h"

namespace
{
constexpr int16_t kSwipeThresholdX = 40;
constexpr int16_t kSwipeThresholdY = 24;
constexpr int16_t kAnimationStep = 28;
}

SwipeController::SwipeController()
    : _count(0),
      _current(0),
      _viewportX(0),
      _viewportY(0),
      _viewportW(0),
      _viewportH(0),
      _tracking(false),
      _dragging(false),
      _animating(false),
      _startX(0),
      _startY(0),
      _dragDx(0),
      _animationTargetDx(0),
      _animationTargetIndex(-1)
{
    for (uint8_t i = 0; i < MaxPages; ++i)
    {
        _pages[i] = nullptr;
    }
}

bool SwipeController::Add(Control* page)
{
    if (page == nullptr || _count >= MaxPages)
    {
        return false;
    }

    _pages[_count] = page;
    page->SetVisible(_count == _current);
    ++_count;
    ApplyIdleLayout();
    return true;
}

bool SwipeController::SetPage(uint8_t index, Control* page)
{
    if (page == nullptr || index >= _count)
    {
        return false;
    }

    if (_pages[index] == page)
    {
        return true;
    }

    if (_pages[index] != nullptr)
    {
        _pages[index]->SetVisible(false);
    }

    _pages[index] = page;

    if (_animating)
    {
        ApplyAnimationLayout();
    }
    else if (_dragging)
    {
        ApplyDragLayout(_dragDx);
    }
    else
    {
        ApplyIdleLayout();
    }

    return true;
}

uint8_t SwipeController::GetCount() const
{
    return _count;
}

uint8_t SwipeController::Current() const
{
    return _current;
}

void SwipeController::SetCurrent(uint8_t index)
{
    if (_count == 0 || index >= _count)
    {
        return;
    }

    _current = index;
    _dragDx = 0;
    _animating = false;
    _dragging = false;
    ApplyIdleLayout();
}

uint8_t SwipeController::Next() const
{
    if (_count == 0)
    {
        return 0;
    }

    return static_cast<uint8_t>((_current + 1) % _count);
}

uint8_t SwipeController::Previous() const
{
    if (_count == 0)
    {
        return 0;
    }

    return (_current == 0) ? static_cast<uint8_t>(_count - 1) : static_cast<uint8_t>(_current - 1);
}

void SwipeController::SetViewport(int16_t x, int16_t y, int16_t w, int16_t h)
{
    _viewportX = x;
    _viewportY = y;
    _viewportW = w;
    _viewportH = h;

    if (_animating)
    {
        ApplyAnimationLayout();
    }
    else if (_dragging)
    {
        ApplyDragLayout(_dragDx);
    }
    else
    {
        ApplyIdleLayout();
    }
}

void SwipeController::TouchStart(int16_t x, int16_t y)
{
    if (_count < 2)
    {
        return;
    }

    _tracking = true;
    _dragging = false;
    _animating = false;
    _startX = x;
    _startY = y;
    _dragDx = 0;

    ApplyDragLayout(0);
}

void SwipeController::TouchMove(int16_t x, int16_t y)
{
    if (!_tracking || _count < 2)
    {
        return;
    }

    const int16_t dx = x - _startX;
    const int16_t dy = y - _startY;
    const int16_t absDx = dx >= 0 ? dx : static_cast<int16_t>(-dx);
    const int16_t absDy = dy >= 0 ? dy : static_cast<int16_t>(-dy);

    if (!_dragging)
    {
        if (absDx < 8)
        {
            return;
        }

        if (absDx <= absDy)
        {
            return;
        }
    }

    _dragging = true;
    _dragDx = dx;

    if (_dragDx > _viewportW)
    {
        _dragDx = _viewportW;
    }
    else if (_dragDx < -_viewportW)
    {
        _dragDx = -_viewportW;
    }

    ApplyDragLayout(_dragDx);
}

bool SwipeController::TouchEnd(int16_t x, int16_t y)
{
    if (_count < 2)
    {
        _tracking = false;
        return false;
    }

    if (_animating)
    {
        return false;
    }

    _tracking = false;

    const int16_t dx = x - _startX;
    const int16_t dy = y - _startY;
    const int16_t absDx = dx >= 0 ? dx : static_cast<int16_t>(-dx);
    const int16_t absDy = dy >= 0 ? dy : static_cast<int16_t>(-dy);

    if (!_dragging || absDx < kSwipeThresholdX || absDx <= absDy + kSwipeThresholdY)
    {
        _dragging = false;
        _animationTargetDx = 0;
        _animationTargetIndex = static_cast<int8_t>(_current);
        _animating = true;
        return false;
    }

    if (dx < 0)
    {
        _animationTargetDx = -_viewportW;
        _animationTargetIndex = static_cast<int8_t>(Next());
    }
    else
    {
        _animationTargetDx = _viewportW;
        _animationTargetIndex = static_cast<int8_t>(Previous());
    }

    _animating = true;
    _dragging = false;
    return true;
}

bool SwipeController::Update()
{
    if (!_animating)
    {
        return false;
    }

    if (_dragDx < _animationTargetDx)
    {
        _dragDx = static_cast<int16_t>(_dragDx + kAnimationStep);
        if (_dragDx > _animationTargetDx)
        {
            _dragDx = _animationTargetDx;
        }
    }
    else if (_dragDx > _animationTargetDx)
    {
        _dragDx = static_cast<int16_t>(_dragDx - kAnimationStep);
        if (_dragDx < _animationTargetDx)
        {
            _dragDx = _animationTargetDx;
        }
    }

    ApplyAnimationLayout();

    if (_dragDx == _animationTargetDx)
    {
        if (_animationTargetIndex >= 0)
        {
            _current = static_cast<uint8_t>(_animationTargetIndex);
        }

        _animating = false;
        _dragDx = 0;
        ApplyIdleLayout();
        return true;
    }

    return false;
}

bool SwipeController::IsSwipeActive() const
{
    return _tracking || _dragging || _animating;
}

bool SwipeController::IsDragging() const
{
    return _dragging || _animating;
}

void SwipeController::ApplyVisibility()
{
    for (uint8_t i = 0; i < _count; ++i)
    {
        if (_pages[i] != nullptr)
        {
            _pages[i]->SetVisible(i == _current);
        }
    }
}

void SwipeController::ApplyIdleLayout()
{
    ApplyVisibility();

    for (uint8_t i = 0; i < _count; ++i)
    {
        if (_pages[i] != nullptr)
        {
            _pages[i]->SetBounds(i == _current ? _viewportX : (_viewportX + _viewportW),
                                 _viewportY,
                                 _viewportW,
                                 _viewportH);
        }
    }
}

void SwipeController::ApplyDragLayout(int16_t dx)
{
    if (_count == 0)
    {
        return;
    }

    const uint8_t previous = Previous();
    const uint8_t next = Next();

    for (uint8_t i = 0; i < _count; ++i)
    {
        if (_pages[i] == nullptr)
        {
            continue;
        }

        _pages[i]->SetVisible(false);
        _pages[i]->SetBounds(_viewportX + _viewportW, _viewportY, _viewportW, _viewportH);
    }

    if (_pages[_current] != nullptr)
    {
        _pages[_current]->SetVisible(true);
        _pages[_current]->SetBounds(_viewportX + dx, _viewportY, _viewportW, _viewportH);
    }

    if (dx < 0 && _pages[next] != nullptr)
    {
        _pages[next]->SetVisible(true);
        _pages[next]->SetBounds(_viewportX + _viewportW + dx, _viewportY, _viewportW, _viewportH);
    }
    else if (dx > 0 && _pages[previous] != nullptr)
    {
        _pages[previous]->SetVisible(true);
        _pages[previous]->SetBounds(_viewportX - _viewportW + dx, _viewportY, _viewportW, _viewportH);
    }
}

void SwipeController::ApplyAnimationLayout()
{
    ApplyDragLayout(_dragDx);
}
