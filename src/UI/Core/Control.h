#ifndef _CONTROL_H_INCLUDED
#define _CONTROL_H_INCLUDED

#include <Arduino.h>

/**
 * @brief Basisklasse für alle GUI-Elemente.
 *
 * Control kapselt die gemeinsame Grundfunktionalität aller sichtbaren und
 * interaktiven GUI-Elemente wie Button, Label, Panel oder Container.
 *
 * Die Klasse enthält bewusst nur:
 * - Geometrie
 * - Sichtbarkeit
 * - Enabled-Zustand
 * - Hit-Test
 * - leere virtuelle Touch-Methoden
 *
 * Child-Verwaltung und zusammengesetzte Controls folgen später in Container.
 */
class Control
{
public:
    /**
     * @brief Erstellt ein leeres Control mit Defaultwerten.
     */
    Control();

    /**
     * @brief Erstellt ein Control mit Position und Größe.
     *
     * @param x X-Position
     * @param y Y-Position
     * @param w Breite
     * @param h Höhe
     */
    Control(int16_t x, int16_t y, int16_t w, int16_t h);

    virtual ~Control() = default;

    /**
     * @brief Zeichnet das Control.
     *
     * Muss in abgeleiteten Klassen überschrieben werden.
     */
    virtual void Draw() = 0;

    /**
     * @brief Wird aufgerufen, wenn ein Touch beginnt.
     *
     * Die Basisklasse macht hier standardmäßig nichts.
     *
     * @param x Touch-X-Koordinate
     * @param y Touch-Y-Koordinate
     */
    virtual void TouchStart(int16_t x, int16_t y);

    /**
     * @brief Wird aufgerufen, wenn sich ein Touch bewegt.
     *
     * Die Basisklasse macht hier standardmäßig nichts.
     *
     * @param x Touch-X-Koordinate
     * @param y Touch-Y-Koordinate
     */
    virtual void TouchMove(int16_t x, int16_t y);

    /**
     * @brief Wird aufgerufen, wenn ein Touch endet.
     *
     * Die Basisklasse macht hier standardmäßig nichts.
     *
     * @param x Touch-X-Koordinate
     * @param y Touch-Y-Koordinate
     */
    virtual void TouchEnd(int16_t x, int16_t y);

    virtual bool IsFocusable() const;
    virtual void SetFocused(bool focused);
    bool IsFocused() const;

    /**
     * @brief Prüft, ob ein Punkt innerhalb des Controls liegt.
     *
     * @param px Punkt X
     * @param py Punkt Y
     * @return true, wenn der Punkt innerhalb des Controls liegt
     */
    bool Contains(int16_t px, int16_t py) const;

    /**
     * @brief Setzt Position und Größe des Controls.
     *
     * @param x X-Position
     * @param y Y-Position
     * @param w Breite
     * @param h Höhe
     */
    void SetBounds(int16_t x, int16_t y, int16_t w, int16_t h);

    /**
     * @brief Setzt die Position des Controls.
     *
     * @param x X-Position
     * @param y Y-Position
     */
    void SetPosition(int16_t x, int16_t y);

    /**
     * @brief Setzt die Größe des Controls.
     *
     * @param w Breite
     * @param h Höhe
     */
    void SetSize(int16_t w, int16_t h);

    /**
     * @brief Setzt die X-Position.
     *
     * @param x X-Position
     */
    void SetX(int16_t x);

    /**
     * @brief Setzt die Y-Position.
     *
     * @param y Y-Position
     */
    void SetY(int16_t y);

    /**
     * @brief Setzt die Breite.
     *
     * @param w Breite
     */
    void SetWidth(int16_t w);

    /**
     * @brief Setzt die Höhe.
     *
     * @param h Höhe
     */
    void SetHeight(int16_t h);

    /**
     * @brief Aktiviert oder deaktiviert das Control.
     *
     * @param enabled Neuer Aktiv-Zustand
     */
    void SetEnabled(bool enabled);

    /**
     * @brief Blendet das Control ein oder aus.
     *
     * @param visible Neuer Sichtbarkeits-Zustand
     */
    void SetVisible(bool visible);

    /**
     * @brief Liefert die X-Position.
     */
    int16_t GetX() const;

    /**
     * @brief Liefert die Y-Position.
     */
    int16_t GetY() const;

    /**
     * @brief Liefert die Breite.
     */
    int16_t GetWidth() const;

    /**
     * @brief Liefert die Höhe.
     */
    int16_t GetHeight() const;

    /**
     * @brief Liefert den Aktiv-Zustand.
     */
    bool IsEnabled() const;

    /**
     * @brief Liefert den Sichtbarkeits-Zustand.
     */
    bool IsVisible() const;

protected:
    /// X-Position des Controls
    int16_t _x;

    /// Y-Position des Controls
    int16_t _y;

    /// Breite des Controls
    int16_t _w;

    /// Höhe des Controls
    int16_t _h;

    /// Sichtbarkeits-Zustand
    bool _visible;

    /// Aktiv-Zustand
    bool _enabled;

    /// Fokus-Zustand
    bool _focused;
};

#endif
