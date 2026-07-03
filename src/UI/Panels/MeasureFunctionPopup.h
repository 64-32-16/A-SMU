#ifndef _MEASURE_FUNCTION_POPUP_H_INCLUDED
#define _MEASURE_FUNCTION_POPUP_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../Controls/Label.h"
#include "../../Core/System.h"

class MeasureFunctionPopup;
typedef void (*MeasureFunctionPopupCloseHandler)(MeasureFunctionPopup* sender, bool accepted);

class MeasureFunctionPopup : public Panel
{
public:
    MeasureFunctionPopup();
    MeasureFunctionPopup(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    void Open();
    void Close();
    bool IsOpen() const;
    void SetOnClose(MeasureFunctionPopupCloseHandler handler);

    virtual void Draw() override;
    void UpdateLayout();

private:
    static void OnOptionClick(Button* sender);
    static void OnCancelClick(Button* sender);

    void HandleOptionClick(Button* sender);
    void HandleCancelClick(Button* sender);
    void DrawSelectedButton() const;
    void SyncSelection();
    int16_t GetWindowX() const;
    int16_t GetWindowY() const;
    int16_t GetWindowWidth() const;
    int16_t GetWindowHeight() const;

private:
    SystemClass* _system;
    MeasureFunctionPopupCloseHandler _onClose;

    Label _leftTitleTopLabel;
    Label _leftTitleBottomLabel;
    Label _rightTitleTopLabel;
    Label _rightTitleBottomLabel;
    Button _leftButtons[4];
    Button _rightButtons[4];
    Button _cancelButton;
};

#endif
