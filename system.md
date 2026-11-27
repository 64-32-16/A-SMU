# A-SMU System Rules

## Entwicklungsweg – 19.11.2026

Zuerst die gesamte GUI mit echtem Pico/Display und simulierten SMU-Funktionen fertigstellen. Ziel: GUI ↔ SystemClass ↔ Hardware-Schnittstelle ↔ Simulation / reale Hardware. SystemClass bleibt die einzige Quelle für Domain-Werte. Die bisherige Demo-Messwertsimulation ist noch kein vollständiger Hardware-Layer. Komponenten und offene Unterschiede zwischen Software- und Hardwarebereichen stehen in [Hardware.md](Hardware.md). SPI-Anschlüsse werden direkt aus dem Schaltplan übernommen.


## Verbindliche Mainboard-Basis – 20.11.2026

Nutzerfestlegung: Der Mainboard-Schaltplan ist die feste Basis für Software und Hardware-Simulation; vorerst sind keine Schaltplanänderungen geplant. Maßgeblich sind die zwölf SVG-Blätter vom **10.11.2026** unter `doc/hardware/Main_Board/`, zusammen mit `ProDoc_MAIN_BOARD 2.0_4_2026-11-10.epro2`. Diese Bestandsaufnahme ersetzt widersprechende ältere Auswahlvorschläge aus README/Backlog. Sie ist keine neue elektrische Freigabe. Die GUI darf nur dokumentierte Hardwarefunktionen als vorhanden behandeln.

### Strombereiche und Messpfad (Blatt 6 / 7)

Nutzerkorrektur vom 20.11.2026: Verbindlich sind **±1 A / ±100 mA / ±1 mA / ±10 µA**. Die abweichende Texttabelle auf Blatt 6 (±1 A / ±10 mA / ±100 µA / ±1 µA) ist veraltet und wird nicht als Softwarevorgabe verwendet. Schaltplan und Bauteilwerte bleiben unverändert:

| Bereich, bipolar | Effektiver Shunt | Freigabe | Kelvin-MUX U21 |
|---|---|---|---|
| ±1 A | 1 Ω | I_RANGE_1_EN | S1A / S1B |
| ±100 mA | 10 Ω | I_RANGE_2_EN | S2A / S2B |
| ±1 mA | 1 kΩ | I_RANGE_3_EN | S3A / S3B |
| ±10 µA | 100 kΩ | I_RANGE_4_EN | S4A / S4B |

U21 ist ein ADG1209YRZ. Steuerung über `IMON_MUX_A0`, `IMON_MUX_A1`, `IMON_MUX_EN`. Relais: K6 G2RL-2 DC12 für Kanal 1; U18/U19/U20 HE3621A6012 für Kanäle 2/3/4. Kanalnummern sind Hardwareadressen, keine Software-Enum-Ordnungswerte: Die GUI sortiert aufsteigend, Hardwarekanal 1 ist dagegen der größte Bereich.

Der 1-Ω-Shunt besteht aus R67/R20/R21/R65 (je 1 Ω / 4 W); der 10-Ω-Pfad aus R4/R52/R59/R60; der 1-kΩ-Pfad aus R19/R24/R49/R51; R58 ist mit 100 kΩ beschriftet. „ODER 1M Ohm“, „100mR“ und ältere Bereichsangaben werden nicht als zusätzlicher auswählbarer Bereich übernommen. Die Leistungsangaben älterer Backlogtabellen sind keine aktuelle Stückliste.

I_MON: U6 OPA4192ID, Eingangspuffer für IM− und IM+, anschließender Differenzverstärker; Widerstandsnetzwerk U31 mit 10-kΩ-Elementen. Die Guard-Funktion verwendet einen weiteren Spannungsfolger, mit R30 = 1 kΩ nach GUARD_PCB und R29 = 10 kΩ nach GUARD_OUT. Guard ist kein zusätzlicher Messbereich.

**Skalierung:** Mit den bestätigten Bereichen fallen nominal an jedem effektiven Shunt bei Bereichsendwert 1 V ab (I × R). Die ältere 100-mA-Bezeichnung am 10-Ω-Pfad passt damit zur Nutzerfestlegung. Vorzeichen, I_MON-Verstärkung, ADC-/DAC-Umrechnung und Kalibrierung sind vor realer Hardwareansteuerung weiterhin zu bestätigen. Diese Änderung setzt die GUI-Grenzen, keine Kalibrierwerte.

### Spannungsbereiche und Sense (Blatt 1 / 10)

- Softwarebereiche: ±5 V und ±30 V; auf Blatt 1 sind die Teilungen 1:2 und 1:12 eingetragen.
- Umschaltung über U2 HE3621A6012 und `V_RANGE_1_EN`. Die Zuordnung des logischen Pegels zum Bereich wird vor dem realen Hardwaretreiber anhand der Relaiskontaktbelegung festgelegt.
- V_MON-Verstärker: U154/U155 OPA192ID, U156 OPA2192ID.
- `OUTPUT_EN` steuert die FORCE-Ausgangstrennung (K5 G2RL-2 DC12); `REMOTE_EN` die Sense-Umschaltung (U73 G6S-2F DC12).
- Output-OFF/Z-HIGH verlangt FORCE getrennt und externe SENSE-Anschlüsse abgewählt. `ZERO` ist dagegen ein aktiver Regelzustand. Der Hardware-Layer muss diese Zustände unterscheiden.

### Regelung und drei PI-Konfigurationen (Blatt 2 / 3 / 9)

Die analoge CV-/CC-Auswahl erfolgt im Error-/MinMax-Pfad. Blatt 3 bezeichnet die Stufen als `min(F_V, F_I_LO)` und `max(U1, F_I_HI)`. Software-Source-Modus und tatsächlicher CV-/CC-Regelzustand sind getrennte Begriffe; es gibt hier keinen digitalen CV-/CC-Umschalter.

- Fehlerbildung: U149 OPA4192ID; Signale V_SET, V_MON, I_SET, I_SET_N und I_MON.
- MinMax: U150 OPA4192ID, Puffer U25 OPA192ID.
- Anti-Windup/Z-HIGH: U157 DG419DY+; Eingang ZHIGH_EN, Ausgang PI_ERROR.
- PI-Verstärker: U12 OPA192ID; Ausgang SET_AMP.
- PI-Umschaltung: U24 ADG1209YRZ, gesteuert über PI_MUX_A0/A1/EN. Je Adresse werden Eingangswiderstand und Rückkopplungszweig gemeinsam gewählt.

| PI-Kanal | Eingangswiderstand | Rückkopplungswiderstand | Kondensator |
|---|---|---|---|
| S1A / S1B | R39 = 10 kΩ | R68 = 2 kΩ | C1 = 1 nF |
| S2A / S2B | R41 = 22 kΩ | R69 = 2 kΩ | C2 = 47 nF |
| S3A / S3B | R40 = 47 kΩ | R74 = 4,7 kΩ | C3 = 100 nF |
| S4A / S4B | nicht angeschlossen | nicht angeschlossen | kein gültiger PI-Modus |

`PI_RESET_EN` steuert U17 TLP170AM über R66 = 1 kΩ. Der Resetpfad überbrückt den PI-Rückkopplungszweig. Die Zuordnung der drei PI-Konfigurationen zu den vier Strombereichen und zu kapazitiven Lasten ist **keine im Schaltplan enthaltene Firmwaretabelle** und bleibt offen. Frühere LTspice-Zuordnungen nicht stillschweigend übernehmen. Eine Bereichsumschaltung muss Ausgangssperre, Relais, Mess-MUX, PI-MUX und Reset koordinieren; reale Wartezeiten sind noch nicht gemessen.

### Sollwertausgabe, Messwerterfassung, Referenz (Blatt 4 / 5 / 8)

| Funktion | Fest im Schaltplan | Softwarebezug |
|---|---|---|
| DAC | U5 DAC80004IPWR | A/B = MSB-/LSB-Zweige für V_SET; C/D = MSB-/LSB-Zweige für I_SET |
| DAC-Ausgänge | OPA4192-Stufen, je ±2,5 V beschriftet | Summierung/Feinanteil und Code-Umrechnung vor realem Treiber auswerten; nicht als vier unabhängige Sollwerte behandeln |
| ADC | U165 AD7177-2BRUZ-RL7 | VMON an AIN0, IMON an AIN2; AIN1/AIN3 als zugehörige Rückleitungen im Schaltplan prüfen |
| ADC-Aufbereitung | U3 OPA2192IDR | V_MON/I_MON von ±2,5 V auf 0…2,5 V laut Beschriftung |
| Referenz | U36 ADR1399KHZ | +2V5_REF und −2V5_REF; die ältere ADR4525-Kandidatenangabe ist überholt |
| Referenzverstärker | U9 OPA192ID, U1 OPA2192ID | Blatt 8 enthält zusätzlich eine LTC2057HV-Notiz; diese ersetzt den bestückten Symbolwert nicht automatisch |

ADC-Rate, Filter, Kalibrierung, DAC-Feinskalierung und Einschwingzeiten sind noch keine aus dem Schaltplan ablesbaren Softwareparameter. Sie bleiben ausdrücklich offen. Der Simulationslayer arbeitet zunächst mit physikalischen Soll-/Messwerten, ohne erfundene Registerkonfigurationen.

### Digitale Steuerleitungen (Blatt 12)

U178 MCP23017T-E/SO steuert:

| Port | Signal | Port | Signal |
|---|---|---|---|
| GPB0 | I_RANGE_1_EN | GPA0 | REMOTE_EN |
| GPB1 | I_RANGE_2_EN | GPA1 | OUTPUT_EN |
| GPB2 | I_RANGE_3_EN | GPA2 | D_IMON_MUX_EN |
| GPB3 | I_RANGE_4_EN | GPA3 | D_IMON_MUX_A1 |
| GPB4 | V_RANGE_1_EN | GPA4 | D_IMON_MUX_A0 |
| GPB5 | ENDSTUFE_1A_EN | GPA5 | D_PI_MUX_EN |
| GPB6 | PI_RESET_EN | GPA6 | D_PI_MUX_A1 |
| GPB7 | unbeschaltet | GPA7 | D_PI_MUX_A0 |

Pico U32: I²C SDA GP4 / Pin 6; SCL GP5 / Pin 7; D_DAC_CS GP10 / Pin 14; D_ADC_CS GP11 / Pin 15; D_ZHIGH_EN GP13 / Pin 17. Output-Taster BTN_OUTPUT_EN: GP3 / Pin 5; Beeper: GP9 / Pin 12. `D_` kennzeichnet hier digitale Signale vor dem Übergang zur Analogdomäne. Registeradresse des Expanders und endgültige Initialisierungsfolge vor Hardwareansteuerung separat bestätigen.

Display gemäß **CN4**: SCK GP14 / Pin 19, MISO GP12 / Pin 16, MOSI GP15 / Pin 20, CS GP17 / Pin 22, PD_N GP20 / Pin 26, INT GP21 / Pin 27. Die Firmware nutzt SPI1. Display, Touch und Beeper wurden vom Nutzer am 19.11.2026 als funktionierend bestätigt. Die abweichenden SPI0-Netznamen am Pico sind bereits in Hardware.md dokumentiert. SPI1 wird im Mainboard auch für weitere Teilnehmer verwendet; spätere Treiber müssen Chip-Select und Buszugriff gemeinsam verwalten.

### Noch nicht implementierte bzw. abzugleichende Funktionen

- Vollständiger Hardware-Simulationslayer einschließlich Output-/Fehlerzuständen und CV-/CC-Verhalten.
- PI-Kanalzuordnung, MUX-Wahrheitstabellen/Enable-Pegel und Hardware-Umschaltzeiten.
- Physische Skalierung und Kalibrierung der vier Strombereiche.
- OVP/OCP-Einstellungen im Code sind bislang Platzhalter; deren Werte nicht mit Shuntbereichen oder nachgewiesenen Hardware-Schutzschwellen gleichsetzen.
- `OutputOffState::Standard` im Code ist noch nicht eindeutig auf das dokumentierte STANDBY-Konzept abgebildet. Keine neue Hardwarewirkung durch die Bereichserweiterung.

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
- Current range values are `10 uA`, `1 mA`, `100 mA` and `1 A` (user correction, 2026-11-20).
- The default is MANUAL / 100 mA; voltage remains MANUAL / 5 V.
- Source current is bipolar; current compliance is a nonnegative magnitude.
- AUTO resolves the smallest fitting range using the maximum magnitude of current measurement, current source setting and current limit setting. A lower measurement must not select a range below a configured source or limit.
- Switching to a smaller MANUAL range resets out-of-range current source/limit values to zero, as before.
- OCP settings are separate from measurement ranges. The legacy `Off / 100 mA / 1 A` OCP placeholder is not a hardware protection implementation.
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

## Output-Bedienung an der Frontplatte – 19.11.2026

Ein physischer Taster mit LED übernimmt Output ON/OFF unabhängig von der angezeigten GUI-Seite. Ein dauerhaft sichtbarer GUI-Schalter ist nicht mehr erforderlich. Der Taster muss Zustandsänderungen über SystemClass auslösen; die LED-Anzeige wird aus dem dort verwalteten Ausgangszustand abgeleitet. Konkrete Verdrahtung und LED-Signalisierung sind noch festzulegen.


### Vorläufiger Simulationsbetrieb – 20.11.2026

Die ON/OFF-Schalter auf Haupt- und Trace-Seite sind entfernt. Für die GUI-Entwicklung startet der simulierte Ausgang immer auf ON (`System.SetOutputOn(true)` im Setup). Die spätere Bedienung erfolgt über den Frontplattentaster mit LED; die vorgesehenen Output-Off-Zustände für die reale Hardware bleiben bestehen.


## GUI-Aufteilung ab 20.11.2026

Wieder 2450-orientiert: Messung dauerhaft oben; Source, Statistics und Graph unten per horizontaler Wischgeste oder Seitenpunkten. Home zeigt Source, Menu oeffnet die vollstaendige MENU-Seite. Die Ansichtsauswahl ist reiner UI-Zustand. Domain-Werte bleiben in SystemClass; Hardware-Simulation, Output ON und die vier Strombereiche bleiben unveraendert. Details und Vorrang gegenueber alten GUI-Entwuerfen stehen am Anfang von ux.md.


## Messstatistik

`SystemClass::GetMeasurementStatistics()` liefert fuer die aktive Messfunktion V/I/P/R die Statistik des aktuellen Ringpufferinhalts (maximal 1024 Messpunkte). Leistung und Widerstand werden pro Messpunkt ausgewertet, nicht aus gemitteltem V und I abgeleitet. Nicht endliche Werte sowie ungueltige Widerstandswerte werden ausgelassen. Span zaehlt die gueltigen Werte. Standard Dev ist die Populationsstandardabweichung (Division durch N), stabil nach Welford berechnet. Bei leerer Statistik zeigt die GUI Striche, bei einem Wert ist die Abweichung null. Clear Active Buffer leert den gemeinsamen Messpuffer ueber SystemClass; bei laufender Simulation fuellt er sich sofort wieder.


## Wiederverwendbare Grafikbibliothek - 22.11.2026

Verbindliche Nutzerfestlegung: Grafikfunktionen werden ueber die `GDClass` aufgebaut. Ziel ist eine leistungsfaehige, fuer spaetere Projekte wiederverwendbare Bibliothek.

Controls und Panels verwenden die Grafik-API der `GDClass`. Direkte EVE-/FT813-Zeichenbefehle und hardwarespezifische Details bleiben in deren Implementierung gekapselt. Allgemeine Funktionen wie abgerundete Rechtecke, einstellbare Radien und Farbverlaeufe werden dort zentral implementiert. Die Grafikbibliothek bleibt unabhaengig von SMU-Fachlogik, `SystemClass` und konkreten Seitenlayouts; Geometrie und Farben werden als Parameter uebergeben.


## Menue-Navigation - 22.11.2026

Die MENU-Seite definiert sechs Gruppen. TRIGGER und SCRIPTS bleiben vorerst ausgeblendet; SOURCE, MEASURE, VIEWS und SYSTEM sind gleichzeitig sichtbar. Alle Menuefunktionen fuehren vorerst nur zu eindeutig betitelten Platzhaltern. BACK zeigt wieder das Menue, HOME die Hauptansicht mit Source. Die Navigation ist UI-Zustand und aendert keine Domain-Werte.


## Graph-Auswertung - 22.11.2026

Die Vollbild-Graph-Seite liest ausschliesslich zeitgestempelte BufferSamples aus SystemClass.Buffer. V/I/P/R stammen aus demselben gespeicherten Sample; I-V verknuepft V und I desselben Zeitpunkts. GraphState verwaltet nur Praesentationszustand (sichtbare Kurven, Achsen, Cursor, Darstellungsart), keine Kopien von SMU-Einstellungen. Einzige Domain-Mutation der Graph-Seite ist das ausdrueckliche Loeschen ueber ClearMeasurementBuffer. Triggersteuerung ist noch nicht implementiert und als Platzhalter markiert. Der 1024-Punkte-Ringpuffer bleibt die einzige Datenquelle; Details stehen in ux.md.

Grafik-Frame-Steuerung: GDClass.BeginFrame prueft den EVE-Coprozessorstatus, bevor ein neuer Frame gesendet wird. Solange der vorherige Frame bearbeitet wird, laeuft die Hauptschleife inklusive Eingabe und System.Execute weiter. Gemeldete Coprozessorfehler werden durch die EVE-Recovery behandelt und mit der Displaylistenlaenge auf Serial protokolliert. Der I-V-/Zeit-Wechsel ist per Host-Test ueber 500 Zyklen geprueft; eine Hardwarebestaetigung des gemeldeten Stillstands steht aus.


### Beeper-Zeitsteuerung (2026-11-22)
- Der aktive Beeper an GP9 wird weiterhin über einen statischen GPIO-Pegel angesteuert (keine tone-Frequenzerzeugung).
- BeeperClass verwendet mbed::Timeout zum Abschalten per Interrupt; die Hauptschleife muss keinen Update-Aufruf mehr ausführen. Klick: 20 ms, Fehler: 80 ms.
- Ein neuer Beep startet seine Laufzeit neu. Stop und Beep(0) löschen den Timer und schalten sofort aus. Timer-Neustart und Stop sind gegen den Interrupt geschützt; der Callback schreibt nur den bereits initialisierten GPIO und den Aktivzustand.
- GUI-/SPI-Zeichenzeiten verlängern den Ton nicht mehr durch ausbleibendes Polling. Lange global gesperrte Interrupts können die Abschaltung weiterhin verzögern.


### Gemeinsame Graph-Einstellungen (2026-11-22)
- SystemClass besitzt GraphSettings (Core/GraphSettings.h). Aktuell enthält dies GraphStyle mit stabilen Werten Lines=0, Points=1, Both=2.
- MENU → Graph → Data ändert die Darstellung über SetGraphStyle; GraphPage und MainPage/GraphPanel lesen denselben Wert direkt über GetGraphSettings(). Keine lokale Kopie in GraphState.
- Buffer-Löschen und Navigation ändern diese Einstellung nicht. System.Reset setzt den Standard Lines. Speicherung im Flash ist noch nicht implementiert; die zentrale, UI-unabhängige Struktur ist für eine spätere versionierte Speicherung vorgesehen.
- Zoom/Achsenausschnitt bleibt je Ansicht lokaler Präsentationszustand.


### Histogramm (2026-11-22)
- SystemClass besitzt die zentralen HistogramSettings: Messgröße (BufferQuantity), Quelle aktiv/entfernt, 1–64 reguläre Klassen, Grenzen, Methode und feste Y-Obergrenze. SetHistogramSettings validiert Änderungen atomar. Noch keine Flash-Speicherung.
- CalculateHistogram ist unabhängig von der GUI. Es berechnet aus dem aktuellen Ringpuffer gleich breite Klassen, darunter eine Unterlaufklasse und darüber eine Überlaufklasse. Reguläre Intervalle sind [Untergrenze, Obergrenze); die letzte Klasse schließt das globale Maximum ein. Nichtendliche Werte und ungültige Widerstandswerte werden gezählt, aber nicht einsortiert.
- Implementierte A-SMU-Methoden: Auto Bin passt X an die Datenextrema an (mit Schutzspanne für identische Werte) und Y an die höchste Klasse. Fit behält X-Grenzen und Klassenanzahl und passt nur Y an. Off hält X und Y fest; höhere Balken werden geclippt. SmartScale wählt Auto Bin, wenn Daten außerhalb der Grenzen liegen oder weniger als ein Viertel der eingestellten X-Spanne belegen, sonst Fit. Dies ist unsere definierte Auswahlregel, keine behauptete Nachbildung eines proprietären Keithley-Algorithmus.
- AUTO SCALE aktiviert SmartScale. Manuell eingegebene Grenzen wechseln zu Fit; eine manuelle Y-Obergrenze wechselt zu Off. Alle Methoden verwenden die vom Nutzer gewählte Klassenanzahl; Änderungen führen zur Neuberechnung aus dem Puffer.
- Datenquelle ist derzeit ausschließlich der existierende Active Buffer; auswählbar sind V/I/P/R. Remove blendet die Quelle aus, CLEAR BUFFER löscht den gemeinsamen Messpuffer und betrifft auch Graph/Statistik. Ausgangszustand wird nicht verändert.


### Reading Table (2026-11-22)
- Die Reading Table liest denselben aktiven Messpuffer wie Graph, Histogramm und Statistik. Alle vier Messgrößen V/I/P/R stammen aus den gespeicherten BufferSamples; keine erneute Berechnung aus aktuellen Ausgangseinstellungen.
- BufferClass führt eine Gesamtzahl der seit Reset hinzugefügten Samples und den ersten Zeitstempel. Damit bleiben die angezeigten Messpunktindizes auch beim Überschreiben des Ringpuffers fortlaufend. ClearMeasurementBuffer setzt diese Metadaten zurück.
- ReadingTableState hält eine lokale Momentaufnahme für Präsentation und Auswahl (maximal 1024 Samples). LIVE aktualisiert sie und zeigt die letzte Seite; HOLD, Blättern oder Antippen eines Messpunkts friert sie ein. Die Messwerterfassung läuft unabhängig weiter. Auch ein zwischenzeitliches Löschen des echten Puffers verändert die gehaltene Ansicht nicht; LIVE übernimmt wieder den aktuellen Puffer.
- Zeitangaben sind relative Millisekunden seit Pufferstart, keine Kalenderzeit. Historische Source-Sollwerte werden derzeit nicht aufgezeichnet und daher nicht als eigene Source-Spalte dargestellt. Die Vorschau liest die zentrale GraphStyle-Einstellung.


## Hardware-Layer und statische Simulation – 22.11.2026
Diese Festlegung ersetzt die frühere sinusförmige Demo-Simulation.

- `Core/Hardware/HardwareBackend.h` definiert `Hardware::Backend` mit `Apply(Request)` und `Read(now, Measurement&)`. Request enthält Spannungssollwert, Betrag der Strom-Compliance, Spannungs-/Strommessbereich, Ausgangsanforderung und Betriebsart. Ein späterer ADC-Treiber darf `false` liefern, solange noch kein vollständiger Datensatz vorliegt. Der Zeitparameter lässt auch zustandsbehaftete dynamische Modelle zu.
- `SimulationBackend` berechnet idealen Spannungsbetrieb mit RESISTOR/OPEN/SHORT. CV: U=Sollwert, I=U/R. Bei Überschreitung der Compliance: symmetrisch begrenzter Strom und U=I·R. OPEN: I=0; SHORT: U=0 und Strom mit Vorzeichen des Sollwerts bis zur Compliance. Bei Sollwert null fließt auch im Kurzschluss kein Strom. OUTPUT OFF liefert ohne externe Quelle U=I=0. Keine künstliche Welligkeit und kein Ersatzwert für einen Null-Sollwert.
- `Hardware::Measurement` ist gleichzeitig `BufferSample`: Zeitstempel, U/I/P/R, Widerstandsgültigkeit, CV/CC/OFF, tatsächlicher Ausgangsstatus, getrennte Spannungs-/Strom-Overrange-Bits und Fehlercode. Die Statusbytes nutzen bisheriges Alignment-Padding; ein Sample bleibt 24 Byte groß. Leistung und Widerstand entstehen im Hardware-Layer.
- SystemClass besitzt Backend, SimulationSettings und den aktuellen gemeinsamen Messdatensatz. MainPage liest diesen Datensatz; der Ringpuffer übernimmt ihn unverändert für Graph, Histogramm, Statistik und Reading Table. Die noch vorhandenen einzelnen Messwert-Setter dienen nur der alten Host-Test-Injektion; Produktions-Erfassung erfolgt atomar über das Backend.
- Erfassung alle 100 ms; Änderungen an Last, Sollwert, Compliance, Ausgang oder Bereichen lösen im nächsten Execute-Aufruf eine sofortige Aktualisierung aus. Fehlerhafte Backend-Samples bleiben mit Fehlerstatus im Puffer; Graph/Histogramm/Statistik überspringen sie und Reading Table zeigt Striche. Ohne Backend werden keine Messpunkte erfunden.
- Simulationslast und Verfügbarkeitsmasken liegen zentral in SystemClass. Standardlast: 1 kOhm, alle Bereiche verfügbar. Positive endliche Widerstände sind erlaubt; die GUI-Einheit ändert nur Ein-/Ausgabe. Keine Flash-Persistenz.
- Verfügbare Bereiche: ±5/±30 V sowie ±10 µA/±1 mA/±100 mA/±1 A. Aktive Bereiche können nicht deaktiviert werden. Setter, AUTO-Auswahl und Widerstandspresets dürfen keine deaktivierten Bereiche auswählen. Die normalen Geräteseiten wählen weiterhin aktive Bereiche.
- Strom-Compliance ist jetzt unabhängig vom Strommessbereich (0 bis 1 A). Ein kleinerer Messbereich löscht oder reduziert die Compliance nicht. OVERRANGE ist ein Status und verändert die erzeugten Werte nicht. Dies ersetzt die frühere Kopplung von CurrentLimitMax an den Messbereich.
- Backend-Wechsel: zunächst OFF an den bisherigen Treiber, dann Backend lösen/wechseln. SIM OFF bedeutet derzeit NO HARDWARE und OUTPUT OFF; ein reales Backend existiert noch nicht. Beim Aktivieren von SIM wird nach dem Wechsel der simulierte Ausgang gemäß aktuellem Entwicklungsbetrieb wieder eingeschaltet. Es wird kein realer Ausgang aktiviert.
- Die Simulation unterstützt zunächst nur Spannungsquellenbetrieb. Source Current meldet MODE UNSUPPORTED und einen tatsächlich ausgeschalteten Ausgang; es wird kein Stromquellenmodell vorgetäuscht.
- Host-Tests: 5 V/20 mA bei 1 kOhm, 100 Ohm, OPEN, SHORT; CV/CC-Grenze, negative/Null-Sollwerte, Null-Compliance, OFF, NaN/Inf, sehr kleine/große Lasten, Overrange ohne Begrenzungsänderung, Bereichsmasken, gemeinsame Samples, Zeitstempelüberlauf und Backend-Wechsel.


### Simulationsrauschen – 22.11.2026
Die Simulation enthält jetzt gleichverteiltes relatives Messrauschen von ±0,01 % für Spannung und Strom mit getrennten Pseudozufallsfolgen. Es wird nach dem idealen Last-/Compliance-Modell aufgebracht; CV/CC wird weiterhin vom idealen Modell bestimmt. Die gemessenen Werte können deshalb geringfügig über Sollwert oder Compliance liegen. Leistung und Widerstand werden aus den verrauschten U/I-Werten berechnet; OVERRANGE bezieht sich ebenfalls auf die verrauschten Messwerte. Nullwerte bleiben null (auch OPEN-Strom, SHORT-Spannung und OUTPUT OFF).

Die Rauschwerte werden aus Zeitstempel und Kanal abgeleitet: Wiederholte Reads desselben Zeitpunkts bei AUTO-Range liefern identisches Rauschen. Kein zusätzlicher Rauschpuffer oder Zufallsgeneratorzustand; die Messkadenz bleibt unverändert. Gemeinsamer Messdatensatz für Anzeige und Auswertungen. ±0,01 % bezeichnet die Amplitudengrenze relativ zum Messwert, nicht zum Messbereich und nicht den RMS-Wert.


### Stabile Zeitkurven bei laufender Erfassung – 22.11.2026
Fehlerkorrektur: Die bisherige Min/Max-Verdichtung teilte alle sichtbaren Samples bei jedem Frame neu in bis zu 32 gleich große Gruppen nach Sampleanzahl ein. Ab dem 33. Punkt konnten dadurch ältere Spitzen verschwinden oder anders verbunden werden, obwohl die gespeicherten Daten unverändert waren.

Zeitkurven verwenden jetzt feste Zeitintervalle innerhalb des aktuellen X-Achsenausschnitts. Bei unveränderter X-Achse ändern angehängte Messpunkte keine bereits abgeschlossenen Intervalle. Das noch offene Intervall darf sich ergänzen; bei Scrollen/Zoom oder automatisch wachsender Gesamthistorie wird der veränderte Ausschnitt neu abgebildet. Min/Max, Datenlücken und Nachbarpunkte zum Clipping bleiben erhalten. Maximal 66 Zeichenpunkte bei 32 Intervallen, weiterhin unter dem vorhandenen 96-Punkte-Limit; keine zusätzlichen dauerhaften RAM-Puffer. I-V behält die chronologische Gruppierung, weil Spannung dort nicht monoton sein muss.

Regression: Neun Sekunden echte Simulationsdaten werden schrittweise angehängt; bisherige Buffer-Samples und die Zeichenpunkte abgeschlossener Zeitintervalle werden vor/nach jedem neuen Sample verglichen. Der Test schlug mit der alten Gruppierung fehl und besteht mit der Korrektur.


### Full buffer als Standard und ruhige Achsen – 22.11.2026
MainPage und MENU/Graph starten jetzt mit Full buffer. Die Zeitachse zeigt den vorhandenen Verlauf und erweitert ihre Spanne in 1/2/5-Stufen (standardmäßig mindestens 10 s, danach 20/50/100 s). Der linke Rand bleibt beim ersten erhaltenen Messpunkt; nach Überschreiben des Ringpuffers rückt dieser weiterhin vor. Unter SCALE heißt die bisherige Window-Einstellung in Full buffer nun Min span.

Ursache des verbleibenden Kurvenwanderns: Die zuvor korrigierten Zeitintervalle waren nur bei unveränderter X-Achse stabil. Full buffer passte deren Breite bisher bei jedem neuen Sample an. Stufenweise Erweiterung verhindert dieses ständige Umgruppieren. Bei einem tatsächlichen Wechsel der Zeitspanne wird die Kurve neu skaliert und verdichtet; dies ist keine Änderung an den Messwerten.

Automatische Y-Achsen behalten ihre Grenzen, solange neue Daten hineinpassen. Nur außerhalb liegende Werte erweitern die Grenzen mit Reserve. AUTO SCALE passt ausdrücklich erneut eng an die Daten an. Konstante Startdaten etablieren noch keine dauerhafte Rauschspanne. Manuelle Achsen bleiben unverändert. Die kleine Reading-Table-Vorschau überspannt weiterhin exakt ihre Daten, passend zur Auswahl per Antippen.

Host-Regression prüft zusätzlich den Full-buffer-Standard, unveränderte alte Kurvenpunkte beim Anhängen zwischen 4 und 9 s, gleichbleibende Y-Achsen bei Werten innerhalb der Grenzen, Erweiterung bei Ausreißern und erneutes Einpassen mit AUTO SCALE.


### Schrittweise Zeitachse als Standard – 22.11.2026
Ersetzt den zuletzt gewählten Full-buffer-Standard: Main und MENU/Graph starten mit Step scrolling, 10 s Fensterbreite und 2 s Schrittweite. Bis einschließlich t=10 s bleibt der Ausschnitt 0–10 s; der erste spätere Messwert verschiebt ihn auf 2–12 s, anschließend 4–14 s usw. Bei verzögerten Frames werden erforderliche Schritte direkt nachgeholt. Die X-Skalierung bleibt gleich.

SCALE bietet weiterhin Full buffer als Alternative. Window bestimmt im Step-scrolling-Modus die Breite; die Schrittweite beträgt 20 % davon (5 s Fenster → 1 s Schritt). Manuelle Skalierung/Pan bleiben bis AUTO SCALE bestehen. Die ruhige Y-Automatik bleibt erhalten.

Die Verdichtung verwendet jetzt am Zeitursprung ausgerichtete Intervalle, damit auch beim Fenstersprung Spitzen im überlappenden, vollständig sichtbaren Bereich gleich bleiben. Höchstens 33 Intervalle plus Clipping-Nachbarn (68 Punkte), weiterhin innerhalb des bestehenden 96-Punkte-Limits. Messpuffer und Erfassung werden nicht geändert.


### Kompakter Messpuffer: P/R dynamisch – 22.11.2026
Ersetzt die bisherige Speicherung von Leistung und Widerstand: Hardware::Measurement / BufferSample enthält nur noch U, I, Zeitstempel, CV/CC, Ausgangs-/Overrange-Flags und Fehlerstatus. Größe jetzt 16 statt 24 Byte; der Ringpuffer mit unverändert 1024 Punkten benötigt 16 statt 24 KiB. Auch alle Sample-Kopien in den Ansichten sind entsprechend kleiner.

GetPower() berechnet U·I und GetResistance() U/I bei Bedarf zentral im Hardware-Datentyp. Widerstandsgültigkeit wird ebenfalls abgeleitet (endliche Werte, |I| mindestens 1 nA, endlicher Quotient); ein eigener Gültigkeitswert wird nicht mehr gespeichert. Die Ansichten prüfen diese Gültigkeit und zeigen bei ungültigem R weiterhin Striche. Fehlerhafte Samples werden von den Auswertungen weiterhin ausgelassen.

Main, Statistik, Graph, Histogramm, Simulation, Reading Table und XY-Vorschau verwenden dieselben dynamischen Berechnungen. Statistik wertet P/R weiterhin pro einzelnem Messpunkt aus, niemals aus gemittelten U/I-Werten. Unbenutzte SetPower/SetResistance und die bisherige Aktualisierung abgeleiteter Cache-Werte wurden entfernt. Keine Änderung an Messrate, Rauschen, Fensterbreite oder Ringpufferkapazität. Host-Tests prüfen 16-Byte-Layout, dynamische Werte nach U/I-Änderung, Null-/ungültige Ströme sowie alle bisherigen Auswertungen.

Pico-Build nach dem Umbau: 158.132 statt 201.148 Byte statisch belegtes RAM (58,5 % statt 74,4 %). Ersparnis einschließlich aller Sample-Kopien: 43.016 Byte, rund 42 KiB. Build und Host-Tests erfolgreich.


### Standardfenster 100 Sekunden – 22.11.2026
Der Standard für MainPage und MENU/Graph beträgt jetzt 100 s bei 10 Rasterteilungen, also 10 s/div. Step scrolling verschiebt weiterhin um 20 % der Fensterbreite: 0–100 s → 20–120 s → 40–140 s. Diese Vorgabe ersetzt das frühere 10-s-Standardfenster. Pufferkapazität (1024 Samples), Messrate und ruhige Y-Automatik bleiben unverändert.


### Schrittweite genau eine Rasterteilung – 22.11.2026
Nutzerkorrektur: Step scrolling verschiebt um genau Window / XDivisions, nicht mehr um 20 %. Beim Standardfenster 100 s mit 10 Rasterteilungen sind das 10 s: 0–100 → 10–110 → 20–120 s. Bei anderer Fensterbreite bleibt der Schritt ebenfalls genau eine Rasterteilung. Messrate, Puffer und Y-Automatik bleiben unverändert.


### Vollständige Messpunkte als Pixelkurven – 22.11.2026
Die starke 32-Intervall-Verdichtung führte bei 100 s Fensterbreite zu nur wenigen dargestellten Werten in den ersten 10 s. Main-Graph, MENU/Graph und Reading-Table-Vorschau verwenden jetzt GDClass::DrawPlotTrace mit einem generischen PlotPointSource. Alle gültigen Samples (maximal 1024) werden gelesen und direkt in ein einfarbiges 1-Bit-Pixelbild gezeichnet. Keine Min/Max-Auswahl in diesen drei Ansichten; nahe Punkte können aufgrund der Displayauflösung natürlich auf dieselbe Pixelspalte fallen. Line/Points/Both, Datenlücken und Clipping bleiben erhalten.

PlotRaster enthält die wiederverwendbare, SMU-unabhängige Rasterisierung. GraphState::Trace liefert daraus nur Koordinaten und Gültigkeit aus den gespeicherten Daten. Das ältere MakePoints bleibt als begrenzte Hüllkurven-API erhalten, wird von den drei Ansichten aber nicht mehr verwendet. Drei bisherige PlotPoint-Arrays wurden entfernt.

GDClass verwendet einen gemeinsamen 25.600-Byte-Arbeitspuffer, maximal vier Kurven pro Frame, Bitmap-Handles 16–19 und zwei wechselnde RAM_G-Bänke (je vier Slots à 25.600 Byte, insgesamt Adressen 0 bis exklusiv 204800). Der Modal-Snapshot ab 0x40000 und ROM-Font-Handles 1/2 bleiben frei. L1-Bitmaps werden per SRC_ALPHA / ONE_MINUS_SRC_ALPHA eingefärbt; Nullbits sind transparent. Der aktive Bildspeicher wird während des nächsten Frame-Aufbaus nicht überschrieben. Die Displayliste wächst nicht mehr mit der Messpunktzahl. Der Bildtransfer verursacht zusätzliche SPI-Zeit, besonders bei vier Kurven.

Host-Tests prüfen alle 100 Punkte der ersten 10 s im Pixelbild, unveränderte Messdaten, Moduswahl, Clipping und Unterbrechungen durch ungültige Daten. EVE-Ausgabe, Farbkomposition und Touch-Reaktionszeit müssen nach Upload am Display geprüft werden.


### L1-Transparenz korrigiert – 22.11.2026
Die erste Bitmap-Version verwendete irrtümlich ONE/ONE-Blending und zeichnete dadurch eine gefüllte Fläche. FT813 L1 enthält ein Alpha-Bit und keine RGB-Farbbits. DrawPlotTrace verwendet jetzt SRC_ALPHA / ONE_MINUS_SRC_ALPHA: Nullbits erhalten Hintergrund und Raster, gesetzte Bits zeichnen die Kurvenfarbe. Quelle: FT81X Series Programmer Guide, BITMAP_LAYOUT-Formattabelle, https://brtchip.com/wp-content/uploads/Support/Documentation/Programming_Guides/ICs/EVE/FT81X_Series_Programmer_Guide.pdf . Die tatsächliche Ausgabe muss am Display nachgetestet werden.


### Main-Diagramm ohne Verschieben – 22.11.2026
Zusätzlich zum bereits deaktivierten Pinch-Zoom ist nun auch Ein-Finger-Pan im Main-Diagramm deaktiviert. Drags innerhalb der Plotfläche werden konsumiert und ändern weder Achsen noch Ansicht; sie gelten nicht als Tipp. Automatische Zeitfenstersprünge und Doppeltipp für AUTO SCALE bleiben erhalten. Die große MENU/Graph-Seite behält Zoom und Verschieben.


### Main-Slide über der Diagrammfläche wiederhergestellt – 22.11.2026
Korrektur der Touch-Zuordnung: Horizontales Wischen über dem Main-Diagramm wechselt wieder Source/Statistics/Graph. Die Kurve selbst bleibt gegen Pan/Zoom gesperrt. SourcePanel übernimmt Swipes auch innerhalb der Plotfläche; kurze Taps gehen weiter an GraphPanel. Vertikale Bewegungen und abgebrochene Gesten wechseln keine Seite. Mehrfingerkontakte bleiben gesperrt.


### Lesbare Diagrammachsen – 22.11.2026
Main und MENU/Graph beschriften die Zeitachse jetzt als MM:SS (100 s → 01:40). Minuten laufen über 59 hinaus; negative manuelle Ausschnitte erhalten ein Minuszeichen. Bei Beschriftungsabständen unter einer Sekunde kommen passend zum Zoom Nachkommastellen hinzu. I-V behält Spannungswerte auf X.

Automatische Werteachsen werden auf 1/2/2,5/5-Schritte pro Dekade mit ausgerichtetem Startwert eingepasst. Die Beschriftung verwendet ein gemeinsames Engineering-Präfix pro Achse und zur Rasterweite passende Nachkommastellen statt pauschal vier signifikanter Stellen. So werden bei etwa 100 mV und ±0,01 % Rauschen unterschiedliche Rasterwerte lesbar (z. B. 99,985 / 99,990 / 99,995 / 100,000 mV). Die ruhige Y-Automatik und manuelle Achseneinstellungen bleiben erhalten.

Host-Tests prüfen MM:SS inklusive Minutenübertrag, negative Zeiten, Subsekundenformat und eindeutig unterschiedliche Ticktexte für den engen 100-mV-Bereich.


### Main-Y-Achse enger eingepasst – 22.11.2026
Nur das kompakte Main-Diagramm verwendet tightYAxis: tatsächliches Minimum/Maximum mit 5 % Reserve pro Seite, ohne zusätzliches Aufrunden der Achsengrenzen auf schöne Rasterwerte. Die Skala bleibt stabil, solange die Werte hineinpassen und mindestens 75 % der Höhe belegen; andernfalls wird neu eingepasst. Die große Graph-Seite behält ihre gerundeten Rasterabstände. Die Beschriftung berücksichtigt auch ungerade Rasterweiten mit begrenzter zusätzlicher Genauigkeit, damit die Texte in die Achsenleiste passen.


### Pico-kompatible Zeit-/Indexausgabe – 22.11.2026
Die bisher verwendeten printf-Formate %llu und %02llu wurden auf dem Pico als lu/0lu ausgegeben. Zeitachse und Reading-Table-Index (einschließlich Detaildialog) verwenden jetzt DisplayFormat::FormatUnsigned zur direkten dezimalen 64-Bit-Konvertierung, ohne Long-long-Unterstützung der libc vorauszusetzen. Minuten werden darüber zweistellig aufgefüllt; Sekunden bleiben zweistellig. Indizes behalten ihren vollen 64-Bit-Wertebereich. Host-Test umfasst Null, Padding, Werte oberhalb 32 Bit, UINT64_MAX und kleine Zielpuffer; Hardware-Ausgabe nach Upload prüfen.


### Automatische X-/Y-Zoomgeste – 22.11.2026
MENU/Graph erkennt die Zoomrichtung aus der ersten deutlichen Änderung der Zwei-Finger-Abstände. Überwiegend horizontales Spreizen/Schließen zoomt nur X, vertikales nur Y, diagonales beide Achsen. Kein Auswahlbutton. Als überwiegend gilt mehr als doppelte Änderung gegenüber der anderen Richtung; Bewegungen unter 4 Pixel werden zunächst ignoriert. Die erkannte Achse bleibt bis zum vollständigen Loslassen gesperrt, damit die Geste nicht ungewollt wechselt.

Die anfängliche Fingermitte bleibt der Bezugspunkt. Bei getrenntem Zoom bleiben die Werte der Gegenachse unverändert. Nach Zoom sind die Achsen manuell, AUTO SCALE aktiviert wieder die Automatik. Der Main-Graph bleibt ohne Zoom/Pan, der Seiten-Slide bleibt erhalten. Host-Tests prüfen horizontale, vertikale und diagonale Gesten, geringe Bewegungen sowie Richtungssperre und unveränderte Gegenachse.


### Reading Table zeilenweise live – 22.11.2026
LIVE zeigt jetzt immer die letzten bis zu acht Messpunkte. Nach Füllen der Ansicht rückt mit jedem neuen Messpunkt eine Zeile nach oben, der neue Wert erscheint unten. Der bisherige Sprung auf eine fast leere neue Seite nach jeweils acht Werten entfällt. Dies gilt vor und nach Erreichen der Ringpufferkapazität. HOLD behält seine Momentaufnahme; Previous/Next blättern weiterhin in Acht-Zeilen-Schritten, begrenzt auf das letzte vollständige Fenster. Last und Messpunktauswahl zeigen am Pufferende ebenfalls ein vollständig gefülltes Fenster, sofern mindestens acht Werte existieren.


### Zeitachse nach Clear Buffer – 24.11.2026
ClearMeasurementBuffer leert den zentralen Puffer; BufferClass erhoeht bei jedem Reset eine Generation. Beide Graph-Ansichten erkennen diese unabhaengig von Sichtbarkeit und Sampleanzahl. Der gemeinsame Zeitursprung ist der erste Messpunkt seit Reset (GetStartTimeMs), auch nach Ringpuffer-Ueberschreiben. GraphState setzt beim Reset die Zeitachse auf automatisch, Start 0 und die eingestellte Fensterbreite; manuelle Y- und I-V-Achsen sowie Kurvenauswahl bleiben erhalten.


### RAM-sparende Messwertansichten und 50-Punkte-Seitenspeicher – 24.11.2026

Diese Festlegung ersetzt die bisherigen vollstaendigen Messpuffer-Kopien in den Ansichten und die vollstaendige HOLD-Momentaufnahme der Reading Table.

- SystemClass.Buffer bleibt der einzige Messringpuffer mit 1024 Samples. BufferView referenziert seine bis zu zwei zusammenhaengenden Speicherabschnitte in zeitlicher Reihenfolge und besitzt keine Messdaten. Main-Graph, MENU/Graph, Histogramm und XY-Popup verwenden diesen lesenden Zugriff. Das XY-Popup betrachtet wie bisher die letzten maximal 256 Punkte.
- Ansichten werden nur synchron vor der naechsten Pufferaenderung ausgewertet. System.Execute und Zeichnen laufen derzeit nacheinander in der Hauptschleife; spaetere parallele Erfassung muss den Zugriff explizit koordinieren. Keine Verlagerung der entfernten Kopien auf Stack oder Heap.
- ReadingTableState speichert genau einen Ausschnitt mit maximal 50 Samples (800 Byte Messdaten), der alle acht sichtbaren Zeilen umfasst. LIVE folgt den neuesten acht Werten. HOLD haelt nur den geladenen Ausschnitt samt Zeit-/Indexbezug fest. Beim Blaettern ausserhalb dieses Ausschnitts werden benoetigte Werte aus dem zentralen Puffer nachgeladen; es gibt kein eingefrorenes Archiv des gesamten Verlaufs mehr.
- Gesamtzahl und Indexgrenzen der gehaltenen Ansicht bleiben bis LIVE konstant. Nachladen prueft fortlaufende Indizes und Reset-Generation: ueberschriebene oder geloeschte Daten werden niemals durch neue Werte mit gleich aussehenden Indizes ersetzt. Ist die angeforderte Seite nicht mehr vorhanden, bleibt die aktuelle Seite erhalten und die GUI weist auf LIVE hin. Bereits gespeicherte 50 Punkte bleiben auch nach Clear erhalten.
- Die Vorschau und ihre Messpunktauswahl beziehen sich auf den geladenen Ausschnitt. Clear Buffer und der gemeinsame Graph-Zeitreset bleiben wirksam.
- Host-Tests pruefen direkte Referenzen, Ringumlaeufe, Gleichheit der Graph-/Histogrammauswertung mit einer Referenzkopie, alle sichtbaren Zeilen ueber Cachegrenzen, HOLD, Ablauf von Seiten, Reset-Generationen, LIVE und Vorschau-/Detaildialoge. Groessengrenzen verhindern erneute grosse UI-Puffer.


### NPLC-Profile und Messkadenz � 24.11.2026
SystemClass besitzt den NPLC-Profilindex mit validierendem SetNplcProfile. MeasurementSettings.h definiert die sechs Profile, Standard und feste 50-Hz-Netzfrequenz einmalig. Zeiten entsprechen den nominalen AD7177-2-Sinc5+Sinc1-Einschwingzeiten bei 16 MHz: 200 / 2000 / 10000 / 20016 / 100000 / 200000 us je Kanal. Paarzeit und Rate werden abgeleitet. Quelle: AD7177-2 Datenblatt Rev. B, Tabelle 20, https://www.analog.com/media/en/technical-documentation/data-sheets/ad7177-2.pdf . NPLC bezeichnet hier eine aequivalente Filterzeit, keine garantierte rechteckige Integrationsapertur.

Ersetzt die feste 100-ms-Simulationskadenz: Execute verwendet einen ueberlaufsicheren Mikrosekundenabstand fuer U/I-Paare. Pro Aufruf entsteht hoechstens ein Sample, verpasste Intervalle werden nicht kuenstlich aufgefuellt. Hohe Raten bleiben durch Polling/Rendering begrenzt. Zeitstempel im kompakten Messpuffer bleiben Millisekunden; Sub-ms-Samples koennen gleiche Zeitstempel und damit gleiche simulierte Rauschwerte haben. Die Simulation bildet bislang weder die ADC-Filterantwort noch die NPLC-abhaengige Rauschreduktion oder zwei getrennte Messfenster ab. Bestehende sofortige Aktualisierungen bei Last-/Source-Aenderungen bleiben erhalten. Reale Backends erhalten den Profilindex im Request und liefern erst nach vollstaendigem U/I-Paar ein Sample; ihre Erfassung wird nicht durch den Simulationstimer gedrosselt. Noch keine Flash-Speicherung.

Die fruehere Formulierung von gleichzeitig gemessenem U/I gilt nur fuer das Simulationsmodell: Die reale Hardware multiplexiert beide Kanaele. Ein gemeinsamer Datensatz behauptet keine simultane Wandlung.


### Unabhaengige Erfassung und GUI - 24.11.2026
Ersetzt die reine Main-Loop-Erfassung im Pico-Simulationsbetrieb: SimulationAcquisition besitzt einen Mbed-Thread mit Prioritaet AboveNormal und statischem 4096-Byte-Stack. Ein Ticker weckt ihn im NPLC-Paartakt. Der Interrupt setzt ausschliesslich ein Event; Modellberechnung erfolgt im Thread. Wiederholte unveraenderte Konfigurationen starten den Takt nicht neu. Verpasste Termine erzeugen keine erfundenen Nachholwerte.

SystemClass bleibt Domain-Eigentuemer. Der Worker erhaelt nur Request-/Simulations-Konfigurationsnachrichten und liefert vollstaendige Measurements. Ein Mutex schuetzt Konfigurationswechsel, Berechnung, FIFO und Diagnose. Der Worker beruehrt weder SystemClass, BufferViews, GUI noch SPI. Die einzige Historie bleibt SystemClass.Buffer; nur die Hauptschleife uebernimmt neue Samples vor dem Zeichnen. Damit bleiben alle direkt referenzierten Ansichten waehrend eines Frames stabil.

AcquisitionCore::Capacity definiert eine feste Transport-FIFO mit 256 noch nicht uebernommenen Samples (4096 Byte), keine Kopie des Messverlaufs. Bei 2500 Paaren/s ueberbrueckt sie 102,4 ms ohne Abnahme. Voll: aeltestes ausstehendes Sample verwerfen, neuesten Wert behalten, queueOverruns erhoehen. Verpasste Erfassungstermine werden als missedDeadlines gezaehlt. GetAcquisitionStats liefert beide Zaehler; GetHardwareStatusText zeigt ACQ OVERRUN. ClearMeasurementBuffer leert auch ausstehende Daten und Diagnose. OFF/Backend- oder Konfigurationswechsel verwerfen alte ausstehende Daten, damit kein alter Zustand nachtraeglich erscheint. Bereits gespeicherte Historie bleibt erhalten.

Die Hauptschleife uebernimmt pro Execute hoechstens die FIFO-Kapazitaet und zeigt den letzten vollstaendigen Messwert. AUTO-Range wird aus dem uebernommenen Wert aufgeloest, die neue Konfiguration gilt ab dem folgenden Erfassungszyklus. Nach Einstellungen beginnt ein neuer Paarzyklus; der asynchrone Betrieb erzeugt keine sofortige kuenstliche Messung. Die synchrone Schnittstelle bleibt fuer bestehende Host-Tests und als gemeldeter Startfehler-Fallback erhalten.

Touch wird unabhaengig von Messung und Frame-Takt alle 10 ms abgefragt, neue Frames fruehestens alle 33 ms. Dies sind Zeitziele, keine garantierten Bild-/Touchraten: EVE/SPI-Zeichenzeit kann die GUI weiter verzoegern, aber der hoeher priorisierte Erfassungsthread kann sie unterbrechen. Keine zusaetzliche Mittelung oder Reduktion gespeicherter Messwerte. Der reale ADC-Treiber fehlt weiterhin; vor parallelem ADC-SPI-Zugriff ist eine gemeinsame Transaktionsarbitrierung mit EVE zwingend erforderlich. Die bestehende ms-Zeitstempelaufloesung und das Simulationsrauschmodell bleiben unveraendert.


### Automatisches Zeitfenster passend zum Messpuffer - 24.11.2026
Ersetzt das feste automatische 100-s-Fenster in Main-Graph und MENU/Graph: Die Fensterbreite wird aus (Pufferkapazitaet - 1) * nominaler U/I-Paarzeit abgeleitet und auf eine passende 1/2/4/5-Dekadenstufe abgerundet. Bei vollem Puffer begrenzt zusaetzlich die tatsaechlich erhaltene Zeitspanne die Breite; so funktioniert auch ein NPLC-Wechsel mit gemischten Abstaenden im Verlauf. Der Zeitursprung bleibt der Pufferstart, die Achse wandert weiterhin in Schritten von einer Rasterteilung.

Bei 1024 Punkten: 0,01 / 0,1 / 0,5 / 1 / 5 / 10 NPLC ergeben nominal 0,4 / 4 / 20 / 40 / 200 / 400 Sekunden Fensterbreite. Bei 1 NPLC zeigt der Start 00:00 bis 00:40 mit 4 s/div. Beim Ueberschreiben alter Werte liegen diese links ausserhalb des automatischen Fensters. Keine Aenderung an Messrate, Speichertiefe oder Erfassungsdaten.

SCALE kennzeichnet die automatische Breite als Auto window. Eine ausdrueckliche Window-/Min-span-Eingabe bleibt auch bei NPLC-Wechseln und Clear erhalten; manuelles X/Pan/Zoom bleibt beim normalen Nachzeichnen erhalten. AUTO SCALE in der Zeitansicht aktiviert die passende automatische Breite wieder. Clear setzt den Zeitursprung weiterhin auf null. AUTO SCALE in I-V veraendert die Zeitfensterwahl nicht.


### Vollbild-Graph: automatische Y-Achse ohne wachsenden Leerraum - 24.11.2026
MENU/Graph passt automatische Y-Grenzen jetzt aus den aktuell sichtbaren Messwerten ein. Die bisherigen bereits gerundeten Achsengrenzen werden bei einer Erweiterung nicht erneut als Messwerte verwendet; dadurch addiert sich kein zusaetzlicher Leerraum. Veraltete grosse Skalen werden bei mindestens 25 % kleinerem passenden Raster zurueckgenommen. Bei mindestens zwei freien Rasterteilungen auf einer Seite und einer moeglichen Verschiebung um mindestens eine Teilung wird neu positioniert. Kleine Aenderungen innerhalb einer weiterhin passenden Skala bleiben ruhig.

Dies ersetzt das bisherige ausschliessliche Erweitern bis zum manuellen AUTO SCALE. Manuelle Y-Achsen, X-Zeitfenster und die engere Main-Graph-Skalierung bleiben erhalten. Regression prueft entfernte Extreme, verschobene Wertebereiche, stabile Wiederholungen, wiederholte obere Grenzueberschreitungen ohne aufsummierte Randerweiterung und manuelle Achsen.


### SYSTEM Settings: KeyClick und Backlight - 25.11.2026
SystemClass besitzt UserInterfaceSettings und validiert Helligkeit 5..100 Prozent sowie Dimmerzeit 0..1440 ganze Minuten. Konstanten werden zentral in UserInterfaceSettings.h definiert. Defaults: KeyClick ON, Backlight 40 Prozent (nahe bisherigem PWM-Wert 50/128), Dimmer 30 Minuten. Keine Flash-Persistenz in diesem Schritt; Neustart/Reset stellt Defaults her.

RegisterUserActivity aktualisiert die Inaktivitaetszeit und hebt Dimmen auf. UpdateUserInterface dimmt nach Ablauf auf den minimalen Backlightwert, ohne die konfigurierte Normalhelligkeit zu aendern. Der abgedimmte Zustand bleibt bis zur Bedienung bzw. einer Settings-Aenderung bestehen, auch ueber den millis-Ueberlauf. 0 Minuten deaktiviert den Dimmer. Alle erkannten Touchkontakte inkl. Multitouch zaehlen als Aktivitaet. WakeTouchGate konsumiert die erste komplette Beruehrung inklusive Loslassen; erst die naechste kann GUI-Aktionen ausloesen. Kein Einfluss auf Ausgang oder Erfassung.

BeeperClass liest die KeyClick-Freigabe ueber einen im Setup registrierten Callback direkt aus SystemClass. Nur Click wird unterdrueckt; Error und direkte Beep-Aufrufe bleiben verfuegbar. GDClass kapselt Backlight-PWM-Konvertierung 0..100 Prozent auf EVE 0..128 und schreibt nur bei geaendertem Hardwarewert. Der Cache ist Hardwarezustand, keine zweite Einstellungsquelle. Alle EVE-Zugriffe bleiben im Hauptthread.


## Geplante RTC und Kalenderzeit - 25.11.2026
Die fruehere Aussage, dass vorerst keine Mainboard-Aenderungen geplant sind, wird fuer folgende Erweiterung ergaenzt: Der Nutzer plant den DS3231 und liefert einen neuen Schaltplan zur gemeinsamen Pruefung. Bis dahin bleibt die bisherige Schaltplanbasis fuer implementierte Hardwarefunktionen massgeblich. Busadressen, Versorgungskonzept und offene Pruefpunkte stehen zentral im RTC-Abschnitt von Hardware.md.

SYSTEM > Settings enthaelt Date / Time als deaktivierte Vorbereitung (Unavailable / RTC not connected). Kein RTC-Treiber, keine I2C-Zugriffe und keine fiktive Kalenderzeit in diesem Schritt. Nach der Schaltplanpruefung sollen Datum/Uhrzeit und deren Gueltigkeitsstatus zentral ueber SystemClass und einen RTC-Hardwarezugriff gelesen/gesetzt werden; die GUI erhaelt keine eigene Uhr. RTC-Fehler bzw. ungueltige Zeit muessen erkennbar bleiben. Kalender-/Schaltjahrvalidierung, OSF-Auswertung und Zeitformat werden bei der Anbindung festgelegt.

Messintervalle, relative Messzeitstempel, Beeper und Backlight-Inaktivitaet bleiben auf monotonen Systemtimern. Stellen der Kalenderuhr darf diese Ablaeufe nicht beeinflussen. Die bisherige Laufzeitanzeige im Header bleibt bis zur RTC-Anbindung unveraendert; keine Kalenderzeit aus der Laufzeit ableiten.


## Source Settings ownership - 25.11.2026
SourceSettingsPage is a read-only presentation of SystemClass state with callbacks for editor requests. Application wiring opens the existing SourceModePopup, SourceRangePopup, NumberPadPopup (LimitValue) and OutputOffStatePopup instances. Source mode determines the displayed source range and opposite compliance quantity. No page-local domain settings, duplicate editors, or additional output enable action.

## Verbindliche Core-Aufteilung – Nutzerfreigabe 27.11.2026

Ein RP2040/Pico, zwei Cores. Diese Festlegung ersetzt den vorangegangenen temporären Single-Core-Timerbetrieb:

- **Core 0:** EVE/GDClass vollständig, Display/Touch/Menüs, USB, Beeper und später W5500. SystemClass bleibt die einzige GUI-Schnittstelle für Domain-Werte und Eigentümer der Messhistorie. Veröffentlichung neuer Messwerte, Statistik, Graphaufbereitung und Zeichnen bleiben synchron auf Core 0.
- **Core 1:** aktuell Simulation; später reale ADC/DAC-Erfassung, Sweep-Timing und Hardware-Bereichsumschaltung. Keine EVE-/GUI-/USB-Aufrufe, keine direkte Änderung an SystemClass oder der Historie.
- **SPI1 ausschließlich Core 0:** EVE/W5500, SCK GP14, MOSI GP15, MISO GP12. **SPI0 ausschließlich Core 1:** ADC/DAC über Isolator, SCK GP18, MOSI GP19, MISO GP16, DAC-CS GP10, ADC-CS GP11. **I²C0 ausschließlich Core 1:** SDA GP4, SCL GP5, MCP23017. Der aktuelle Simulator aktiviert diese künftigen SMU-Schnittstellen nicht. Teilnehmer eines Busses brauchen weiterhin nacheinander abgeschlossene Transaktionen mit passenden SPI-Einstellungen/CS; kein Buszugriff vom jeweils anderen Core.
- Eine einzige **256 × 16 Byte = 4 KiB Messwert-FIFO** transportiert Samples Core 1 → Core 0. Die 1024-Punkte-Historie bleibt auf Core 0; keine zweite vollständige Historie. Bei voller FIFO ältesten Eintrag verwerfen, neuesten aufnehmen und Verlust zählen. Das ist keine Garantie lückenloser Sweep-Aufzeichnung.
- Pico-SDK-Mutex (hardwaregestützt) schützt kurze FIFO-/Steuerzugriffe. Simulation und später Hardware-I/O laufen außerhalb der Sperre; kein Zeichnen, Warten auf Messung oder USB unter der Sperre. Die lokale Interruptsperre des Beepers ist dafür kein Ersatz.
- Einstellungen gehen als vollständiger aktueller Datensatz in einer kleinen separaten Mailbox an Core 1. Noch nicht angewendete Sollwerte dürfen durch neuere ersetzt werden. OFF ist zusätzlich verriegelt, bis Core 1 es übernommen hat; OFF→ON zwischen zwei Worker-Aufrufen geht damit nicht verloren. CLEAR leert die FIFO und verwirft durch eine Revision auch bereits laufende Ergebnisse. Requested/Applied-Revision quittieren die Übernahme, nicht einen künftig physisch abgeschlossenen Hardwarevorgang.
- Start/Stop eines realen Sweeps und reale Hardwaretreiber existieren noch nicht. Diese diskreten Aktionen später als ausdrücklich quittierte Befehle ergänzen; nicht als überschreibbaren Sollwert in die Mailbox stecken.
- Kein zusätzlicher dynamischer Messpuffer. Für Core 1 wird ausdrücklich ein separater 8-KiB-Stack reserviert (`core1_separate_stack`), damit Core 0 seinen bisherigen Stack behält.
