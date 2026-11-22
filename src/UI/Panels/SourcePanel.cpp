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
    : SourcePanel()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
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
    DrawNavigation();
}

void SourcePanel::ShowSource()
{
    Panel::TouchEnd(-1, -1);
    _tracking = false;_graphPanel.CancelTouch();
    _menuVisible = false;
    _view = 0;
    UpdateSourcePage();
}

void SourcePanel::ShowMenu()
{
    Panel::TouchEnd(-1, -1);
    _tracking = false;_graphPanel.CancelTouch();
    _menuVisible = true;
    UpdateSourcePage();
}

void SourcePanel::TouchStart(int16_t x, int16_t y)
{
    if (!IsVisible() || !IsEnabled() || !Contains(x, y)) return;
    _tracking = true;
    _moved = false;
    _touchX = x;
    _touchY = y;
    if (y >= GetY() + 32) Panel::TouchStart(x, y);
}

void SourcePanel::TouchMove(int16_t x, int16_t y)
{
    if (!_tracking) return;
    const int dx = x - _touchX;
    const int dy = y - _touchY;
    if (!_moved && (dx > 12 || dx < -12 || dy > 12 || dy < -12))
    {
        _moved = true;
        // Cancel the original button press before interpreting a swipe.
        Panel::TouchEnd(-1, -1);
    }
    if (!_moved) Panel::TouchMove(x, y);
}

void SourcePanel::TouchEnd(int16_t x, int16_t y)
{
    if(x<0 || y<0) {_tracking=false;_moved=false;Panel::TouchEnd(-1,-1);return;}
    if (!_tracking) return;
    TouchMove(x, y);
    _tracking = false;
    const int dx = x - _touchX;
    const int dy = y - _touchY;
    const int absDx = dx < 0 ? -dx : dx;
    const int absDy = dy < 0 ? -dy : dy;
    if (_moved)
    {
        if (!_menuVisible && absDx >= 40 && absDx > absDy + 24)
        {
            _view = static_cast<uint8_t>((_view + (dx < 0 ? 1 : 2)) % 3);
            UpdateSourcePage();
        }
        return;
    }
    if (_touchY < GetY() + 32 && y < GetY() + 32 && Contains(x, y))
    {
        if (_menuVisible) ShowSource();
        else
        {
            const int first = GetX() + GetWidth() / 2 - 54;
            if (x >= first && x < first + 108)
            {
                _view = static_cast<uint8_t>((x - first) / 36);
                UpdateSourcePage();
            }
        }
        return;
    }
    Panel::TouchEnd(x, y);
}

void SourcePanel::UpdateLayout()
{
    const int16_t x = GetX() + 20;
    const int16_t y = GetY() + 32;
    const int16_t w = GetWidth() - 40;
    const int16_t h = GetHeight() - 32;
    _voltagePanel.SetBounds(x, y, w, h);
    _currentPanel.SetBounds(x, y, w, h);
    _statisticsPanel.SetBounds(x, y, w, h);
    _graphPanel.SetBounds(x, y, w, h);
    _menuPanel.SetBounds(x, y, w, h);
}

void SourcePanel::UpdateSourcePage()
{
    const SourceMode mode = _system->GetSourceMode();
    _voltagePanel.SetVisible(!_menuVisible && _view == 0 && mode == SourceMode::Voltage);
    _currentPanel.SetVisible(!_menuVisible && _view == 0 && mode == SourceMode::Current);
    _statisticsPanel.SetVisible(!_menuVisible && _view == 1);
    _graphPanel.SetVisible(!_menuVisible && _view == 2);
    _menuPanel.SetVisible(_menuVisible);
}

void SourcePanel::DrawNavigation()
{
    static const char* titles[] = {"Source", "Statistics", "Graph"};
    GD.SaveContext();
    GD.cmd_fgcolor(0x313131);
    GD.ColorRGB(Theme::ToolbarText);
    GD.cmd_button(GetX(), GetY(), GetWidth(), 32, Theme::FontLabel, OPT_FLAT, "");
    GD.cmd_text(GetX() + 20, GetY() + 16, Theme::FontLabel, OPT_CENTERY,
                _menuVisible ? "Menu - tap to return" : titles[_view]);
    if (!_menuVisible)
    {
        GD.VertexFormat(0);
        GD.Begin(POINTS);
        GD.PointSize(9 * 16);
        for (uint8_t i = 0; i < 3; ++i)
        {
            GD.ColorRGB(i == _view ? Theme::SourceBlue : 0xAAAAAA);
            GD.Vertex2f(GetX() + GetWidth() / 2 + (static_cast<int>(i) - 1) * 36, GetY() + 16);
        }
    }
    GD.RestoreContext();
}

bool SourcePanel::HandleMultiTouch(const TouchFrame& frame,bool allowStart)
{
    if(!_graphPanel.HandleMultiTouch(frame,allowStart && IsVisible() && IsEnabled())) return false;
    _tracking=false;_moved=false;
    Panel::TouchEnd(-1,-1);
    return true;
}
