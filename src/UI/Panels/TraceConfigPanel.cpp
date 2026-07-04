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

    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    _topDivider.SetColor(Theme::SourceBlue);
    _topDivider.SetThickness(16);

    SetupButton(_graphButton, "GRAPH");
    SetupButton(_statisticsButton, "STATISTICS");

    _graphButton.SetOnClick(OnGraphClick);
    _statisticsButton.SetOnClick(OnStatisticsClick);

    _outputSwitch.SetCaption("");
    _outputSwitch.SetValue(_system->GetOutputOn());
    _outputSwitch.SetOnChange(OnOutputSwitchChanged);

    AddChild(&_topDivider);
    AddChild(&_graphButton);
    AddChild(&_statisticsButton);
    AddChild(&_outputSwitch);

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
    _outputSwitch.SetValue(_system->GetOutputOn());
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

BooleanButton& TraceConfigPanel::GetOutputSwitch()
{
    return _outputSwitch;
}

void TraceConfigPanel::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateLayout();
    _outputSwitch.SetValue(_system->GetOutputOn());
    Panel::Draw();
}

void TraceConfigPanel::UpdateLayout()
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t w = GetWidth();
    const int16_t buttonY = y + 18;
    const int16_t buttonH = Theme::ButtonHeight;
    const int16_t outputSwitchH = Theme::ButtonHeight - 10;
    const int16_t outputSwitchY = buttonY + ((buttonH - outputSwitchH) / 2);
    const int16_t outputSwitchW = 104;
    const int16_t graphButtonW = 132;
    const int16_t statisticsButtonW = 178;
    const int16_t gap = 10;

    _topDivider.SetBounds(x, y + 3, w, 1);
    _graphButton.SetBounds(x + 8, buttonY, graphButtonW, buttonH);
    _statisticsButton.SetBounds(x + 8 + graphButtonW + gap, buttonY, statisticsButtonW, buttonH);
    _outputSwitch.SetBounds(x + w - outputSwitchW - 8, outputSwitchY, outputSwitchW, outputSwitchH);
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
        _activeView == TraceConfigView::Graph ? Theme::SourceBlue : Theme::ToolbarButtonBackground);
    _statisticsButton.SetBackgroundColor(
        _activeView == TraceConfigView::Statistics ? Theme::SourceBlue : Theme::ToolbarButtonBackground);
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

void TraceConfigPanel::OnOutputSwitchChanged(BooleanButton* sender, bool value)
{
    if (g_traceConfigPanel == nullptr || sender != &g_traceConfigPanel->GetOutputSwitch())
    {
        return;
    }

    g_traceConfigPanel->GetSystem()->SetOutputOn(value);
}
