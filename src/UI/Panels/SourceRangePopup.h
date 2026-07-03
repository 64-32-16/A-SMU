#ifndef _SOURCE_RANGE_POPUP_H_INCLUDED
#define _SOURCE_RANGE_POPUP_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../Controls/Label.h"
#include "../../Core/System.h"

class SourceRangePopup;
typedef void (*SourceRangePopupCloseHandler)(SourceRangePopup* sender, bool accepted);

class SourceRangePopup : public Panel
{
public:
    SourceRangePopup();
    SourceRangePopup(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    void OpenForMode(SourceMode mode);
    void Close();
    bool IsOpen() const;
    void SetOnClose(SourceRangePopupCloseHandler handler);

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
    void UpdateTexts();

private:
    SystemClass* _system;
    SourceMode _mode;
    SourceRangePopupCloseHandler _onClose;

    Label _titleLabel;
    Button _autoButton;
    Button _option1Button;
    Button _option2Button;
    Button _cancelButton;
};

#endif
