#ifndef _UI_THEME_H_INCLUDED
#define _UI_THEME_H_INCLUDED

#include <Arduino.h>

class Theme
{
public:
    static constexpr uint32_t MainScreenBackground = 0x000000;
    static constexpr uint32_t PanelBackground = 0x101820;
    static constexpr uint32_t PanelBorder = 0x2F4354;
    static constexpr uint32_t ScreenFrame = 0xFF4040;

    static constexpr uint32_t TabActive = 0x1086A8;
    static constexpr uint32_t TabInactive = 0x124858;
    static constexpr uint32_t TabPressed = 0x26B6D4;
    static constexpr uint32_t ToolbarText = 0xF2F2F2;
    static constexpr uint32_t ToolbarAccent = 0x3FCF72;
    static constexpr uint32_t ToolbarButtonBackground = 0x1B2836;
    static constexpr uint32_t FlatToolbarBackground = 0x404040;
    static constexpr uint32_t ToolbarButtonPressed = 0x606060;
    static constexpr uint32_t ToolbarButtonSeparator = 0x202020;
    static constexpr int16_t ToolbarHeight = 60;
    static constexpr uint32_t ToolbarButtonText = ToolbarText;
    static constexpr uint32_t StatusMuted = 0x45525E;
    static constexpr uint32_t ErrorText = 0xFF000E;

    static constexpr uint32_t MeasureLabel = 0x3FCF72;
    static constexpr uint32_t MeasureValue = 0x67F5A6;
    static constexpr uint32_t SourceLabel = 0x2EA8E6;
    static constexpr uint32_t SourceValue = 0x52D0FF;
    static constexpr uint32_t SourcePanelHeaderBackground = 0x1B2836;
    static constexpr uint32_t SourcePanelHeaderText = ToolbarText;

    static constexpr uint32_t MainButtonBackground = 0x7A7A7A;
    static constexpr uint16_t MainButtonRadius = 6;
    static constexpr uint32_t MainButtonText = 0x000000;

    static constexpr uint32_t ButtonBackground = 0x565656;
    static constexpr uint32_t ButtonBackgroundDisabled = 0x404040;
    static constexpr uint32_t ButtonTextColor = ToolbarText;
    static constexpr uint32_t ButtonTextDisabledColor = 0x404040;
    static constexpr uint32_t BooleanOnTrack = 0x237A45;
    static constexpr uint32_t BooleanOffTrack = 0x2F4354;
    static constexpr uint32_t BooleanThumb = 0xF2F2F2;
    static constexpr uint32_t BooleanThumbPressed = 0xD8E0E8;
    static constexpr uint32_t BooleanOnText = ToolbarText;
    static constexpr uint32_t BooleanOffText = 0xC8D0D8;

    static constexpr uint32_t PopupBackdrop = 0x101010;
    static constexpr uint32_t PopupBackground = 0x1A3857;
    static constexpr uint32_t PopupBorder = 0x808080;
    static constexpr uint32_t SelectionFrame = 0xF2EA0A;

    static constexpr uint32_t Background = MainScreenBackground;
    static constexpr uint32_t HeaderText = ToolbarText;
    static constexpr uint32_t LabelGreen = MeasureLabel;
    static constexpr uint32_t ValueGreen = MeasureValue;
    static constexpr uint32_t SourceBlue = SourceLabel;
    static constexpr uint32_t SourceValueBlue = SourceValue;
    static constexpr uint32_t ButtonGray = ButtonBackground;
    static constexpr uint32_t ButtonGrayDisabled = ButtonBackgroundDisabled;
    static constexpr uint32_t ButtonText = ButtonTextColor;
    static constexpr uint32_t ButtonTextDisabled = ButtonTextDisabledColor;
    static constexpr uint32_t TextMuted = StatusMuted;
    static constexpr uint32_t FocusYellow = SelectionFrame;
    static constexpr uint32_t ErrorRed = ErrorText;

    static constexpr uint8_t FontHeader = 28;
    static constexpr uint8_t FontLabel = 28;
    static constexpr uint8_t FontButton = 28;
    static constexpr int16_t NumericPopupWidth = 600;
    static constexpr int16_t NumericPopupHeight = 430;
    static constexpr uint8_t FontPopupButton = 27;
    static constexpr uint8_t FontPopupTitle = 29;
    static constexpr uint8_t FontPopupInput = 31;
    static constexpr uint8_t FontValue = 2;

    static constexpr int16_t ButtonHeight = 47;
    static constexpr int16_t HeaderTopPadding = 12;
    static constexpr int16_t HeaderRowY = 12;
    static constexpr int16_t PanelGap = 10;
    static constexpr int16_t SourceHeaderHeight = 22;
    static constexpr int16_t PanelValueTopY = 28;
    static constexpr int16_t PanelValueBlockHeight = 84;
    static constexpr int16_t PanelControlsTopY = 136;
};

#endif
