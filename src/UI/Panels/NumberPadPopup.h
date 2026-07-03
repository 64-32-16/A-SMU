#ifndef _NUMBER_PAD_POPUP_H_INCLUDED
#define _NUMBER_PAD_POPUP_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Core/DisplayFormat.h"
#include "../Controls/Button.h"
#include "../Controls/Label.h"
#include "../../Core/System.h"

enum class NumberPadTargetType : uint8_t
{
    SourceValue,
    LimitValue
};

enum class NumberPadUnitKind : uint8_t
{
    Voltage,
    Current
};

enum class NumberPadUnitScale : uint8_t
{
    Base,
    Milli,
    Micro
};

class NumberPadPopup;
typedef void (*NumberPadPopupCloseHandler)(NumberPadPopup* sender, bool accepted);

class NumberPadPopup : public Panel
{
public:
    NumberPadPopup();
    NumberPadPopup(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    void Open(SourceMode mode, NumberPadTargetType targetType);
    void Close();
    bool IsOpen() const;
    void SetOnClose(NumberPadPopupCloseHandler handler);

    virtual void Draw() override;
    void UpdateLayout();

private:
    Button* GetSelectedUnitButton() const;
    DisplayQuantity GetDisplayQuantity() const;
    int16_t GetWindowX() const;
    int16_t GetWindowY() const;
    int16_t GetWindowWidth() const;
    int16_t GetWindowHeight() const;

    void LoadCurrentValue();
    void SetInputFromDisplayValue(float displayValue);
    uint8_t GetDisplayDecimals(NumberPadUnitScale scale) const;
    void BufferToDisplay();
    void ValidateInput();
    float GetAsFloat() const;
    float GetCurrentValue() const;
    float GetMinValue() const;
    float GetMaxValue() const;
    void ApplyValue(float value);
    bool AllowSigned() const;
    NumberPadUnitKind GetUnitKind() const;
    const char* GetUnitText(NumberPadUnitScale scale) const;
    NumberPadUnitScale GetSuggestedUnitScale(float value) const;
    void SetUnitScale(NumberPadUnitScale scale);

    void AddDigit(char c);
    void AddDecimalPoint();
    void ToggleSign();
    void Backspace();
    void ClearInput();
    void Submit();

    static void OnDigitClick(Button* sender);
    static void OnActionClick(Button* sender);

private:
    SystemClass* _system;
    SourceMode _mode;
    NumberPadTargetType _targetType;
    NumberPadUnitScale _unitScale;
    bool _inputValid;
    NumberPadPopupCloseHandler _onClose;

    char _inputBuffer[32];
    char _displayBuffer[64];
    char _titleBuffer[48];

    Label _titleLabel;
    Label _inputLabel;
    Button _digitButtons[10];
    Button _decimalButton;
    Button _signButton;
    Button _backButton;
    Button _clearButton;
    Button _okButton;
    Button _cancelButton;
    Button _unitBaseButton;
    Button _unitMilliButton;
    Button _unitMicroButton;
};

#endif
