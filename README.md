# A-SMU

## Floating High-Voltage Analog Source Measure Unit (Simulation)

# Motivation

Dieses Projekt ist inspiriert durch:

-  [J-SMU von Jaromir Sukuba](https://github.com/jaromir-sukuba/J-SMU) 
-  [DIY-SMU von Dave Erickson](https://www.djerickson.com/diy_smu/) 
     
Beide Projekte sind inspiriert durch Keithley 236 und zeigen eindrucksvoll, dass leistungsfähige SMUs diskret
realisierbar sind.

Die A-SMU verfolgt jedoch einen eigenen Ansatz mit Fokus auf:

-   konsequente galvanische Trennung
-   floating Reglerdomäne
-   explizite Hardware-Schutzmechanismen
-   deterministische Abschaltlogik
-   robuste Behandlung kritischer SOA-Fälle


------------------------------------------------------------------------

# Projektübersicht

Die **A-SMU (Analog Source Measure Unit)** ist eine vollständig diskret
aufgebaute, analog geregelte SMU-Architektur mit klar getrennter
Hochvolt- und Reglerdomäne.\
Das Projekt befindet sich aktuell im Simulationsstadium (LTspice) und
dient als Grundlage für eine robuste, hochpräzise und deterministisch
abschaltbare SMU.

Ziel ist eine Architektur, die:

-   vollständig floating betrieben werden kann
-   eine klare Domänentrennung besitzt (HV / Control)
-   deterministisches Abschalten ermöglicht (ZHIGH)
-   explizite Schutzlogik enthält (OVP, Clamp)
-   auch kritische Lastfälle (z.B. 1,45 A im linearen Bereich) sicher
    beherrscht

------------------------------------------------------------------------

# Was ist eine SMU?

Eine **Source Measure Unit (SMU)** ist ein Messgerät, das gleichzeitig:

-   Spannung oder Strom liefern (Source)
-   Spannung und Strom messen (Measure)
-   nahtlos zwischen CV (Constant Voltage) und CC (Constant Current)
    wechseln

Eine SMU vereint damit:

-   Netzgerät
-   elektronische Last
-   Präzisionsmultimeter

in einem einzigen geschlossenen Regelkreis.

------------------------------------------------------------------------



------------------------------------------------------------------------

# Architektur

## 1. Domänentrennung

### HV-Domain

-   ±30 V (perspektivisch ±100 V)
-   Versorgung der MOSFET-Endstufe
-   galvanisch isoliert

### Control / Regler-Domain

-   ±15 V
-   AGND floatet auf FORCE_LO
-   enthält CV-Regler, Clamp-Logik, OVP
-   keine Verbindung zu PE, USB oder System-GND

Diese Struktur verhindert Masseschleifen und ermöglicht echtes Floating.

------------------------------------------------------------------------

# Funktionsblöcke

## 2. Ausgangsstufe

Diskrete Push-Pull-Endstufe mit:

-   IRFP250 (N-Kanal)
-   IRFP9240 (P-Kanal)
-   Source-Shunts: 0,5 Ω und 0,1 Ω

Eigenschaften:

-   Linearbetrieb
-   symmetrisches Verhalten
-   definierte SOA-Betrachtung
-   aktive Gate-Abschaltung über ZHIGH

------------------------------------------------------------------------

## 3. ZHIGH -- deterministisches Abschalten

ZHIGH ist eine Hardware-Sicherheitsfunktion.

Im aktiven Zustand:

-   Gate N-MOS → −15 V
-   Gate P-MOS → +15 V

Damit wird die Endstufe aktiv deaktiviert.\
Keine schwebenden Gates, kein undefiniertes Verhalten.

------------------------------------------------------------------------

## 4. CV-Regler (Spannungsregelung)

-   Ein PI-Regler
-   arbeitet vollständig in der floating Domain
-   Summenknoten erlaubt Clamp-Einspeisung

Der Regler ist bewusst einfach und deterministisch ausgelegt.

------------------------------------------------------------------------

## 5. CC-Clamp-System (Strombegrenzung)

Getrennte positive und negative Clamp-Blöcke:

-   CLAMP+
-   CLAMP−

Diese wirken direkt auf den Summenknoten des CV-Reglers.

Vorteile:

-   stabile CV→CC Übergänge
-   keine Loop-Crossover-Probleme
-   symmetrisches Verhalten

------------------------------------------------------------------------

## 6. IMON -- Strommessung

Skalierung:

1 V = 1 A

Direkte analoge Repräsentation des Ausgangsstroms.\
Wird für Clamp-Erzeugung und ADC genutzt.

------------------------------------------------------------------------

## 7. VMON -- Spannungsmessung

Umschaltbare Skalierung:

-   1:2
-   1:12

Gebufferte Ausgabe für:

-   ADC
-   OVP-Vergleich
-   externe Messung

------------------------------------------------------------------------

## 8. OVP -- Überspannungsschutz

-   getrennte positive/negative Komparatoren
-   Latch-Logik
-   manuelles Reset erforderlich
-   kann ZHIGH auslösen

Verhindert automatisches Wiederanlaufen nach Fehler.

------------------------------------------------------------------------

# Kritischer Designfall: 1,45 A

Ein zentraler Designpunkt ist der Fall:

1,45 A bei hoher Ausgangsspannung.

Bei 30 V entspricht dies ca. 43,5 W Verlustleistung im linearen Bereich.

Typische Risiken:

-   SOA-Verletzung
-   thermisches Durchgehen
-   asymmetrisches Verhalten

Die A-SMU adressiert diesen Fall durch:

-   explizite SOA-Betrachtung
-   definierte Shunt-Struktur
-   getrennte Clamp-Architektur
-   deterministisches ZHIGH-Abschalten
-   floating Reglerdomäne

------------------------------------------------------------------------

# Aktueller Stand im Repository

Simulation umfasst:

-   vollständige analoge Architektur
-   MOSFET-Endstufe
-   CV-Regler
-   CC-Clamps
-   ZHIGH-Logik
-   OVP-Latch
-   IMON / VMON Skalierung

Simulationsplattform: LTspice

------------------------------------------------------------------------

# Status

Projekt befindet sich aktuell in der Simulationsphase.

Geplante nächste Schritte:

-   Stabilitätsanalyse
-   SOA-Validierung
-   Thermische Simulation
-   PCB-Layout
-   Hardware-Prototyp

------------------------------------------------------------------------

# Lizenz

Noch festzulegen.
