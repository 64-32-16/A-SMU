#pragma once
#include "../Core/Panel.h"
#include "../Controls/Button.h"

class InfoPage : public Panel {
public:
    InfoPage(int16_t x,int16_t y,int16_t w,int16_t h);
    void SetOnBack(ButtonClickHandler h) {_back.SetOnClick(h);}
    void SetOnHome(ButtonClickHandler h) {_home.SetOnClick(h);}
    void Open();
    void Draw() override;
private:
    Button _back, _home;
};
