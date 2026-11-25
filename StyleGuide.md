# A-SMU Style Guide

## Verbindliche Vorlage

Stand: 25.11.2026. Nutzerentscheidung: **System Settings** ist die visuelle Vorlage fuer neue und ueberarbeitete MENU-Unterseiten. Info verwendet dieselbe Vorlage. Bestehende andere Seiten werden bei ihrer Ueberarbeitung angeglichen. Diagramme und Tabellen behalten ihren notwendigen Inhaltsaufbau, verwenden aber denselben Seitenkopf und dieselbe Navigation, soweit moeglich.

## Seitenkopf und Navigation

- Zielanzeige: 800 x 480 Pixel, schwarzer Hintergrund.
- Durchgehender petrolfarbener Kopf: x=8, y=4, Breite=784, Hoehe=52; Theme::TabInactive.
- Seitentitel links bei x=20, y=14, Schrift 29, Theme::ToolbarText, Grossbuchstaben.
- BACK und HOME oben rechts, in dieser Reihenfolge: x=584 / 688, y=8, jeweils 96 x 40 Pixel. Schrift 28, bestehende native Buttons und Theme-Farben.
- BACK fuehrt zum Menue, HOME zur Hauptansicht. Keine Navigation unten auf diesen Formularseiten.
- Kopf und Navigation gemeinsam aus src/UI/Pages/MenuPageStyle.h verwenden. Lange Titel duerfen nicht in die Navigation ragen.

## Inhalte nach System Settings

- Vier Grundzeilen: y=74, 154, 234, 314; Abstand 80 Pixel.
- Labels rechtsbuendig bei x=394, Werte links ab x=414. 20 Pixel Abstand.
- Labels blau (Theme::SourceBlue), Werte weiss (Theme::ToolbarText), beide Schrift 28.
- Erklaerungen unter dem Label mit 31 Pixel Versatz, Schrift 26. Kurz und konkret formulieren.
- Einstellbare Werte als native Buttons: 240 x 54 Pixel, bestehende Theme-Farben.
- Reine Informationen als Text, ohne scheinbar bedienbare Buttons. Mehrteilige Werte koennen in zwei Zeilen mit 32 Pixel Abstand stehen.
- Info zeigt Device / A-SMU / Hobby SMU, V- und I-Messbereiche sowie Firmware build. Datum und Uhrzeit auf zwei Zeilen erhalten Schrift 28.
- Bereichslisten beziehen ihre Werte aus SystemClass. Keine zusaetzlichen GUI-Kopien der Einstellungen.
- Ergaenzende Hinweise unterhalb der vier Zeilen, Status bei Bedarf unten rechts. Keine Inhalte ueberlagern.

## Interaktion und Dialoge

- Bestehendes Touch-Feedback und KeyClick beibehalten.
- Nicht verfuegbare Funktionen sichtbar deaktivieren und kurz erklaeren.
- Modale Eingaben folgen weiterhin SET LIMIT: 600 x 430 bei x=100, y=25; Hintergrundbedienung inklusive Seitennavigation gesperrt.
- Grafikoperationen bleiben in GDClass; Seiten verwenden die gemeinsame Vorlage und Theme.

## Pruefung bei Aenderungen

- Titel, Navigation, Zeilen und Werte duerfen sich nicht ueberdecken.
- Labels und Werte gleich gross; lange Werte umbrechen statt verkleinern.
- BACK/HOME sowie gesperrte Hintergrundbedienung pruefen.
- Bestehende Host-Tests und Pico-Build verwenden; Darstellung abschliessend am Display pruefen.

## MENU-Uebersicht und Measure Settings

- Measure Settings verwendet das gemeinsame Vier-Zeilen-Raster: Voltage Range, Current Range, NPLC und Time / Channel. NPLC zeigt die nominalen U/I-Paare pro Sekunde als Hinweis. Feste Netzfrequenz, Averaging und Messfunktion stehen unterhalb der Felder.
- Die MENU-Uebersicht verwendet denselben Kopf und HOME oben rechts. BACK erscheint nur in Platzhalter-Unterseiten und fuehrt zur Uebersicht.
- Die Uebersicht behaelt ihre vier Funktionsspalten mit blauen Gruppenlabels (Schrift 28) und gleich grossen nativen Menuebuttons (186 x 44, Schrift 27 fuer laengere Namen). Sie ist eine Navigation, kein Label/Wert-Formular. Backendstatus unten rechts.

## Dialoge wiederverwenden

Vor einer neuen Dialogimplementierung vorhandene Dialoge pruefen und wiederverwenden. U/I-Range verwenden in Hauptansicht und Measure Settings dieselbe SourceRangePopup-Instanz, keine eigene Auswahl je Seite. Bestehende Dialoggestaltung nur zentral aendern.

## Simulation mit fuenf Einstellzeilen

Simulation verwendet den gemeinsamen Kopf und BACK/HOME oben rechts. Fuer die zusaetzlichen Last-, Einheiten- und Bereichsbuttons gilt ein kompakteres Raster: Labels rechtsbuendig x=240, Werte ab x=260, fuenf Zeilen ab y=74 mit 64 Pixel Abstand. Schrift 28 fuer Labels und Werte, 26 fuer Hinweise. I-Bereichsbuttons zeigen den Bereichsnamen und direkt darunter ON/OFF; der aktive Bereich bleibt gesperrt. Live-Messwerte und Ausgangsstatus stehen unter dem Formular. Der vorhandene NumericEntryDialog wird weiterverwendet.

## Source Settings

Source Settings verwendet das Standardraster mit vier Zeilen und dieselben Dialoginstanzen wie Main. Die Limit-Zeile nennt passend zur Source Function Current Limit oder Voltage Limit. Keine Kopien von Dialogen, Bereichslogik oder Domainwerten in der Seite.


## Verbindliche Messgroessenfarben - 25.11.2026

Nutzerentscheidung: Farben kennzeichnen eine Messgroesse dauerhaft, unabhaengig von Startansicht, Auswahlreihenfolge, Sichtbarkeit oder Anzahl der Kurven.

| Messgroesse | Farbe | RGB-Hex |
| --- | --- | --- |
| Voltage / U | Gruen | `#40FF60` |
| Current / I | Cyan | `#40D8FF` |
| Power / P | Gelb | `#FFE050` |
| Resistance / R | Rosa/Lila | `#FF80DC` |

- Diese Zuordnung gilt fuer die farbliche Kennzeichnung von Messdaten in Graphen, Histogrammen, Legenden und zugehoerigen Achsen/ausgewaehlten Mengenbuttons. Dieselbe Messgroesse erhaelt auf allen Seiten dieselbe Farbe.
- Kurve/Balken, Legende und zugehoerige Achse muessen zusammenpassen. Keine Rotation der Palette und kein pauschales Gruen fuer die erste Kurve.
- Bei I(V) ist die dargestellte Messgroesse der Strom: Kurve und Y-Achse verwenden Cyan; eine farblich gekennzeichnete Spannungsachse verwendet Gruen.
- Allgemeine Formularlabels bleiben blau und normale Formularwerte weiss. Status-, Warn- und Auswahlfarben sind keine Messgroessenfarben.
- Bei neuen oder ueberarbeiteten Ansichten diese Regeln anwenden und vorhandene Abweichungen angleichen. Farbkonstanten bei Codeanpassungen zentral in Theme halten, keine weiteren seitenlokalen Paletten einfuehren.
- Umsetzungsstand: Menu > Graph verwendet die festen Farben fuer Zeitkurven. Die bisher gruene I(V)-Kurve sowie weitere bisher pauschal gruene Messdarstellungen muessen noch angeglichen werden; die fruehere I(V)-Ausnahme in ux.md ist kein kuenftiger Designstandard.
