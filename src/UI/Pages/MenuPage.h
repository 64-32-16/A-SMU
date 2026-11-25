#ifndef _MENU_PAGE_H_INCLUDED
#define _MENU_PAGE_H_INCLUDED

#include "../Core/Panel.h"
#include "../Controls/Button.h"

// Full-screen menu and its function placeholder views. No domain settings are changed.
class MenuPage : public Panel
{
public:
    MenuPage(int16_t x, int16_t y, int16_t w, int16_t h);
    void Open();
    void SetOnInfo(ButtonClickHandler h) {_onInfo=h;}
    void SetOnSystemSettings(ButtonClickHandler h) {_onSystemSettings=h;}
    void SetOnSourceSettings(ButtonClickHandler h) {_onSourceSettings=h;}
    void SetOnMeasureSettings(ButtonClickHandler h) {_onMeasureSettings=h;}
    void SetOnSimulation(ButtonClickHandler handler) {_onSimulation=handler;}
    void SetOnHome(ButtonClickHandler handler);
    void SetOnGraph(ButtonClickHandler handler);
    void SetOnHistogram(ButtonClickHandler handler);
    void SetOnReadingTable(ButtonClickHandler handler);
    void Draw() override;
    void TouchEnd(int16_t x, int16_t y) override;
    void UpdateLayout();

private:
    void UpdateVisibility();
    void OpenFunction(uint8_t group, uint8_t item);
    ButtonClickHandler _onInfo=nullptr;
    ButtonClickHandler _onSourceSettings=nullptr;
    ButtonClickHandler _onMeasureSettings=nullptr;
    ButtonClickHandler _onSystemSettings=nullptr;
    Button _simulation;
    ButtonClickHandler _onSimulation=nullptr;
    Button _home;
    Button _back;
    Button _items[6][5];
    ButtonClickHandler _onGraph = nullptr;
    ButtonClickHandler _onHistogram = nullptr;
    ButtonClickHandler _onReadingTable = nullptr;
    bool _placeholder = false;
    char _title[80] = {};
};

#endif
