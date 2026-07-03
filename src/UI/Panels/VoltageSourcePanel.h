#ifndef _VOLTAGE_SOURCE_PANEL_H_INCLUDED
#define _VOLTAGE_SOURCE_PANEL_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../Controls/Divider.h"
#include "../Controls/Label.h"
#include "../Core/DisplayFormat.h"
#include "../../Core/System.h"

class VoltageSourcePanel : public Panel
{
public:
    VoltageSourcePanel();
    VoltageSourcePanel(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;

    Button& GetRangeButton();
    Button& GetSourceButton();
    Button& GetLimitButton();

    virtual void Draw() override;
    void UpdateLayout();

private:
    void UpdateTexts();

private:
    SystemClass* _system;
    char _readingTextBuffer[48];
    char _rangeTextBuffer[48];
    char _sourceTextBuffer[48];
    char _limitTextBuffer[48];
    DisplayFormatState _measFormatState;
    DisplayFormatState _sourceFormatState;
    DisplayFormatState _limitFormatState;

    Divider _topDivider;
    Label _titleLabel;
    Label _modeLabel;
    Label _valueLabel;
    Label _measLabel;
    Label _rangeLabel;
    Label _limitLabel;
    Button _rangeButton;
    Button _sourceModeButton;
    Button _sourceButton;
    Button _limitButton;
};

#endif
