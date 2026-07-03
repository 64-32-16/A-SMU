#ifndef _MEASURE_MODE_POPUP_H_INCLUDED
#define _MEASURE_MODE_POPUP_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../Controls/Label.h"
#include "../../Core/System.h"

class MeasureModePopup;
typedef void (*MeasureModePopupCloseHandler)(MeasureModePopup* sender, bool accepted);

class MeasureModePopup : public Panel
{
public:
    MeasureModePopup();
    MeasureModePopup(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    void Open();
    void Close();
    bool IsOpen() const;
    void SetOnClose(MeasureModePopupCloseHandler handler);

    virtual void Draw() override;
    void UpdateLayout();

private:
    Button* GetSelectedButton() const;
    void DrawSelectedButton() const;
    int16_t GetWindowX() const;
    int16_t GetWindowY() const;
    int16_t GetWindowWidth() const;
    int16_t GetWindowHeight() const;
    static void OnOptionClick(Button* sender);
    static void OnCancelClick(Button* sender);

    void HandleOptionClick(Button* sender);
    void HandleCancelClick(Button* sender);

private:
    SystemClass* _system;
    MeasureModePopupCloseHandler _onClose;

    Label _titleLabel;
    Button _currentButton;
    Button _voltageButton;
    Button _powerButton;
    Button _resistanceButton;
    Button _cancelButton;
};

#endif
