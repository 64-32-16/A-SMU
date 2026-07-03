#ifndef _SWIPE_CONTROLLER_H_INCLUDED
#define _SWIPE_CONTROLLER_H_INCLUDED

#include <Arduino.h>
#include "Control.h"

class SwipeController
{
public:
    static constexpr uint8_t MaxPages = 8;

    SwipeController();

    bool Add(Control* page);
    bool SetPage(uint8_t index, Control* page);
    uint8_t GetCount() const;
    uint8_t Current() const;
    void SetCurrent(uint8_t index);
    uint8_t Next() const;
    uint8_t Previous() const;
    void SetViewport(int16_t x, int16_t y, int16_t w, int16_t h);

    void TouchStart(int16_t x, int16_t y);
    void TouchMove(int16_t x, int16_t y);
    bool TouchEnd(int16_t x, int16_t y);
    bool Update();
    bool IsSwipeActive() const;
    bool IsDragging() const;

private:
    void ApplyVisibility();
    void ApplyIdleLayout();
    void ApplyDragLayout(int16_t dx);
    void ApplyAnimationLayout();

private:
    Control* _pages[MaxPages];
    uint8_t _count;
    uint8_t _current;
    int16_t _viewportX;
    int16_t _viewportY;
    int16_t _viewportW;
    int16_t _viewportH;
    bool _tracking;
    bool _dragging;
    bool _animating;
    int16_t _startX;
    int16_t _startY;
    int16_t _dragDx;
    int16_t _animationTargetDx;
    int8_t _animationTargetIndex;
};

#endif
