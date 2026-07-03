#ifndef _BEEPER_H_INCLUDED
#define _BEEPER_H_INCLUDED

#include <Arduino.h>

/**
 * @brief Einfache nicht-blockierende Beeper-Klasse.
 *
 * Die Klasse schaltet einen aktiven Beeper für eine definierte Zeit ein
 * und beendet den Ton automatisch wieder in Update().
 */
class BeeperClass
{
public:
    /**
     * @brief Erstellt einen neuen Beeper.
     */
    BeeperClass();

    /**
     * @brief Initialisiert den Beeper-Pin.
     *
     * @param pin GPIO-Pin des aktiven Beepers
     * @param activeHigh true = HIGH schaltet ein, false = LOW schaltet ein
     */
    void Begin(uint8_t pin, bool activeHigh = true);

    /**
     * @brief Muss zyklisch aufgerufen werden.
     *
     * Beendet laufende Beeps automatisch, wenn die Dauer abgelaufen ist.
     */
    void Update();

    /**
     * @brief Startet einen Beep mit beliebiger Dauer.
     *
     * @param durationMs Dauer in Millisekunden
     */
    void Beep(uint16_t durationMs);

    /**
     * @brief Standard-Klickton für UI-Bedienung.
     */
    void Click();

    /**
     * @brief Standard-Fehlerton.
     */
    void Error();

    /**
     * @brief Stoppt einen laufenden Beep sofort.
     */
    void Stop();

    /**
     * @brief Prüft, ob der Beeper aktuell aktiv ist.
     */
    bool IsActive() const;

private:
    void SetOutput(bool on);

private:
    uint8_t _pin;
    bool _activeHigh;
    bool _initialized;
    bool _active;
    uint32_t _endTimeMs;
};

extern BeeperClass Beeper;

#endif