# A-SMU – Hardwareübersicht

Stand: 20.11.2026

## Aktueller Aufbau und Entwicklungsweg

Zuerst wird die gesamte GUI auf dem Raspberry Pi Pico mit echtem Display und Touch fertiggestellt. Die SMU-Elektronik wird durch einen Hardware-Simulationslayer ersetzt. Danach folgt die Anbindung der realen SMU-Hardware.

Zielarchitektur: **GUI ↔ SystemClass ↔ Hardware-Schnittstelle ↔ Simulation / reale SMU-Hardware**.

`SystemClass` bleibt Eigentümer aller Geräteeinstellungen und Messwerte. Display und Touch werden real betrieben. Messwerterfassung, Sollwertausgabe, Bereichsumschaltung, Ausgang und Fehlerzustände sollen über die Simulation abgebildet werden.

**Implementierungsstand:** Die frühere Prototyp-Anbindung ist entfernt. Die vorhandene Demo-Messwertsimulation in `src/Core/System.cpp` bleibt aktiv. Sie erzeugt zeitabhängige Last-/Messwerte, bildet aber Output-OFF, Stromquellenbetrieb und Compliance noch nicht vollständig ab. Eine austauschbare Hardware-Schnittstelle und das vollständige Simulationsmodell sind der nächste Entwicklungsschritt.

## Hardware für die GUI

| Komponente | Festlegung | Beleg / Status |
|---|---|---|
| Output-Bedienung | Frontplattentaster mit LED für Output ON/OFF | Nutzerfestlegung 19.11.2026; konkreter Typ, Anschluss und LED-Ansteuerung noch offen |
| Mikrocontrollerboard | Raspberry Pi Pico, RP2040 | Nutzerfestlegung; PlatformIO-Target `pico` |
| Displaymodul | Riverdi **RVT50UQFNWC03 V2** | Vom Nutzer am 19.11.2026 angegebene Bezeichnung; ersetzt die ältere Schreibweise ohne `U` im Backlog |
| Grafik / Touch | EVE-Anbindung des Displaymoduls | Bestehender EVE-Treiber; genaue Controller- und Touch-Ausführung nicht zusätzlich aus der Modellnummer abgeleitet |
| GUI-Auflösung | 800 × 480 | Aktuelles GUI-Layout und `EVE_RVT50`-Buildkonfiguration |
| Displaykommunikation | SPI1 gemäß CN4 plus CS und PDN | Verdrahtung direkt nach Schaltplan, wie vom Nutzer festgelegt |
| Displayversorgung / Stecker | Nach konkretem Modul und Schaltplan | Hier keine ungeprüfte Versorgungsspannung oder Steckerbelegung ergänzen |

Die aktuellen Software-Pinwerte stehen in `platformio.ini`. Sie sind kein Ersatz für den Schaltplan und wurden bei dieser Dokumentationspflege nicht geändert. Anschlüsse für die spätere SMU-Hardware werden ebenfalls direkt aus deren Schaltplan übernommen.

## Wichtige Komponenten der späteren SMU

**Aktualisierung 20.11.2026:** Die verbindliche Schaltplanbasis mit Bereichen, PI-Konfigurationen, ADC/DAC, Referenz und Steuerleitungen steht in [system.md](system.md). Sie hat Vorrang vor der nachfolgenden älteren Backlog-Zusammenfassung.

Diese Übersicht übernimmt die folgenden Angaben aus `smu_backlog.md` (Stand 30.10.2026). Sie ist eine Orientierung, keine freigegebene Bestellstückliste. Konkrete Bestückung und Bauteilreferenzen sind am aktuellen Schaltplan/PCB abzugleichen. Referenzbezeichner gelten jeweils nur innerhalb ihrer Baugruppe.

| Funktionsblock | Bauteile / Baugruppen | Dokumentierter Stand |
|---|---|---|
| Leistungsendstufe | IRFP250N / IRFP9240 | Entwurfsfestlegung |
| Endstufentreiber | BC547B / BC557B | Entwurfsstand |
| Elektronische Ausgangssperre Z_HIGH | DG419, im geprüften Blatt DG419BDY-T1-E3 | Zusätzlich zur Relaistrennung |
| Analoge Regelung / Signalaufbereitung | OPA192, OPA172; OPA4192 für dokumentierte Mehrfach-OPV-Stufen | Endgültige Zuordnung je Stufe am Schaltplan prüfen |
| Strommess-Multiplexer | ADG1209; DG409 als diskutierte Alternative | Endgültige Umsetzung abgleichen |
| FORCE-Ausgangsrelais | Omron G2RL-2 DC12, K5 | Backlog Abschnitt 5.4 |
| Local-/Remote-Sense-Relais | Omron G6S-2F DC12, U73 | Backlog Abschnitt 5.4 |
| Relaistreiber | BSS138 | Dokumentierter Power-Out-Entwurf |
| Shuntrelais | HE3621A6012 / Pickering 109P-1-A-12/2D als besprochene Kandidaten | Keine pauschale Zuordnung zu allen Strombereichen |
| Temperaturmessung | TC74, U13/U15; Adressen 0x48/0x49 | Endstufen-Prüfregister |
| Kühlung | 2 × OMNI-UNI-41-75, 60 × 60-mm-PWM-Lüfter | Geplant; konkrete Montage/Auslegung offen |
| ADC | AD7177-2BRUZ-RL7, U165 | Mainboard Blatt 5; Anbindung folgt nach GUI/Simulation |
| Spätere SPI-Isolation | ADuM4154 | Kandidat für ADC-Anbindung |
| DAC | DAC80004IPWR, U5 | Mainboard Blatt 4; A/B für V_SET, C/D für I_SET |
| Präzisionsreferenz | ADR1399KHZ, U36; Erzeugung von ±2,5 V | Mainboard Blatt 8; ersetzt ADR4525-Kandidatenangabe |
| Weitere Steuerung | Zweiter RP2040 für SMU-Ansteuerung | Früheres Entwicklungsziel; für aktuellen GUI-Aufbau nicht erforderlich |
| Erweiterungen | MCP23017; AD9834 mit Komparatoren | Besprochene Funktionsblöcke, nicht Voraussetzung für GUI-Entwicklung |
| Digitalversorgung | IRM-15-12, IRM-10-3.3 | Netzteil-Entwurf |
| Analog-/Leistungsversorgung | Analogtrafo, HV-Ringkerntrafo, Gleichrichtung und Regler; 7805; DN2540/TL431 für ±15V0 | Details in Backlog Abschnitt 6 |

## Bereiche und Potentialdomänen

Verbindliche Nutzerkorrektur vom 20.11.2026; ersetzt die veraltete Texttabelle auf Mainboard-Blatt 6. Die Shuntwerte bleiben unverändert:

| Strombereich | Effektiver Shunt | Bestückungspfad | Signal |
|---|---|---|---|
| ±1 A | 1 Ω | R67/R20/R21/R65 | I_RANGE_1_EN |
| ±100 mA | 10 Ω | R4/R52/R59/R60 | I_RANGE_2_EN |
| ±1 mA | 1 kΩ | R19/R24/R49/R51 | I_RANGE_3_EN |
| ±10 µA | 100 kΩ | R58 | I_RANGE_4_EN |

Nach Nutzerkorrektur vom 20.11.2026 verwendet die GUI 10 µA / 1 mA / 100 mA / 1 A, zusätzlich AUTO. Spannung bleibt 5 V / 30 V. Die abweichende Schaltplan-Texttabelle ist veraltet; siehe system.md. Reale Skalierung und Schutz bleiben zu prüfen.

Potentialdomänen laut korrigierter Hardwaredokumentation: floating +15 V / −15 V / +5 V gegen AGND; digital +12 V / +3,3 V gegen DGND; Leistung +HV / −HV gegen HV_GND; +15V0 / −15V0 gegen HV_GND. AGND ist im dokumentierten Endstufenkonzept über R150 mit AMP_OUT verbunden. Diese Bezugspunkte nicht mit älteren vereinfachten README-Angaben vermischen.

## Dokumentation und Pflege

- [system.md](system.md): Systemregeln, Zustandsverwaltung und Softwarebereiche.
- [ux.md](ux.md): Bedienkonzept und GUI-Struktur.
- [smu_backlog.md](smu_backlog.md): Hardwareentscheidungen, Prüfnachweise, Kandidaten und historische Entwicklung.
- [README.md](README.md): Projekteinstieg und ursprüngliche Architekturübersicht.
- [AGENTS.md](AGENTS.md): Entwicklungsregeln.
- Schaltpläne und Boardunterlagen: `doc/hardware/` und `doc/Schematik/`.

Projekt-Markdown-Dateien liegen im Repository-Hauptverzeichnis. Neue Komponentenentscheidungen mit Datum und Quelle ergänzen; Kandidaten erst nach einer tatsächlichen Festlegung als ausgewählt kennzeichnen. Die genaue Displaybezeichnung stammt vom Nutzer; zusätzliche Herstellerdaten wurden hier nicht recherchiert.

## Display-Anschlussabgleich Mainboard – 19.11.2026

Quellen: `doc/hardware/Main_Board/SCH_MAIN_BOARD 2.0_12-RP2040_2026-11-10.svg`; ergänzend extrahierte PCB-Pads des Mainboard-Prüfstands 16.11.2026 unter `work/main-review-final-1116/pads.json` im übergeordneten Projektverzeichnis.

**Offene Abweichung:** Am Pico U32 heißen die GP16/18/19-Netze SPI0_MISO/CLK/MOSI. Der Displaystecker CN4 verwendet jedoch SPI1_MISO/CLK/MOSI. Die aktuelle Firmware konfiguriert das Display auf SPI0. Eine Verdrahtung nach CN4 passt daher nicht zur aktuellen Display-Firmware.

| Signal | CN4-Pin | Netz am CN4 | Pico GPIO / physischer Pin | Aktuelle Firmware GPIO |
|---|---|---|---|---|
| SCK | 3 | SPI1_CLK | GP14 / 19 | GP18 |
| MISO | 4 | SPI1_MISO | GP12 / 16 | GP16 |
| MOSI | 5 | SPI1_MOSI | GP15 / 20 | GP19 |
| CS_N | 6 | DISP_CS_N | GP17 / 22 | GP17 |
| INT | 7 | DISP_INT | GP21 / 27 | nicht verwendet |
| PD_N | 8 | DISP_PD_N | GP20 / 26 | GP20 |

BUZZER liegt auf GP9 / Pico-Pin 12 und stimmt mit der Firmware überein. Vor einer Firmwareänderung klären, ob der aktuelle Aufbau die SPI1-Zuordnung des CN4 oder die SPI0-Zuordnung am Pico verwendet. Ein Wechsel erfordert auch die Auswahl der richtigen SPI-Instanz im EVE-Treiber, nicht nur andere Pin-Konstanten. Kein Schaltplan oder Firmware-Pin durch diesen Abgleich geändert.

### Umsetzung nach Bestätigung der Verdrahtung – 19.11.2026

Der Nutzer bestätigt die Verdrahtung gemäß CN4 an GP14/12/15. Firmware angepasst: `EVE_SPI=spi1`, SCK GP14, MISO GP12, MOSI GP15; CS GP17 und PD_N GP20 bleiben bestehen. GDClass initialisiert SPI1 über ein MbedSPI-Objekt mit den expliziten CN4-Pins statt über das Arduino-Standardobjekt SPI. Der EVE-Treiber verwendet dieselbe Instanz über EVE_SPI=spi1. INT wird weiter nicht benötigt. EVE_SOFT_RESET ermöglicht einen Software-Reset beim Warmstart ohne angeschlossene PD_N-Leitung. Der serielle Monitor meldet `EVE init status: 0` bei erfolgreicher Initialisierung. Die obige Vergleichstabelle beschreibt den Zustand vor dieser Änderung; der widersprüchliche SPI0-Netzname am Pico im Schaltplan ist dadurch nicht geändert.


### Software-Hardware-Layer – 22.11.2026
- Neuer Hardware::Backend-Vertrag in src/Core/Hardware/HardwareBackend.h, zunächst mit SimulationBackend. Keine Änderung an Verdrahtung, GDClass oder Display-/Beeper-Hardware; keine Wiederaufnahme des LTC2602/LTC2402-Prototyps.
- MENU → SYSTEM → Simulation konfiguriert statische ohmsche Last, OPEN/SHORT und verfügbare Messbereiche. Aktive Bereiche/Sollwert/Compliance bleiben auf Main. Standard: 1 kOhm, alle Bereiche verfügbar.
- Bei SIM OFF ist kein realer Treiber angeschlossen: NO HARDWARE, Ausgang OFF. Ein späterer realer Treiber muss den gemeinsamen Datensatz einschließlich CV/CC, Overrange und Fehler liefern und OFF vor dem Backend-Wechsel umsetzen.


### Kompakter Messpuffer: P/R dynamisch – 22.11.2026
Ersetzt die bisherige Speicherung von Leistung und Widerstand: Hardware::Measurement / BufferSample enthält nur noch U, I, Zeitstempel, CV/CC, Ausgangs-/Overrange-Flags und Fehlerstatus. Größe jetzt 16 statt 24 Byte; der Ringpuffer mit unverändert 1024 Punkten benötigt 16 statt 24 KiB. Auch alle Sample-Kopien in den Ansichten sind entsprechend kleiner.

GetPower() berechnet U·I und GetResistance() U/I bei Bedarf zentral im Hardware-Datentyp. Widerstandsgültigkeit wird ebenfalls abgeleitet (endliche Werte, |I| mindestens 1 nA, endlicher Quotient); ein eigener Gültigkeitswert wird nicht mehr gespeichert. Die Ansichten prüfen diese Gültigkeit und zeigen bei ungültigem R weiterhin Striche. Fehlerhafte Samples werden von den Auswertungen weiterhin ausgelassen.

Main, Statistik, Graph, Histogramm, Simulation, Reading Table und XY-Vorschau verwenden dieselben dynamischen Berechnungen. Statistik wertet P/R weiterhin pro einzelnem Messpunkt aus, niemals aus gemittelten U/I-Werten. Unbenutzte SetPower/SetResistance und die bisherige Aktualisierung abgeleiteter Cache-Werte wurden entfernt. Keine Änderung an Messrate, Rauschen, Fensterbreite oder Ringpufferkapazität. Host-Tests prüfen 16-Byte-Layout, dynamische Werte nach U/I-Änderung, Null-/ungültige Ströme sowie alle bisherigen Auswertungen.

Pico-Build nach dem Umbau: 158.132 statt 201.148 Byte statisch belegtes RAM (58,5 % statt 74,4 %). Ersparnis einschließlich aller Sample-Kopien: 43.016 Byte, rund 42 KiB. Build und Host-Tests erfolgreich.


### Vollständige Messpunkte als Pixelkurven – 22.11.2026
Die starke 32-Intervall-Verdichtung führte bei 100 s Fensterbreite zu nur wenigen dargestellten Werten in den ersten 10 s. Main-Graph, MENU/Graph und Reading-Table-Vorschau verwenden jetzt GDClass::DrawPlotTrace mit einem generischen PlotPointSource. Alle gültigen Samples (maximal 1024) werden gelesen und direkt in ein einfarbiges 1-Bit-Pixelbild gezeichnet. Keine Min/Max-Auswahl in diesen drei Ansichten; nahe Punkte können aufgrund der Displayauflösung natürlich auf dieselbe Pixelspalte fallen. Line/Points/Both, Datenlücken und Clipping bleiben erhalten.

PlotRaster enthält die wiederverwendbare, SMU-unabhängige Rasterisierung. GraphState::Trace liefert daraus nur Koordinaten und Gültigkeit aus den gespeicherten Daten. Das ältere MakePoints bleibt als begrenzte Hüllkurven-API erhalten, wird von den drei Ansichten aber nicht mehr verwendet. Drei bisherige PlotPoint-Arrays wurden entfernt.

GDClass verwendet einen gemeinsamen 25.600-Byte-Arbeitspuffer, maximal vier Kurven pro Frame, Bitmap-Handles 16–19 und zwei wechselnde RAM_G-Bänke (je vier Slots à 25.600 Byte, insgesamt Adressen 0 bis exklusiv 204800). Der Modal-Snapshot ab 0x40000 und ROM-Font-Handles 1/2 bleiben frei. L1-Bitmaps werden per SRC_ALPHA / ONE_MINUS_SRC_ALPHA eingefärbt; Nullbits sind transparent. Der aktive Bildspeicher wird während des nächsten Frame-Aufbaus nicht überschrieben. Die Displayliste wächst nicht mehr mit der Messpunktzahl. Der Bildtransfer verursacht zusätzliche SPI-Zeit, besonders bei vier Kurven.

Host-Tests prüfen alle 100 Punkte der ersten 10 s im Pixelbild, unveränderte Messdaten, Moduswahl, Clipping und Unterbrechungen durch ungültige Daten. EVE-Ausgabe, Farbkomposition und Touch-Reaktionszeit müssen nach Upload am Display geprüft werden.


### L1-Transparenz korrigiert – 22.11.2026
Die erste Bitmap-Version verwendete irrtümlich ONE/ONE-Blending und zeichnete dadurch eine gefüllte Fläche. FT813 L1 enthält ein Alpha-Bit und keine RGB-Farbbits. DrawPlotTrace verwendet jetzt SRC_ALPHA / ONE_MINUS_SRC_ALPHA: Nullbits erhalten Hintergrund und Raster, gesetzte Bits zeichnen die Kurvenfarbe. Quelle: FT81X Series Programmer Guide, BITMAP_LAYOUT-Formattabelle, https://brtchip.com/wp-content/uploads/Support/Documentation/Programming_Guides/ICs/EVE/FT81X_Series_Programmer_Guide.pdf . Die tatsächliche Ausgabe muss am Display nachgetestet werden.


### RAM-Ersparnis durch direkte Messpufferansichten – 24.11.2026

Main-Graph, MENU/Graph, Histogramm und XY-Popup besitzen keine Messpuffer-Kopien mehr. Die Reading Table verwendet statt 1024 kopierter Samples einen einzigen 50-Sample-Ausschnitt (800 Byte Messdaten). Der zentrale Ringpuffer bleibt bei 1024 Samples / 16 KiB. Der gemeinsame 25.600-Byte-Pixelpuffer in GDClass bleibt unveraendert.

Pico-Build nach dem Umbau: 108.052 statt 176.860 Byte statische RAM-Belegung (40,0 % statt 65,4 % von 270.336 Byte). Nettoersparnis: 68.808 Byte, rund 67,2 KiB. Dies ist die Build-Angabe, keine Laufzeitmessung des maximalen Stack-/Heap-Bedarfs. Host-Tests und Pico-Build erfolgreich; nicht auf das Geraet uebertragen.


### NPLC-Bedienung und Simulationskadenz � 24.11.2026
MEASURE > Settings bietet sechs NPLC-aequivalente Sinc5+Sinc1-Profile fuer 50 Hz, Standard 1 NPLC. Die Simulation verwendet die abgeleitete U/I-Paarzeit, begrenzt durch die Hauptschleife; es gibt weiterhin keinen realen AD7177-2-Treiber. Keine Aenderung an SPI, Schaltplan oder Kalibrierung. Die angezeigten Messungen/s sind nominale U/I-Paarraten, keine gemessene Systemleistung. Filterwirkung und NPLC-abhaengige Rauschreduktion sind noch nicht simuliert. Details und Zeitquelle stehen in system.md.

Pico-Build nach der NPLC-Seite: 108.836 Byte statisches RAM (40,3 %), zuvor 108.052 Byte; Zunahme 784 Byte. Host-Tests fuer Profile/Kadenz/Timerueberlauf, Auswahl/Navigation und bestehende Graph-/Bereichs-/Statistikfunktionen erfolgreich. Nicht auf das Geraet uebertragen; Sicht- und Touchpruefung am Display steht aus.

Keithley-Layout-Nachtrag 24.11.2026: Pico-Build erfolgreich, statisches RAM 109.156 Byte (40,4 %), +320 Byte gegenueber der ersten NPLC-Seite. Host-Test fuer modale Auswahl/Cancel, Bereichsaenderungen, Navigation und unveraenderten Ausgang erfolgreich. Kein Upload; Sicht-/Touchpruefung am Geraet ausstehend.


### Erfassungsthread fuer Simulation - 24.11.2026
Pico-Simulation jetzt mit Mbed-Ticker/EventFlags/Thread (AboveNormal), separater Modellinstanz und mutexgeschuetzter 256-Sample-Transportqueue. Keine ADC- oder SPI-Aufrufe im Worker; EVE bleibt im Hauptthread. Der spaetere reale ADC braucht vor parallelem Betrieb gemeinsame SPI-Arbitrierung und nachgewiesene Timinggrenzen. Hauptpuffer und Graph-Arbeitsspeicher bleiben unveraendert. Stack des Workers: statisch 4096 Byte; Transportdaten: 4096 Byte.

Pico-Build: 117.836 Byte statisch belegtes RAM (43,6 %), zuvor 109.156 Byte. Zunahme 8680 Byte inklusive Queue, Threadstack und Verwaltungsdaten. Kein Laufzeitnachweis fuer Stackreserve oder erreichbare Maximalrate; kein Upload. Host-Test prueft alle NPLC-Stufen, Erfassung ohne GUI-Abnahme, FIFO-Overflow, verpasste Termine, Timerueberlauf, OFF/Konfigurationswechsel, Clear und stabile BufferViews.

Automatisches Graph-Zeitfenster, 24.11.2026: Host-Graphsuite inkl. aller sechs NPLC-Stufen ueber drei Pufferumlaeufe, gemischter Zeitabstaende, manueller Fenster und Clear erfolgreich. Pico-Build erfolgreich: 117.852 Byte statisches RAM (43,6 %), +16 Byte. Keine neuen Messdatenpuffer, kein Upload.


### Backlight und KeyClick angebunden - 25.11.2026
Die bisherige feste REG_PWM_DUTY-Ausgabe wird durch GDClass::SetBacklightPercent ersetzt (EVE PWM 0..128). SystemClass liefert Normal-/Dimmerhelligkeit; Hardwarecache verhindert unnoetige SPI-Schreibzugriffe. Keine Pin-/Busaenderungen. Beeper-Klickfreigabe wirkt auf den vorhandenen GPIO-/Timeout-Pfad, Fehlerbeeps bleiben aktiv.

Pico-Build erfolgreich: 119.796 Byte statisches RAM (44,3 %), zuvor 117.852 Byte; Zunahme 1944 Byte fuer Seite/Dialog und Steuerzustand. Kein Upload. Sicht-/Touchpruefung des Sliders sowie reale PWM-Helligkeit und Aufwecken am Geraet noch pruefen. Host-Tests decken Grenzen, Dimmerablauf/Timerueberlauf, Wake-Geste, Klicksperre mit weiterhin aktivem Fehlerbeep, Slider-Endpunkte und NumberPad-Validierung/Cancel ab.


## Geplante Mainboard-Erweiterung: DS3231 RTC - 25.11.2026
Nutzerentscheidung: Auf dem Mainboard ist ein DS3231 vorgesehen. Der Nutzer erstellt den aktualisierten Schaltplan; danach erfolgt die gemeinsame Pruefung von Versorgung, Bus, Beschaltung und Layout. Der DS3231 ist noch kein nachgewiesener Bestandteil der bisherigen Schaltplanbasis vom 10.11.2026. Es wurden keine Schaltplan-/PCB-Dateien oder Pinzuordnungen geaendert.

| Baustein | 7-Bit-I2C-Adresse | Versorgung |
| --- | --- | --- |
| TC74A0-3.3VAT | 0x48 | geschaltete 3,3 V |
| TC74A1-3.3VAT | 0x49 | geschaltete 3,3 V |
| DS3231 | 0x68 | VCC: geschaltete 3,3 V; VBAT: dauerhaft versorgt |

Gemeinsamer SDA/SCL-Bus mit 100 kHz, begrenzt durch TC74. Ein gemeinsames Pull-up-Paar an geschalteten 3,3 V; Widerstandswerte werden anhand Buskapazitaet und vorhandener Pull-ups festgelegt. Alle direkt verbundenen Teilnehmer benoetigen denselben Massebezug. Gegen die bestehenden Analog-/Digital-/Endstufen-Potentialdomaenen am fertigen Schaltplan pruefen; keine Masseverbindung implizit ergaenzen. Die Versorgung des Pico und weiterer Busteilnehmer muss im ausgeschalteten Zustand ebenfalls beruecksichtigt werden, insbesondere USB-Versorgung/interne Pull-ups. SDA/SCL duerfen dann nicht von einer anderen Versorgung auf High gespeist werden.

Wichtig: Die DS3231-I2C-Schnittstelle bleibt bei gueltiger VBAT auch ohne VCC zugaenglich. Rueckspeisungsvermeidung entsteht daher durch das gesamte Versorgungskonzept, nicht durch eine angenommene Abschaltung seiner I2C-Pins. Nur VBAT erhaelt die dauerhafte Versorgung. Backup-Quelle, genaue DS3231-Variante, Abblockung und eventuelle INT/SQW-/32-kHz-/RST-Anbindungen werden im neuen Schaltplan geprueft; keine neuen GPIOs reserviert.

Quellen: [Microchip TC74 DS21462D](https://ww1.microchip.com/downloads/en/devicedoc/21462d.pdf), Adresstabelle und 100-kHz-Grenze; [Analog Devices DS3231](https://www.analog.com/media/en/technical-documentation/data-sheets/DS3231.pdf), Adresse 0x68, Versorgung und I2C-Interface bei VBAT.

## Arduino-Pico-Migration – 27.11.2026

Auf Nutzerauftrag ersetzt Arduino-Pico von Earle Philhower den bisherigen ArduinoCore-mbed. Es bleibt **ein RP2040/Pico**; Core 1 wird in diesem Schritt nicht gestartet. Ein RP2350-Wechsel ist nicht Teil dieser Migration.

- PlatformIO verwendet die in der [Arduino-Pico-Dokumentation](https://arduino-pico.readthedocs.io/en/latest/platformio.html) beschriebene Plattform von maxgerhardt, auf Commit `5d4561a05e3b212660ac6fdd3fbfb328d1988aa1` fixiert, und `board_build.core = earlephilhower`. Die Plattform legt ihrerseits den Framework-Commit fest. CPU-Takt bleibt 133 MHz.
- EVE/GDClass, Touch, Menüs, USB und Zeichnen bleiben auf Core 0. SPI1 wird über das Arduino-Pico-Objekt `SPI1` initialisiert; CN4-Pins bleiben GP14/12/15, CS GP17, PDN GP20. EVE 5.x verwendet den Mbed-Namen `_spi_init`; ein dokumentiertes Build-Alias bildet ihn auf die Pico-SDK-Funktion `spi_init` ab. Keine Änderung am heruntergeladenen Bibliothekscode erforderlich.
- Der Mbed-Simulationsthread mit 4096 Byte eigenem Stack entfällt. Ein Pico-SDK-Repeating-Timer auf Core 0 ruft den begrenzten Simulationsschritt auf. Pollintervall ist das schnellste konfigurierte Messpaarintervall (derzeit 400 µs); andere NPLC-Intervalle können dadurch um weniger als ein Pollintervall verzögert sein, zusätzlich zu realer IRQ-Latenz. Verpasste Messperioden werden gezählt, nicht als erfundene Samples nachgeholt.
- Der Timer führt nur Simulation aus, keine SPI-, USB-, EVE-, Heap- oder Historienzugriffe. Die 256 × 16 Byte große Transport-FIFO bleibt erhalten. Kurze Interruptsperren mit Wiederherstellung des vorherigen Zustands schützen sie gegen gleichzeitige Main-/IRQ-Zugriffe. Das ist **keine Multicore-Synchronisation** und muss vor Nutzung von Core 1 ersetzt werden.
- Nur SystemClass veröffentlicht Messwerte in die bestehende Historie; BufferView und synchrone Darstellung bleiben unverändert. Keine zusätzliche vollständige Messhistorie, kein Core-1-Stack.
- Beeper-Abschaltung erfolgt über einen Pico-SDK-Alarm; Retrigger/Stop sind gegen den IRQ abgesichert. Bei fehlendem Alarmplatz bleibt der Beeper aus. Falls der Erfassungstimer nicht startet, bleibt der bisherige synchrone Fallback aktiv, mit serieller Fehlermeldung.

### Test am Gerät

Die erste Firmware nach dem Core-Wechsel per BOOTSEL/UF2 aufspielen. Beim USB-Anstecken BOOTSEL gedrückt halten und die neu gebaute `firmware.uf2` auf `RPI-RP2` kopieren. Windows kann eine neue COM-Portnummer vergeben; `monitor_port = COM5` gegebenenfalls lokal anpassen.

1. Seriell bei 115200: `A-SMU Arduino-Pico / Core 0 / simulation`, `EVE init status: 0`, `Setup done` prüfen.
2. Kaltstart und Neustart, Display/Backlight, Touch, Menüs und Dialoge testen.
3. Klick-/Fehlerton einschließlich rasch wiederholter Bedienung prüfen; kein hängender Ton.
4. Simulation, Graph/Zoom, Histogramm, Statistik und Reading Table prüfen. NPLC bis 0.01 ändern und unter Zeichnen auf `ACQ OVERRUN` achten: echte Timer-/Renderlast und USB sind nur am Gerät abschließend prüfbar.
5. SIM OFF muss weiterhin NO HARDWARE ergeben. Reale ADC/DAC-Treiber und Sweeps werden durch diese Migration nicht hinzugefügt.

Später: Core 1 übernimmt ADC/DAC und Sweep-Timing; Messdaten/Befehle erhalten eine coresichere Übergabe. SPI0 gehört zum ADC/DAC-Pfad, SPI1 zu Display/W5500; noch keine parallelen Busnutzer aktivieren. Statistik/Graphvorbereitung erfordern zusätzlich eine klar geregelte Historien-Lebensdauer.

### Verifikation der Migration

Build am 27.11.2026 erfolgreich mit Arduino-Pico **6.4.0** (Framework-Commit `fd65f6d4ab168d4bb181a3709738fe353ac93585`). Log: `tmp/logs/arduino-pico-build.log`. Acht Host-Testgruppen erfolgreich: Pico-Timer/IRQ-Adapter, Beeper, Acquisition, Hardware-Simulation, Strombereiche, Statistik, Graph und Menünavigation. Die Tests verwenden SDK-Testdoubles; sie ersetzen keine IRQ-Latenz-/USB-/Displaymessung am Gerät.

PlatformIO: **85.620 Byte statisches RAM / 262.144 Byte (32,7 %)**, **162.076 Byte Flash / 2.093.056 Byte (7,7 %)**. ELF-Abgleich: `.data` 9.576, `.bss` 76.044 Byte; zusätzlich 192 Byte RAM-Vektortabelle. Der Linker trennt 8 KiB oberen RAM für den Stackbereich ab; Core 1 bleibt ungestartet. Der ausgewiesene Heapbereich umfasst 176.332 Byte vor Laufzeit-Allokationen. Die Prozentangabe ist keine Messung des freien RAM im Betrieb. Kein zusätzlicher Worker-Stack oder vollständiger Messpuffer wurde angelegt.

Die EVE-Bibliothek meldet weiterhin Register-Makro-Neudefinitionen und pedantische Warnungen aus Pico-SDK-Headern; der Build ist ohne Fehler abgeschlossen. `UiTiming.h` verwendet jetzt die Arduino-Pico-USB-API mit begrenzter TX-Portion statt Mbed `send_nb`.

Testdatei: `.pio/build/pico/firmware.uf2` (348.672 Byte). Kein Upload durchgeführt. Hardwaretest steht aus.

### Korrektur: fehlende Zahlen nach Core-Wechsel – 27.11.2026

Am Gerät erschien im Main-Messfeld nur die Einheit (z. B. mV). Die erste Arduino-Pico-Firmware enthielt nur die Integer-Variante von snprintf, ohne `_printf_float`; `%f` lieferte dadurch keinen Zahlentext. `platformio.ini` erzwingt jetzt mit `-Wl,-u,_printf_float` die Float-Ausgabe, entsprechend der Arduino-IDE-Linkkonfiguration des installierten Cores. Kein Eingriff in Simulation oder Messwertübergabe.

Neubau erfolgreich; Float-Konvertierung im ELF anhand `_printf_float` und `_dtoa_r` geprüft. Statisches RAM unverändert 85.620 Byte (32,7 %), Flash jetzt 169.140 Byte (8,1 %, +7.064 Byte). Log: `tmp/logs/arduino-pico-float-build.log`. Die UF2 wurde neu erzeugt; die vorherige Dateigröße oben beschreibt den ersten Migrationsbuild. Anzeige am Gerät nach erneutem PlatformIO-Upload prüfen. Host-Tests allein konnten diesen Linkerfehler nicht erkennen, weil sie die C-Bibliothek des PCs verwenden.

### Umsetzung der Core-Aufteilung – 27.11.2026

Nutzerfreigegebene Architektur in system.md/AGENTS.md festgehalten. `setup1` meldet Bereitschaft, `loop1` führt SimulationAcquisition aus; der frühere Erfassungs-Timer auf Core 0 entfällt. Core 1 prüft mit 50 µs Pause die NPLC-Fristen, erzeugt maximal einen tatsächlichen Messpunkt je Schritt und zählt verpasste Perioden. Core 0 wartet beim Start höchstens 500 ms auf Bereitschaft und meldet sonst den synchronen Simulationsfallback.

Messwertberechnung und FIFO-Veröffentlichung sind getrennt. Der SDK-Mutex umfasst nur kleine Datensatz-/Index-/Statusoperationen; CLEAR verwirft die FIFO in konstanter Zeit. Eine Revisionsprüfung verwirft Ergebnisse, deren Konfiguration während der Berechnung geändert oder gelöscht wurde. Der Konfigurationsschritt unter der Sperre kopiert ausschließlich Simulationsparameter; reale Backend-Aufrufe müssen bei späterer Ergänzung außerhalb dieser Sperre erfolgen. Eigenständige SPI-Treiber, reale Sweep-Funktion und Ethernet sind damit noch nicht implementiert.

Build erfolgreich: **85.672 Byte statisches RAM (32,7 %)**, 165.612 Byte Flash (7,9 %). Statisches RAM +52 Byte gegenüber dem korrigierten Single-Core-Build. Zusätzlich reserviert Arduino-Pico zur Laufzeit **8.192 Byte für Core 1**; dieser Betrag ist in der statischen RAM-Zahl nicht enthalten. Core 0 behält seinen bisherigen Stack. Die FIFO bleibt 4 KiB, keine neue vollständige Historie. Log: `tmp/logs/multicore-build.log`.

Host-Test `run_multicore.cmd`: echtes Producer-/Consumer-Threadpaar mit Mutex-Testdouble, 2000 Konfigurationswechsel, FIFO-Überlauf/Drop-oldest, OFF→ON, CLEAR, Revisionsquittierung und gezielt erzwungenes CLEAR zwischen Berechnungsvorbereitung und Veröffentlichung. Hardwaretest der echten Spinlocks, Stackauslastung und Latenzen steht aus.

Nach PlatformIO-Upload seriell auf `Core 1 acquisition ready` achten; Main/Graph/Histogramm/Tabelle, schnelle Sollwertwechsel, SIM OFF/ON und CLEAR bei laufender Darstellung testen. Bei NPLC 0.01 auf ACQ OVERRUN achten. Keine Übertragung auf das Gerät durch den Agenten durchgeführt.

Verifikation abgeschlossen: alle acht Host-Testgruppen (Multicore, Acquisition, Beeper, Hardware, Statistik, Graph, Menünavigation, Strombereiche) erfolgreich. ELF enthält `setup1`, `loop1`, `multicore_launch_core1_with_stack` sowie weiterhin `_printf_float`. Der Hardwaretest auf dem Pico bleibt erforderlich; es wurde nichts hochgeladen.
