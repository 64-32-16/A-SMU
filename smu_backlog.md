# A-SMU – Backlog, Architektur und verbindlicher Entwicklungsstand

## Software-/Schaltplanbasis – 20.11.2026

Der unveränderte Mainboard-Schaltplan ist laut Nutzer die Basis für die weitere Entwicklung. Die konsolidierte aktuelle Beschreibung von Bereichen, PI-Regler, ADC/DAC, Referenz und Steuerleitungen steht in [system.md](system.md). Nutzerkorrektur vom 20.11.2026: Die GUI verwendet **1 A / 100 mA / 1 mA / 10 µA** bei unveränderten Shunts 1 Ω / 10 Ω / 1 kΩ / 100 kΩ. Dies ersetzt die veraltete Texttabelle im I_Shunt-Blatt sowie die Bereichsfestlegung vom 29.10.2026 weiter unten. Widersprechende ältere Kandidaten und Simulationseinstellungen im folgenden Verlauf gelten nicht automatisch für diese Basis.


## Aktuelle Ergänzung – 19.11.2026

Nutzerfestlegung: zuerst die gesamte GUI mit Hardware-Simulationslayer fertigstellen, danach reale SMU-Anbindung. Pico/RP2040 und Display/Touch bleiben reale Hardware. Displaybezeichnung: **RVT50UQFNWC03 V2**. Komponentenübersicht: [Hardware.md](Hardware.md). Die nachfolgenden älteren ADC-/DAC-Kandidaten sind keine Voraussetzung für den GUI-Aufbau. Die bisherige Demo-Simulation wird zum separaten Layer weiterentwickelt.


**Stand:** 30.10.2026 · **Dokument:** `smu_backlog.md` · **Gerät:** Analog Source-Measure Unit (A-SMU) · **Status:** Entwicklung / LTspice-Vorvalidierung

> **Single Source of Truth:** Diese Datei ist die zentrale Dokumentation für technische Festlegungen, offene Entscheidungen, Bauteilwahl, Simulationsergebnisse und nächste Schritte der A-SMU. Der **aktuelle Teil** ist maßgeblich. Der unverändert erhaltene **historische Anhang** dient nur der Nachvollziehbarkeit und darf bei Widersprüchen nicht als aktuelle Bestückung verwendet werden. **Simulationserfolg ≠ Hardwarefreigabe.**

## 0. Dokumentationsregeln und Status

- **FESTGELEGT** = vereinbarte Entwicklungsrichtung/Dimensionierung; bei Hardware noch zu verifizieren.
- **SIMULIERT** = am genannten LTspice-Modell und den dokumentierten Testbedingungen beobachtet, nicht am realen Gerät gemessen.
- **KANDIDAT** = besprochene, noch nicht verbindlich gewählte Lösung oder Stücklistenposition.
- **OFFEN** = muss noch entschieden, dimensioniert oder getestet werden.
- **HISTORISCH/ERSETZT** = ehemals gültige Angabe; nicht ohne neue Freigabe übernehmen.
- Bei künftigen Änderungen jeweils **Datum, betroffenen Funktionsblock, neue Entscheidung, Mess-/Testbeleg und offene Auswirkungen** ergänzen. Zahlen aus Tests immer mit Modell, Last und Rahmenbedingungen angeben.

### 0.1 Wichtigste Änderungen gegenüber dem Backlog vom 24.10.2026

1. **Aktuelle Nutzerfestlegung vom 29.10.2026 – ±30-V-Version:** ±1 A / **1 Ω / 8 W** → `I_RANGE_1_EN`; ±10 mA / **10 Ω / 0,5 W** → `I_RANGE_2_EN`; ±100 µA / **1 kΩ / 0,25 W** → `I_RANGE_3_EN`; ±1 µA / **100 kΩ / 0,25 W** → `I_RANGE_4_EN`. Ersetzt als aktuelle Hardware-/Bereichszuordnung die am 27.10. dokumentierten ±100 mA/±1 mA/±10 µA an denselben kleinen Shunts. Verbindliche Tabelle und Überlastannahmen in Abschnitt 3.
2. **Versionsneutrale Steuerbezeichnungen:** `I_RANGE_n_EN` und `V_RANGE_n_EN` bezeichnen Hardwarekanäle, keine fest codierten Messwerte oder Widerstände. Zuordnung je ±30-V-/±140-V-Gerätevariante separat dokumentieren. Ein geplanter 1-MΩ-Shunt ist keine Änderung der Nummernbedeutung; konkrete zusätzliche Bereichs-/MUX-Zuordnung bleibt offen. **±1 µA an 100 kΩ ist jetzt einer der vier regulären Bereiche**, nicht mit dem früher experimentellen ±1-µA-/1-MΩ-Test gleichsetzen.
3. **I_MON im bisherigen Simulationsaufbau (kein neuer Skalierungsentscheid vom 29.10.):** nominal **−2 V pro 1 V Shuntspannung** (Betragsverstärkung 2). Es gilt `V(I_MON) ≈ −2 × I_OUT × R_SHUNT`. Die frühere ±100-mV-Shuntannahme ist für diese Testmatrix ersetzt: bei Nennstrom fällt jetzt nominal **1 V** über dem Shunt ab.
4. **PI-Kompensation, Aktualisierung mit Codex am 26.10.2026:** Für ±1 A und ±100 mA bleibt **R1/R3/C1 = 2 kΩ / 10 kΩ / 1 nF**; ±1 mA vorläufig **2 kΩ / 22 kΩ / 47 nF**. Für ±10 µA ist **4,7 kΩ / 47 kΩ / 100 nF** der neue Kandidat für **5 V und 30 V**: bei **5 V / 100 nF Last** wurde eine deutlich bessere Dämpfung berichtet, **30 V noch gezielt zu prüfen**. Frühere 32/32-ohmsche-Tests im 10-µA-Bereich beziehen sich weiterhin auf **R3/C1 = 22 kΩ / 47 nF** und dürfen nicht auf den neuen Kandidaten übertragen werden.
5. **Grobtest der ohmschen Lasten:** 128/128 OK für die **vier regulären Bereiche**; zusätzlich 32/32 als OK gemeldet für 1 µA, allerdings mit widersprüchlichem Beschreibungstext im 1-µA-Protokoll – vor Aufnahme als verbindlich Netlist kontrollieren.
6. **Kapazitive Lasttests:** noch nicht abgeschlossen; letzter hier vorliegender Stand **74/128**, davon 63 OK, 2 Schwingverdacht, 4 Timeout, 5 falscher Arbeitspunkt. Einzelheiten in Abschnitt 9.
7. **Hardware-Endstufenbegrenzung:** im kleinen Bereich in den LTspice-Tests auf ungefähr **200 mA** modelliert (nicht auf exakt 100 mA), im 1-A-Bereich M3/M4 aktiviert. Diese 200-mA-Grenze ist **kein ausreichender Fehler-/Überlastschutz** für jeden Shunt.

### 0.2 Schaltungs-Prüfregister – laufend pflegen

**Stand: 29.10.2026.** Dieses Register dokumentiert, welche Funktionsblöcke tatsächlich betrachtet wurden und bis zu welcher Prüftiefe. Bei jeder weiteren Schaltungsprüfung Datum, konkrete Revision/Datei, Ergebnis, offene Punkte und gegebenenfalls ersetzte Aussagen aktualisieren. Nicht allein aus „besprochen“, einer Zeichnungsnotiz „geprüft“ oder einer Teilprüfung einen vollständigen Prüfstatus ableiten.

**Prüfstatus:** **SCHALTPLAN** = visuell/rechnerisch geprüft, keine Hardwarefreigabe; **TEILPRÜFUNG** = nur genannter Teilaspekt; **DOKUMENTIERT** = Ergebnis aus bestehendem Backlog übernommen, hier nicht erneut getestet; **OFFEN** = noch kein vollständiger Prüfnachweis. Simulation, ERC/Netzliste, PCB und Messung jeweils getrennt angeben. Die Floating-Randbedingung aus Abschnitt 4.1 ist bei jeder Potentialbewertung verbindlich.

| Datum / Beleg | Schaltungsteil | Prüfstand / Ergebnis | Noch offen |
|---|---|---|---|
| 28.10.2026; ENDSTUFE Blatt 1, Export 28.10.2026; Nutzerklärung; Abschnitt 4.1 | Floating-Konzept, R150, AGND/AMP_OUT | **SCHALTPLAN:** AGND = AMP_OUT über R150 ist beabsichtigt. Frühere Kurzschlussbeanstandung zurückgenommen. ±15 V und analog +5 V beziehen sich auf mitbewegte AGND. | Physische Versorgung, Isolation und Fehlerzustände im Gesamtsystem nachweisen. |
| 28.10.2026; ENDSTUFE Blatt 1 | Leistungsstufe Q1/Q2 | **SCHALTPLAN:** Sourcefolger unter Floating-Randbedingung grundsätzlich plausibel; frühere pauschale ±15-V-Aussteuergrenze für den DUT zurückgenommen. | Reale ±30-V-Compliance bei Last und minimaler HV-Versorgung, DC-SOA, Transienten, Hardwaremessung. |
| 28.10.2026; ENDSTUFE Blatt 1 | Z_HIGH: U34/U36, DG419BDY-T1-E3 | **SCHALTPLAN:** Typ, Pinbelegung, VL = +5 V, S1/S2 und HIGH-Abschaltlogik geprüft. Lokale Abschaltung ±15 V grundsätzlich plausibel. DG419 selbst ist kein galvanischer Isolator. | ZHIGH_ON-Übergang von digital zu floating analog, Reset/Power-Off/Fremdspeisung, Restleckage, Ausgangsrelais-Sequenz. |
| 28.10.2026; ENDSTUFE Blatt 1 | Bereichsumschaltung Q3/Q4, U7/U8, R9/R12 | **SCHALTPLAN:** Mit Source = AMP_OUT = AGND ergeben sich etwa 0 V VGS aus und +15 V/−15 V ein. Frühere DUT-spannungsabhängige ±25-/±45-V-VGS-Beanstandung zurückgenommen. | Reale Schaltzeiten, Versorgungstoleranzen/Überschwinger, Bereichswechsel unter Strom, Hardwaretest. |
| 28.10.2026; ENDSTUFE Blatt 1 | Hardware-Strombegrenzung U12/Q5, D1/D2, R1/R2/R5/R6 | **SCHALTPLAN:** Erfassungsprinzip und grobe Grenzen geprüft: etwa 170–200 mA ohne Bypass bzw. 1,2–1,4 A mit Bypass. Kein Präzisionslimit. | Toleranzen/Temperatur, Dynamik, gemessene Schwellen und ausreichender Schutz kleiner Messshunts. |
| 28.10.2026; ENDSTUFE Blatt 1 | SOURCE/SINK-Summierer U14, R79–R86, C33/C34 | **SCHALTPLAN:** Summenbildung SET_AMP + OFFSET und lokale Rückkopplung geprüft; RC-Größenordnung betrachtet. | Stabilität mit endgültiger OPV-Bestückung, Leistungsstufe und Hauptregler. |
| 28.10.2026; ENDSTUFE Blatt 1 | Bias/Offset U37/U38, R75–R78, NTC R3/R4 | **SCHALTPLAN:** Spannungsfolger und Offsetbeträge rechnerisch plausibel. | Ruhestrom in beiden Leistungsbereichen, thermische Kopplung/Temperaturgang, Sensor-/Trimmerfehler. |
| 28.10.2026; ENDSTUFE Blatt 1; Nutzerbestätigung zur Abblockung | Analoge Versorgung und Abblockkondensatoren | **TEILPRÜFUNG:** Die unten angeordneten 100-nF-Kondensatoren je Baustein an ±15 V sind vorhanden und berücksichtigt; kein pauschaler Fehlbestand. | Lokale +5-V-Abblockung der DG419-VL und +3,3-V-Abblockung der TC74 konkret zuordnen/nachweisen; PCB-Platzierung und Versorgungsquellen prüfen. |
| 28.10.2026; ENDSTUFE Blatt 1 | Temperaturmessung U13/U15 (TC74) | **SCHALTPLAN:** Unterschiedliche Adressen 0x48/0x49, Versorgung, Pull-ups und GND-Bezug der Metallfahne betrachtet. | Montage/Isolation, Abblockung, Busbelastung und Messung. |
| 28.10.2026; ENDSTUFE Blatt 1; Nutzerbestätigung | Aktive Kühlung, Kühlkörper und Leistungs-MOSFET-Montage | **TEILPRÜFUNG:** Aktive Kühlung ist vorgesehen und berücksichtigt. Unterschiedliche Drainpotentiale und TC74-GND-Fahne müssen bei Montage isoliert bleiben. | Konkrete Wärmebilanz, Kühlkörper-/Chiptemperatur, Lüfterausfall und DC-SOA/Dauer-Senkenbetrieb; keine thermische Freigabe allein durch Lüfter. |
| 28.10.2026; ENDSTUFE Blatt 1 | Lüfteranschluss U20, FAN_PWM/FAN_SENSE | **TEILPRÜFUNG:** Anschlussfunktionen betrachtet. | Konkreter Lüfter, PWM-Treiber, Tacho-Pull-up/Pegel, Anlaufstrom und Mainboardanschluss. |
| 28.10.2026; ENDSTUFE Blatt 2 | Endstufen-Interface U2 und HV-Eingang CN2 | **SCHALTPLAN:** Sichtbare Pinbelegung, getrennte Bezugsnetze und parallele Lastkontakte erfasst. | Gegenstecker-/Kabelabgleich, Stromtragfähigkeit, Isolation und tatsächliche Rückstromführung. |
| 28.10.2026; MAIN_BOARD Blatt 6 I_Shunt, Export 27.10.2026 | AMP_OUT–Shunt–FORCE_LO und FORCE_HI/HV_GND | **TEILPRÜFUNG:** Topologische Referenzführung zur Korrektur der Floating-Analyse geprüft. | Vollständige Bestückungs-/Bereichsprüfung einschließlich Abweichungen zwischen SVG und aktuellen Backlog-Shuntwerten; Schutz, Relais, MUX und Kelvinführung nicht insgesamt freigegeben. |
| 27.10.2026 laut Abschnitt 2.2; ergänzende Sichtprüfung 28.10.2026; MAIN_BOARD Blatt 1 V_Mon | V_MON-Bereichsumschaltung und Dämpfungsoptionen | **DOKUMENTIERT / TEILPRÜFUNG:** Relais U2 überbrückt R57 im 5-V-Bereich; R25 = 0 Ω überbrückt C48. Am 28.10. zusätzlich zur Referenzeinordnung betrachtet, kein vollständiger Neucheck. | Ganze Stufe mit allen Referenzen/Versorgungen, Vorzeichen, Toleranzen und Stabilität nachweisen. |
| 25.–26.10.2026 laut Abschnitten 8/8.1/9 | CV/CC-/PI-Regelung, ohmsche und kapazitive LTspice-Tests | **DOKUMENTIERT:** Vorhandene Modelltests und Kandidaten bleiben in Abschnitten 8/9 maßgeblich. Am 28.10. keine neue Simulation. | Offene kapazitive Fälle, finale OPV-Modelle und Übertragbarkeit auf EasyEDA-/Hardwarestand. |
| Stand 28.10.2026 | MAIN_BOARD Error, Regler_Min_Max, DAC, ADC, I_Mon und VREF – vollständiger EasyEDA-Stand | **OFFEN:** Im Rahmen dieser Endstufenprüfung nicht vollständig geprüft. Bestehende Diskussionen/Modelltests sind kein vollständiger Blattnachweis. | Je Blatt eigenständige Prüfung mit Revision, Bezugspotentialen, Bauteilgrenzen und Prüfergebnis durchführen. |
| Stand 28.10.2026 | EDA-Netzliste/ERC, PCB, Gesamtsystem und reale Inbetriebnahme | **OFFEN:** Die hier besprochenen SVG-Prüfungen enthalten keinen entsprechenden Nachweis. | Separater ERC-/Netzlistenabgleich, Layout-/Isolationsprüfung sowie Messprotokoll zu Ruhestrom, Begrenzung, Z_HIGH, Stabilität und Temperatur. |

| 28.10.2026; NETZTEIL Blatt 1 Power_on, Export 28.10.2026; Nutzerbestätigung C96 | Standby, Taster/Flipflop, Einschaltreset und Relaisansteuerung | **SCHALTPLAN:** Grundfunktion, Rückführung, Freilaufdiode und Abblockung betrachtet. **C96 laut Nutzer auf 10 µF geändert**; τ beim Drücken 1 s, beim Loslassen 2 s. Ersetzt die vorherigen 330 µF. | Gewünschte Langzeitbetätigung bestätigen, Schmitt-Schwellen/Tantal-Leckstrom, Brownout/Neustart und reale Schaltzeiten messen. Änderung auf 10 µF vom Nutzer gemeldet; SVG nicht erneut geprüft. |
| 28.10.2026; NETZTEIL Blatt 1 und 5 | Netzanschluss, F6/F7, MOV, K1 und NTC U10 | **TEILPRÜFUNG:** Topologie und Standby/geschaltete Netzpfade betrachtet; SYSTEM_OFF ist keine vollständige Netztrennung. | Konkrete Sicherungseinsätze, Kontakt-/Einschaltstromfestigkeit, heißer Neustart, Netz-/Gehäuse-/PCB-Isolation; keine Netzfreigabe aus SVG. |
| 28.10.2026; NETZTEIL Blatt 2 ±15V_5V | Floating-Analogversorgung, U4/D18, 7815/7915/7805 | **SCHALTPLAN:** Getrennte AGND-Domäne passt zum Floating-Konzept; Gleichrichtung, Elkopolarität und Schutzdioden plausibel. U4 etwa 10 VA gesamt; +5 V belastet zusätzlich den +15-V-Reglerzweig. | Gemeinsames DC-Lastbudget, minimale/maximale Rohspannung, Thermik, Trafo-/PPTC-Schutz und Regler-Footprints. |
| 28.10.2026; NETZTEIL Blatt 1 und 3 | Digital +12 V und +3V3 | **SCHALTPLAN:** +12 V dauerhaft aus IRM-15-12; +3V3 geschaltet aus IRM-10-3.3; Puffer-/Abblockkondensatoren vorhanden. Parallele Stecker teilen die Gesamtlast. | Lastbudget mit PPTC-Temperatur-Derating, Rail-Sequenz, Rückspeisung und Lastsprünge. |
| 28.10.2026; NETZTEIL Blatt 4 ±HV | HV-Gleichrichter, Siebung, RXEF185 F2/F3 | **SCHALTPLAN:** 2×30 V AC ergeben grob ±41…42 V DC nominal, je 6600 µF. **OFFENER SCHUTZPUNKT:** Bei +HV↔−HV-Fehler kann mehr als 72 V über einem zuerst auslösenden RXEF185 liegen. | Maximale Einzel-/Gesamtspannung, Schutzspannungsfestigkeit/Fehlerstrom, Trafo-/Rippel-/Thermiknachweis und definierte Entladung. |
| 28.10.2026; NETZTEIL Blatt 4; Nutzerpräzisierung | ±15V0 für den OPA192 im V_MON gegen HV_GND | **SCHALTPLAN / FESTGELEGT:** ±15V0 versorgt in A-SMU ausschließlich den OPA192 im V_MON. Für diesen einzelnen Verbraucher ist die DN2540/TL431-Versorgung grundsätzlich plausibel; kein festgestelltes Lastbudgetproblem. | Stromquellenstreuung, Reglerstabilität und Last des OPV einschließlich Ausgangsbeschaltung im realen Betrieb verifizieren. Bei anderer Projektnutzung neu bewerten. |
| 28.10.2026; NETZTEIL Blatt 5 Interface; Nutzeranforderung | Netzteilanschlüsse und projektübergreifende Nutzung | **SCHALTPLAN / FESTGELEGT:** AGND, DGND und HV_GND bleiben klar getrennte Domänen; Netzteil soll auch in anderen Projekten eingesetzt werden. Siehe Abschnitt 6.1/6.2. | Verbindliche Stecker-/Last-/Potentialdokumentation, Verwechslungsschutz und Freigabe je Projektprofil; kein pauschales Zusammenlegen der Massen. |

| 29.10.2026; Schematik/Remote-Sense.svg | Ausgangsrelais K1 und Floating-/Shunt-Anbindung | **SCHALTPLAN:** Zweipolige FORCE-Trennung und AGND links vom Shunt plausibel. HGND muss dem bestehenden HV_GND entsprechen. Lokale Sense-Abgriffe liegen nach K1. | K1-DC-Schaltleistung/Fehlerstrom und tatsächliche Kelvinführung prüfen. K1 allein trennt Sense-Verbindungen nicht. |
| 29.10.2026; Schematik/Remote-Sense.svg | K2 Local/Remote und je 100 kΩ FORCE↔SENSE | **SCHALTPLAN:** Grundfunktion und schwacher lokaler Rückfall bei Sense-Unterbrechung plausibel. 100 kΩ liegen parallel zum Sense-Pfad, nicht in Serie zum externen Eingang. | Definierte Ruhe-/Umschaltkontakte, Open-Sense-Diagnose, FORCE-Leitungsbruch, Bias/Leckstrom-/µA-Fehlerbudget, Kabelstabilität und eigener V_MON-Eingangsschutz. |
| 29.10.2026; Schematik/Remote-Sense.svg; Abschnitt 5.1 | Output-OFF mit verbleibender Sense-Verbindung | **TEILPRÜFUNG:** K1 offen/Local: FORCE_TERM direkt am Sense-Eingang. K1 offen/Remote: SENSE_TERM direkt sowie FORCE_TERM über 100 kΩ am Sense-Eingang. Keine vollständige Vierleitertrennung dargestellt. | Ziel am 29.10.2026 festgelegt: vollständige vierleitige DUT-Trennung. Zusätzliche Sense-Trennung einschließlich Local-/100-kΩ-Pfaden noch umsetzen; Regler-Hold/Z_HIGH/Relais sequenzieren. |

| 29.10.2026; doc/Schematik/circuit-20261029-0651.svg | Überarbeitete Sense-Rückfallwiderstände vor K1 | **SCHALTPLAN:** 100 kΩ jetzt intern; K2-Local-Abgriffe weiterhin extern nach K1. Damit verbleiben bei OFF/Local hochohmige Brücken über K1 und DUT-Einfluss auf die Rückführung. | Vollständige Trennung noch nicht erreicht. Kandidat: auch K2-Local vor K1 verlegen; lokaler Kontaktspannungsabfall wird dann nicht mitgeregelt. Siehe Abschnitt 5.2. |

| 29.10.2026; doc/Schematik/circuit-20261029-0704.svg | Korrigierte FORCE-/Sense-Trennung, aktueller Konzeptstand | **SCHALTPLAN:** Local-Abgriffe und 100 kΩ vor K1; K1 offen/K2 Local trennt alle vier externen Klemmen, interne Rückführung bleibt verbunden. Kein zusätzliches Trennrelais nötig. Ersetzt 06:51-Topologie. | OFF-Verknüpfung K1 offen UND K2 Local hardware-/steuerungsseitig umsetzen; Relaisdaten, Sequenz/Hold, Local-Kontaktspannungsfehler, Eingangsschutz und Messung prüfen. |

| 29.10.2026; MAIN_BOARD Blatt 10 Power-Out | EasyEDA-Kontaktbelegung K5/U73 und R47/R48 | **SCHALTPLAN + HERSTELLERABGLEICH:** FORCE-Schließer 6–5/3–4, Sense-COM 9/4, Local 10/3, Remote 8/5 und U73-Spule +1/−12 korrekt. Treiber-/Freilaufanbindung plausibel. | PCB-/Symbol-Footprint-Zuordnung, blattübergreifende Netzzuordnung, DC-Schaltleistung und OFF-Verknüpfung/Timing separat prüfen. Siehe Abschnitt 5.4. |

| 29.10.2026; MAIN_BOARD Blatt 7 I_Mon | GUARD_PCB/GUARD_OUT, U6.3/U6.4, D12, R29/R30/R31 | **TEILPRÜFUNG:** IM+-Nachführung als Spannungsfolger und vorhandene Differenzklemme D12 geprüft. Interne Guardfläche braucht kein Trennrelais. | Externen GUARD_OUT bei vollständigem OFF mittrennen; Fremdspannungs-/ESD-/Power-Off-Schutz und dynamische Last vor PCB dimensionieren. Kein vollständiger I_MON-/Guard-Schutznachweis. Siehe Abschnitt 5.5. |

**Korrekturhistorie:** Die erste Endstufenanalyse vom 28.10.2026 mit fester AGND-Annahme ist bezüglich R150, Aussteuerbarkeit und daraus abgeleiteten Gateüberspannungen ersetzt. Maßgeblich sind Abschnitt 4.1 und die korrigierte Floating-Prüfung. Die bedingte Einschätzung „als Prototyp grundsätzlich baubar“ ist keine vollständige Hardware- oder ±30-V-/±1-A-Dauerlastfreigabe.

## 1. Geräteziel und Varianten

| Merkmal | ±30-V-Variante (aktiv) | ±140-V-Variante (geplant) |
|---|---|---|
| Quadranten | Vier Quadranten: Spannung und Strom in positiver und negativer Richtung; Quellen und Senken | Ebenso vorgesehen |
| Ausgang | `FORCE_HI`, `FORCE_LO`; Remote Sense über `SENSE_HI`, `SENSE_LO` | Prinzip beibehalten, Hochspannungsprüfung erforderlich |
| Betriebsarten | CV und CC, kontinuierlicher Übergang über analoge Reglerselektion | Vorgesehen, **nicht** durch ±30-V-Tests validiert |
| Zielgrenzen | ±30 V, ±1 A | ±140 V, ±100 mA |
| Trafo | 2 × 30 V AC / 120 VA | ca. 2 × 120 V AC, Kandidat |
| Zielzustand | Laborprototyp, LTspice-Schaltung und PCB-Entwicklung | Zweite Version, eigenes SOA-, Spannungs- und Isolationsdesign offen |

- **FESTGELEGT:** Ausgangsrelais startet im stromlosen/sicheren Zustand **OFF**; Hardware darf sich nicht allein auf Firmware für die Sicherheitsfreigabe verlassen.
- **FESTGELEGT (28.10.2026, Nutzerklärung): Floating-Analogdomäne.** `AGND` ist über den vorgesehenen 0-Ω-Link **R150 mit AMP_OUT** verbunden. ±15 V und +5 V analog beziehen sich auf diese mitbewegte AGND. `FORCE_LO` liegt hinter dem gewählten Messshunt und ist daher unter Last nicht exakt mit AGND gleichzusetzen. `FORCE_HI` ist im aktuellen Mainboard-Blatt I_Shunt mit `HV_GND` verbunden. Digitale Domäne: galvanisch getrennt, z. B. +12 V/+3,3 V; Trennstellen und Versorgungspotentiale im endgültigen Layout prüfen. Siehe verbindliche Prüfrandbedingung in Abschnitt 4.1.
- Die **±36-V-Versorgung in den aktuellen LTspice-Tests** ist fest; Umschalten zwischen „5-V-“ und „30-V-Bereich“ bedeutet dort vor allem Änderung der **V_MON-Skalierung**, **keine** simulierte physische Versorgungsschienenumschaltung.

## 2. Funktionsübersicht und Signalfluss

```text
  V_SET ───────────────┐
                      ▼
 FORCE_HI/LO → V_MON → CV-Regler ────┐
                                    │
 Shunt → I_MON → I+ / I−-Regler ─────┤ → Min/Max-Auswahl → ERROR
                                    │                       │
                                    └───────────────────────┤
                                                            ▼
                                              PI-/Integrator-/Treiberstufe
                                                            │
                                               MOSFET-Endstufe ±
                                                            │
                                              FORCE_HI ↔ Last ↔ FORCE_LO

 SENSE_HI/LO → V_MON; FORCE↔SENSE / Open-Sense-Schutz separat.
 Hardware-Schutz, Bereichsrelais und Z_HIGH greifen unabhängig ein.
```

Das Diagramm ist eine **funktionale Vereinfachung**; exakte Knotenpolaritäten, Diode-Orientierungen, Priorität und Integratortopologie sind aus dem jeweiligen Schaltplan abzuleiten.

### 2.1 Regelung CV/CC und Min/Max

- **FESTGELEGT (Konzept):** drei analoge Regelpfade: **CV**, positiver Stromgrenzregler **I+**, negativer Stromgrenzregler **I−**. Sie liefern Führungs-/Fehlersignale an einen **Min/Max-Selektor**; der jeweils begrenzende Pfad erhält die Regelhoheit. Ziel: fließender CV↔CC-Übergang und Vierquadrantenbetrieb, ohne Software im schnellen Regelkreis.
- **Selektor:** frühe LT1058A-Modelle; für PCB wurden **OPA4192** und Serien-/Entkopplungswiderstände um **1 kΩ**, antiparallele BAT54-Klemmdioden sowie ca. **220 kΩ Pull-down** am `MIN_OUT`/`ERROR` besprochen. **Bauteilwerte/Diode-Orientierung am aktuellen EasyEDA-Plan bestätigen**; nicht alle Entwurfsstände sind identisch.
- **ERROR / Integrator:** die PI-Kompensation berücksichtigt **R1 (P-Anteil), R3 und C1**. Der neu dokumentierte Codex-Arbeitsstand steht in **Abschnitt 8.1**. In der bisherigen Prüftabelle war die vereinfachte Zuordnung **P ∝ R1/R3**, **I ∝ 1/(R3·C1)**; die tatsächliche Übertragungsfunktion und das Vorzeichen sind am aktuellen Schaltplan zu prüfen. Eine echte Anti-Windup-/Integrator-Haltefunktion und sicheres Verhalten bei Bereichsumschaltung sind noch zu prüfen.
- **Integrator-Umschalten / Halten:** DG419 bzw. Analogschalter wurden diskutiert; beim 1-A↔kleiner-Bereichswechsel sollte der Integrator gehalten werden. **Kondensator hart kurzschließen/resetten ist nicht als endgültige Lösung festgelegt.**
- **OP-Amps:** für LTspice aktuell überwiegend **LT1058A** zur schnellen Untersuchung; für PCB **OPA192 / OPA172** diskutiert (genaue Stufenzuordnung offen). Entsprechend ist das LT1058A-Stabilitätsergebnis **nicht** automatisch auf die finalen Operationsverstärker übertragbar.

### 2.2 V_MON – Spannungsrückführung

- Die Differenzspannung des Ausgangs wird über `SENSE_HI/SENSE_LO` (Remote Sense) bzw. lokale Sense-Anbindung erfasst: `V_OUT = V(FORCE_HI) − V(FORCE_LO)`; Schutzüberwachung des FORCE↔SENSE-Unterschieds separat als `VMON_PROT` vorgesehen.
- **V_MON-Skalierung und PCB-Bereichsumschaltung (korrigiert am 27.10.2026):** `V_MON ≈ −V_OUT / 2` im 5-V-Bereich und `V_MON ≈ −V_OUT / 12` im 30-V-Bereich. Im EasyEDA-Pro-Export `doc/hardware/MAIN_BOARD/SCH_MAIN_BOARD 2.0_1-V_Mon_2026-10-27.svg` überbrückt Relais **U2** den Widerstand **R57 = 100 kΩ**: **5 V → Kontakt geschlossen, R57 überbrückt; 30 V → Kontakt offen, R57 wirksam.** Die früheren Angaben „R26 (100 kΩ) im 30-V-Bereich überbrücken“ und „R45/R51 auf 60 kΩ umschalten“ sind falsch. Die Bauteilbezeichnungen des LTspice-Modells sind getrennt vom PCB-Plan zu betrachten; die bisher dokumentierten `R22/R29 = 10 kΩ` bleiben konstant. Beleg: Sichtprüfung des genannten Schaltplanexports; kein neuer Simulations- oder Hardwaretest.
- **Optionale V_MON-Dämpfung (27.10.2026):** Im PCB-Plan sind **R25 und R26 zunächst mit 0 Ω** vorgesehen. R25 liegt parallel zu **C48 = 150 pF**; bei R25 = 0 Ω ist C48 überbrückt. R25/C48 bieten eine Option zur Frequenzkompensation um U155.1, R26 zur Ausgangsentkopplung. Die Wirkung möglicher Widerstandswerte vorab mit den vorgesehenen OPA192-Modellen simulieren; endgültige Werte bei Bedarf durch Messungen am Prototyp bestimmen. Noch keine zusätzlichen Dämpfungswerte festgelegt.
- Bei den Skripten gilt `V_SET = ±2 V` und als CV-Ausgangssollwert daraus ±4 V (5-V-Skalierung) bzw. ±24 V (30-V-Skalierung). **Vorzeichen am tatsächlichen CV-Summenknoten aus der Netlist prüfen**; mit invertiertem Monitor ist `V_MON` dem Ausgang entgegengesetzt gepolt.
- **Open-Sense-Schutz:** Remote-Sense-Umschaltung über Relais; diskutiert sind ca. **100 kΩ von FORCE zu SENSE** als lokale Rückfall-/Bias-Verbindung. Verhalten bei fehlendem, vertauschtem oder unterbrochenem Sense muss noch fehlerfallsicher geprüft werden.
- Spannungsbereiche UI: `{Auto, 5 V, 30 V}`; Default bisher 5 V. Erweiterung für ±140 V benötigt eigenständige skalierende, spannungsfeste Eingänge/Relais.

### 2.3 I_MON – Strommessung und Bereichswahl

- **FESTGELEGT für die getestete Regler-Topologie:** Kelvin-Messung der Shuntspannung, danach invertierte Strommonitor-Skalierung mit Betrag ungefähr 2. **Getestete Beschaltung:** `R41/R58 = 20 kΩ` konstant für beide Spannungsbereiche.
- Mit `I_OUT` positiv aus `FORCE_HI` in die Last: `V(I_MON) ≈ −2 · I_OUT · R_SHUNT`. Beispielsweise **+1 A an 1 Ω ⇒ etwa −2 V I_MON**, **−1 A ⇒ etwa +2 V**. Dasselbe ±2-V-Ziel gilt für die anderen Bereiche bei deren Nennstrom.
- **4 Kanäle am ADG1209:** Die vier aktuell regulären Shunts belegen diese Kanäle. **DG409** als Kelvin-Multiplexer diskutiert; Architektur und zulässige analoge Signalbereiche von ADG1209/DG409 noch auf PCB-/Schaltplanebene validieren.
- Strombereichsrelais: 1-A-Zweig mit geeignetem Leistungsrelais; für kleinere Zweige Reed-Relais (z. B. **HE3621A6012**, für besonders kleine Ströme **Pickering 109P-1-A-12/2D** besprochen). Detaillierte Aufteilung, Schaltreihenfolge, Leckage und **Kontakt-/Schaltleistung** vor Bestellung prüfen.
- Präzisionsziel Shunts: vorzugsweise ±0,1 % und `|TCR| ≤ 10 ppm/K`, Kelvinanschluss für niederohmige Shunts. Messbereichsgenauigkeit/Offset/Kalibrierung sind noch nicht vermessen.

## 3. Aktuelle Strommessbereiche der ±30-V-Version

**FESTGELEGT – Nutzerauftrag vom 29.10.2026:** Folgende Tabelle ist die aktuelle Zuordnung für die **±30-V-Version mit maximal ±1 A**. Keine Schaltplan-/Firmwareänderung und keine neue Simulation durch diesen Dokumentationsauftrag.

| Messbereich | Shunt | Normalbetrieb¹ | Überlast² | Vorgesehene Nennleistung | Steuersignal |
|---|---:|---:|---:|---:|---|
| **±1 A** | **1 Ω** | **1 W** | **1 W** | **8 W** | **`I_RANGE_1_EN`** |
| **±10 mA** | **10 Ω** | **1 mW** | **10 W** | **0,5 W** | **`I_RANGE_2_EN`** |
| **±100 µA** | **1 kΩ** | **10 µW** | **0,9 W** | **0,25 W** | **`I_RANGE_3_EN`** |
| **±1 µA** | **100 kΩ** | **0,1 µW** | **9 mW** | **0,25 W** | **`I_RANGE_4_EN`** |

¹ Normalverlustleistung bei Bereichsnennstrom nach `P = I² × R`; Werte rechnerisch konsistent.

² Überlastwerte wie vom Nutzer vorgegeben übernommen. Sie sind **keine pauschalen maximalen Fehlerleistungen** und kein Nachweis, dass die vorgesehene Nennleistung diese dauerhaft verträgt. 1 W an 1 Ω entspricht 1 A; 10 W an 10 Ω entspricht 1 A; 0,9 W an 1 kΩ und 9 mW an 100 kΩ entsprechen jeweils 30 V am Shunt. Höhere reale Hardwaregrenzen, Fremdspeisung und zeitabhängige Überlast separat betrachten. Vor allem 10-Ω-/1-kΩ-Pfade benötigen bei diesen Überlastannahmen passenden Schutz.

**Signalnamenskonvention:** `_EN` bezeichnet aktive-HIGH-Freigabe eines Hardwarekanals. Die Nummern sind versionsneutral. Für die ±140-V-Version kann beispielsweise `I_RANGE_1_EN` einen anderen Messbereich/Shunt ansteuern; diese Zuordnung bleibt gesondert festzulegen. Entsprechend `V_RANGE_1_EN`, `V_RANGE_2_EN` für die Spannungskanäle verwenden. Software zeigt Messwerte aus dem jeweiligen Geräteprofil, statt sie aus dem Signalnamen abzuleiten.

**Auswirkungen offen:** I_MON-/Sollwert-/ADC-Skalierung und PI-Auslegung an den aktuellen Bereichssatz anpassen bzw. bestätigen. Bei unveränderter bisheriger I_MON-Verstärkung −2 ergeben sich bei positivem Nennstrom −2 V im 1-A-Bereich, aber −0,2 V in den drei kleinen Bereichen (je 0,1 V Shuntspannung). Das dokumentiert eine Konsequenz, keine neue Verstärkungsfestlegung.

### 3.1 Bisheriger Simulationsbereichssatz – historische Testzuordnung

**Ersetzt als aktuelle Bereichsdefinition:** Die folgende Tabelle bleibt als Beleg des bisherigen LTspice-Teststands erhalten. Dessen Bereichsnamen, Prüfergebnisse und PI-Werte dürfen nicht auf die neue Tabelle umetikettiert werden.

**Status:** Vier reguläre Bereiche im LTspice-Modell erfolgreich auf stationäre Plausibilität getestet; **PCB-Bestückung noch nicht als fertige Hardware freigegeben**.

| Strombereich (symmetrisch) | R_SHUNT (R23 im Test) | U_SHUNT bei Nennstrom | Soll-I_MON bei +Nennstrom | Shuntleistung bei Nennstrom | PI: R3/C1 | LTspice-Grobtest |
|---|---:|---:|---:|---:|---|---|
| **±1 A** | **1 Ω** | 1 V | −2 V | **1 W** | **10 kΩ / 1 nF** | **32/32 OK** |
| **±100 mA** | **10 Ω** | 1 V | −2 V | **100 mW** | **10 kΩ / 1 nF** | **32/32 OK** |
| **±1 mA** | **1 kΩ** | 1 V | −2 V | **1 mW** | **22 kΩ / 47 nF** | **32/32 OK** |
| **±10 µA** | **100 kΩ** | 1 V | −2 V | **10 µW** | **22 kΩ / 47 nF** | **32/32 OK** |
| **±1 µA (OPTION)** | **1 MΩ** | 1 V | −2 V | **1 µW** | **22 kΩ / 47 nF** | **32/32 gemeldet; Shunt-/Netlist-Prüfung offen** |

**Hinweis zur PI-Spalte:** Sie dokumentiert die **R3/C1-Einstellungen der abgeschlossenen ohmschen Grobtests**, **nicht in jedem Fall die neuesten Codex-Kandidaten**. Insbesondere ist ±10 µA mit **47 kΩ / 100 nF** ein neuer Versuchskandidat; siehe **Abschnitt 8.1**. Die Tabelle dokumentiert den **aktuellen simulierten Bereichssatz**, nicht die ursprüngliche 0,1-Ω-Variante. Bei anderer I_MON-Verstärkung ändern sich Messbereich, Setpoint und Kompensation. 1 Ω bei 1 A erfordert **mindestens 1 W nominelle Shuntleistung** plus ausreichend thermisches Derating. Die genauen Bauteile, Relais und Kalibrierwerte sind **OFFEN**.

### 3.2 Früherer experimenteller 1-MΩ-Test und noch offene Erweiterung

**Abgrenzung zum aktuellen Stand:** Die folgenden Punkte betreffen den früheren 1-MΩ-Versuch. Der aktuell festgelegte ±1-µA-Bereich verwendet 100 kΩ und ist regulärer Kanal 4. Eine spätere 1-MΩ-Erweiterung hat noch keinen neu festgelegten Messbereich oder Steuersignalindex.

- Fünfter Messbereich benötigt einen weiteren Messkanal oder ein geändertes MUX-/Relaiskonzept; ADG1209 stellt gegenwärtig nur **vier verwendete Kanäle** bereit.
- Das hochgeladene 1-µA-Protokoll trägt **1-MΩ-Testnamen**, aber mehrere Kopf-/Konfigurationszeilen nennen noch **100 kΩ / 10 µA**. Vor verbindlicher Festlegung sind die von LTspice erzeugten **`.net`-Dateien** auf `R23 = 1Meg`, richtige Stromgrenze und `V(I_MON)` zu prüfen.
- Bei 1 MΩ sind Leckströme, Platinenoberfläche/Guard, MUX-Off-Leckage, Relais-Isolationswiderstand, Störkapazität und Rauschen potenziell entscheidender als thermische Belastbarkeit.

### 3.3 Thermische Worst-Case-Prüfung (interne ±30-V-Grenze, stationär)

| Shunt | Betrachteter Fall | Rechnung | Konsequenz |
|---|---|---|---|
| 1 Ω | 1 A durch Shunt | `1² · 1 Ω = 1 W` | thermisches Derating, Gehäuse und Kelvinlayout prüfen |
| 10 Ω | HW-Modell begrenzt auf ca. 200 mA | `(0,2 A)² · 10 Ω = 0,4 W` | mindestens 0,4 W plus Reserve; reale Begrenzungsdynamik prüfen |
| 1 kΩ | gesamte 30 V über dem Shunt | `30²/1000 = 0,9 W` | deutlich oberhalb Nennwert; Überlastbetrieb/Netzwerk, Spannungspfad/Abschaltung prüfen |
| 100 kΩ | gesamte 30 V über dem Shunt | `30²/100000 = 9 mW` | **thermisch unkritisch**, wenn Differenzspannung tatsächlich auf 30 V begrenzt |
| 1 MΩ | gesamte 30 V über dem Shunt | `30²/1000000 = 0,9 mW` | **thermisch unkritisch**, wenn Differenzspannung tatsächlich auf 30 V begrenzt |

**Keine pauschale „200 mA schützt jeden Shunt“-Annahme:** Im 1-kΩ-Bereich setzt bereits die Ausgangsspannung den stationären Strom bei direktem Spannungsabfall auf ca. 30 mA; daraus folgen immer noch 0,9 W. Im 100-kΩ-/1-MΩ-Bereich ergibt sich unter der **internen** 30-V-Begrenzung keine relevante thermische Überlast. **Nicht abgedeckt** sind externe Fremdspannungen, Relais-Umschaltspitzen, Überspannung am ADG1209 sowie reale Versorgungshub-/Schutzzustände. Spannungsfestigkeit des Widerstands beachten.

## 4. Endstufe, Leistungsregelung und Z_HIGH

- **FESTGELEGT (Entwurf):** komplementäre MOSFET-Endstufe mit **IRFP250N / IRFP9240**, Treiber **BC547B / BC557B**, Ruhestrom zunächst etwa **50 mA** beibehalten (nicht gleichbedeutend mit Ausgangsstromgrenze). Dimensionierung für Vierquadrantenbetrieb und Wärmeabfuhr.
- **Zwei Leistungsstufen im LTspice-Modell:** im **1-A-Bereich** M3/M4 leitend bzw. aktiviert; für **100 mA und kleinere Bereiche** M3/M4 ausgeschaltet; R55/R56 ca. **3 Ω** und R20/R21 ca. **0,5 Ω** liefern im betrachteten Modell eine Hardwarebegrenzung um **200 mA**. Dies ist noch **kein gemessener, toleranzbereinigter Grenzwert**.
- **1A_ON:** Signal für Endstufenfreigabe der großen Stromstufe; ob es direkt mit der Shunt-Umschaltung gekoppelt bleibt oder aus Sicherheitsgründen separat angesteuert wird, ist **OFFEN**. Für Testskripte muss die richtige Gate-Anbindung je Bereich explizit geprüft werden.
- **Z_HIGH:** über **DG419** vorgesehener analoger Abschalt-/Hochimpedanzmodus; zusätzlich Ausgangsrelais zur galvanischen Trennung des DUT. Funktionen sind nicht austauschbar: Relais trennt physisch, Z_HIGH verhindert unerwünschte Ansteuerung/verbessert Umschaltsicherheit. Ausfallverhalten, Schutz gegen externe Spannung und Restleckage müssen noch validiert werden.
- **Schutzanforderungen:** hardwarebasierter Überstrom-/Überlastpfad, Open-Sense-Erkennung, Bereichsumschaltung mit gesperrtem Ausgang, Software-Überwachung von I_MON (Schaltschwelle etwa 2,5 V als Idee diskutiert; **nicht als finale Schutzschwelle freigegeben**).
- **Kühlung:** je nach Endstufenaufbau 2 × OMNI-UNI-41-75 und 60×60-mm-PWM-Lüfter geplant. Temperaturmessung, SOA und Dauer-Senken/Quellen bei voller Spannung/Strom sind **OFFEN**.
- **HV-Variante:** IRFP250N/IRFP9240 dürfen wegen Spannungs-/SOA-Grenzen nicht ungeprüft für ±140 V übernommen werden. Gesonderte HV-MOSFET-/Treiber-/Isolationsauslegung erforderlich.

### 4.1 Verbindliche Floating-Architektur und korrigierte Endstufenprüfung (28.10.2026)

- **FESTGELEGT / Nutzerentscheidung:** Die A-SMU ist ein floating System. Die ±15-V-Treiberversorgung ist gegen **AGND** definiert und bewegt sich mit dieser gegenüber `HV_GND`/Erde. Eine Verbindung nach AGND ist Bestandteil des Konzepts und darf nicht pauschal als Kurzschluss gegen eine feste Systemmasse bewertet werden.
- **Beleg:** Nutzerklärung vom 28.10.2026; Sichtprüfung der EasyEDA-SVGs `ENDSTUFE/..._1-Endstufe_2026-10-28.svg` und `MAIN_BOARD/..._6-I_Shunt_2026-10-27.svg`. Endstufe: `R150 = 0 Ω` verbindet `AMP_OUT` mit `AGND`. Mainboard: `AMP_OUT` führt über den Messshunt zu `FORCE_LO`; `FORCE_HI` liegt an `HV_GND`. Keine neue Simulation oder Hardwaremessung.
- **R150 bleibt vorgesehen.** Die frühere Codex-Empfehlung, R150 wegen eines angeblichen Ausgangskurzschlusses zu entfernen, ist **zurückgenommen**. Keine zusätzliche Verbindung zwischen `AGND` und `HV_GND`/`DGND`/PE aus dieser Prüfung ableiten.
- **Bezugspunkte für jede Prüfung explizit angeben:** `V_OUT = V(FORCE_HI) - V(FORCE_LO)`; Gatebelastung immer `V_GS = V(Gate) - V(Source)`. Eine DUT-Ausgangsspannung von ±30 V bedeutet hier nicht `AMP_OUT = ±30 V gegen AGND`, da `AMP_OUT` und `AGND` verbunden sind. Für Source-Knoten von Q1/Q2 die lokalen Widerstandsabfälle berücksichtigen.
- **Korrigierte Bewertung:** Der pauschale Einwand „±30 V Ausgang mit ±15-V-Gatetreibern nicht erreichbar“ ist für diese Floating-Architektur **zurückgenommen**. Ebenso sind die zuvor allein aus ±10/±30 V DUT-Spannung berechneten ±25/±45 V VGS an Q1–Q4 **nicht gültig**. Bei Q3/Q4 liegen die Sources an AGND; die gezeichnete Ansteuerung ergibt nominal etwa 0 V VGS aus und +15 V/−15 V ein. Q1/Q2 werden relativ zu AGND und ihren lokalen Source-Abfällen angesteuert; die ±15-V-Abschaltung ist damit grundsätzlich plausibel.
- **OFFEN – physischer Nachweis:** Floating-Versorgungen müssen diese Bezugspunkte tatsächlich einhalten; Potentialbereich/Isolation aller Analog-/Digitalübergänge und zulässige lokale VGS bei Versorgungsausfall, Leitungsunterbrechung, Fremdspeisung und Umschalttransienten prüfen. Keine uneingeschränkte Z_HIGH-/Power-Off-Freigabe allein aus dem Normalbetrieb ableiten.
- **OFFEN – unverändert:** DC-SOA/Kühlung einschließlich Senkenbetrieb, Ruhestrom in beiden Leistungsbereichen, genaue Hardwarebegrenzung und Schutz kleiner Messshunts, Kelvin-/Rückstromführung, Reset-/Bereichswechsel-Sequenz und Stabilität mit den finalen OPVs. DG419B sind Analogschalter und selbst keine galvanische Isolation; die Floating-Isolation entsteht durch die gesamte Versorgungs-/Signalarchitektur.
- **Prüfbericht:** Die erste Codex-Endstufenbewertung vom 28.10.2026 ist bezüglich der festen Masseannahme überholt; maßgeblich ist die korrigierte Floating-Bewertung. Für künftige Reviews Abschnitt 4.1 vor einer Bewertung der Gatepotentiale lesen.

## 5. Schutz, Remote Sense, Shunts, Umschalten

- **Relais-Sicherheit (OFFEN):** zuerst Sollstrom reduzieren/Ausgang sperren; elektrischen Abfall prüfen; Relais kontaktlos bzw. stromarm schalten; Kontakte beruhigen lassen; neuen Bereich plausibilisieren; erst dann Ausgang freigeben. Dabei externe Energie aus dem DUT ausdrücklich berücksichtigen.
- **Shunt-Relais/Signalleitungen:** korrekte Kelvin-Abgriffe direkt am Widerstand, keine Relaiskontaktwiderstände in der Shunt-Messung. Für 1 A **HE3621A6012 nicht ungeprüft als Lastschaltrelais** benutzen (historische Untersuchung: Kontakt-Schaltstrom geringer als möglicher 1-A-Dauerstrom).
- **ADG1209 / DG409 und OpAmp-Eingänge schützen:** Differenzspannung bei fehlerhaft gewähltem Bereich, Kurzschluss, externer Quelle und offenem Shunt muss innerhalb absoluter Grenzwerte bleiben. Ein thermisch sicherer 100-kΩ-Shunt schützt den MUX **nicht automatisch** gegen zu große Spannung.
- **Remote Sense:** lokale/ferne Umschaltung, 100-kΩ-FORCE↔SENSE-Hilfsanbindung und `VMON_PROT` bzw. Differenzüberwachung als Schutzkonzept. Fehlerfälle (Sense offen, Sense vertauscht, Sense an Fremdspannung) testen.
- **Ausgangssnubber:** In früheren LTspice-Tests verursachte die damals vorhandene Ausgangs-Snubber-Schaltung Schwingen; **ohne Snubber** waren bestimmte Leerlauf-/1-kΩ-Fälle stabil. Das ist eine Beobachtung der damaligen Topologie, **keine generelle Anweisung**, bei endgültiger Hardware auf Snubber/Entkopplung zu verzichten. Für kapazitive Lasten neu prüfen.
- Bei Klemmdioden/BAV199 beachten: Leckströme im µA-Bereich relativer Messbereiche, parasitäre Kapazitäten, Temperatur und Off-State-Verhalten separat modellieren und messen.

### 5.1 Ausgangstrennung und Remote Sense – Prüfung 29.10.2026

**Beleg:** `doc/hardware/Schematik/Remote-Sense.svg`, visuelle/rechnerische Konzeptprüfung unter bestätigter Floating-Architektur. Keine Netzliste, konkreten Relaistypen, Simulation oder Messung geprüft. Schaltplan nicht geändert.

- **SCHALTPLAN – plausibel:** K1 trennt FORCE_LO und FORCE_HIGH zweipolig. AGND liegt am Endstufenknoten vor dem Messshunt; HGND in dieser Darstellung muss HV_GND entsprechen. K2 wählt je Eingang zwischen der zugehörigen lokalen FORCE-Klemme und externer SENSE-Klemme. Lokale Abgriffe hinter K1 erfassen die tatsächliche Ausgangsklemmenspannung.
- **100-kΩ-Protection bestätigt eingeordnet:** Je 100 kΩ FORCE_TERM↔interner SENSE-Auswahlknoten ergeben einen schwachen lokalen Rückfallpfad bei offener Sense-Leitung; im Local-Zustand weitgehend durch den Kontakt überbrückt. Wert grundsätzlich plausibel bei hochohmigem V_MON. Das ist weder eine garantierte Open-Sense-Erkennung noch ein allgemeiner Schutz bei FORCE-Leitungsbruch.
- **Präzision:** Bei 100 mV FORCE↔SENSE-Differenz fließt 1 µA durch 100 kΩ. Dies ist nicht automatisch derselbe Betrag als Shuntmessfehler, muss aber mit den tatsächlichen Strompfaden besonders in µA-Bereichen betrachtet werden. Bias-/Schutz-/Relais-/Platinenleckströme und Kabelkapazität prüfen.
- **Fremdspannungs-/ESD-Schutz separat:** SENSE_TERM führt im Remote-Zustand direkt über K2 zum V_MON-Eingang; die 100 kΩ liegen daneben, nicht in Serie. Tatsächliche V_MON-Eingangswiderstände/Klemmen einschließlich floating ±15 V versus HV_GND-bezogenen ±15V0 und unversorgtem Zustand prüfen.
- **OFF-Befund:** Bei K1 offen bleibt im Local-Zustand FORCE_TERM direkt mit der internen Sense-Elektronik verbunden. Im Remote-Zustand bleibt SENSE_TERM direkt und FORCE_TERM über 100 kΩ verbunden. **Kein Nachweis vollständiger galvanischer Trennung aller vier DUT-Anschlüsse.** Nur K2 auf Local schalten behebt dies nicht.
- **FESTGELEGT – vollständige DUT-Trennung (29.10.2026):** Für Z_HIGH/Output-OFF werden FORCE_HI, FORCE_LO, SENSE_HI und SENSE_LO von der internen Elektronik getrennt. **Umsetzung im korrigierten Konzept `circuit-20261029-0704.svg` topologisch geprüft:** K1 offen trennt FORCE; K2 in Local trennt externe SENSE-Klemmen und verbindet intern rück. Local-Abgriffe und 100-kΩ-Pfade liegen vor K1. Kein zusätzliches Sense-Trennrelais erforderlich. **OFF muss K1 offen UND K2 Local erzwingen.** Z_HIGH bleibt elektronische Sperre; Relaistypen, Ausfall-/Umschaltsequenz und PCB noch prüfen. Bei vollständiger Trennung keine DUT-Spannungsmessung über diese Leitungen. Siehe Abschnitt 5.3.
- **K2-Ausführung:** Beide Umschaltpole eindeutig gekoppelt und mit COM/NC/NO kennzeichnen; bevorzugter stromloser Zustand Local. Kontaktfolge so wählen, dass externe SENSE- und lokale FORCE-Klemme beim Umschalten nicht niederohmig gebrückt werden; Totzeit/Prellen mit Regler-Hold beherrschen.
- **Regelung:** K1 offen trennt Stellgröße und externe Rückführung; Integrator halten/zurücksetzen bzw. interne Rückführung vorsehen. Z_HIGH, K1, K2 und ggf. weitere Sense-Trennung abgestimmt schalten. Bei extern gespeistem DUT keine pauschale stromlose Relaisumschaltung annehmen.
- **Nächste Tests:** Jede einzelne/both Sense-Unterbrechung, FORCE-Unterbrechungen, Sense vertauscht, Fremdspannung, K1-OFF in beiden K2-Stellungen, Power-Off, Relaistotzeiten/Prellen sowie Stabilität mit realen Kabeln/Schutzbeschaltung. K1 nach konkreter DC-Schaltleistung wählen; K2 nach Isolation/Leckstrom/Signalqualität.

### 5.2 Neuer Remote-Sense-Vorschlag und Relaisalternative (29.10.2026)

**Beleg:** `doc/Schematik/circuit-20261029-0651.svg`, neue Sichtprüfung; keine Netzliste/Simulation/Messung. Nicht mit dem älteren `doc/hardware/Schematik/Remote-Sense.svg` gleichsetzen.

- **Änderung erkannt:** Beide 100-kΩ-Rückfallwiderstände liegen jetzt an den internen FORCE-Knoten **vor K1**. Die Local-Anschlüsse von K2 liegen weiterhin an den externen FORCE-Klemmen **nach K1**.
- **Keine vollständige Trennung:** Bei K1 offen und K2 Local bleibt pro Pol der Weg `FORCE intern → 100 kΩ → SENSE intern → K2 Local → FORCE_TERM` erhalten. Die offenen FORCE-Kontakte werden damit hochohmig überbrückt; die externen SENSE_TERM sind zwar abgewählt, die FORCE-Klemmen aber nicht vollständig von der Elektronik getrennt. Bei K2 Remote bleiben SENSE_TERM direkt mit interner Sense-Elektronik verbunden.
- **Regelung nur bedingt intern rückgeführt:** Die 100 kΩ bieten einen lokalen Rückfallpfad, die Sense-Knoten bleiben über K2 aber vom DUT beeinflussbar. Beispiel K1 offen/K2 Local und externer Kurzschluss zwischen FORCE_TERM: beide Sense-Knoten werden verbunden, V_MON erfasst annähernd 0 V. Ein weiter aktiver Spannungsintegrator mit von null verschiedenem Sollwert kann auflaufen. Z_HIGH/Hold bleiben abgestimmt erforderlich.
- **KANDIDAT ohne zusätzliches Trennrelais:** K1 als zweipolige FORCE-Trennung beibehalten; **auch die Local-Anschlüsse von K2 auf die internen FORCE-Knoten vor K1 legen**, die 100 kΩ dort belassen. Dann trennt K1 offen/K2 Local in der vorgesehenen Kontaktstellung alle vier externen Anschlüsse. Der lokale Rückführpfad liegt intern. Im Local-Betrieb werden dann K1-Kontakt-/nachfolgende Leitungsabfälle nicht erfasst; im Remote-Betrieb werden sie mitkompensiert. Nicht als bereits geänderte SVG-Verbindung oder freigegebene Hardware behandeln.
- **Keithley-Vergleich, dokumentierte Funktion:** 2450-Referenzhandbuch Rev. E, Seiten 4-8/4-17: bei Output-OFF werden Remote-Sense-Leitungen getrennt und auf 2-wire umgeschaltet; High-Z-OFF öffnet zusätzlich das Ausgangsrelais. Daraus folgt kein Beleg für die genaue interne Relaiszahl/-verschaltung. Quelle: https://download.tek.com/manual/2450-901-01E_Sept_2019_Ref.pdf
- **Präzisierung zu Abschnitt 5.1:** Vollständige DUT-Trennung bleibt das Ziel. Ein zusätzliches Sense-Trennrelais oder vierpoliges Relais ist **nicht zwingend**, wenn vorhandener K2 die externen Sense-Leitungen zuverlässig abwählt und sämtliche Local-/100-kΩ-Verbindungen intern vor K1 bleiben. Ruhestellungen, Fehler-/Umschaltverhalten und Regler-Hold sind noch konkret auszulegen.

### 5.3 Korrigierte Ausgangs-/Sense-Topologie 07:04 (29.10.2026)

**Aktueller Konzeptstand:** `doc/Schematik/circuit-20261029-0704.svg`. Sichtprüfung bestätigt: Beide 100-kΩ-Rückfallwiderstände **und beide Local-Anschlüsse von K2 liegen vor K1**. Dieser Stand ersetzt für die weitere Umsetzung die abweichenden Topologien aus dem ursprünglichen Remote-Sense.svg und dem Vorschlag 06:51. Keine Netzliste, Simulation, konkrete Relaisdaten oder Hardwaremessung geprüft.

- **SCHALTPLAN – topologisch plausibel:** K1 offen/K2 Local trennt beide externen FORCE- und beide externen SENSE-Klemmen kontaktmäßig von der internen Elektronik. Die 100 kΩ umgehen K1 nicht mehr. Ein zusätzliches vierpoliges Ausgangs- oder zweipoliges Sense-Trennrelais ist für diese Topologie nicht erforderlich.
- **Interne Rückführung:** K2 Local verbindet V_MON direkt mit den internen FORCE-Knoten. Im Remote-Zustand und während Kontaktlücken bleiben die 100 kΩ als schwache lokale Rückbindung wirksam, sofern kein anderer angeschlossener Pfad die Sense-Knoten bestimmt. Ein vorhandener Rückführpfad allein garantiert bei aktivem Z_HIGH keine wirksame Regelung/keinen windup; Z_HIGH, Sollwerte und Hold bleiben zu koordinieren.
- **OFF-Verknüpfung verbindlich:** OUTPUT_OFF/Z_HIGH muss **K1 offen UND K2 Local** herstellen; K1 offen/K2 Remote lässt die externen SENSE-Klemmen verbunden und erfüllt die vollständige Trennung nicht. Stromlose Ruhestellungen entsprechend wählen, keinen Remote-Betrieb bei offenem K1 freigeben.
- **Messkompromiss akzeptiert zu dokumentieren:** Im Local-Betrieb wird vor K1 erfasst; Kontakt-/nachfolgende Leiterabfälle werden nicht kompensiert. Im Remote-Betrieb wird am DUT erfasst und mitkompensiert. Aus tatsächlichem Kontaktwiderstand und Laststrom den zusätzlichen Local-Spannungsfehler berechnen.
- **OFFEN:** K1 nach DC-Fehlerschaltleistung, K2 nach Isolation/Leckstrom/geeigneter Umschaltfolge wählen; Hold/Z_HIGH/Relaisfolge inklusive Prellen und Versorgungsausfall prüfen. V_MON-Eingangsschutz und Open-Sense-/FORCE-Unterbrechungsprüfung bleiben gesondert erforderlich.

### 5.4 EasyEDA Power-Out – Pin-/Kontaktprüfung (29.10.2026)

**Beleg:** `doc/hardware/Main_Board/SCH_MAIN_BOARD 2.0_10-Power-Out_2026-10-29.svg`, Sichtprüfung und visueller Abgleich mit Omron-Anschlusszeichnungen: G2RL S. 6 (Bottom View), G6S S. 5 (G6S-2F: Top View). PDF-Herstellerzeichnungen lokal gerendert und geprüft. Keine EDA-Netzliste oder PCB-Footprint geprüft.

- **K5 = G2RL-2 DC12:** Verwendete Schließerpaare **6–5 für FORCE_HI↔FORCE_HI_TERM**, **3–4 für FORCE_LO↔FORCE_LO_TERM** sind passend. Unbenutzte Ruhekontakte 7/2 bleiben offen. Spule 8 an +12 V, 1 an RE_POWER_ON; G2RL-Spule ohne vorgeschriebene Polarität. Keine vertauschte HI-/LO-Verbindung festgestellt.
- **U73 = G6S-2F DC12:** Gemeinsame Sense-Kontakte **9 = SENSE_HI**, **4 = SENSE_LO**. Stromlos: **9–10** verbindet intern FORCE_HI mit SENSE_HI, **4–3** intern FORCE_LO mit SENSE_LO. Angezogen: **9–8** verbindet SENSE_HI_TERM, **4–5** SENSE_LO_TERM. Die gezeichnete Local-/Remote-Belegung ist korrekt.
- **U73-Spulenpolarität korrekt:** **Pin 1 = +12 V**, **Pin 12 = RE_REMOTE_ON** zum Low-Side-Treiber. G6S ist polarisiert; diesen Unterschied zum G2RL beim Layout beachten.
- **R47/R48 = 100 kΩ:** Jeweils zwischen internem FORCE und zugehörigem SENSE; keine Verbindung auf die externe FORCE-Klemmenseite. Entspricht dem bestätigten 07:04-Konzept.
- **Treiber:** Q14/Q15 schalten die Relaisspulen nach DGND; R133/R134 ziehen die Gates nach DGND. D14/D15 sind mit Anode am geschalteten Spulenende und Kathode an +12 V als Freilaufpfad plausibel eingezeichnet. BSS138-Symbol-/Footprint-Pinzuordnung ist ohne EDA-/PCB-Daten nicht unabhängig bestätigt.
- **Steuerzustand OFF:** **POWER_ON = LOW und REMOTE_ON = LOW** erforderlich. Dann K5 offen und U73 Local: alle vier externen Klemmen kontaktmäßig getrennt, lokale Rückführung intern. Bei POWER_ON LOW/REMOTE_ON HIGH bleiben externe Sense-Leitungen verbunden. Steuerverknüpfung, Versorgungsausfälle und Relaistiming separat prüfen.
- **Prüfergebnis:** Sichtbare Relais-Pinbelegung und Verdrahtung dieses Blatts ohne festgestellten Belegungsfehler. Keine Gesamtfreigabe für Relais-DC-Fehlerschaltleistung, komplette Netzanbindung anderer Blätter oder PCB. G6S-2F-Top-View versus G2RL-Bottom-View beim Footprint nicht verwechseln.
- Quellen: https://omronfs.omron.com/en_US/ecb/products/pdf/en-g6s.pdf und https://omronfs.omron.com/en_US/ecb/products/pdf/en-g2rl.pdf

### 5.5 GUARD_PCB / GUARD_OUT – konkrete A-SMU-Prüfung (29.10.2026)

**Beleg:** `doc/hardware/Main_Board/SCH_MAIN_BOARD 2.0_7-I_Mon_2026-10-27.svg`; erstmals konkrete Guard-Topologie geprüft. Die zuvor betrachtete Beispielschaltung `Beispiele/MainBoardSchR3.pdf`, S. 2, ist nicht die A-SMU-Schaltung. Keine Simulation/PCB-/Messprüfung; keine Schaltplanänderung.

- **Nachverfolgte Funktion:** U6.4 puffert IM+; dessen Ausgang führt über R31 = 1 kΩ auf U6.3 (OPA4192), Pin 12 (+). Pin 13 (−) ist direkt mit Ausgang Pin 14 verbunden: korrekter Spannungsfolger. R30 = 1 kΩ verbindet den Treiberausgang mit GUARD_PCB, R29 = 1 kΩ mit GUARD_OUT. Rückkopplung liegt jeweils vor den Ausgangswiderständen. Beide Ausgänge teilen einen Guard-Treiber.
- **Potentialbezug:** Guard folgt grundsätzlich IM+ bzw. dem internen FORCE_LO-Knoten hinter dem Messshunt, relativ zur floating AGND. Er ist keine allgemeine Massefläche und darf nicht pauschal alle empfindlichen Knoten unterschiedlicher Potentiale umgeben. Guard-Fläche auf die zu schützenden, annähernd gleichpotentialigen Knoten abstimmen. Kapazität, Isolationsabstände und Leckstromfehler gesondert prüfen.
- **Vorhandene Schutzdiode D12:** BAV199LT1G antiparallel zwischen Guard-Treiberausgang (vor R29/R30) und IM+-Eingangsknoten **hinter R27 und vor R35**. Begrenzt deren Differenzspannung bei leitender Diode. Kein vollständiger Schutz des externen Guardanschlusses gegen beliebige Fremdspannung, insbesondere bei ausgeschalteter Versorgung. Im Fehlerfall kann D12 Fremdstrom in die Messkette koppeln.
- **D8 aus dem Beispiel:** Nicht pauschal zusätzlich zwischen ungepuffertem FORCE und Guard übernehmen. Für die Guard-Erzeugung nicht nötig; im A-SMU-Plan ist bereits D12 mit eigenem Anschluss-/Widerstandskonzept vorhanden. Zusätzliche Begrenzung darf R27 oder Messshunt nicht mit unerwünschten Strompfaden umgehen. Kein neuer D8-Bestückungsentscheid ohne konkrete Schutzdimensionierung.
- **Relais:** Für ausschließlich interne GUARD_PCB-Fläche ist kein Ausgangs-Trennrelais erforderlich. Wird GUARD_OUT extern zugänglich, muss er für das festgelegte vollständige DUT-Trennkonzept bei OUTPUT_OFF ebenfalls getrennt werden; sonst entsteht ein fünfter verbleibender Geräte-/DUT-Pfad. Einen passenden stromlos offenen Guard-Trennkontakt mit OUTPUT_EN koordinieren; konkrete Umsetzung noch offen. Alle Schutz-/Koppeldioden müssen die Trennstelle respektieren.
- **Externe Protection offen VOR PCB-FREIGABE:** R29 = 1 kΩ bietet Strombegrenzung/Entkopplung, ist allein aber kein nachgewiesener Überspannungs-/ESD-/Rückspeiseschutz. Erlaubte externe Spannung und Differenz zur beweglichen AGND festlegen, dann Ausgangsklemmen, zulässigen Rückstrom/Versorgungsaufnahme und Widerstands-Pulsleistung auch unversorgt auslegen. Bei beispielhaft 30 V allein über 1 kΩ entstehen 30 mA und 0,9 W; tatsächliche Fehlerströme erst aus dem vollständigen Schutzpfad berechnen. Ein weiterer Buffer kann die interne/externe Last entkoppeln, ersetzt aber selbst keinen Fremdspannungsschutz.
- **Prüfergebnis:** Spannungsfolger und Signalführung grundsätzlich plausibel; keine vollständige Guard-Schutz-/PCB-Freigabe. D12, R29/R30 und IM+-Leckpfade bei Fremdspannung, Guard-Kurzschluss und Kabel-/Flächenkapazität simulieren/messen. Die bloße Existenz der Diode bestätigt diese Fehlerfälle nicht.

### 5.6 Guard-Fehlerbetrachtung und einfacher Schutzkandidat (29.10.2026)

**Randbedingung:** Zunächst ausschließlich ±30-V-A-SMU. Zugängliche Anschlüsse betrachten: FORCE_HI_TERM, FORCE_LO_TERM, SENSE_HI/LO_TERM, geplanter GUARD_OUT_TERM sowie externe Spannungsquellen/Erde. AGND ist kein externer Anschluss; sein internes Potential folgt AMP_OUT vor dem Messshunt. Keine ungeprüfte Übernahme einer festen AGND-Annahme.

| Fehlverbindung / Zustand | Bewertung / Wirkung | Einfache Gegenmaßnahme |
|---|---|---|
| GUARD_OUT an FORCE_LO, beide annähernd gleichpotentialig | Im eingeschalteten Normalbetrieb grundsätzlich keine volle 30-V-Differenz; lokale Shunt-/Kontakt-/Leiterabfälle berücksichtigen. Als Verdrahtung trotzdem Guard nicht zum Lastleiter machen. | Guard eindeutig kennzeichnen; kein Laststrom über Guard führen. |
| GUARD_OUT versehentlich an FORCE_HI bei großer Ausgangsspannung | Guard-Treiber möchte FORCE_LO folgen; über R29 kann ungefähr die DUT-Ausgangsspannung plus lokale Abfälle anliegen. | Strombegrenzender, ausreichend belastbarer R29; bei weitergehender Schutzanforderung Guardfehler erkennen und Ausgang/Guard trennen. |
| Separate ±30-V-Quelle zwischen GUARD_OUT und FORCE_HI | Je nach Polarität und gegenläufigem FORCE_LO bis ungefähr 60 V plus Shunt-/Leiterabfälle gegen den internen Treiber möglich. Gleiche absolute 30-V-Potentiale sind dagegen kein solcher Differenzfehler. | R29 auf tatsächliche maximale Differenzspannung dimensionieren; nicht schlicht 30 V als lokalen Grenzwert verwenden. |
| Gerät aus / Analogversorgung fehlt, externe Quelle noch angeschlossen | Unversorgter OPV kann über seinen Ausgang/Schutzpfade rückgespeist werden; D12 kann Messknoten mitbeeinflussen. | Stromlos offener Guard-Trennkontakt; Versorgungsausfall und Kontaktabfallzeit gesondert nachweisen. R29 entschärft den Strom, garantiert allein keine unversorgte Festigkeit. |
| Guard an PE/Chassis, während die SMU andernorts referenziert ist | Je nach externem Aufbau eine reale Differenz zum Guard-/Floating-Potential; kein genereller Schutz durch Floating. | Kein PE-Schirmersatz; Anschlusskennzeichnung, R29 und Isolations-/Bezugsprüfung. |
| Große Kabelkapazität / Guardkurzschluss | Ladestrom, verzögerte Nachführung, ggf. Treiberüberlast oder Schwingung. | Serienwiderstand, definierte Kabel-/Lastgrenze, Sprung-/Kurzschlusstest. |

**Rechenvergleich (Spannung jeweils über dem Widerstand, nicht pauschal gegen Erde):**

| Spannung über R29 | Bisher 1 kΩ | Kandidat 10 kΩ |
|---|---|---|
| 30 V | 30 mA, 0,9 W | 3 mA, 0,09 W |
| 60 V | 60 mA, 3,6 W | 6 mA, 0,36 W |
| 65 V als Rechenreserve, keine zugesicherte Fehlergrenze | 65 mA, 4,225 W | 6,5 mA, 0,4225 W |

**KANDIDAT – möglichst einfache Hobby-Ausführung:** Zunächst vorhandenen Guardfolger/D12/R30 beibehalten; **externen Serienwiderstand R29 als 10 kΩ / 1 W prüfen** (konkreter Widerstand mit ausreichender Spannungs-/Pulsfestigkeit und Temperatur-Derating) und externen Guard über stromlos offenen Kontakt mit OUTPUT_EN koordinieren. Kein Bauteilwert geändert und noch kein Bestückungsentscheid. Zusätzlicher Buffer bzw. Portschutz-IC wird durch diese Vorprüfung nicht als zwingend festgelegt.

**Grenzen:** Größerer R29 senkt Fehlerstrom und OPV-Belastung, erhöht aber Guard-Spannungsabfall unter Last und die RC-Zeit am Kabel. Beispiel: 1 µA Guardstrom erzeugt bei 10 kΩ 10 mV Guardfehler; 1 nF Kabelkapazität ergibt näherungsweise 10 µs RC. Das ist kein nachgewiesener Schutz gegen beliebige Fremdspannung, Power-Off oder besonders empfindliche DUTs. Kurzschluss-/Temperaturschutz des OPA4192 ist kein allgemeiner Nachweis seiner Fremdspannungsfestigkeit am Ausgang. Vor PCB-Freigabe aktive und unversorgte Fehlerpfade einschließlich D12, Guard-PCB-Kopplung und der gemeinsam genutzten OPA4192-Kanäle prüfen. Für empfindliche DUTs bei Guardfehlverdrahtung ggf. Differenzüberwachung über R29 mit definierter Abschaltung zusätzlich vorsehen.

**Verifikation vor endgültiger Dimensionierung:** Guard auf IM+-Sollpotential halten lassen, den externen Fehlerstrom über R29 aufprägen, Roh-Guard/Messknoten/±15-V-Rails/Temperatur erfassen; beide Polaritäten und Versorgungsausfall testen bzw. vorab simulieren. Den 10-kΩ-Kandidaten erst danach als ausreichend geschützt bewerten. Quelle zu den OPV-Grenzen: https://www.ti.com/lit/ds/symlink/opa4192.pdf

## 6. Versorgung, Mess- und Digitalelektronik

| Baugruppe | Geplante Lösung / heutiger Stand | Status |
|---|---|---|
| ADC | **AD7177-2** mit isoliertem SPI über **ADuM4154** | KANDIDAT/Entwicklungsstand; Auflösung, Geschwindigkeit, Isolation prüfen |
| DAC | **DAC80004** (16 Bit, vier Kanäle); AD5686 als diskutierte Alternative | KANDIDAT |
| Referenzen | **ADR4525**, 2,5 V + Puffer; −2,5-V-Erzeugung; ADR1399 früher erwogen | Entwurf, Details prüfen |
| ADC-Frontend | Bipolare ±2,5-V-Monitorsignale auf ADC-gerechten Bereich verschieben, z. B. skalierende OpAmp-Stufe | Skalierung und Gleichtakt-/Fehlerbereiche OFFEN |
| Analog-OpAmps | OPA192/OPA172 für PCB-Stufen besprochen; OPA4192 für Referenz/MinMax; weitere OPA140/141/2333 diskutiert | stufenspezifische finale Auswahl OFFEN |
| Steuerung | zwei **RP2040**: System/GUI und A-SMU-Regler | Entwicklungsziel |
| Display | Riverdi **RVT50UQFNWC03 V2** | bisher gewählte GUI-Hardware |
| Schnittstellen | SPI0 Display/Ethernet; isoliertes SPI1, I²C0, PWM-Lüfter, Encoder/Taster, MCP23017 | Boardstand prüfen |
| DDS | AD9834 mit externen Komparatoren für zwei Rechteckfunktionen besprochen | optionaler Funktionsblock |

**GUI-Ziele:** Spannung `{Auto, 5 V, 30 V}`; Strom in früherem GUI-Entwurf `{Auto, 100 mA, 1 A}` bei Default 5 V / 100 mA. **GUI-Messbereichsliste muss an die aktuellen fünf (vier regulären + optionalen) Shunts angepasst werden**, nicht mit älterer Hardware-Auswahl verwechseln.

### 6.1 Netzteil als wiederverwendbare Baugruppe (28.10.2026)

- **FESTGELEGT – Nutzeranforderung:** Das Netzteil der A-SMU soll **auch für andere Projekte verwendet werden**. Es wird als eigenständig dokumentierte Versorgungsbaugruppe weitergeführt; nicht nur als fest auf A-SMU zugeschnittenes Hilfsboard.
- **Verbindliche Potentialdomänen:** Floating **+15 V/−15 V/+5 V gegen AGND**, digital **+12 V/+3V3 gegen DGND**, Leistung **+HV/−HV gegen HV_GND** und kleine **+15V0/−15V0 gegen HV_GND**. Für A-SMU bleibt die externe Anbindung **AGND = AMP_OUT über R150** maßgeblich. Keine ungeprüfte AGND-/DGND-/HV_GND-Brücke ergänzen; andere Projekte brauchen eine ausdrückliche eigene Referenzfestlegung.
- **OFFEN – Schnittstellendokumentation:** Je Ausgang Nominal-/Maximalspannung, garantierten Dauerstrom, Gesamtlastbudget, Ripple, Temperatur-Derating, Steckerbelegung, Rückstrompfad, erlaubte Masseanbindungen und kapazitive Last festlegen. Parallele Stecker vervielfachen die verfügbare Leistung nicht.
- **OFFEN – Projektprofile:** Neue Anwendungen separat auf Anlaufstrom, Lastsprünge, Rückspeisung, Kurzschluss, Abschalt-/Standby-Verhalten, Isolation, Kühlung und gegebenenfalls Trafo-/Bestückungsvarianten prüfen. Die 230-V-Trafos machen den Gesamtaufbau nicht allein wegen der Universalnetzeingänge der IRM-Module zu einem Universalnetzgerät.
- **Standby-Verhalten:** +12 V bleibt bei SYSTEM_OFF aktiv; +3V3, Analogtrafo und Ringkerntrafo werden über K1 geschaltet. Das ist zu dokumentieren und je Zielprojekt zu bewerten.

### 6.2 Netzteilprüfung der fünf EasyEDA-Blätter (28.10.2026)

**Beleg:** Sichtprüfung/rechnerischer Herstellerabgleich der Exporte unter `doc/hardware/NETZTEIL/SCH_NETZTEIL 2.0_1-Power_on_2026-10-28.svg` bis Blatt 5 `Interface`; keine neue Simulation, ERC-/PCB-Prüfung oder Hardwaremessung. Das Prüfregister in Abschnitt 0.2 enthält den Status je Funktionsblock. Keine Schaltplanwerte geändert.

- **C96 aktualisiert (Nutzeränderung 28.10.2026):** **10 µF statt zuvor 330 µF**. Mit R1 = 100 kΩ beträgt die Entladezeitkonstante beim Drücken **1 s**; über R1 + R136 = 200 kΩ beträgt die Ladezeitkonstante nach Loslassen **2 s**. Beispielhafte Schmitt-Schwellen 0,4·VDD / 0,6·VDD ergeben aus vollständig eingeschwungenem Zustand ungefähr **0,92 s bis zur Reaktion beim Drücken** und **1,83 s bis zum Eingangsrückschalten nach Loslassen**. Das sind Rechenbeispiele, keine garantierten Schaltzeiten. Die vorherigen Angaben 33 s / 66 s sind ersetzt. Nutzeränderung dokumentiert; SVG nicht erneut geprüft. **OFFEN:** tatsächliche Schmitt-Schwellen, Kondensatortoleranz/Leckstrom und gewünschte Bedienfunktion messen bzw. bestätigen.
- **Floating-Analogversorgung grundsätzlich plausibel:** U4 = 2×18 V / 2×277 mA, ungefähr 10 VA gesamt. Wicklungs-RMS-Strom ist bei kapazitiver Gleichrichtung nicht identisch mit nutzbarem DC-Strom. +5 V aus 7805 belastet zusätzlich den +15-V-Zweig. **OFFEN:** gemeinsames Lastbudget und thermische Auslegung aller drei Regler; Netz-/Leerlaufmaximum gegen deren etwa 35-V-Eingangsgrenze prüfen.
- **HV-Spannung:** 2×30 V AC ergeben nominal ungefähr **±41…42 V DC** vor Last-/Brummeinflüssen; je Schiene **6600 µF**. Das ist nicht die feste ±36-V-Versorgung des bisherigen LTspice-Modells. **OFFEN:** reale Min-/Maximalwerte, Trafo-/Gleichrichter-/Elkorippelstrom, Wärme und Entladezeit. 63-V-Elkos anhand des konkreten Trafo-Leerlaufmaximums bewerten.
- **Prioritärer HV-Schutzpunkt:** RXEF185 sind mit 72 V spezifiziert. Bei Kurzschluss +HV gegen −HV sind nominal ungefähr 82…84 V Gesamtspannung möglich; nach zuerst erfolgendem Auslösen kann ein einzelner PPTC zu viel Spannung übernehmen. **OFFEN VOR HV-FREIGABE:** Fehlerfallanalyse, maximale Schutzgliedspannung und prospektiver Kurzschlussstrom; gegebenenfalls anderes Schutzkonzept mit ausreichend bemessener Spannungs-/Ausschaltstromfestigkeit.
- **Netzschutz offen:** F6/F7 zeigen keine vollständige Spezifikation der Sicherungseinsätze. Stromwert, Charakteristik und Ausschaltvermögen sowie MOV-/Trafo-/Relais-/NTC-Koordination festlegen. U10 begrenzt kalten Einschaltstrom, aber kein garantierter Schutz beim heißen Neustart. Netzabstände, Schutzklasse/Gehäuse und gegebenenfalls PE-Anbindung erst am realen Aufbau/PCB freigeben; AGND bleibt davon getrennt zu bewerten.
- **±15V0 – konkrete Last festgelegt (Nutzerpräzisierung 28.10.2026):** Diese Schienen versorgen in A-SMU **ausschließlich den OPA192 im V_MON**. DN2540/150 Ω plus TL431-Teiler 50 kΩ/10 kΩ sind für diesen einzelnen Verbraucher grundsätzlich plausibel; die frühere pauschale Lastbudgetbeanstandung wird zurückgenommen. Teiler- und TL431-Strom sind interne Auslegungsanteile, kein nachgewiesener Versorgungsmangel für den V_MON-OPV. **OFFEN zur Verifikation:** Stromquellenstreuung, OPV-Versorgungsstrom einschließlich Ausgangslast sowie Stabilität/Abblockung im realen Betrieb. Für andere Projekte eine geänderte Last gesondert dimensionieren.
- **Digitalversorgung:** IRM-15-12 nominell 12 V/1,25 A; IRM-10-3.3 nominell 3,3 V/2,5 A bzw. 8,25 W. Nachgeschaltete PPTCs, Kontakte und Temperatur begrenzen das reale Lastbudget. Vorhandene Puffer-/100-nF-Kondensatoren berücksichtigt. **OFFEN:** Rail-Sequenz/Rückspeisung und tatsächliche Projektlasten.
- **Nächste Verifikation:** Last-/Temperaturmatrix, Netz-Min/Max, kalter/heißer Neustart, brownout/reset, Abschalt-/Entladeverläufe und Kurzschluss-/Rückspeisefälle; separate Netzlisten-/Footprint-/Layoutprüfung. Keine allgemeine Hardwarefreigabe aus dieser SVG-Prüfung ableiten.

## 7. Platinen und Verbindungen

- **Vier Platinen:** Digital/GUI, Analog/Regler/Messung, MOSFET-Endstufe und Netzteil.
- Analoge Signalführung: Stern-/Kelvinbezüge, GUARD-Flächen für hochohmige `I_MON`/`V_MON`, entkoppelte Referenzspannungen und getrennte Last-/Messrückwege vor endgültigem Routing prüfen.
- Verbindung über **32-polige gewinkelte Harting-Messerleiste**; auch digitale Signale zur Endstufe über diese Verbindung vorgesehen.
- **FESTGELEGT (28.10.2026):** `AGND = AMP_OUT` über R150; `FORCE_LO` liegt hinter dem Messshunt. **OFFEN:** konkrete Kelvin-Abgriffe, Rückstromführung und zusätzliche Messleitungen über Harting. Die elektrische Floating-Referenz nicht mit der noch offenen Leiterführung verwechseln; siehe Abschnitt 4.1.
- Endstufen-/Shunt-PCB thermisch und für 1 A / Hochspannungsvariante isolationsgerecht prüfen; Hochspannungsvariante benötigt eigene Kriech-/Luftstreckenbeurteilung.

## 8. LTspice-Stabilität – abgeschlossene ohmsche Grobtests

**Zuordnungshinweis vom 29.10.2026:** Dieser Teststand gehört zum bisherigen Bereichssatz in Abschnitt 3.1 und ist keine neue Verifikation der aktuellen Nutzerfestlegung in Abschnitt 3.

**Quelle:** `FINAL_CV_CC_Umschaltung.asc` aus `C:\Users\aw\OneDrive\Projects\A-SMU\LTSpice\`, nur in Testkopien modifiziert. **LTspice XVII** unter `C:\Program Files\LTC\LTspiceXVII\XVIIx64.exe`, im Batch `-Run -b`; Python 3.10.6. Pro Simulationsversuch **60 s Timeout**.

| Testserie | Umfang | Ergebnis | PI R3/C1 | Einordnung |
|---|---:|---|---|---|
| `Test_1A` | 32 | **32 OK** | 10 kΩ / 1 nF | stationärer Grobtest |
| `Test_100mA` | 32 | **32 OK** | 10 kΩ / 1 nF | stationärer Grobtest |
| `Test_1mA` | 32 | **32 OK** | 22 kΩ / 47 nF | stationärer Grobtest |
| `Test_10uA` | 32 | **32 OK** | 22 kΩ / 47 nF | stationärer Grobtest |
| `Test_1uA` | 32 | **32 OK laut Testbericht** | 22 kΩ / 47 nF | **Netlist des 1-MΩ-Shunts wegen widersprüchlicher Metadaten verifizieren** |

**Verifizierter regulärer Stand: 128/128 OK.** Einschließlich des optionalen 1-µA-Berichts **160/160 als OK gemeldet**, vorbehaltlich des Shunt-Nachweises. Tests: 5-V-/30-V-Skalierung, CV+/CV−/CC+/CC−, normale Last, Kurzschluss, Leerlauf und Lastwechsel. `OK` bedeutet plausibler stationärer Arbeitspunkt und keine im späten Auswertefenster erkannte Schwingung; **kein Frequenzgang-, SOA-, Phasenreserve- oder Hardware-Nachweis**.

**Historische Stabilitätsbeobachtungen:** ältere Varianten mit I_MON-Verstärkung 10 und hochohmigen Shunts schwingungsanfällig; Verstärkung 2 führte zu besseren Voruntersuchungen. Erhöhung der Integrator-Kompensation auf 22 kΩ/47 nF verhinderte zuvor bei hohen Shunts auftretende langsame/timeoutbehaftete Fälle. Frühere Versuche mit 50 kΩ/100 nF waren ebenfalls stabil, aber vergleichsweise träge. Diese Beobachtungen stammen aus verschiedenen Modellständen, daher nicht ohne Kennzeichnung vermischen.

### 8.1 Aktuelle PI-Kompensationsmatrix aus Codex (26.10.2026)

**Quelle:** vom Nutzer übermittelte erste Codex-Tabelle der laufenden Testreihe; **Zwischenstand**, kein Abschlussbericht und keine Hardwarefreigabe. Der Teststand in Abschnitt 8 erfasst ältere ohmsche Simulationen und ist davon zu unterscheiden.

| Strombereich | V-Bereich | R1 (P-Anteil) | R3 | C1 | Status / Evidenz |
|---|---|---:|---:|---:|---|
| **±1 A** | 5 V / 30 V | **2 kΩ** | **10 kΩ** | **1 nF** | Bisherige Einstellung **beibehalten**; **32/32 automatische ohmsche Grobtests OK** |
| **±100 mA** | 5 V / 30 V | **2 kΩ** | **10 kΩ** | **1 nF** | Bisherige Einstellung **beibehalten**; **32/32 automatische ohmsche Grobtests OK** |
| **±1 mA** | 5 V / 30 V | **2 kΩ** | **22 kΩ** | **47 nF** | **Vorläufig beibehalten**; lange RC-Verläufe bei kapazitiven Lasten **offen**. Ohmsche Grobtests zuvor **32/32 OK** |
| **±10 µA** | **5 V** | **4,7 kΩ** | **47 kΩ** | **100 nF** | **NEUER KANDIDAT**; laut Codex-Tabelle deutlich bessere Dämpfung bei **100 nF Last**; vollständige Validierung offen |
| **±10 µA** | **30 V** | **4,7 kΩ** | **47 kΩ** | **100 nF** | **Gleicher KANDIDAT**, ausdrücklich **noch gezielt zu prüfen** |

**Abgrenzung / technische Hinweise:**

- Für den **±10-µA-Bereich** gilt die frühere Konfiguration **R3=22 kΩ / C1=47 nF** weiterhin als *historisch erfolgreich getestete ohmsche Grobtest-Einstellung*. Der neue Kandidat **R1=4,7 kΩ, R3=47 kΩ, C1=100 nF** ist **noch nicht mit 32/32 für die Gesamtmatrix nachgewiesen**.
- Die Ergebnisse bei **±1 mA** bestätigen die alten ohmschen Arbeitspunkte, ersetzen jedoch **keinen Nachweis** für lange kapazitive RC-Verläufe.
- In diesem Codex-Zwischenstand sind **keine neuen Werte für den optionalen ±1-µA-Bereich** enthalten. Dessen früherer experimenteller R3/C1-Wert **22 kΩ / 47 nF** wird daher **nicht geändert**; **R1 ist aus dieser Tabelle nicht bestimmt**.
- Bei der bisherigen vereinfachten Reglerinterpretation gilt **P ∝ R1/R3**, **I ∝ 1/(R3·C1)**; damit entspricht der neue 10-µA-Kandidat einer reduzierten Integratorwirkung gegenüber 22 kΩ / 47 nF. Tatsächliche Werte, Vorzeichen und Stabilitätsreserve nur aus der vollständigen Topologie/RAW-/AC-Analyse ableiten.
- **Änderung erst als verbindliche PCB-Bestückung freigeben**, wenn bei 5 V und 30 V sowohl stationäre als auch dynamische CV+/CV−/CC+/CC−-Fälle mit normalen, offenen, kapazitiven und wechselnden Lasten geprüft sind.

## 9. Kapazitive Lasttests – aktueller Zwischenstand (25.10.2026)

**Zuordnungshinweis vom 29.10.2026:** Dieser Teststand gehört zum bisherigen Bereichssatz in Abschnitt 3.1 und ist keine neue Verifikation der aktuellen Nutzerfestlegung in Abschnitt 3.

- Testskript `Test_Kapazitiv.py`; geplante Matrix: **4 Strombereiche × 2 Spannungsbereiche × 4 Betriebsarten × 4 Kapazitäten = 128 Tests**; optionaler 1-µA-Bereich gesondert.
- Kapazitäten **1 nF, 100 nF, 1 µF, 10 µF** zwischen `FORCE_HI` und `FORCE_LO`; ohmsche Last über `R5` als **100 % → 70 % → 100 %** umgeschaltet.
- Letzter vorliegender Bericht: **74/128 abgeschlossen:** **63 OK**, **2 SCHWINGVERDACHT**, **4 TIMEOUT**, **5 ARBEITSPUNKT_FALSCH**. *Dies ist ein Zwischenstand, keine Freigabe für kapazitive Lasten.*
- Auffällige Einzelfälle: `V30_1A_CVminus_C1u` (Schwingverdacht); `V30_100mA_CVplus_C100n` (Schwingverdacht, Spitze laut Auswertung ~8,17 V); `V30_1A_CVplus_C10u` (~+28,71 statt +24 V); `V30_1A_CVminus_C10u` (~−28,35 statt −24 V); `V30_100mA_CVminus_C100n` (~−31,77 statt −24 V); `V30_100mA_CVplus_C1u` und `...CVminus_C1u` (Timeout); bei 1-mA-/5-V-CV-/10-µF-Test ebenfalls Timeout berichtet. **Keinen dieser Fälle durch PI-Änderung kaschieren, bevor Schaltungsansteuerung, Simulationsfenster und Messauswertung kontrolliert wurden.**
- Frühere Testskriptfehler: M3/M4 versehentlich auch für 1-A-Test ausgeschaltet → ca. 200-mA-HW-Limit, resultierende **3,14 V statt 4 V** bei 16 Ω/250 mA Soll; in späterem Testskript korrigiert. **Nur die spätere 74-Fälle-Serie für die Beurteilung verwenden.**
- Automatisierungsproblem: `PermissionError [WinError 5]` beim Ersetzen der MD-Ergebnisdatei innerhalb von OneDrive. Empfehlung: Testausgaben unter `C:\A_SMU_Test_Kapazitiv` (außerhalb OneDrive), robuste CSV-Zwischenspeicherung und `--resume` mit Erhalt der 74 Fälle.
- **OFFEN:** Restliche 54 Fälle simulieren; RAW-Verläufe für die oben genannten Fälle prüfen, ggf. Auswertung und Integrator-/MinMax-Übergänge validieren, dann getrennte Prüfungen mit realistisch modellierten Kapazitäten/ESR/Leitungsinduktivitäten.

## 10. Zweite Gerätevariante ±140 V – nur Planungsstand

- **Ziel:** ±140 V, ±100 mA, Transformator ungefähr 2 × 120 V AC; möglichst gleiche Basisarchitektur, aber eigene sichere Endstufen-/Versorgungsauslegung.
- Alte Backlog-Tabelle zur 140-V-Version (im Anhang) ist **historisch** und **kein freigegebener Strombereichssatz**. Übernahme der für ±30 V getesteten 1-Ω/10-Ω/1-kΩ/100-kΩ/1-MΩ-Bereiche nicht ungeprüft möglich, insbesondere wegen Energie, Eingangsspannungsgrenzen und Isolationsanforderungen.
- **Versionsneutrale Namen festgelegt (29.10.2026):** Auch hier `I_RANGE_n_EN` / `V_RANGE_n_EN` verwenden. Die konkrete ±140-V-Zuordnung von Messbereich, Shunt und Kanal ist noch offen; die aktuelle ±30-V-Tabelle nicht ungeprüft übernehmen.
- **OFFEN:** Strombereiche und Shuntleistung; Schutz des 1-kΩ-Pfads gegen bis 140 V (theoretisch **19,6 W** bei gesamter Spannung am 1-kΩ-Widerstand); MUX-/OP-Amp-Überspannung; MOSFET-SOA; Abstand/Relais; Spannungsbereichs-V_MON und Kalibrierung.

## 11. Entscheidungen, die noch zu treffen sind (priorisierter Backlog)

### Ergänzung 30.10.2026 – Versorgung, Masseführung und Signalstecker Mainboard/Endstufe

- **Nutzerangabe:** Mainboard und Endstufe benötigen jeweils +12 V, +3V3 und DGND. `SET_AMP` wird an der Endstufe einfach gegen lokale AGND ausgewertet, nicht differentiell.
- **OFFEN / besprochener Kandidat:** Digitalen Masse-Verteilpunkt am Mainboard vorsehen: Netzteil-DGND zum Mainboard, von dort eigene ausreichend dimensionierte DGND-Rückleitung zur Endstufe; dann keine zusätzliche direkte Netzteil-DGND-Leitung zur Endstufe. Rückströme direkt zum Verteilpunkt führen, nicht durch empfindliche Analogbereiche. Noch keine Schaltungs-/Layoutfreigabe.
- **Wichtige Korrektur zur Chat-Empfehlung:** Die pauschale Empfehlung, AGND und DGND an der Endstufe zu verbinden, ist nicht gültig für die dokumentierte Floating-Architektur. Gemäß Abschnitt 4.1 bleibt AGND über R150 auf AMP_OUT bezogen und von DGND/HV_GND getrennt. Der digitale Sternpunkt legt die analoge Referenzführung nicht fest.
- [ ] Tatsächliche AGND-, DGND- und SET_AMP-Verbindungen auf beiden Platinen und am Netzteil anhand der aktuellen Schaltpläne abgleichen. Zusätzliche Masseadern können bei bereits vorhandenen Verbindungen Schleifen bilden; keine ungeprüfte Brücke ergänzen. Referenzfehler durch Leitungsabfälle und Rückstrompfade beurteilen.
- [ ] Signalanschluss für `SET_AMP`, `ENDSTUFE_1A_EN`, `Z_HIGH_EN`, `FAN_PWM`, `FAN_SENSE`, `SCLK`, `SDA` festlegen. 10-poliger Wannenstecker als Kandidat; endgültige Polzahl, AGND-/DGND-Kontakte, Pinout und Trennung der Potentialdomänen erst nach obigem Abgleich bestimmen. Keine endgültige Steckerbelegung aus dem Chat übernehmen.
- **Beleg:** Gespräch vom 30.10.2026; keine neue Simulation, Netzlistenprüfung oder Hardwaremessung.

### P0 – Funktion / Sicherheit

- [ ] **Codex-PI-Kandidaten (26.10.2026):** Für **±10 µA** `R1=4,7 kΩ / R3=47 kΩ / C1=100 nF` bei **30 V** sowie vollständige kapazitive Lastmatrix verifizieren; bisherige 5-V-/100-nF-Dämpfungsverbesserung durch RAW-Daten / Metriken dokumentieren. R1-Werte für alle Bereiche in Netlist prüfen.
- [ ] **±1 mA:** lange kapazitive RC-Verläufe mit `R1=2 kΩ / R3=22 kΩ / C1=47 nF` durch angemessene Simulationsdauer und Einschwingkriterien gesondert bewerten.
- [ ] `Test_Kapazitiv.py` mit Codex reparieren, 74 Ergebnisse ohne Verlust übernehmen, fehlende 54 Testfälle ausführen und Fehlfälle anhand RAW/Netlist beurteilen.
- [ ] Bei CV↔CC- und Polaritätswechseln Überschwingen, Einschwingzeit, inaktive Integratoren und **Anti-Windup/Haltefunktion** des Min/Max-Reglers prüfen.
- [ ] Ausgangsrelais + Z_HIGH + Shunt-Relais gemeinsam fehlerfallsicher sequenzieren (auch extern gespeistes DUT, Sense offen, Relais klebt, Stromausfall).
- [ ] HW-Grenze im kleinen Bereich (~200 mA) mit der **tatsächlichen Schaltung/OpAmp-Rückwirkung** verifizieren; thermische Shunt-Fälle 1 Ω/10 Ω/1 kΩ einschließlich Versorgung, Dauer, Pulsenergie und Derating bewerten.
- [ ] Absolute Grenzwerte und transienten Schutz von ADG1209, DG409 und Monitor-Eingängen verifizieren.
- [ ] Stabilität mit den **tatsächlich für PCB vorgesehenen OpAmp-Modellen** sowie ausreichende Phasenreserve/Frequenzgang prüfen.

### P1 – Messqualität / Bauteile / Layout

- [ ] Fünften ±1-µA-/1-MΩ-Bereich: Netlist korrekt? Messrauschen/Leckage? fünfter Hardwarepfad? Entscheidung **beibehalten oder streichen**.
- [ ] Endgültige Präzisionsshunts je Messbereich (insbesondere **1 Ω bei 1 A**) nach Toleranz, TCR, Leistung, Kelvin, Spannung und Preis auswählen; historisches Widerstandsarchiv im Anhang verwenden, Bestände neu prüfen.
- [ ] Messfehlerbudget V_MON/I_MON über Bereichswechsel, OP-Amp-Offset, Referenzen, Relaisleckage, Temperatur; Kalibrierstrategie festlegen.
- [ ] Remote-Sense-Drahtbruch, FORCE↔SENSE-Hilfswiderstände, VMON_PROT und Kelvin-Rückleitung über Harting abschließen.
- [ ] Ausgangssnubber/ESR/Leitungsinduktivität und reale DUT-Kapazitäten untersuchen; SOA und MOSFET-Thermik prüfen.
- [ ] Entscheidung **1A_ON gekoppelt an Shunt-Relais oder unabhängig**; Logik/Failsafe dokumentieren.

### P2 – System / Zweitvariante

- [ ] GUI-Bereichsliste und Firmware-Interlocks auf aktuelle Messbereiche umstellen.
- [ ] ADC-/DAC-/Referenz-/OpAmp-Bestückung pro PCB-Stufe finalisieren; Harting-Pinout und Trennungs-/Massekonzept fixieren.
- [ ] HV-Variante ±140 V getrennt auslegen und validieren (MOSFET, Schutz, Relais, MUX, Wärme, Isolationsabstände).

## 12. Test- und Dokumentationsreferenzen

- **Schaltung:** `C:\Users\aw\OneDrive\Projects\A-SMU\LTSpice\FINAL_CV_CC_Umschaltung.asc` (**nie von Tests überschreiben**).
- **Automatisierung:** `C:\Users\aw\OneDrive\Projects\A-SMU\LTSpice\TEST-SCRIPT\Test_Kapazitiv.py` sowie `Test_1A.py`, `Test_100mA.py`, `Test_1mA.py`, `Test_10uA.py`, optional `Test_1uA.py`.
- **Berichte:** `Test_1A_Ergebnisse(4).md`, `Test_100mA_Ergebnisse(1).md`, `Test_1mA_Ergebnisse(1).md`, `Test_10uA_Ergebnisse.md`, `Test_1uA_Ergebnisse.md`, `Test_Kapazitiv_Ergebnisse(1).md`; bei Namensunterschieden **aktuellste vollständige Version** und Testzeitpunkt prüfen.
- **Arbeitsweise:** Künftige Testserie liefert `.csv`, `.md`, und `.raw/.log/.net` zu **jeder Testschaltung**. Nach verifizierter Änderung Stand + Hinweis im aktuellen Backlog aktualisieren; Vergleichsstudien/Einzelplots können in gesonderten Testprotokollen bleiben.

---

# Anhang A – Unveränderter historischer Backlog (Stand 24.10.2026)

> **HISTORISCH/ERSETZT:** Der folgende Text bleibt vollständig erhalten, einschließlich ehemaliger Bereichsangaben und früherer Händlerlisten. **Bei Widersprüchen gelten ausschließlich die aktuellen Abschnitte 0–12 oben.** Historische Preis-/Bestandsangaben nicht ohne aktuelle Prüfung als lieferbar interpretieren.

# A-SMU – Backlog

Stand: 24.10.2026  
Einträge: Strommessbereiche und Shuntbestückung vom 17./18.10.2026; OP-Amp-Bestückung und Simulation vom 24.10.2026.

Diese Datei hält die hier besprochenen Festlegungen fest. Eine bestehende Datei namens smu_backlog war beim Anlegen nicht auffindbar; frühere Backlogpunkte sind hier deshalb nicht vollständig enthalten.

## Festlegungen

- Vier Strommessbereiche je Gerätevariante; kein zusätzlicher 1-MΩ-/±100-nA-Bereich.
- Der ADG1209 stellt vier differentielle Messkanäle bereit; alle vier werden benötigt.
- Nennspannungsabfall am Shunt bei Bereichsendwert: jeweils ±100 mV.
- Ziel für die Präzisionsshunts: möglichst ±0,1 % und |TK| ≤ 10 ppm/K; konkrete Typen teilweise noch offen.
- Vier gleiche Widerstände werden bei den unten genannten Netzwerken als zwei Reihenzweige parallel geschaltet: (R + R) || (R + R) = R.
- Leistungsangaben sind Berechnungswerte. Zulässige Dauerleistung einschließlich Temperatur-Derating und Leiterplattenwärmeabfuhr prüfen.

## ±30-V-Version, maximal ±1 A

| Strommessbereich | Effektiver Shunt | Vorgesehene Bestückung | Nennleistung gesamt | Verlustleistung bei Bereichsendwert | Überlast bei bisherigem Maximum 1 A / 30 V | Überlast mit Ausgangsstrombegrenzung auf 100 mA |
|---|---|---|---|---|---|---|
| ±1 A | 0,1 Ω | 1 × 0,1 Ω / 1 W | 1 W | 100 mW | 100 mW | entfällt: 1-A-Betrieb in diesem Bereich |
| ±10 mA | 10 Ω | 4 × 10 Ω / 0,5 W, Reihen-Parallel | 2 W | 1 mW | 10 W | 100 mW gesamt; 25 mW je Widerstand |
| ±100 µA | 1 kΩ | 4 × 1 kΩ / 0,5 W, Reihen-Parallel | 2 W | 10 µW | 0,9 W | 0,9 W gesamt; 225 mW je Widerstand |
| ±1 µA | 100 kΩ | 1 × 100 kΩ / 0,25 W | 0,25 W | 0,1 µW | 9 mW | 9 mW |

### Endstufenbegrenzung

- Im ±1-A-Messbereich bleibt die Endstufe für 1 A freigegeben.
- Für die kleineren Shuntbereiche ist eine Umschaltung der Endstufenbegrenzung auf 100 mA vorgesehen.
- Die Begrenzung muss den tatsächlichen Ausgangsstrom in beiden Richtungen begrenzen; Ruhestrom und Bauteiltoleranzen bei der Dimensionierung berücksichtigen.
- Diese Maßnahme schützt den 10-Ω-Shunt thermisch im betrachteten stationären Überlastfall.
- Beim 1-kΩ-Shunt begrenzen bereits 30 V den Strom auf 30 mA; die 100-mA-Begrenzung greift dort nicht. Das Vierernetzwerk trägt die 0,9 W.
- Kurze Stromspitzen, Umschaltvorgänge und externe Einspeisung sind durch diese stationäre Rechnung noch nicht bewertet.

## ±140-V-Version, maximal ±100 mA

| Strommessbereich | Effektiver Shunt | Bisher vorgesehene Bestückung | Verlustleistung bei Bereichsendwert | Überlast bei 100 mA / 140 V | Status |
|---|---|---|---|---|---|
| ±100 mA | 1 Ω | 1 × 1 Ω / mindestens 0,5 W | 10 mW | 10 mW | thermisch ausreichend im betrachteten Fall |
| ±10 mA | 10 Ω | 1 × 10 Ω / 0,5 W | 1 mW | 100 mW | thermisch ausreichend im betrachteten Fall |
| ±100 µA | 1 kΩ | ursprünglich 0,25 W; endgültige Ausführung offen | 10 µW | 10 W | Schutz bzw. Dimensionierung offen; auch 4 × 0,5 W = 2 W reichen für Dauerüberlast nicht |
| ±1 µA | 100 kΩ | 1 × 100 kΩ / 0,5 W, Betriebsspannung ≥ 200 V | 0,1 µW | 196 mW | Derating und Spannungsfestigkeit prüfen |

## Widerstände

Archiv der bereits besprochenen DigiKey- und Mouser-Kandidaten. Die folgenden Preis- und Bestandswerte stammen aus den damaligen Recherchen, wurden am 18.10.2026 nicht erneut abgefragt und sind keine aktuellen Lieferzusagen. Frühere Assistentenvorschläge sind als Kandidaten dokumentiert, nicht als verbindliche Nutzerentscheidung.

### DigiKey – 1-kΩ-Widerstände, Recherche vom 17.10.2026

Preise netto bei Einzelabnahme.

| Hersteller | Hersteller-Teilenummer / Produktlink | Bauform | Leistung | Toleranz | TK | Preis/Stück | Damaliger Bestand |
|---|---|---|---|---|---|---|---|
| TE Connectivity | [RQ73C2B1K0BTD](https://www.digikey.de/en/products/detail/te-connectivity-passive-product/RQ73C2B1K0BTD/9482432) | 1206 | 0,5 W¹ | ±0,1 % | ±10 ppm/K | 0,72 € | 3.738 |
| Vishay | [TNPW12101K00BEEA](https://www.digikey.de/en/products/detail/vishay-dale/TNPW12101K00BEEA/1608380) | 1210 | 0,5 W | ±0,1 % | ±25 ppm/K | 1,42 € | 3.497 |
| TE Connectivity | [UPF50B1K0V](https://www.digikey.de/en/products/detail/te-connectivity-passive-product/UPF50B1K0V/5584399) | bedrahtet, axial | 0,5 W | ±0,1 % | ±5 ppm/K | 8,12 € | 561 |

¹ Das [RQ73-Datenblatt](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=1773270-2&DocType=DS&DocLang=English) nennt 0,5 W bei 85 °C für RQ73 2B; die TE-Produktübersicht nennt abweichend 0,4 W. Bei Bestellung Datenblattrevision und konkrete Ausführung prüfen.

**Bisheriger Favorit des Assistenten:** RQ73C2B1K0BTD für das 4 × 1-kΩ-/0,5-W-Netzwerk der 30-V-Version. Vier Stück kosten zum damaligen Einzelpreis 2,88 € netto. Noch keine verbindliche Auswahl durch den Nutzer. Der Vishay-Typ mit ±25 ppm/K erfüllt das angestrebte ≤10-ppm/K-Ziel nicht.

### DigiKey – preisgünstigere 100-mΩ-Shunts ab 1 W, Recherche vom 17.10.2026

Alle sechs Typen wurden als zweipolige Alternativen aufgeführt. Preise und Spezifikationen gemäß damaliger Vergleichsliste; konkrete Datenblattbedingungen und Ausführung vor Auswahl nochmals prüfen.

| Hersteller | Hersteller-Teilenummer | Bauform | Leistung | Toleranz | TK | Damaliger Preis/Stück | Damaliger Bestand |
|---|---|---|---|---|---|---|---|
| Yageo | PE2512FKE070R1L | 2512 | 1 W | ±1 % | ±50 ppm/K | 0,50 € | 8.544 |
| Ohmite | LVM25FVR100E-TR | 2512 | 1 W | ±1 % | ±50 ppm/K | 0,62 € | 89.897 |
| Bourns / Riedon | MSR5-0R1F1 | radial, bedrahtet | 5 W | ±1 % | ±20 ppm/K | 1,62 € | 615 |
| C&B / RESI | PCSK2512DR100P6 | 2512 | 1 W | ±0,5 % | ±25 ppm/K | 2,18 € | 6.547 |
| Isabellenhütte | VMP-R100-1.0-U | 2010 | 3 W | ±1 % | ±20 ppm/K | 2,32 € | 3.174 |
| C&B / RESI | PCSR2512BR100M6 | 2512 | 1 W | ±0,1 % | ±15 ppm/K | 4,82 € | 1.408 |

**Damals hervorgehobene Alternativen:** VMP-R100-1.0-U und PCSR2512BR100M6. Keine verbindliche Auswahl dokumentiert. Keiner der sechs Typen erreicht das angestrebte ≤10-ppm/K-Ziel; sie bleiben als günstigere Kompromisse erhalten. Vollständige Produktdetail-URLs waren in der wiedergefundenen Vergleichsliste nicht erhalten; Bestellung über die exakte Hersteller-Teilenummer zuordnen.

### DigiKey – zuvor besprochene Präzisionsshunts, Recherche vom 16.10.2026

| Hersteller-Teilenummer | Widerstand | Bauform / Anschlüsse | Leistung | Toleranz | TK | Damaliger Preis/Stück | Damaliger Bestand |
|---|---|---|---|---|---|---|---|
| Y14750R10000B9R | 0,1 Ω | CSM3637F, 4-polig | 3 W | ±0,1 % | ±10 ppm/K | 29,16 € | 296 |
| Y08501R00000B9R | 1 Ω | VCS1625, 4-polig | 0,5 W | ±0,1 % | ±10 ppm/K | 27,62 € | 3.968 |
| Y08560R10000F9W | 0,1 Ω | VCS1625P, 4-Kelvin | 1 W | ±1 % | ±20 ppm/K maximal | 22,32 € | 665 |

Bei Y08560R10000F9W wurden ±2 ppm/K nur als typischer Wert genannt, nicht als garantierter Maximalwert. Diese teuren Kandidaten sind als Vergleich dokumentiert; die Suche vom 17.10.2026 sollte günstigere Alternativen liefern. Vollständige Produktdetail-URLs sind im wiedergefundenen Verlauf nicht erhalten.

### Mouser – frühere Einzelwiderstände, Recherche vom 18.07.2026

| Hersteller | Hersteller-Teilenummer | Wert | Bauform | Leistung laut erhaltenem Verlauf | Toleranz | TK | Damaliger Preis 1 / ab 10 Stück | Damaliger Bestand |
|---|---|---|---|---|---|---|---|---|
| Vishay | TNPW120610R0BEEA | 10 Ω | 1206 | 0,52 W; Betriebsbedingungen prüfen | ±0,1 % | ±25 ppm/K | 0,611 € / 0,445 € | 16.657 |
| Susumu | RG2012V-102-W-T5 | 1 kΩ | 0805 | nicht vollständig überliefert | ±0,05 % | ±5 ppm/K | 1,96 € / 1,38 € | 3.108 |
| Susumu | RG2012N-104-W-T1 | 100 kΩ | 0805 | nicht vollständig überliefert | ±0,05 % | ±10 ppm/K | 0,568 € / 0,411 € | 15.066 |

Frühere Assistentenempfehlungen; keine daraus abgeleitete neue Bestückungsentscheidung. Die beiden 0805-Typen sind nicht automatisch Ersatz für die aktuell geforderten 0,5-W-Widerstände. Vollständige Mouser-Produktlinks und Mouser-Bestellnummern sind im wiedergefundenen Verlauf nicht erhalten. Auch MSR5-0R1F1 wurde zuvor mit Mouser-Bezug besprochen; seine technischen Vergleichsdaten stehen in der DigiKey-Tabelle oben.

### Mouser – bereits besprochene Präzisionsnetzwerke

| Hersteller | Hersteller-Teilenummer | Wert / Bauform, soweit belegt | Historischer Händlerstand | Auswahlstatus |
|---|---|---|---|---|
| Vishay | NOMCT16032002AT1 | 20 kΩ, ±0,1 %, SOIC-16 | Mouser-Bezug vom 01.06.2026; Preis und Bestand nicht überliefert | damals vom Nutzer akzeptiert; Anwendung/Bestückungsstand vor Übernahme abgleichen |
| Vishay | NOMCT16031002AT1 | 10-kΩ-Netzwerk; weitere Daten separat prüfen | 26.07.2026: 0 Stück, ca. 6,96 US-$ einzeln; damals genannter Liefertermin ab 26.02.2027 | besprochener Kandidat |
| Vishay | TOMC16031002BUF | 10-kΩ-Netzwerk; weitere Daten separat prüfen | 26.07.2026: Mouser Europa 531 Stück, ca. 13,41 € einzeln / 12,28 € ab 10 | besprochene Alternative |

Keine aktuellen Liefertermine aus den historischen Daten ableiten. Für diese Netzwerke sind vollständige Produktdetail-URLs und Mouser-Bestellnummern im wiedergefundenen Verlauf nicht erhalten.

## OP-Amp-Bestückung und Simulation – Festlegung vom 24.10.2026

- **PCB: OPA192 oder OPA172 vorsehen.** Endgültige Auswahl und Zuordnung zu den einzelnen Stufen bleiben offen.
- **LT1058A vorläufig für schnelle Voruntersuchungen in LTspice verwenden.** Dies ist keine Festlegung für die PCB-Bestückung.
- [ ] Kritische CV- und CC-Testfälle mit den Modellen der tatsächlich vorgesehenen PCB-OP-Amps prüfen; endgültige PI-Kompensation erst danach festlegen. Die mit LT1058A beobachtete Stabilität gilt nicht automatisch für OPA192 oder OPA172.

## Offene Prüfpunkte

- [ ] Überspannungsschutz am ADG1209 und nachfolgenden Messverstärker ausarbeiten. Thermisch ausreichend dimensionierte Shunts verhindern die hohen Messknotenspannungen im Fehlerfall nicht; bei der 30-V-Version können bis zu 30 V am Messpfad auftreten.
- [ ] Für den 1-kΩ-Shunt der 140-V-Version die mögliche Dauerüberlast von 10 W beherrschen.
- [ ] Hardwarebegrenzung 1 A / 100 mA einschließlich Ruhestrom, Toleranzen und Reaktionszeit dimensionieren und prüfen.
- [ ] Stromlose Bereichsumschaltung sicherstellen: Strom auf null bringen, abklingen lassen, Relais umschalten und Kontaktberuhigung abwarten, dann Ausgang wieder freigeben; externe Einspeisung berücksichtigen.
- [ ] HE3621A6012 für den 1-A-Zweig abschließend prüfen: HE3621A-Familiendaten nennen 1,2 A DC Dauerstrom bei 25 °C, aber nur 0,5 A DC Schaltstrom und 10 W Schaltleistung. Die Grenzwerte der konkreten Ausführung bestätigen; kein Öffnen/Schließen unter 1 A vorsehen.
- [ ] Kelvin-Abgriffe direkt an den Shunts ausführen, sodass Relaiskontakt- und Leiterbahnwiderstände nicht mitgemessen werden.
- [ ] Endgültige Typen für 0,1 Ω / 1 Ω / 10 Ω / 100 kΩ auswählen und Verfügbarkeit vor Bestellung prüfen.

