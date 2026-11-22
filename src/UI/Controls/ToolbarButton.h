#ifndef _TOOLBAR_BUTTON_H_INCLUDED
#define _TOOLBAR_BUTTON_H_INCLUDED

#include "Button.h"

// Toolbar palette using Button's native EVE rendering and touch/click handling.
class ToolbarButton : public Button
{
public:
    ToolbarButton();
    void Draw() override;
};

#endif
