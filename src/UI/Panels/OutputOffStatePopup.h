#ifndef _OUTPUT_OFF_STATE_POPUP_H_INCLUDED
#define _OUTPUT_OFF_STATE_POPUP_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../Controls/Label.h"
#include "../../Core/System.h"

class OutputOffStatePopup;
typedef void (*OutputOffStatePopupCloseHandler)(OutputOffStatePopup* sender, bool accepted);

class OutputOffStatePopup : public Panel
{
public:
    OutputOffStatePopup();
    OutputOffStatePopup(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    void Open();
    void Close();
    bool IsOpen() const;
    void SetOnClose(OutputOffStatePopupCloseHandler handler);

    virtual void Draw() override;
    void UpdateLayout();

private:
    Button* GetSelectedButton() const;
    void DrawSelectedButton() const;
    static void OnOptionClick(Button* sender);
    static void OnCancelClick(Button* sender);

    void HandleOptionClick(Button* sender);
    void HandleCancelClick(Button* sender);
    int16_t GetWindowX() const;
    int16_t GetWindowY() const;
    int16_t GetWindowWidth() const;
    int16_t GetWindowHeight() const;

private:
    SystemClass* _system;
    OutputOffStatePopupCloseHandler _onClose;

    Label _titleLabel;
    Button _highZButton;
    Button _normalButton;
    Button _zeroButton;
    Button _cancelButton;
};

#endif
