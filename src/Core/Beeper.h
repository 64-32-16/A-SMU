#ifndef _BEEPER_H_INCLUDED
#define _BEEPER_H_INCLUDED

#include <Arduino.h>
#include <pico/time.h>

/**
 * @brief Einfache nicht-blockierende Beeper-Klasse.
 *
 * Die Klasse schaltet einen aktiven Beeper für eine definierte Zeit ein
 * und beendet den Ton durch einen Pico-Timer-Interrupt, unabhängig von der GUI.
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
     * @brief Startet einen Beep mit beliebiger Dauer.
     *
     * @param durationMs Dauer in Millisekunden
     */
    void Beep(uint16_t durationMs);

    /**
     * @brief Standard-Klickton für UI-Bedienung.
     */
    void Click();
    void SetClickAllowed(bool (*allowed)()) {_clickAllowed=allowed;}

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
    void OnTimeout();
    static int64_t Alarm(alarm_id_t id, void* context);

private:
    uint8_t _pin;
    bool _activeHigh;
    bool _initialized;
    volatile bool _active;
    alarm_id_t _alarm=0;
    bool (*_clickAllowed)()=nullptr;
};

extern BeeperClass Beeper;

#endif
