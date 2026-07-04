#ifndef _HEADER_PANEL_H_INCLUDED
#define _HEADER_PANEL_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../Controls/Label.h"
#include "../../Core/System.h"

class HeaderPanel : public Panel
{
public:
    HeaderPanel();
    HeaderPanel(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;

    Button& GetHomeButton();
    Button& GetMenuButton();

    virtual void Draw() override;
    void UpdateLayout();

private:
    void SetupControls();
    void UpdateTexts();

private:
    SystemClass* _system;
    char _timeTextBuffer[9];
    Button _homeButton;
    Button _menuButton;
    Label _triggerLabel;
    Label _controlLabel;
    Label _lanLabel;
    Label _timeLabel;
};

#endif
