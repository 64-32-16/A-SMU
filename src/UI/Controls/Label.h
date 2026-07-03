#ifndef _LABEL_H_INCLUDED
#define _LABEL_H_INCLUDED

#include <Arduino.h>
#include "../Core/Control.h"

/**
 * @brief Einfaches Text-Control ohne eigene Touch-Funktion.
 *
 * Label dient zur Darstellung von statischem oder dynamischem Text.
 * Es kann später für Statusanzeigen, Überschriften oder Feldbeschriftungen
 * verwendet werden.
 */
class Label : public Control
{
public:
    /**
     * @brief Erstellt ein leeres Label.
     */
    Label();

    /**
     * @brief Erstellt ein Label mit Position, Größe und Text.
     *
     * @param x X-Position
     * @param y Y-Position
     * @param w Breite
     * @param h Höhe
     * @param text Anzuzeigender Text
     */
    Label(int16_t x, int16_t y, int16_t w, int16_t h, const char* text);

    /**
     * @brief Zeichnet das Label.
     */
    virtual void Draw() override;

    /**
     * @brief Setzt den Label-Text.
     */
    void SetText(const char* text);

    /**
     * @brief Liefert den Label-Text.
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
     * Beispiel: OPT_CENTER, OPT_CENTERX, OPT_CENTERY
     */
    void SetTextOptions(uint16_t options);

    /**
     * @brief Liefert die Textoptionen.
     */
    uint16_t GetTextOptions() const;

    /**
     * @brief Setzt die Textfarbe.
     *
     * @param color RGB-Farbe im Format 0xRRGGBB
     */
    void SetTextColor(uint32_t color);

    /**
     * @brief Liefert die Textfarbe.
     */
    uint32_t GetTextColor() const;

private:
    const char* _text;
    uint8_t _font;
    uint16_t _textOptions;
    uint32_t _textColor;
};

#endif