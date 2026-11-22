#ifndef _MAIN_CONFIG_PANEL_H_INCLUDED
#define _MAIN_CONFIG_PANEL_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/ToolbarButton.h"
#include "../../Core/System.h"

class MainConfigPanel : public Panel
{
public:
    MainConfigPanel();
    MainConfigPanel(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;

    Button& GetConfigButton();
    Button& GetAwgButton();
    Button& GetTraceButton();

    virtual void Draw() override;
    void UpdateLayout();

private:
    void SetupButton(Button& button, const char* text);

private:
    SystemClass* _system;
    ToolbarButton _configButton;
    ToolbarButton _awgButton;
    ToolbarButton _traceButton;
};

#endif
