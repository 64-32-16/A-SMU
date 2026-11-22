#ifndef _TRACE_CONFIG_PANEL_H_INCLUDED
#define _TRACE_CONFIG_PANEL_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/ToolbarButton.h"
#include "../../Core/System.h"

enum class TraceConfigView
{
    Graph,
    Statistics
};

typedef void (*TraceConfigViewChangedHandler)(TraceConfigView view);

class TraceConfigPanel : public Panel
{
public:
    TraceConfigPanel();
    TraceConfigPanel(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;

    void SetActiveView(TraceConfigView view);
    TraceConfigView GetActiveView() const;
    void SetOnViewChanged(TraceConfigViewChangedHandler handler);

    Button& GetGraphButton();
    Button& GetStatisticsButton();

    virtual void Draw() override;
    void UpdateLayout();

private:
    void SetupButton(Button& button, const char* text);
    void UpdateButtonStyles();
    static void OnGraphClick(Button* sender);
    static void OnStatisticsClick(Button* sender);

private:
    SystemClass* _system;
    TraceConfigView _activeView;
    TraceConfigViewChangedHandler _onViewChanged;
    ToolbarButton _graphButton;
    ToolbarButton _statisticsButton;
};

#endif
