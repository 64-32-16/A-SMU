#ifndef _MAIN_PAGE_H_INCLUDED
#define _MAIN_PAGE_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Panels/HeaderPanel.h"
#include "../Panels/MainConfigPanel.h"
#include "../Panels/SourcePanel.h"
#include "../../Core/System.h"

class MainPage : public Panel
{
public:
    MainPage();
    MainPage(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;

    SourcePanel& GetSourcePanel();
    MainConfigPanel& GetConfigPanel();
    HeaderPanel& GetHeaderPanel();

    virtual void Draw() override;
    void UpdateLayout();

private:
    SystemClass* _system;
    HeaderPanel _headerPanel;
    SourcePanel _sourcePanel;
    MainConfigPanel _configPanel;
};

#endif
