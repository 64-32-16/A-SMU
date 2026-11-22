#include "TraceConfigPanel.h"

#include "../Core/Theme.h"

namespace
{
TraceConfigPanel* g_traceConfigPanel = nullptr;
}

TraceConfigPanel::TraceConfigPanel()
    : Panel(),
      _system(&System),
      _activeView(TraceConfigView::Graph),
      _onViewChanged(nullptr)
{
    g_traceConfigPanel = this;

    SetShowBackground(true);
    SetBackgroundColor(Theme::MainScreenBackground);
    SetShowBorder(false);
    SetShowTitle(false);


    SetupButton(_graphButton, "Graph");
    SetupButton(_statisticsButton, "Statistics");

    _graphButton.SetOnClick(OnGraphClick);
    _statisticsButton.SetOnClick(OnStatisticsClick);


    AddChild(&_graphButton);
    AddChild(&_statisticsButton);

    UpdateButtonStyles();
    UpdateLayout();
}

TraceConfigPanel::TraceConfigPanel(int16_t x, int16_t y, int16_t w, int16_t h)
    : TraceConfigPanel()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void TraceConfigPanel::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

SystemClass* TraceConfigPanel::GetSystem() const
{
    return _system;
}

void TraceConfigPanel::SetActiveView(TraceConfigView view)
{
    if (_activeView == view)
    {
        UpdateButtonStyles();
        return;
    }

    _activeView = view;
    UpdateButtonStyles();

    if (_onViewChanged != nullptr)
    {
        _onViewChanged(_activeView);
    }
}

TraceConfigView TraceConfigPanel::GetActiveView() const
{
    return _activeView;
}

void TraceConfigPanel::SetOnViewChanged(TraceConfigViewChangedHandler handler)
{
    _onViewChanged = handler;
}

Button& TraceConfigPanel::GetGraphButton()
{
    return _graphButton;
}

Button& TraceConfigPanel::GetStatisticsButton()
{
    return _statisticsButton;
}

void TraceConfigPanel::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateLayout();
    GD.SaveContext();
    // Rectangle coordinates are pixels, independent of preceding EVE widgets.
    GD.VertexFormat(0);
    GD.ColorA(255);
    GD.LineWidth(1);
    Panel::Draw();
    GD.RestoreContext();
}

void TraceConfigPanel::UpdateLayout()
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t buttonH = GetHeight();
    const int16_t graphButtonW = 132;
    const int16_t statisticsButtonW = 178;
    const int16_t gap = 4;

    _graphButton.SetBounds(x, y, graphButtonW, buttonH);
    _statisticsButton.SetBounds(x + graphButtonW + gap, y, statisticsButtonW, buttonH);
}

void TraceConfigPanel::SetupButton(Button& button, const char* text)
{
    button.SetFont(Theme::FontButton);
    button.SetTextColor(Theme::ToolbarButtonText);
    button.SetTextOptions(OPT_CENTER);
    button.SetText(text);
}

void TraceConfigPanel::UpdateButtonStyles()
{
    _graphButton.SetBackgroundColor(
        _activeView == TraceConfigView::Graph ? Theme::SourceBlue : Theme::FlatToolbarBackground);
    _statisticsButton.SetBackgroundColor(
        _activeView == TraceConfigView::Statistics ? Theme::SourceBlue : Theme::FlatToolbarBackground);
}

void TraceConfigPanel::OnGraphClick(Button* sender)
{
    if (g_traceConfigPanel == nullptr || sender != &g_traceConfigPanel->GetGraphButton())
    {
        return;
    }

    g_traceConfigPanel->SetActiveView(TraceConfigView::Graph);
}

void TraceConfigPanel::OnStatisticsClick(Button* sender)
{
    if (g_traceConfigPanel == nullptr || sender != &g_traceConfigPanel->GetStatisticsButton())
    {
        return;
    }

    g_traceConfigPanel->SetActiveView(TraceConfigView::Statistics);
}
