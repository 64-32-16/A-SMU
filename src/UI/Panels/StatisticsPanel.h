#ifndef _STATISTICS_PANEL_H_INCLUDED
#define _STATISTICS_PANEL_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Core/DisplayFormat.h"
#include "../Controls/Button.h"
#include "../Controls/Label.h"
#include "../../Core/System.h"

class StatisticsPanel : public Panel
{
public:
    StatisticsPanel();
    StatisticsPanel(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;

    virtual void Draw() override;
    void UpdateLayout();

private:
    void UpdateTexts();
    void FormatStat(DisplayQuantity quantity, float value, bool showSign,
                    DisplayFormatState& state, char* buffer, size_t bufferSize);
    static void OnClearClick(Button* sender);

private:
    SystemClass* _system;

    Label _legendValue;
    Label _legendPeak;
    Label _legendAverage;
    Label _legendMin;
    Label _legendMax;

    Label _currentHeader;
    Label _currentValue;
    Label _currentPeak;
    Label _currentAverage;
    Label _currentMin;
    Label _currentMax;

    Label _voltageHeader;
    Label _voltageValue;
    Label _voltagePeak;
    Label _voltageAverage;
    Label _voltageMin;
    Label _voltageMax;

    Label _bufferInfoLabel;
    Label _bufferInfoValue;
    Button _clearButton;

    char _bufferCountText[16];
    char _currentValueText[32];
    char _currentPeakText[32];
    char _currentAverageText[32];
    char _currentMinText[32];
    char _currentMaxText[32];
    char _voltageValueText[32];
    char _voltagePeakText[32];
    char _voltageAverageText[32];
    char _voltageMinText[32];
    char _voltageMaxText[32];

    DisplayFormatState _currentValueState;
    DisplayFormatState _currentPeakState;
    DisplayFormatState _currentAverageState;
    DisplayFormatState _currentMinState;
    DisplayFormatState _currentMaxState;
    DisplayFormatState _voltageValueState;
    DisplayFormatState _voltagePeakState;
    DisplayFormatState _voltageAverageState;
    DisplayFormatState _voltageMinState;
    DisplayFormatState _voltageMaxState;
};

#endif
