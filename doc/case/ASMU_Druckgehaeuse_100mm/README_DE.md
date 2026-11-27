# A-SMU – Druckgehäuse 100 mm

Stand: 27.11.2026. B × H × T = **240 × 100 × 300 mm**. Einheiten mm.
Separate Kunststoff-Druckvariante auf Basis der Metallvariante; die Metall-CAD-Dateien
bleiben unverändert.

## Ausführung

- Wände 3 mm, äußere Längskanten Radius 2 mm.
- Front und Rückplatte je **233,6 × 93,6 × 3 mm**, beide **2 mm eingelassen**.
  Sie werden bei offenem Deckel von oben in 3,4 mm breite Führungsnuten geschoben;
  der geschlossene Deckel hält die oberen Kanten. Kantenspiel 0,2 mm.
- Alle bisherigen acht Frontausschnitte bleiben in Form und Größe erhalten,
  darunter das 124 × 79 mm große Displayfenster und die Buchsengruppe mit
  19,05 × 19,05 mm Raster. Die obere Ø-7- und Ø-9-mm-Bohrung rechts sind
  **vorläufig um 5 mm nach unten** verschoben. X-Positionen bleiben gleich.
  Vertikalversatz in `parameter.json` unter `upper_right_drop` ändern.
- Der verstärkte Frontrand ist hinter der Platte für die 136 × 93 mm große
  Displayrahmen-Referenz aus der alten Skizze freigestellt. Diese Referenz wurde
  gegen die Schalen geometrisch geprüft. Das ersetzt keine Prüfung des echten
  Moduls mit Leiterplatte, Befestigung und Kabeln.
- Die Rückplatte ist geschlossen. Lüfter- und Steckerausschnitte folgen später.
- Im Deckel bleiben zwei Felder mit je 37 abgerundeten **2 × 40 mm** großen
  Lüftungsschlitzen; Abstand der Achsen 6 mm, Stege dazwischen 4 mm.
- Zwei getrennte innere Verbindungsstege, je 260 × 30 × 8 mm, überbrücken
  die seitliche Fuge. Ihre Auflagepunkte überbrücken das sonstige 0,2-mm-Spiel.
- Je Seite drei M3-Befestigungen für den Boden und drei für den Deckel,
  insgesamt **12 Befestigungen mit M3-Gewindeeinsätzen**.
  Aufnahmen vorläufig Ø 4,1 mm, 6 mm tief; an die vorhandenen Einsätze anpassen.
  Hierfür Einsätze für Kunststoff verwenden. Die Metallvariante hatte andere
  Anforderungen an die Einsätze.

## Aufteilung für den Prusa CORE One mit INDX

Die 300-mm-Tiefe passt liegend nicht auf dessen Druckbett. Boden und Deckel
sind deshalb jeweils bei Y = 171 mm geteilt, mit 0,3 mm Fügespiel.
Die vorderen Teile sind 170,85 mm tief, die hinteren 128,85 mm.
Die durchgehenden seitlichen Stege und je zwei zusätzliche innere Verbinder
halten die beiden Teilstücke zusammen. Die Verbindungen sind verschraubt.

Jede STL im Ordner `STL` enthält genau ein geschlossenes, bereits ausgerichtetes
Teil mit Unterseite bei Z = 0. Die Schalen liegen auf ihrer großen Außenfläche;
die Stege liegen diagonal mit den Hülsenaufnahmen nach oben. Sie sind für den
nominalen Bauraum **248 × 205 × 270 mm** einschließlich 3 mm Brim-Rand
geometrisch geprüft. Kein fertiger G-Code, kein fertiges Slicerprofil.

Quelle zum INDX-Bauraum:
https://www.prusa3d.com/product/indx-8-tool-conversion-kit-for-core-one-gen1/

| Druckdateien | Anzahl |
| --- | ---: |
| Boden_vorne.stl, Boden_hinten.stl | je 1 |
| Deckel_vorne.stl, Deckel_hinten.stl | je 1 |
| Frontplatte.stl, Rueckplatte.stl | je 1 |
| Steg_links.stl, Steg_rechts.stl | je 1 |
| Verbinder_Boden_1.stl, Verbinder_Boden_2.stl | je 1 |
| Verbinder_Deckel_1.stl, Verbinder_Deckel_2.stl | je 1 |
| Teststueck_M3_Huelsen.stl | zuerst 1 Probe |

Das Teststück enthält von links nach rechts drei Hülsenbohrungen mit
Ø 4,0 / 4,1 / 4,2 mm bei jeweils 6 mm Tiefe sowie Ø-3,4-mm-Durchgängen.
Zuerst die Passung mit den tatsächlich vorhandenen Einsätzen ermitteln.
Wenn deren Außendurchmesser/Länge andere Aufnahmegrößen erfordert,
`insert_bore` und `insert_depth` vor dem großen Druck anpassen.
Die drei Proben sind keine allgemeingültige Zuordnung für alle M3-Einsätze.

Die Prüfergebnisse mit den Druckmaßen jeder Einzeldatei stehen in
`CAD/Pruefprotokoll.json`. Die CAD-Gesamtbaugruppe ist nicht die Druckanordnung;
im Slicer die einzelnen STL-Dateien verwenden.

## Schrauben und Montage

Zusätzlich zu den Druckteilen vorgesehen:

- 12 passende M3-Gewindeeinsätze für Kunststoff für die beiden seitlichen Stege.
- 12 M3-Schrauben für die Seiten, vorläufig M3 × 8; Länge anhand der gewählten
  Einsätze und Unterlegscheiben prüfen. Nicht am Ende der Blindbohrung aufsetzen.
- 8 M3-Muttern für die inneren Verbinder. Deren Taschen haben 5,8 mm Schlüsselweite
  und 2,6 mm Tiefe. Die tatsächlichen Muttern vor dem Zusammenbau einpassen.
- 8 M3 × 10 für die vier inneren Verbinder; passende Unterlegscheiben vorsehen.

1. Hülsen-Probestück drucken; Bohrungsmaße prüfen und gegebenenfalls ändern.
2. Die zwölf Hauptteile drucken. Keine automatische Skalierung im Slicer benutzen.
3. Gewindeeinsätze in die Stege setzen, Muttern in die Verbinder einlegen.
4. Die beiden Bodenstücke mit zwei inneren Verbindern und vier Schrauben verbinden.
5. Die beiden Deckelstücke ebenso verbinden.
6. Die seitlichen Stege mit je drei Schrauben an den Boden schrauben.
7. Front- und Rückplatte von oben einschieben, Deckel aufsetzen.
8. Deckel mit je drei Schrauben an den Stegen befestigen.

Zum Öffnen nur die sechs oberen seitlichen Schrauben lösen und den
zusammengefügten Deckel senkrecht abheben. Die Platten können danach nach oben
herausgezogen werden. Hülsen und Schrauben sind nicht als CAD-Körper modelliert.

## CAD und Neuaufbau

- `CAD/ASMU_Druckgehaeuse_100mm.step`: farbige Gesamtbaugruppe mit zwölf Körpern.
- `CAD/ASMU_Explosionsansicht.step`: Deckel und Platten abgehoben.
- `CAD/*.step`: Einzelteile; zusätzlich ungeteilte Boden-/Deckelgeometrie als
  Referenz für größere Drucker. Diese 300 mm langen Referenzen passen liegend
  nicht auf das genannte Druckbett und sind nicht im STL-Drucksatz enthalten.
- `CAD/ASMU_Vorschau.png`: Vorschau der tatsächlichen Geometrie.
- `parameter.json`, `ASMU_Druckgehaeuse.py`, `ASMU_Frontausschnitte.py`,
  `Vorschau.py`: reproduzierbare parametrische Quelle für CadQuery.
- `Front_Quelle.sc`: unveränderte alte Profilquelle. Die aktuelle Front ist die
  erzeugte STEP/STL, nicht diese alte SolidCraft-Datei.

Zum Neuaufbau in einer vorhandenen CadQuery-Umgebung:

```sh
python3 ASMU_Druckgehaeuse.py
```

Geprüft: zwölf gültige zusammenhängende Volumenkörper; keine gegenseitigen
Volumenüberschneidungen; Deckelabheben mit eingesetzten Platten; Einschieben der
Platten bei offenem Deckel; STEP-Export und Rücklesen; geschlossene STL-Netze;
Druckmaße mit Brim-Rand und acht unveränderte beziehungsweise versetzte
Frontdurchbrüche. Alle Angaben beziehen sich auf die ideale CAD-Geometrie,
Druckpassungen müssen mit dem Probedruck abgeglichen werden.

**Kein Tragfähigkeitsnachweis für den Ringkern.** Die Masse, tatsächliche
Befestigung und örtliche Bodenverstärkung müssen vor Belastung geprüft werden.
Das Modell enthält keine Trafo-, Platinen-, Lüfter- oder Netzanschlussbefestigung.
