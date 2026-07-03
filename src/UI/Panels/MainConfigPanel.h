#ifndef _MAIN_CONFIG_PANEL_H_INCLUDED
#define _MAIN_CONFIG_PANEL_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/BooleanButton.h"
#include "../Controls/Button.h"
#include "../Controls/Divider.h"
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
    BooleanButton& GetOutputSwitch();

    virtual void Draw() override;
    void UpdateLayout();

private:
    void SetupButton(Button& button, const char* text);

private:
    SystemClass* _system;
    Divider _topDivider;
    Button _configButton;
    Button _awgButton;
    Button _traceButton;
    BooleanButton _outputSwitch;
};

#endif
