# A-SMU – Gehäuseentwurf 100 mm

Stand: 27.11.2026. Einheiten: mm. B × H × T = **240 × 100 × 300 mm**.

Dieser CAD-Entwurf setzt die zuletzt festgelegte Blechbauweise um: Boden und
Deckel sind U-Schalen aus jeweils 2 mm Aluminium. Der frühere Druckentwurf
mit Stecküberlappung und langen inneren Schraubenzugängen wurde ersetzt.
Die STEP-Dateien sind ein Geometrie- und Montageentwurf, keine Fertigungsfreigabe.

## Änderungen

- Gehäusehöhe 100 statt 130 mm; Trennfuge mittig, 0,3 mm breit.
- Frontplatte 235,6 × 95,6 × 3 mm, an den Ecken Radius 2 mm.
- Alle acht vorhandenen Frontausschnitte behalten Größe und Form. Sie werden
  gegenüber dem bisherigen 130-mm-CAD gemeinsam um 15 mm nach unten verschoben.
  Durch die dünneren Seitenwände ändert sich die Frontplattenbreite, nicht die
  globale X-Position der Ausschnitte. Displayöffnung 124 × 79 mm; Buchsenraster
  19,05 × 19,05 mm. Vier zusätzliche M3-Montagebohrungen in den Ecken.
- Zwei innere Verbindungsstege aus Aluminium, je 260 mm lang, 30 mm hoch und
  8 mm dick; sie überbrücken die seitlichen Stoßfugen. Die 8 mm Dicke ist eine
  Entwurfsannahme, um 6 mm tiefe Hülsenaufnahmen mit Restwand darzustellen;
  der frühere Vorschlag mit 3 mm Streifendicke ist damit ersetzt.
- Drei M3-Befestigungen je Seite für den Boden und drei für den Deckel,
  insgesamt zwölf. Achsen Y = 35 / 150 / 265 mm, Z = 42 / 58 mm.
  Durchgang in den Schalen Ø 3,4 mm.
- Hülsenaufnahmen in den Stegen vorläufig Ø 4,1 × 6 mm tief. Diese Maße sind
  Platzhalter, kein festgelegter Hülsentyp. Passenden M3-Gewindeeinsatz für
  Aluminium auswählen und dessen Aufnahme-, Randabstands- und Einbaumaße
  übernehmen. Einschmelzeinsätze für Kunststoff gehören nicht in die Alustege.
  Hülsen und Schrauben selbst sind nicht als Körper modelliert.
- Lüftung ausschließlich auf der horizontalen Deckelfläche: zwei Schlitzfelder
  mit je 37 Langlöchern, jeweils **2 mm breit und 40 mm lang**; Teilung 6 mm,
  Materialsteg 4 mm. Die Angabe „40 mm“ wurde als Schlitzlänge interpretiert.
  Die seitlichen Schraubbereiche bleiben geschlossen.
- Rückplatte, Lüfter- und Steckerausschnitte sind zurückgestellt. Das Modell
  ist hinten offen; eine Rückplatte ist nicht Bestandteil der Baugruppe.
- Vier kleine innere Frontwinkel ergänzen die Befestigung an den Blechschalen.
  Ihre Ø-2,5-mm-Pilotbohrungen sind für direkt geschnittene M3-Gewinde vorgesehen;
  dies ist unabhängig von den Hülsen in den seitlichen Stegen.

## Montage

1. Hülsentyp festlegen und Stegbohrungen anpassen. Einsätze in die Stege setzen.
2. Stege mit je drei Schrauben an den Bodenseiten befestigen.
3. Untere Frontwinkel an den Boden, obere Frontwinkel an den Deckel schrauben.
4. Deckel aufsetzen und mit je drei seitlichen Schrauben an den Stegen befestigen.
5. Frontplatte mit vier Schrauben an den inneren Winkeln befestigen.

Zum Öffnen die Frontplatte und die sechs oberen seitlichen Schrauben entfernen.
Der Deckel einschließlich seiner oberen Frontwinkel lässt sich nach oben abheben.

## Noch vor der Fertigung festzulegen

Die Stege verteilen die Verbindungsstellen über die Gehäusetiefe. Eine
Tragfähigkeitsberechnung ist damit nicht ersetzt. Masse und Abmessungen des
Ringkerntrafos, Befestigung und örtliche Lastverteilung am Boden fehlen noch.
Eine Verstärkungsplatte unter dem Ringkern sowie Schrauben-/Hülsenbelastung
sind anhand dieser Daten auszulegen. Es wird keine zulässige Traglast angegeben.

Der Displayausschnitt ist geometrisch frei. Für den Displaykörper mit etwa
93 mm Höhe stehen innen nominell 96 mm zwischen den ebenen Flächen zur Verfügung;
Rahmen, Befestigung, Leiterplatte und Kabel müssen mit den tatsächlichen
Bauteilmaßen geprüft werden. Die Zeichnungsquelle enthält eine offene 136 × 93 mm
Referenzkontur, keinen vollständig modellierten Displaykörper.

Biegeradius innen vorläufig 2 mm, außen 4 mm. Materiallegierung, Biegewerkzeug,
Biegezugabe, Abwicklung und Fertigungstoleranzen sind noch festzulegen.
Luftdurchsatz und thermische Wirkung der Schlitze sind nicht berechnet.
Vorgesehener Netzanschluss und elektrische Schutzmaßnahmen sind nicht enthalten.

## Dateien und Prüfung

`CAD/ASMU_Gehaeuse_100mm.step` enthält neun getrennte, farbige Körper:
Boden, Deckel, Frontplatte, zwei Stege und vier Frontwinkel. In SolidCraft
oder einem anderen STEP-fähigen CAD als Baugruppe öffnen. STEP enthält keine
native parametrische SolidCraft-Bearbeitungshistorie. `Front_Quelle.sc` ist
die unveränderte alte Frontquelle und dient ausschließlich als Profilquelle;
sie ist **nicht** die aktualisierte Frontplatte.

`CAD/ASMU_Explosionsansicht.step` und alle Einzelteil-STEPs sind enthalten.
`CAD/ASMU_Vorschau.png` zeigt die tatsächliche CAD-Geometrie.
`parameter.json` und `ASMU_parametrisch.py` sind die reproduzierbare Quelle.
Zum Neuaufbau in einer vorhandenen CadQuery-Umgebung:

```sh
python3 ASMU_parametrisch.py
```

Geprüft: alle neun Körper gültig und einzeln zusammenhängend; keine
Volumenüberschneidungen in Montageposition; Abheben des Deckels nach Entfernen
der Frontplatte; STEP-Export und erneutes Einlesen; Außenmaße; zwölf geschlossene
Frontdurchbrüche einschließlich der vier Montagebohrungen. Ergebnisse stehen
in `CAD/Pruefprotokoll.json`. Keine Festigkeits- oder thermische Prüfung.
