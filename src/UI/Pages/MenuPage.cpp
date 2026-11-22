#include "MenuPage.h"

#include <stdio.h>
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
#include "../../Core/System.h"

namespace
{
struct MenuGroup
{
    const char* title;
    bool visible;
    const char* items[5];
};

// Keep all six groups in their original order; enable these flags when needed.
constexpr MenuGroup groups[] = {
    {"SOURCE", true, {"Quickset", "Settings", "Sweep", "Config Lists", nullptr}},
    {"MEASURE", true, {"Settings", "Calculations", "Config Lists", "Reading Buffers", nullptr}},
    {"VIEWS", true, {"Graph", "Histogram", "Reading Table", nullptr, nullptr}},
    {"TRIGGER", false, {"Templates", "Configure", nullptr, nullptr, nullptr}},
    {"SCRIPTS", false, {"Run", "Manage", "Save Setup", "Record", "Apps"}},
    {"SYSTEM", true, {"Event Log", "Communication", "Settings", "Calibration", "Info/Manage"}}
};
constexpr int16_t margin = 16;
constexpr int16_t gap = 8;
constexpr int16_t itemHeight = 44;

uint8_t VisibleGroupCount()
{
    uint8_t count = 0;
    for (const auto& group : groups) if (group.visible) ++count;
    return count;
}

void StyleButton(Button& button, const char* text)
{
    button.SetText(text);
    button.SetFont(Theme::FontPopupButton);
    button.SetBackgroundColor(Theme::MainButtonBackground);
    button.SetTextColor(Theme::MainButtonText);
    button.SetCornerRadius(0);
}
}

MenuPage::MenuPage(int16_t x, int16_t y, int16_t w, int16_t h) : Panel(x, y, w, h)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);
    StyleButton(_back, "BACK");
    StyleButton(_home, "HOME");
    AddChild(&_back);
    AddChild(&_home);
    for (uint8_t g = 0; g < 6; ++g)
    {
        for (uint8_t i = 0; i < 5; ++i)
        {
            StyleButton(_items[g][i], groups[g].items[i] ? groups[g].items[i] : "");
            AddChild(&_items[g][i]);
        }
    }
    StyleButton(_simulation,"Simulation");
    AddChild(&_simulation);
    UpdateLayout();
    UpdateVisibility();
    SetVisible(false);
}

void MenuPage::SetOnHome(ButtonClickHandler handler) { _home.SetOnClick(handler); }

void MenuPage::Open()
{
    // Cancel any old press when entering this page again.
    Panel::TouchEnd(-1, -1);
    _placeholder = false;
    UpdateVisibility();
    UpdateLayout();
    SetVisible(true);
}

void MenuPage::UpdateVisibility()
{
    _back.SetVisible(_placeholder);
    _simulation.SetVisible(!_placeholder);
    for (uint8_t g = 0; g < 6; ++g)
        for (uint8_t i = 0; i < 5; ++i)
            _items[g][i].SetVisible(!_placeholder && groups[g].visible && groups[g].items[i] != nullptr);
}

void MenuPage::UpdateLayout()
{
    _back.SetBounds(GetX() + margin, GetY() + 14, 104, 44);
    _home.SetBounds(GetX() + GetWidth() - margin - 104, GetY() + 14, 104, 44);
    const uint8_t count = VisibleGroupCount();
    const int16_t width = (GetWidth() - 2 * margin - (count - 1) * gap) / count;
    uint8_t column = 0;
    for (uint8_t g = 0; g < 6; ++g)
    {
        if (!groups[g].visible) continue;
        for (uint8_t i = 0; i < 5; ++i)
            _items[g][i].SetBounds(GetX() + margin + column * (width + gap),
                                  GetY() + 112 + i * (itemHeight + gap), width, itemHeight);
        ++column;
    }
    _simulation.SetBounds(_items[5][0].GetX(),GetY()+112+5*(itemHeight+gap),width,itemHeight);
}

void MenuPage::SetOnGraph(ButtonClickHandler handler) { _onGraph=handler; }
void MenuPage::SetOnHistogram(ButtonClickHandler handler) { _onHistogram=handler; }
void MenuPage::SetOnReadingTable(ButtonClickHandler handler) { _onReadingTable=handler; }

void MenuPage::OpenFunction(uint8_t group, uint8_t item)
{
    if(group==2 && item==2 && _onReadingTable) { _onReadingTable(&_items[group][item]); return; }
    if(group==2 && item==1 && _onHistogram) { _onHistogram(&_items[group][item]); return; }
    if(group==2 && item==0 && _onGraph) { _onGraph(&_items[group][item]); return; }
    snprintf(_title, sizeof(_title), "%s - %s", groups[group].title, groups[group].items[item]);
    _placeholder = true;
    UpdateVisibility();
}

void MenuPage::TouchEnd(int16_t x, int16_t y)
{
    if (!IsVisible() || !IsEnabled()) return;
    const bool simulation=_simulation.IsVisible() && _simulation.IsPressed() && _simulation.Contains(x,y);
    const bool back = _back.IsVisible() && _back.IsPressed() && _back.Contains(x, y);
    int selectedGroup = -1;
    int selectedItem = -1;
    for (uint8_t g = 0; g < 6; ++g)
        for (uint8_t i = 0; i < 5; ++i)
            if (_items[g][i].IsVisible() && _items[g][i].IsEnabled() &&
                _items[g][i].IsPressed() && _items[g][i].Contains(x, y))
            {
                selectedGroup = g;
                selectedItem = i;
            }
    // Finish the press (including the existing click beep) before changing views.
    Panel::TouchEnd(x, y);
    if(simulation && _onSimulation) {_onSimulation(&_simulation);return;}
    if (back) Open();
    else if (selectedGroup >= 0) OpenFunction(selectedGroup, selectedItem);
}

void MenuPage::Draw()
{
    if (!IsVisible()) return;
    UpdateLayout();
    GD.ColorRGB(Theme::FocusYellow);
    GD.cmd_text(156,36,26,OPT_CENTERY,System.GetBackendText());
    GD.ColorRGB(Theme::ToolbarText);
    GD.cmd_text(GetX() + GetWidth() / 2, GetY() + 36, Theme::FontHeader, OPT_CENTER,
                _placeholder ? "A-SMU" : "MENU");
    if (_placeholder)
    {
        GD.ColorRGB(Theme::SourceBlue);
        GD.cmd_text(GetX() + 24, GetY() + 112, Theme::FontPopupTitle, 0, _title);
        GD.ColorRGB(Theme::ToolbarText);
        GD.cmd_text(GetX() + 24, GetY() + 170, Theme::FontLabel, 0, "Not implemented yet.");
    }
    else
    {
        for (uint8_t g = 0; g < 6; ++g)
        {
            if (!groups[g].visible) continue;
            GD.ColorRGB(Theme::SourceBlue);
            GD.cmd_text(_items[g][0].GetX() + _items[g][0].GetWidth() / 2,
                        GetY() + 88, Theme::FontLabel, OPT_CENTER, groups[g].title);
        }
    }
    Panel::Draw();
}
