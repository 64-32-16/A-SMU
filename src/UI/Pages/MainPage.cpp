#include "MainPage.h"

#include "../Core/Theme.h"

MainPage::MainPage()
    : Panel(),
      _system(&System)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    AddChild(&_headerPanel);
    AddChild(&_sourcePanel);
    AddChild(&_measurePanel);
    AddChild(&_configPanel);

    UpdateLayout();
}

MainPage::MainPage(int16_t x, int16_t y, int16_t w, int16_t h)
    : MainPage()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void MainPage::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
    _headerPanel.SetSystem(_system);
    _sourcePanel.SetSystem(_system);
    _measurePanel.SetSystem(_system);
    _configPanel.SetSystem(_system);
}

SystemClass* MainPage::GetSystem() const
{
    return _system;
}

SourcePanel& MainPage::GetSourcePanel()
{
    return _sourcePanel;
}

MeasurePanel& MainPage::GetMeasurePanel()
{
    return _measurePanel;
}

MainConfigPanel& MainPage::GetConfigPanel()
{
    return _configPanel;
}

HeaderPanel& MainPage::GetHeaderPanel()
{
    return _headerPanel;
}

void MainPage::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateLayout();
    Panel::Draw();
}

void MainPage::UpdateLayout()
{
    const int16_t left = GetX() + 20;
    const int16_t top = GetY() + 4;
    const int16_t width = GetWidth() - 40;
    const int16_t headerHeight = 54;
    const int16_t configHeight = 78;
    const int16_t configY = GetY() + GetHeight() - configHeight - 4;
    const int16_t sourceY = top + headerHeight - 1;
    const int16_t contentHeight = configY - sourceY;
    const int16_t sourceHeight = 170;
    const int16_t measureY = sourceY + sourceHeight + Theme::PanelGap;
    const int16_t measureHeight = contentHeight - sourceHeight - Theme::PanelGap;

    _headerPanel.SetBounds(left, top, width, headerHeight);
    _sourcePanel.SetBounds(left, sourceY, width, sourceHeight);
    _sourcePanel.SetExpanded(true);
    _measurePanel.SetBounds(left, measureY, width, measureHeight);
    _configPanel.SetBounds(left, configY, width, configHeight);
}
