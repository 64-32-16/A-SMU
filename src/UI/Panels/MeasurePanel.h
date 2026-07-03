#ifndef _MEASURE_PANEL_H_INCLUDED
#define _MEASURE_PANEL_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../Controls/Divider.h"
#include "../Controls/Label.h"
#include "../Core/DisplayFormat.h"
#include "../../Core/System.h"

class MeasurePanel : public Panel
{
public:
    MeasurePanel();
    MeasurePanel(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;

    void SetCaption(const char* caption);
    const char* GetCaption() const;

    Button& GetRangeButton();
    Button& GetFunctionButton();

    virtual void Draw() override;
    void UpdateLayout();

private:
    void UpdateTexts();
    void FormatMainValue(char* valueBuffer, size_t valueBufferSize,
                         char* unitBuffer, size_t unitBufferSize);

private:
    SystemClass* _system;
    const char* _caption;
    char _titleTextBuffer[48];
    char _readingTextBuffer[48];
    DisplayFormatState _voltageFormatState;
    DisplayFormatState _currentFormatState;
    DisplayFormatState _resistanceFormatState;
    DisplayFormatState _powerFormatState;

    Divider _topDivider;
    Label _titleLabel;
    Label _valueLabel;
    Label _azeroLabel;
    Label _rangeLabel;
    Label _rangeValueLabel;
    Button _rangeButton;
    Button _functionButton;
};

#endif
