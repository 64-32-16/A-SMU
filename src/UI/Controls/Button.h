#ifndef _BUTTON_H_INCLUDED
#define _BUTTON_H_INCLUDED

#include <Arduino.h>
#include "../Core/GDClass.h"
#include "../Core/Control.h"

class Button;

/**
 * @brief Funktionspointer für Button-Klicks.
 *
 * Der Sender-Button wird mitgegeben.
 */
typedef void (*ButtonClickHandler)(Button* sender);

/**
 * @brief Einfaches klickbares GUI-Control.
 *
 * Der Button zeichnet sich über die bestehende GDClass und behandelt
 * Press/Release intern. Ein Click wird nur dann ausgelöst, wenn der
 * Touch innerhalb des Buttons begonnen und auch innerhalb beendet wurde.
 */
class Button : public Control
{
public:
    /**
     * @brief Erstellt einen leeren Button.
     */
    Button();

    /**
     * @brief Erstellt einen Button mit Position, Größe und Text.
     *
     * @param x X-Position
     * @param y Y-Position
     * @param w Breite
     * @param h Höhe
     * @param text Button-Text
     */
    Button(int16_t x, int16_t y, int16_t w, int16_t h, const char* text);

    /**
     * @brief Zeichnet den Button.
     */
    virtual void Draw() override;
    virtual bool IsFocusable() const override;

    /**
     * @brief Verarbeitet den Start eines Touchs.
     */
    virtual void TouchStart(int16_t x, int16_t y) override;

    /**
     * @brief Verarbeitet die Bewegung eines Touchs.
     */
    virtual void TouchMove(int16_t x, int16_t y) override;

    /**
     * @brief Verarbeitet das Ende eines Touchs.
     */
    virtual void TouchEnd(int16_t x, int16_t y) override;

    /**
     * @brief Setzt den Button-Text.
     */
    void SetText(const char* text);

    /**
     * @brief Liefert den Button-Text.
     */
    const char* GetText() const;

    /**
     * @brief Setzt den Font.
     *
     * @param font EVE-Fontnummer
     */
    void SetFont(uint8_t font);

    /**
     * @brief Liefert den Font.
     */
    uint8_t GetFont() const;

    /**
     * @brief Setzt die Textoptionen.
     *
     * Beispiel: OPT_CENTER
     */
    void SetTextOptions(uint16_t options);

    /**
     * @brief Liefert die Textoptionen.
     */
    uint16_t GetTextOptions() const;

    /**
     * @brief Setzt den Klick-Callback.
     */
    void SetOnClick(ButtonClickHandler handler);

    /**
     * @brief Setzt die Textfarbe im aktiven Zustand.
     */
    void SetTextColor(uint32_t color);

    /**
     * @brief Liefert die Textfarbe im aktiven Zustand.
     */
    uint32_t GetTextColor() const;

    /**
     * @brief Setzt die Hintergrundfarbe im aktiven Zustand.
     */
    void SetBackgroundColor(uint32_t color);

    /**
     * @brief Liefert die Hintergrundfarbe im aktiven Zustand.
     */
    uint32_t GetBackgroundColor() const;

    /**
     * @brief Setzt die Hintergrundfarbe im deaktivierten Zustand.
     */
    void SetDisabledBackgroundColor(uint32_t color);

    /**
     * @brief Liefert die Hintergrundfarbe im deaktivierten Zustand.
     */
    uint32_t GetDisabledBackgroundColor() const;

    /**
     * @brief Setzt die Textfarbe im deaktivierten Zustand.
     */
    void SetDisabledTextColor(uint32_t color);

    /**
     * @brief Liefert die Textfarbe im deaktivierten Zustand.
     */
    uint32_t GetDisabledTextColor() const;

    /**
     * @brief Liefert true, wenn der Button aktuell gedrückt ist.
     */
    bool IsPressed() const;

    /**
     * @brief Aktiviert oder deaktiviert den Beep bei erfolgreichem Click.
     */
    void SetBeepOnClick(bool enabled);

    /**
     * @brief Liefert den aktuellen Beep-On-Click-Zustand.
     */
    bool GetBeepOnClick() const;

private:
    /**
     * @brief Löst den Click aus.
     */
    void Click();

private:
    const char* _text;
    uint8_t _font;
    uint16_t _textOptions;

    bool _pressed;
    bool _trackingTouch;
    bool _beepOnClick;
    uint32_t _backgroundColor;
    uint32_t _disabledBackgroundColor;
    uint32_t _textColor;
    uint32_t _disabledTextColor;

    ButtonClickHandler _onClick;
};

#endif
