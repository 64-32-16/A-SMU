# A-SMU System Rules

## Output-Off Hardware-Verhalten

Der Output-Off-Zustand beschreibt, wie sich die A-SMU elektrisch verhält, wenn der Benutzer den Ausgang ausschaltet.

Output-Off ist nicht gleichbedeutend mit Power-Off oder SMU-Shutdown. Die SMU bleibt eingeschaltet, die Regelung und Messung können weiterhin vorbereitet oder aktiv sein. Der Ausgang wird jedoch in einen definierten sicheren Zustand gebracht.

Es werden drei Output-Off-Modes vorgesehen:

| Mode      | Hardware-Verhalten                                                    | Ziel                          |
| --------- | --------------------------------------------------------------------- | ----------------------------- |
| `Z-HIGH`  | Ausgang wird hochohmig geschaltet, Ausgangsrelais offen, Z-HIGH aktiv | DUT sicher trennen            |
| `ZERO`    | Ausgang bleibt verbunden, Sollwert wird aktiv auf 0 V / 0 A geregelt  | DUT definiert auf Null führen |
| `STANDBY` | Endstufe/Regler deaktiviert, DAC auf Safe-Wert, Ausgang nicht aktiv   | interner Ruhezustand          |

### Z-HIGH

`Z-HIGH` ist der sichere Standardzustand.

Hardware-Verhalten:

- Ausgangsrelais öffnen
- Z-HIGH-Schalter aktivieren
- FORCE-Ausgang hochohmig
- DAC auf sicheren Wert setzen
- Integratoren halten oder zurücksetzen
- keine aktive Energieabgabe an den DUT

Dieser Zustand wird verwendet bei:

- Power-On
- Fault
- Range-Wechsel
- Open-Sense-Fehler
- Benutzer schaltet Output OFF
- SMU-Shutdown

### ZERO

`ZERO` ist ein aktiver Regelzustand.

Hardware-Verhalten:

- Ausgang bleibt mit DUT verbunden
- Regler bleibt aktiv
- Sollwert wird auf 0 gesetzt
- Limits bleiben aktiv
- Ausgang wird kontrolliert gegen 0 V / 0 A geregelt

Wichtig: `ZERO` ist kein vollständig sicherer Trennzustand. Bei unbekanntem DUT, externer Energiequelle oder Fehlerzustand ist `Z-HIGH` zu verwenden.

### STANDBY

`STANDBY` ist ein interner Ruhezustand.

Hardware-Verhalten:

- Endstufe gesperrt
- DAC auf Safe-Wert
- Regler deaktiviert oder Integratoren Reset
- Ausgang nicht aktiv
- Z-HIGH je nach Sicherheitsanforderung aktiv

Für V1 kann `STANDBY` zunächst intern bleiben. Sichtbar für den Benutzer sind zuerst `Z-HIGH` und optional `ZERO`.

## State Ownership

- `SystemClass` is the single source of truth for domain values and settings.
- GUI controls and panels read current domain values from `SystemClass` when displaying them.
- GUI actions write changed domain values through `SystemClass` setters.
- System changes may be triggered by different frontends, such as GUI, SCPI, hardware controls, startup defaults, or automation.
- All frontends must route domain changes through `SystemClass` or system-level APIs that enforce the same rules.
- Domain rules must not live only in GUI callbacks.
- GUI code must not add redundant mirror or pending variables for domain values.
- Local UI state is allowed only for presentation and interaction mechanics, such as formatting, focus, touch handling, text entry, temporary text buffers, and display format state.

## Modes And Ranges

- Source mode is changed through `System.SetSourceMode(...)`.
- Measure mode is changed through `System.SetMeasureMode(...)`.
- Voltage range, current range, and resistance range are owned by `SystemClass`.
- Range buttons display values read from `SystemClass`.
- Range dialogs write changes through the matching `SystemClass` setters.

## Auto Range Mode

- Voltage and current range each have a range mode: `AUTO` or `MANUAL`.
- Voltage and current range each have an actual range value.
- Voltage range values are `5 V` and `30 V`.
- Current range values are `100 mA` and `1 A`.
- When voltage range mode is `AUTO`, `SystemClass` may switch the voltage range value up or down to fit a requested source, limit, or measure value.
- When current range mode is `AUTO`, `SystemClass` may switch the current range value up or down to fit a requested source, limit, or measure value.
- This automatic range switch happens only while the corresponding range mode is `AUTO`.
- In `MANUAL` range mode, requested source, limit, or measure values must not change the selected range.
- Number pad validation and `SystemClass` setters must reject source or limit values outside the currently allowed range.
- In `AUTO` range mode, the allowed source or limit input range is the largest available range for that quantity.
- In `MANUAL` range mode, the allowed source or limit input range is the selected fixed range.

## GUI Dialog Boundaries

- Source mode and measure mode are separate dialogs.
- The source mode dialog changes only source mode.
- The measure mode dialog changes only measure mode.
- Range dialogs change only the range they are responsible for.

## Display Formatting

- Panels may keep local buffers for rendered text.
- Panels may keep local formatting state for stable display formatting.
- These buffers and formatting states are not domain state and must not become an alternate source of truth.

## Persistent Settings

- Persistent settings are owned by the system layer, not by GUI panels.
- Settings persistence must store domain settings from `SystemClass`, such as source mode, measure mode, range modes, ranges, source values, limit values, and related system settings.
- GUI panels must not save or restore persistent domain settings directly.
- Other frontends, such as SCPI, must use the same system-level persistence rules.
- Domain changes mark persistent settings as dirty.
- Dirty settings are saved only after 30 seconds without further persistent setting changes.
- Number pad editing must not mark settings dirty while text is being edited; only accepted values written to `SystemClass` may mark settings dirty.
- Persistent storage should use versioning and validation, such as magic value, format version, and checksum or CRC.
- Invalid, missing, or unsupported persisted settings must fall back to safe defaults.
