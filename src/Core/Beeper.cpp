#include "Beeper.h"
#include "InterruptGuard.h"

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
      _active(false)
{
}

/**
 * @brief Initialisiert den Beeper-Pin.
 */
void BeeperClass::Begin(uint8_t pin, bool activeHigh)
{
    InterruptGuard lock;
    Stop(); // Cancel any previous timer and silence the old pin before reinitializing.
    _pin = pin;
    _activeHigh = activeHigh;
    _initialized = true;
    _active = false;

    pinMode(_pin, OUTPUT);
    SetOutput(false);
}

// Runs in interrupt context: only write the already-initialized GPIO and state.
void BeeperClass::OnTimeout()
{
    _alarm=0;
    SetOutput(false);
    _active=false;
}

int64_t BeeperClass::Alarm(alarm_id_t, void* context)
{
    static_cast<BeeperClass*>(context)->OnTimeout();
    return 0;
}

/**
 * @brief Startet einen Beep mit gegebener Dauer.
 */
void BeeperClass::Beep(uint16_t durationMs)
{
    InterruptGuard lock;
    if (!_initialized) return;
    if(_alarm>0) cancel_alarm(_alarm);
    _alarm=0;
    if(durationMs==0)
    {
        SetOutput(false);_active=false;return;
    }
    SetOutput(true);
    _active=true;
    // A new request restarts the duration. Cancellation and rearming are atomic
    // with respect to the timeout interrupt, so an old timeout cannot cut it short.
    _alarm=add_alarm_in_ms(durationMs,Alarm,this,true);
    // Fail silent if the SDK alarm pool is exhausted.
    if(_alarm<=0) {SetOutput(false);_active=false;_alarm=0;}
}

/**
 * @brief Standard-Klickton.
 */
void BeeperClass::Click()
{
    if(!_clickAllowed || _clickAllowed()) Beep(20);
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
    InterruptGuard lock;
    if(_alarm>0) cancel_alarm(_alarm);
    _alarm=0;
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
