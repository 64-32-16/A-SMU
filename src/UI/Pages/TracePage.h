#ifndef _TRACE_PAGE_H_INCLUDED
#define _TRACE_PAGE_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Panels/GraphPanel.h"
#include "../Panels/HeaderPanel.h"
#include "../Panels/StatisticsPanel.h"
#include "../Panels/TraceConfigPanel.h"
#include "../../Core/System.h"

class TracePage : public Panel
{
public:
    TracePage();
    TracePage(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;

    HeaderPanel& GetHeaderPanel();
    GraphPanel& GetGraphPanel();
    StatisticsPanel& GetStatisticsPanel();
    TraceConfigPanel& GetConfigPanel();

    void SetActiveView(TraceConfigView view);
    TraceConfigView GetActiveView() const;

    virtual void Draw() override;
    void UpdateLayout();

private:
    void UpdateVisiblePanel();
    static void OnConfigViewChanged(TraceConfigView view);

private:
    SystemClass* _system;
    HeaderPanel _headerPanel;
    GraphPanel _graphPanel;
    StatisticsPanel _statisticsPanel;
    TraceConfigPanel _configPanel;
};

#endif
