#include "SourcePanel.h"

#include "../Core/Theme.h"

SourcePanel::SourcePanel()
    : Panel(),
      _system(&System),
      _expanded(false),
      _onExpandChanged(nullptr)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);
    AddChild(&_voltagePanel);
    AddChild(&_currentPanel);
    AddChild(&_statisticsPanel);
    AddChild(&_graphPanel);
    AddChild(&_menuPanel);
    _statisticsPanel.SetVisible(false);
    _graphPanel.SetVisible(false);
    _menuPanel.SetVisible(false);
    _currentPanel.SetVisible(false);

    UpdateLayout();
    UpdateSourcePage();
}

SourcePanel::SourcePanel(int16_t x, int16_t y, int16_t w, int16_t h)
    : Panel(x, y, w, h),
      _system(&System),
      _expanded(false),
      _onExpandChanged(nullptr)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);
    AddChild(&_voltagePanel);
    AddChild(&_currentPanel);
    AddChild(&_statisticsPanel);
    AddChild(&_graphPanel);
    AddChild(&_menuPanel);
    _statisticsPanel.SetVisible(false);
    _graphPanel.SetVisible(false);
    _menuPanel.SetVisible(false);
    _currentPanel.SetVisible(false);

    UpdateLayout();
    UpdateSourcePage();
}

void SourcePanel::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
    _voltagePanel.SetSystem(_system);
    _currentPanel.SetSystem(_system);
    _statisticsPanel.SetSystem(_system);
    _graphPanel.SetSystem(_system);
    _menuPanel.SetSystem(_system);
    UpdateSourcePage();
}

SystemClass* SourcePanel::GetSystem() const
{
    return _system;
}

void SourcePanel::SetExpanded(bool expanded)
{
    _expanded = expanded;
}

bool SourcePanel::IsExpanded() const
{
    return _expanded;
}

void SourcePanel::SetOnExpandChanged(SourcePanelExpandHandler handler)
{
    _onExpandChanged = handler;
}

VoltageSourcePanel& SourcePanel::GetVoltagePanel()
{
    return _voltagePanel;
}

CurrentSourcePanel& SourcePanel::GetCurrentPanel()
{
    return _currentPanel;
}

StatisticsPanel& SourcePanel::GetStatisticsPanel()
{
    return _statisticsPanel;
}

GraphPanel& SourcePanel::GetGraphPanel()
{
    return _graphPanel;
}

MenuPanel& SourcePanel::GetMenuPanel()
{
    return _menuPanel;
}

void SourcePanel::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateSourcePage();
    UpdateLayout();
    Panel::Draw();
}

void SourcePanel::TouchStart(int16_t x, int16_t y)
{
    if (!IsVisible() || !IsEnabled())
    {
        return;
    }

    Panel::TouchStart(x, y);
}

void SourcePanel::TouchMove(int16_t x, int16_t y)
{
    if (!IsVisible() || !IsEnabled())
    {
        return;
    }

    Panel::TouchMove(x, y);
}

void SourcePanel::TouchEnd(int16_t x, int16_t y)
{
    if (!IsVisible() || !IsEnabled())
    {
        return;
    }

    Panel::TouchEnd(x, y);
}

void SourcePanel::UpdateLayout()
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t w = GetWidth();
    const int16_t h = GetHeight();

    _voltagePanel.SetBounds(x, y, w, h);
    _currentPanel.SetBounds(x, y, w, h);
}

void SourcePanel::UpdateSourcePage()
{
    const SourceMode mode = (_system != nullptr) ? _system->GetSourceMode() : SourceMode::Voltage;

    _voltagePanel.SetVisible(mode == SourceMode::Voltage);
    _currentPanel.SetVisible(mode == SourceMode::Current);
    _statisticsPanel.SetVisible(false);
    _graphPanel.SetVisible(false);
    _menuPanel.SetVisible(false);
}
