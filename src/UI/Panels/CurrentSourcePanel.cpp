#include "CurrentSourcePanel.h"

#include <stdio.h>
#include "../Core/DisplayFormat.h"
#include "../Core/Theme.h"

CurrentSourcePanel::CurrentSourcePanel()
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
    _modeLabel.SetText("CURRENT");

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
    _rangeButton.SetBackgroundColor(Theme::ButtonBackground);
    _rangeButton.SetTextColor(Theme::ButtonTextColor);
    _rangeButton.SetTextOptions(OPT_CENTER);

    _sourceModeButton.SetFont(Theme::FontButton);
    _sourceModeButton.SetBackgroundColor(Theme::ButtonBackground);
    _sourceModeButton.SetTextColor(Theme::ButtonTextColor);
    _sourceModeButton.SetTextOptions(OPT_CENTER);
    _sourceModeButton.SetText("Source:\nCURRENT(I)");

    _sourceButton.SetFont(Theme::FontButton);
    _sourceButton.SetBackgroundColor(Theme::ButtonBackground);
    _sourceButton.SetTextColor(Theme::ButtonTextColor);
    _sourceButton.SetTextOptions(OPT_CENTER);

    _limitButton.SetFont(Theme::FontButton);
    _limitButton.SetBackgroundColor(Theme::ButtonBackground);
    _limitButton.SetTextColor(Theme::ButtonTextColor);
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

CurrentSourcePanel::CurrentSourcePanel(int16_t x, int16_t y, int16_t w, int16_t h)
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
    _modeLabel.SetText("CURRENT");

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
    _rangeButton.SetBackgroundColor(Theme::ButtonBackground);
    _rangeButton.SetTextColor(Theme::ButtonTextColor);
    _rangeButton.SetTextOptions(OPT_CENTER);

    _sourceModeButton.SetFont(Theme::FontButton);
    _sourceModeButton.SetBackgroundColor(Theme::ButtonBackground);
    _sourceModeButton.SetTextColor(Theme::ButtonTextColor);
    _sourceModeButton.SetTextOptions(OPT_CENTER);
    _sourceModeButton.SetText("Source:\nCURRENT(I)");

    _sourceButton.SetFont(Theme::FontButton);
    _sourceButton.SetBackgroundColor(Theme::ButtonBackground);
    _sourceButton.SetTextColor(Theme::ButtonTextColor);
    _sourceButton.SetTextOptions(OPT_CENTER);

    _limitButton.SetFont(Theme::FontButton);
    _limitButton.SetBackgroundColor(Theme::ButtonBackground);
    _limitButton.SetTextColor(Theme::ButtonTextColor);
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

void CurrentSourcePanel::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
    UpdateTexts();
}

SystemClass* CurrentSourcePanel::GetSystem() const
{
    return _system;
}

Button& CurrentSourcePanel::GetRangeButton()
{
    return _rangeButton;
}

Button& CurrentSourcePanel::GetSourceModeButton()
{
    return _sourceModeButton;
}

Button& CurrentSourcePanel::GetSourceButton()
{
    return _sourceButton;
}

Button& CurrentSourcePanel::GetLimitButton()
{
    return _limitButton;
}

void CurrentSourcePanel::Draw()
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

void CurrentSourcePanel::UpdateLayout()
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t w = GetWidth();
    const int16_t setupButtonX = x + w - 170;
    const int16_t sourceModeButtonX = setupButtonX - 170;
    const int16_t limitLabelX = setupButtonX - 72;
    const int16_t rangeButtonX = x + 176;
    const int16_t controlOffsetY = -10;
    const int16_t topButtonY = y + 34 + controlOffsetY;
    const int16_t limitButtonY = y + 112 + controlOffsetY;
    const int16_t buttonH = Theme::ButtonHeight;
    const int16_t labelOffsetY = (buttonH - 24) / 2;

    _topDivider.SetBounds(x + 8, y + 8, w - 16, 1);
    _titleLabel.SetBounds(x + 14, y + 38 + controlOffsetY, 150, 28);
    _modeLabel.SetBounds(x + 14, y + 66 + controlOffsetY, 150, 28);
    _valueLabel.SetBounds(x + 42, y + 92 + controlOffsetY, 430, 90);
    _measLabel.SetBounds(x + w - 72, y + 160 + controlOffsetY, 60, 24);

    _rangeLabel.SetBounds(rangeButtonX - 88, topButtonY + labelOffsetY, 80, 24);
    _rangeButton.SetBounds(rangeButtonX, topButtonY, 150, buttonH);

    _sourceModeButton.SetBounds(sourceModeButtonX, topButtonY, 138, buttonH);
    _sourceButton.SetBounds(setupButtonX, topButtonY, 154, buttonH);

    _limitLabel.SetBounds(limitLabelX, limitButtonY + labelOffsetY, 64, 24);
    _limitButton.SetBounds(setupButtonX, limitButtonY, 154, buttonH);
}

void CurrentSourcePanel::UpdateTexts()
{
    if (_system == nullptr)
    {
        _valueLabel.SetText("+0.0000 A");
        _rangeButton.SetText("--");
        _sourceButton.SetText("--");
        _limitButton.SetText("--");
        return;
    }

    char valueText[32];
    char unitText[16];

    DisplayFormat::FormatFixedValue(DisplayQuantity::Current, _system->GetCurrent(),
                                    _measFormatState,
                                    true,
                                    valueText, sizeof(valueText), unitText, sizeof(unitText));
    snprintf(_readingTextBuffer, sizeof(_readingTextBuffer), "%s %s", valueText, unitText);

    DisplayFormat::FormatValue(DisplayQuantity::Current, _system->GetCurrentSourceValue(),
                               _sourceFormatState,
                               true,
                               valueText, sizeof(valueText), unitText, sizeof(unitText));
    snprintf(_sourceTextBuffer, sizeof(_sourceTextBuffer), "%s%s", valueText, unitText);

    DisplayFormat::FormatValue(DisplayQuantity::Voltage, _system->GetVoltageLimitValue(),
                               _limitFormatState,
                               false,
                               valueText, sizeof(valueText), unitText, sizeof(unitText));
    snprintf(_limitTextBuffer, sizeof(_limitTextBuffer), "%s%s", valueText, unitText);

    _valueLabel.SetText(_readingTextBuffer);
    if (_system->GetCurrentRangeMode() == RangeMode::Auto)
    {
        snprintf(_rangeTextBuffer, sizeof(_rangeTextBuffer), "AUTO\n%s", _system->GetCurrentRangeText());
    }
    else
    {
        switch (_system->GetCurrentRange())
        {

            case CurrentRange::Range100mA:
                snprintf(_rangeTextBuffer, sizeof(_rangeTextBuffer), "RANGE\n100 mA");
                break;

            case CurrentRange::Range1A:
                snprintf(_rangeTextBuffer, sizeof(_rangeTextBuffer), "RANGE\n1 A");
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
