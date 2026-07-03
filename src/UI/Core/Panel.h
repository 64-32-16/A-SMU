#ifndef _PANEL_H_INCLUDED
#define _PANEL_H_INCLUDED

#include <Arduino.h>
#include "Container.h"

/**
 * @brief Sichtbarer Container mit Hintergrund, Rahmen und optionalem Titel.
 *
 * Panel ist die erste sichtbare Basisklasse für strukturierte GUI-Bereiche.
 * Es dient später als Grundlage für MeasurePanel, SourcePanel oder andere
 * größere UI-Abschnitte.
 */
class Panel : public Container
{
public:
    /**
     * @brief Erstellt ein leeres Panel.
     */
    Panel();

    /**
     * @brief Erstellt ein Panel mit Position und Größe.
     *
     * @param x X-Position
     * @param y Y-Position
     * @param w Breite
     * @param h Höhe
     */
    Panel(int16_t x, int16_t y, int16_t w, int16_t h);

    /**
     * @brief Zeichnet Panel-Hintergrund, Rahmen, Titel und Kinder.
     */
    virtual void Draw() override;

    /**
     * @brief Setzt den Titel des Panels.
     */
    void SetTitle(const char* title);

    /**
     * @brief Liefert den Titel des Panels.
     */
    const char* GetTitle() const;

    /**
     * @brief Setzt die Hintergrundfarbe.
     */
    void SetBackgroundColor(uint32_t color);

    /**
     * @brief Liefert die Hintergrundfarbe.
     */
    uint32_t GetBackgroundColor() const;

    /**
     * @brief Setzt die Rahmenfarbe.
     */
    void SetBorderColor(uint32_t color);

    /**
     * @brief Liefert die Rahmenfarbe.
     */
    uint32_t GetBorderColor() const;

    /**
     * @brief Setzt die Titelfarbe.
     */
    void SetTitleColor(uint32_t color);

    /**
     * @brief Liefert die Titelfarbe.
     */
    uint32_t GetTitleColor() const;

    /**
     * @brief Setzt den Font für den Titel.
     */
    void SetTitleFont(uint8_t font);

    /**
     * @brief Liefert den Font für den Titel.
     */
    uint8_t GetTitleFont() const;

    /**
     * @brief Aktiviert oder deaktiviert die Hintergrundzeichnung.
     */
    void SetShowBackground(bool show);

    /**
     * @brief Aktiviert oder deaktiviert die Rahmendarstellung.
     */
    void SetShowBorder(bool show);

    /**
     * @brief Aktiviert oder deaktiviert die Titeldarstellung.
     */
    void SetShowTitle(bool show);

    /**
     * @brief Liefert, ob der Hintergrund gezeichnet wird.
     */
    bool GetShowBackground() const;

    /**
     * @brief Liefert, ob der Rahmen gezeichnet wird.
     */
    bool GetShowBorder() const;

    /**
     * @brief Liefert, ob der Titel gezeichnet wird.
     */
    bool GetShowTitle() const;

protected:
    /**
     * @brief Zeichnet nur den Panel-Hintergrund.
     */
    void DrawBackground();

    /**
     * @brief Zeichnet nur den Panel-Rahmen.
     */
    void DrawBorder();

    /**
     * @brief Zeichnet nur den Titel.
     */
    void DrawTitle();

protected:
    const char* _title;

    uint32_t _backgroundColor;
    uint32_t _borderColor;
    uint32_t _titleColor;

    uint8_t _titleFont;

    bool _showBackground;
    bool _showBorder;
    bool _showTitle;
};

#endif