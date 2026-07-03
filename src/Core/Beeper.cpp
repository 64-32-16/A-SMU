#include "Beeper.h"

/**
 * @brief Globale Beeper-Instanz.
 */
BeeperClass Beeper;

/**
 * @brief Erstellt einen neuen Beeper mit Defaultwerten.
 */
BeeperClass::BeeperClass()
    : _pin(255),
      _activeHigh(true),
      _initialized(false),
      _active(false),
      _endTimeMs(0)
{
}

/**
 * @brief Initialisiert den Beeper-Pin.
 */
void BeeperClass::Begin(uint8_t pin, bool activeHigh)
{
    _pin = pin;
    _activeHigh = activeHigh;
    _initialized = true;
    _active = false;
    _endTimeMs = 0;

    pinMode(_pin, OUTPUT);
    SetOutput(false);
}

/**
 * @brief Muss zyklisch aufgerufen werden und beendet abgelaufene Beeps.
 */
void BeeperClass::Update()
{
    if (!_initialized || !_active)
    {
        return;
    }

    const uint32_t now = millis();

    if ((int32_t)(now - _endTimeMs) >= 0)
    {
        Stop();
    }
}

/**
 * @brief Startet einen Beep mit gegebener Dauer.
 */
void BeeperClass::Beep(uint16_t durationMs)
{
    if (!_initialized)
    {
        return;
    }

    SetOutput(true);
    _active = true;
    _endTimeMs = millis() + durationMs;
}

/**
 * @brief Standard-Klickton.
 */
void BeeperClass::Click()
{
    Beep(20);
}

/**
 * @brief Standard-Fehlerton.
 */
void BeeperClass::Error()
{
    Beep(80);
}

/**
 * @brief Stoppt einen laufenden Ton sofort.
 */
void BeeperClass::Stop()
{
    if (!_initialized)
    {
        return;
    }

    SetOutput(false);
    _active = false;
}

/**
 * @brief Liefert true, wenn aktuell ein Ton aktiv ist.
 */
bool BeeperClass::IsActive() const
{
    return _active;
}

/**
 * @brief Schaltet den Ausgang abhängig von activeHigh.
 */
void BeeperClass::SetOutput(bool on)
{
    if (!_initialized)
    {
        return;
    }

    digitalWrite(_pin, (_activeHigh ? on : !on) ? HIGH : LOW);
}