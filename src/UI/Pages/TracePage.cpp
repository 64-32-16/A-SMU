#include "TracePage.h"

#include "../Core/Theme.h"

namespace
{
TracePage* g_tracePage = nullptr;
}

TracePage::TracePage()
    : Panel(),
      _system(&System)
{
    g_tracePage = this;

    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    _configPanel.SetOnViewChanged(OnConfigViewChanged);

    AddChild(&_headerPanel);
    AddChild(&_graphPanel);
    AddChild(&_statisticsPanel);
    AddChild(&_configPanel);

    UpdateVisiblePanel();
    UpdateLayout();
}

TracePage::TracePage(int16_t x, int16_t y, int16_t w, int16_t h)
    : TracePage()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void TracePage::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
    _headerPanel.SetSystem(_system);
    _graphPanel.SetSystem(_system);
    _statisticsPanel.SetSystem(_system);
    _configPanel.SetSystem(_system);
}

SystemClass* TracePage::GetSystem() const
{
    return _system;
}

HeaderPanel& TracePage::GetHeaderPanel()
{
    return _headerPanel;
}

GraphPanel& TracePage::GetGraphPanel()
{
    return _graphPanel;
}

StatisticsPanel& TracePage::GetStatisticsPanel()
{
    return _statisticsPanel;
}

TraceConfigPanel& TracePage::GetConfigPanel()
{
    return _configPanel;
}

void TracePage::SetActiveView(TraceConfigView view)
{
    _configPanel.SetActiveView(view);
    UpdateVisiblePanel();
}

TraceConfigView TracePage::GetActiveView() const
{
    return _configPanel.GetActiveView();
}

void TracePage::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateLayout();
    UpdateVisiblePanel();
    Panel::Draw();
}

void TracePage::UpdateLayout()
{
    const int16_t left = GetX() + 20;
    const int16_t top = GetY() + 4;
    const int16_t width = GetWidth() - 40;
    const int16_t headerHeight = 54;
    const int16_t configHeight = 78;
    const int16_t configY = GetY() + GetHeight() - configHeight - 4;
    const int16_t contentY = top + headerHeight - 1;
    const int16_t contentHeight = configY - contentY - Theme::PanelGap;

    _headerPanel.SetBounds(left, top, width, headerHeight);
    _graphPanel.SetBounds(left, contentY, width, contentHeight);
    _statisticsPanel.SetBounds(left, contentY, width, contentHeight);
    _configPanel.SetBounds(left, configY, width, configHeight);
}

void TracePage::UpdateVisiblePanel()
{
    const TraceConfigView activeView = _configPanel.GetActiveView();
    const bool graphActive = (activeView == TraceConfigView::Graph);
    const bool statisticsActive = (activeView == TraceConfigView::Statistics);

    _graphPanel.SetVisible(graphActive);
    _statisticsPanel.SetVisible(statisticsActive);
}

void TracePage::OnConfigViewChanged(TraceConfigView view)
{
    (void)view;

    if (g_tracePage != nullptr)
    {
        g_tracePage->UpdateVisiblePanel();
    }
}
