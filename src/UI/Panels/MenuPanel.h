#ifndef _MENU_PANEL_H_INCLUDED
#define _MENU_PANEL_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../../Core/System.h"

class MenuPanel : public Panel
{
public:
    MenuPanel();
    MenuPanel(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;
    Button& GetOutputOffStateButton();
    Button& GetOvpButton();
    Button& GetOcpButton();
    Button& GetSweepButton();
    Button& GetQuicksetButton();
    Button& GetGraphButton();

    virtual void Draw() override;
    void UpdateLayout();

private:
    SystemClass* _system;
    Button _outputOffStateButton;
    Button _ovpButton;
    Button _ocpButton;
    Button _sweepButton;
    Button _quicksetButton;
    Button _graphButton;
};

#endif
