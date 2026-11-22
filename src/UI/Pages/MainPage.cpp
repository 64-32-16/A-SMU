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
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t w = GetWidth();
    const int16_t splitY = y + GetHeight() / 2;
    _headerPanel.SetBounds(x + 20, y + 4, w - 40, 50);
    _measurePanel.SetBounds(x + 20, y + 54, w - 40, splitY - y - 54);
    _sourcePanel.SetBounds(x, splitY, w, GetHeight() / 2);
}
