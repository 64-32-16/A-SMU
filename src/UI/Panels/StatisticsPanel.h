#ifndef _STATISTICS_PANEL_H_INCLUDED
#define _STATISTICS_PANEL_H_INCLUDED

#include "../Core/Panel.h"
#include "../Core/DisplayFormat.h"
#include "../Controls/Button.h"
#include "../../Core/System.h"

class StatisticsPanel : public Panel
{
public:
    StatisticsPanel();
    StatisticsPanel(int16_t x, int16_t y, int16_t w, int16_t h);
    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;
    void Draw() override;
    void TouchEnd(int16_t x, int16_t y) override;
    void UpdateLayout();

private:
    void DrawStatistic(const char* label, float value, bool valid, bool signedValue,
                       DisplayQuantity quantity, int16_t x, int16_t valueX, int16_t y);
    void DrawFooter();
    SystemClass* _system;
    Button _clearButton;
};

#endif
