#ifndef _SOURCE_PANEL_H_INCLUDED
#define _SOURCE_PANEL_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../../Core/System.h"
#include "CurrentSourcePanel.h"
#include "GraphPanel.h"
#include "MenuPanel.h"
#include "StatisticsPanel.h"
#include "VoltageSourcePanel.h"

class SourcePanel;
typedef void (*SourcePanelExpandHandler)(SourcePanel* sender, bool expanded);

class SourcePanel : public Panel
{
public:
    SourcePanel();
    SourcePanel(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;
    void SetExpanded(bool expanded);
    bool IsExpanded() const;
    void SetOnExpandChanged(SourcePanelExpandHandler handler);

    VoltageSourcePanel& GetVoltagePanel();
    CurrentSourcePanel& GetCurrentPanel();
    StatisticsPanel& GetStatisticsPanel();
    GraphPanel& GetGraphPanel();
    MenuPanel& GetMenuPanel();

    virtual void Draw() override;
    virtual void TouchStart(int16_t x, int16_t y) override;
    virtual void TouchMove(int16_t x, int16_t y) override;
    virtual void TouchEnd(int16_t x, int16_t y) override;
    void UpdateLayout();

private:
    void UpdateSourcePage();

private:
    SystemClass* _system;
    bool _expanded;
    SourcePanelExpandHandler _onExpandChanged;

    VoltageSourcePanel _voltagePanel;
    CurrentSourcePanel _currentPanel;
    StatisticsPanel _statisticsPanel;
    GraphPanel _graphPanel;
    MenuPanel _menuPanel;
};

#endif
