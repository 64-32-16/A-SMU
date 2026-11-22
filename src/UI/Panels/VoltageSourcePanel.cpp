#include "VoltageSourcePanel.h"

#include <stdio.h>
#include "../Core/DisplayFormat.h"
#include "../Core/Theme.h"

VoltageSourcePanel::VoltageSourcePanel()
    : Panel(),
      _system(&System)
{
    _readingTextBuffer[0] = '\0';
    _rangeTextBuffer[0] = '\0';
    _sourceTextBuffer[0] = '\0';
    _limitTextBuffer[0] = '\0';

    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    _topDivider.SetColor(Theme::SourceBlue);
    _topDivider.SetThickness(16);

    _titleLabel.SetFont(Theme::FontLabel);
    _titleLabel.SetTextColor(Theme::SourceBlue);
    _titleLabel.SetText("SOURCE");

    _modeLabel.SetFont(Theme::FontLabel);
    _modeLabel.SetTextColor(Theme::SourceBlue);
    _modeLabel.SetText("VOLTAGE");

    _valueLabel.SetFont(Theme::FontValue);
    _valueLabel.SetTextColor(Theme::SourceValueBlue);
    _valueLabel.SetTextOptions(OPT_MONO);

    _measLabel.SetFont(Theme::FontLabel);
    _measLabel.SetTextColor(Theme::SourceBlue);
    _measLabel.SetText("MEAS");
    _measLabel.SetVisible(false);

    _rangeLabel.SetFont(Theme::FontLabel);
    _rangeLabel.SetTextColor(Theme::SourceBlue);
    _rangeLabel.SetText("Range");
    _rangeLabel.SetVisible(false);

    _limitLabel.SetFont(Theme::FontLabel);
    _limitLabel.SetTextColor(Theme::SourceBlue);
    _limitLabel.SetText("Limit");

    _rangeButton.SetFont(Theme::FontButton);
    _rangeButton.SetBackgroundColor(Theme::MainButtonBackground);
    _rangeButton.SetCornerRadius(0);
    _rangeButton.SetTextColor(Theme::MainButtonText);
    _rangeButton.SetTextOptions(OPT_CENTER);

    _sourceModeButton.SetFont(Theme::FontButton);
    _sourceModeButton.SetBackgroundColor(Theme::MainButtonBackground);
    _sourceModeButton.SetCornerRadius(0);
    _sourceModeButton.SetTextColor(Theme::MainButtonText);
    _sourceModeButton.SetTextOptions(OPT_CENTER);
    _sourceModeButton.SetText("Source:\nVOLTS(V)");

    _sourceButton.SetFont(Theme::FontButton);
    _sourceButton.SetBackgroundColor(Theme::MainButtonBackground);
    _sourceButton.SetCornerRadius(0);
    _sourceButton.SetTextColor(Theme::MainButtonText);
    _sourceButton.SetTextOptions(OPT_CENTER);

    _limitButton.SetFont(Theme::FontButton);
    _limitButton.SetBackgroundColor(Theme::MainButtonBackground);
    _limitButton.SetCornerRadius(0);
    _limitButton.SetTextColor(Theme::MainButtonText);
    _limitButton.SetTextOptions(OPT_CENTER);

    AddChild(&_topDivider);
    AddChild(&_titleLabel);
    AddChild(&_modeLabel);
    AddChild(&_valueLabel);
    AddChild(&_measLabel);
    AddChild(&_rangeLabel);
    AddChild(&_limitLabel);
    AddChild(&_rangeButton);
    AddChild(&_sourceModeButton);
    AddChild(&_sourceButton);
    AddChild(&_limitButton);

    UpdateLayout();
    UpdateTexts();
}

VoltageSourcePanel::VoltageSourcePanel(int16_t x, int16_t y, int16_t w, int16_t h)
    : Panel(x, y, w, h),
      _system(&System)
{
    _readingTextBuffer[0] = '\0';
    _rangeTextBuffer[0] = '\0';
    _sourceTextBuffer[0] = '\0';
    _limitTextBuffer[0] = '\0';

    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    _topDivider.SetColor(Theme::SourceBlue);
    _topDivider.SetThickness(16);

    _titleLabel.SetFont(Theme::FontLabel);
    _titleLabel.SetTextColor(Theme::SourceBlue);
    _titleLabel.SetText("SOURCE");

    _modeLabel.SetFont(Theme::FontLabel);
    _modeLabel.SetTextColor(Theme::SourceBlue);
    _modeLabel.SetText("VOLTAGE");

    _valueLabel.SetFont(Theme::FontValue);
    _valueLabel.SetTextColor(Theme::SourceValueBlue);
    _valueLabel.SetTextOptions(OPT_MONO);

    _measLabel.SetFont(Theme::FontLabel);
    _measLabel.SetTextColor(Theme::SourceBlue);
    _measLabel.SetText("MEAS");
    _measLabel.SetVisible(false);

    _rangeLabel.SetFont(Theme::FontLabel);
    _rangeLabel.SetTextColor(Theme::SourceBlue);
    _rangeLabel.SetText("Range");
    _rangeLabel.SetVisible(false);

    _limitLabel.SetFont(Theme::FontLabel);
    _limitLabel.SetTextColor(Theme::SourceBlue);
    _limitLabel.SetText("Limit");

    _rangeButton.SetFont(Theme::FontButton);
    _rangeButton.SetBackgroundColor(Theme::MainButtonBackground);
    _rangeButton.SetCornerRadius(0);
    _rangeButton.SetTextColor(Theme::MainButtonText);
    _rangeButton.SetTextOptions(OPT_CENTER);

    _sourceModeButton.SetFont(Theme::FontButton);
    _sourceModeButton.SetBackgroundColor(Theme::MainButtonBackground);
    _sourceModeButton.SetCornerRadius(0);
    _sourceModeButton.SetTextColor(Theme::MainButtonText);
    _sourceModeButton.SetTextOptions(OPT_CENTER);
    _sourceModeButton.SetText("Source:\nVOLTS(V)");

    _sourceButton.SetFont(Theme::FontButton);
    _sourceButton.SetBackgroundColor(Theme::MainButtonBackground);
    _sourceButton.SetCornerRadius(0);
    _sourceButton.SetTextColor(Theme::MainButtonText);
    _sourceButton.SetTextOptions(OPT_CENTER);

    _limitButton.SetFont(Theme::FontButton);
    _limitButton.SetBackgroundColor(Theme::MainButtonBackground);
    _limitButton.SetCornerRadius(0);
    _limitButton.SetTextColor(Theme::MainButtonText);
    _limitButton.SetTextOptions(OPT_CENTER);

    AddChild(&_topDivider);
    AddChild(&_titleLabel);
    AddChild(&_modeLabel);
    AddChild(&_valueLabel);
    AddChild(&_measLabel);
    AddChild(&_rangeLabel);
    AddChild(&_limitLabel);
    AddChild(&_rangeButton);
    AddChild(&_sourceModeButton);
    AddChild(&_sourceButton);
    AddChild(&_limitButton);

    UpdateLayout();
    UpdateTexts();
}

void VoltageSourcePanel::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
    UpdateTexts();
}

SystemClass* VoltageSourcePanel::GetSystem() const
{
    return _system;
}

Button& VoltageSourcePanel::GetRangeButton()
{
    return _rangeButton;
}

Button& VoltageSourcePanel::GetSourceModeButton()
{
    return _sourceModeButton;
}

Button& VoltageSourcePanel::GetSourceButton()
{
    return _sourceButton;
}

Button& VoltageSourcePanel::GetLimitButton()
{
    return _limitButton;
}

void VoltageSourcePanel::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateLayout();
    UpdateTexts();
    GD.cmd_fgcolor(Theme::ButtonGray);
    Panel::Draw();
}

void VoltageSourcePanel::UpdateLayout()
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t w = GetWidth();
    const int16_t controlsY = y + 140;
    _topDivider.SetVisible(false);
    _measLabel.SetVisible(false);
    _titleLabel.SetBounds(x + 12, y + 8, 130, 28);
    _modeLabel.SetBounds(x + 142, y + 8, 180, 28);
    _valueLabel.SetBounds(x + 20, y + 42, w - 40, 90);
    _sourceModeButton.SetBounds(x + w - 170, y + 2, 158, 47);
    _rangeLabel.SetVisible(true);
    _rangeLabel.SetBounds(x + 12, controlsY + 12, 70, 24);
    _rangeButton.SetBounds(x + 86, controlsY, 134, 47);
    _sourceButton.SetBounds(x + 252, controlsY, 190, 47);
    _limitLabel.SetBounds(x + 466, controlsY + 12, 64, 24);
    _limitButton.SetBounds(x + 542, controlsY, w - 554, 47);
}

void VoltageSourcePanel::UpdateTexts()
{
    if (_system == nullptr)
    {
        _valueLabel.SetText("+0.0000 V");
        _rangeButton.SetText("--");
        _sourceButton.SetText("--");
        _limitButton.SetText("--");
        return;
    }

    char valueText[32];
    char unitText[16];

    DisplayFormat::FormatFixedValue(DisplayQuantity::Voltage, _system->GetVoltage(),
                                    _measFormatState,
                                    true,
                                    valueText, sizeof(valueText), unitText, sizeof(unitText));
    snprintf(_readingTextBuffer, sizeof(_readingTextBuffer), "%s %s", valueText, unitText);

    DisplayFormat::FormatValue(DisplayQuantity::Voltage, _system->GetVoltageSourceValue(),
                               _sourceFormatState,
                               true,
                               valueText, sizeof(valueText), unitText, sizeof(unitText));
    snprintf(_sourceTextBuffer, sizeof(_sourceTextBuffer), "Source\n%s%s", valueText, unitText);

    DisplayFormat::FormatValue(DisplayQuantity::Current, _system->GetCurrentLimitValue(),
                               _limitFormatState,
                               false,
                               valueText, sizeof(valueText), unitText, sizeof(unitText));
    snprintf(_limitTextBuffer, sizeof(_limitTextBuffer), "%s%s", valueText, unitText);

    _valueLabel.SetText(_readingTextBuffer);
    if (_system->GetVoltageRangeMode() == RangeMode::Auto)
    {
        snprintf(_rangeTextBuffer, sizeof(_rangeTextBuffer), "AUTO\n%s", _system->GetVoltageRangeText());
    }
    else
    {
        switch (_system->GetVoltageRange())
        {

            case VoltageRange::Range5V:
                snprintf(_rangeTextBuffer, sizeof(_rangeTextBuffer), "RANGE\n5 V");
                break;

            case VoltageRange::Range30V:
                snprintf(_rangeTextBuffer, sizeof(_rangeTextBuffer), "RANGE\n30 V");
                break;

            default:
                snprintf(_rangeTextBuffer, sizeof(_rangeTextBuffer), "--");
                break;
        }
    }

    _rangeButton.SetText(_rangeTextBuffer);
    _sourceButton.SetText(_sourceTextBuffer);
    _limitButton.SetText(_limitTextBuffer);
}
