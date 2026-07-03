#ifndef _RESISTANCE_RANGE_POPUP_H_INCLUDED
#define _RESISTANCE_RANGE_POPUP_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../Controls/Label.h"
#include "../../Core/System.h"

class ResistanceRangePopup;
typedef void (*ResistanceRangePopupCloseHandler)(ResistanceRangePopup* sender, bool accepted);

class ResistanceRangePopup : public Panel
{
public:
    ResistanceRangePopup();
    ResistanceRangePopup(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    void Open();
    void Close();
    bool IsOpen() const;
    void SetOnClose(ResistanceRangePopupCloseHandler handler);

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
    ResistanceRangePopupCloseHandler _onClose;

    Label _titleLabel;
    Button _autoButton;
    Button _range10Button;
    Button _range100Button;
    Button _range1kButton;
    Button _range100kButton;
    Button _cancelButton;
};

#endif
