# Touch-Laufzeitdiagnose

In `platformio.ini` aktiviert `ASMU_UI_TIMING=1` die Diagnose. Mit `0` und
erneutem Build entfallen Messcode und Diagnosespeicher vollstaendig.
Die Diagnose veraendert weder Touch-/Frame-Zielintervalle noch die Erfassung.

Seriellen Monitor mit 115200 Baud oeffnen. Etwa alle zwei Sekunden erscheint
eine Zeile `UI ms=... window=... gap>20ms=...; us avg/max/n: ...`.
`ms` ist die Laufzeit, `window` das tatsaechliche Auswertungsfenster in ms.
Jede Messgruppe enthaelt Mittelwert / Maximum in Mikrosekunden / Anzahl.

| Gruppe | Bedeutung |
| --- | --- |
| gap | Abstand zwischen dem Beginn zweier Touch-Abfragen, auch ohne Beruehrung |
| system | Dauer eines System.Execute-Aufrufs; beide Aufrufe im Loop werden erfasst |
| input | Dauer des Einlesens der EVE-Touchdaten |
| dispatch | Verarbeitung der Touchdaten einschliesslich Gesten und Callbacks |
| begin | Dauer von BeginFrame, auch bei nicht bereitstehendem Frame |
| draw | Dauer von root.Draw, einschliesslich darin ausgefuehrter Berechnungen und SPI-Aufrufe |
| swap | Dauer der Frame-Uebergabe; keine Messung bis zur sichtbaren Anzeige |

`gap>20ms` zaehlt Touch-Abstaende ueber 20 ms pro Fenster. Die erste Abfrage
hat noch keinen Abstand. Zeitdifferenzen funktionieren ueber den micros-Ueberlauf.
Die Ausgabe enthaelt keine Seitenkennung: Seitenwechsel beim Aufzeichnen notieren.

## Vergleich am Geraet

1. Eine einfache Einstellungsseite mindestens zehn Sekunden beobachten.
2. Eine betroffene komplexe Seite bei gleicher Messkonfiguration und vergleichbarer
   Pufferfuellung mindestens zehn Sekunden beobachten.
3. Auf beiden Seiten kurze Tastendruecke und Ziehen testen. Das erste
   Auswertungsfenster nach dem Seitenwechsel kann noch beide Seiten enthalten.
4. `gap`-Maxima und `gap>20ms` mit `draw`, `system`, `input`, `dispatch` und
   `begin` vergleichen. Hohe `draw`-Werte weisen auf Zeichenarbeit als Engpass hin;
   sie beweisen noch nicht, welcher Zeichenaufruf dafuer verantwortlich ist.
5. Nach einer Optimierung denselben Ablauf wiederholen. Ziel: Touch-Abstaende
   moeglichst unter 20 ms und keine verlorenen kurzen Tastendruecke.

Es werden nur Summen, Zaehler und Maxima gehalten, keine Messwertkopien.
Seit der Arduino-Pico-Migration vom 27.11.2026 erfolgt die Ausgabe in hoechstens
32-Byte-Portionen ueber Serial.write, begrenzt auf Serial.availableForWrite.
Nur Core 0 schreibt USB; es wird kein zusaetzliches flush aufgerufen.
Ohne Monitor werden Berichte verworfen.
Bei Rueckstau wird das naechste Berichtsfenster laenger; `window` zeigt dies an.
Messung und Logging haben einen kleinen Eigenaufwand; dessen Einfluss kann mit
einem Build ohne Diagnose verglichen werden. Die Diagnose misst weder den
physischen Beruehrungsbeginn noch den Zeitpunkt der sichtbaren Bildschirmreaktion.

## Buildpruefung vom 26.11.2026

Historischer Stand mit ArduinoCore-mbed; neue Buildwerte siehe Hardware.md.

Pico-Build mit und ohne Diagnose erfolgreich. Statischer RAM laut Build:
120716 Byte ohne, 121484 Byte mit Diagnose (+768 Byte, insgesamt 44,9 %).
Die Firmware mit aktiver Diagnose liegt unter `.pio/build/pico/firmware.uf2`.
Noch keine Laufzeitmessung am Display und kein Upload im Rahmen dieser Aenderung.

## Messung am Pico – 27.11.2026, voller Graph

Auslesung COM6/115200, bestehende Diagnose, keine Firmwareänderung. Nutzer bestätigte volle Graphansicht für Ruhe und anschließend wiederholte Sliderbewegung. Exakte Bedeutung von Slider sowie NPLC/Kurvenzahl noch nicht separat erfasst. Je Aufnahme erstes Berichtsfenster verworfen; Mittelwerte nach jeweiliger Aufrufzahl gewichtet, keine Perzentile verfügbar.

| Kennzahl | Ruhe: Mittel / Maximum | Bewegung: Mittel / Maximum |
|---|---:|---:|
| Touch-Abstand | 175,753 / 181,273 ms | 34,384 / 182,034 ms |
| Zeichnen | 174,631 / 180,123 ms | 58,598 / 180,265 ms |
| Touch einlesen | 0,150 / 0,209 ms | 0,130 / 0,208 ms |
| Touch verarbeiten | 0,053 / 0,108 ms | 0,059 / 0,196 ms |
| System.Execute | 0,087 / 0,188 ms | 0,015 / 0,192 ms |

Ruhe: 7 Fenster, 84 Bilder/Touch-Abstände, alle Abstände >20 ms. Bewegung: 9 Fenster, 252 Bilder, 534 Touch-Abstände, davon 169 >20 ms. Die Bewegung enthält deutlich unterschiedliche Phasen: Zeichenmittel je Fenster 17,645 bis 176,937 ms. Der Gesamtmittelwert bedeutet daher nicht, dass die Bedienung durchgehend flüssig ist.

Gesichert: Lange Zeichenläufe dominieren die beobachteten Touch-Abstände; Touch-Einlesen/-Verarbeitung und System.Execute sind wesentlich kürzer. Noch nicht getrennt gemessen: Rasterisierung, Graphberechnungen, SPI-Übertragung und sonstige Zeichenarbeit. Aus diesen Daten noch keine einzelne Zeichenfunktion als Ursache festlegen und keine Optimierung umgesetzt.

Rohdaten: `tmp/logs/touch-full-idle.log`, `tmp/logs/touch-full-drag.log`; aggregierte Werte in den gleichnamigen JSON-Dateien.

## Getrennte Graph-Diagnose – 27.11.2026

Zusätzliche Diagnosefelder, weiterhin Mikrosekunden als Mittelwert/Maximum/Anzahl:

| Feld | Gemessener Umfang |
|---|---|
| main_graph | Gesamter Main-GraphPanel-Zeichenlauf einschließlich Achsen, Raster und Transfer |
| plot_prepare | GraphState.Resolve im Main-Graph (Bereiche/Achsen aus der Historie bestimmen) |
| plot_raster | Pixelbild erzeugen, einschließlich Lesen/Umrechnen der Kurvenpunkte und Clipping |
| plot_upload | EVE_memWrite_sram_buffer: blockierender Transfer des Pixelbilds einschließlich Bibliotheksaufwand |

`plot_samples` und `plot_bytes` geben Punktzahl und Nutzbytes des letzten Kurvenbilds im jeweiligen Berichtsfenster an. Raster/Upload werden in GDClass für alle Graphansichten gemessen; für den Vergleich ausschließlich im Main-Graph bleiben. Die Messzeiten sind verschachtelt: Teilzeiten nicht zusätzlich zur Gesamtzeit addieren. Achsenbeschriftung, Gitter und weitere EVE-Kommandos liegen außerhalb von plot_upload und bleiben im Rest von main_graph enthalten. plot_upload misst nicht isoliert die elektrische SPI-Leitungszeit.

Die Diagnosezähler liegen jetzt einmal zentral in UiTiming.cpp, damit Messungen aus GraphPanel/GDClass im Bericht aus main.cpp ankommen. Der Textpuffer wächst auf 768 Byte. Keine Änderung an SPI-Takt, Messrate, Kurvenauflösung oder Aktualisierungsstrategie.

Nach Upload: gleichen vollen Main-Graph anzeigen, gleiche Messrate/Diagrammeinstellungen beibehalten, zunächst ohne Wischen messen. Die erste Ausgabe nach Seitenwechsel auslassen. Erst danach über eine Optimierung oder einen SPI-Taktvergleich entscheiden.

## Ergebnis der getrennten Messung – 27.11.2026

Voller Main-Graph, ohne Berührung, COM6. Neun vollständige Berichtsfenster nach Verwerfen des ersten Fensters, 108 Bilder. Durchgehend 1024 Punkte und 12.719 Nutzbytes pro Kurvenbild.

| Anteil | Mittel | Maximum |
|---|---:|---:|
| Gesamtes Zeichnen | 174,787 ms | 180,231 ms |
| Main-Graph gesamt | 172,850 ms | 178,323 ms |
| Graph-/Achsenaufbereitung | 47,126 ms | 48,940 ms |
| Rasterisierung inklusive Punktumrechnung | 96,637 ms | 100,295 ms |
| Pixeltransfer inklusive Treiberaufwand | 26,270 ms | 26,284 ms |
| Touch-Abstand | 176,027 ms | 181,434 ms |

Aufbereitung + Rasterisierung = 143,763 ms, etwa 82 % des gesamten Zeichnens; Transfer etwa 15 %. Selbst vollständiges Wegfallen des gemessenen Pixeltransfers ließe rund 148,5 ms Zeichenzeit übrig. 12.719 Nutzbytes benötigen bei nominal 8 MHz rechnerisch 12,719 ms reine Leitungszeit; der gemessene Treiberaufruf ist länger. Eine Verdoppelung des Takts halbiert daher nicht zwangsläufig die gesamte gemessene Transferzeit.

Quellcodebefund für die anschließende gemeinsame Entscheidung: GraphState::Resolve läuft für die automatische Y-Achsenskalierung über alle vier Größen, auch wenn Main nur eine aktiviert. PlotRaster::Draw berechnet pro Punkt erneut die Koordinatennormierung mit Divisionen. Vorschlag: zuerst die unnötige Achsenaufbereitung im Main vermeiden; anschließend erneut messen, bevor Rasterisierung/Takt geändert werden. Keine Optimierung in dieser Messrunde umgesetzt.

Rohdaten: `tmp/logs/touch-graph-detail.log`; Zusammenfassung: `tmp/logs/touch-graph-detail.json`.

## Erste Optimierung: nur dargestellte Main-Messgröße skalieren – 27.11.2026

Auf Nutzerauftrag nutzt ausschließlich GraphPanel (Main) den neuen optionalen selectedOnly-Parameter von GraphState::Resolve. Die automatische Y-Skalierung überspringt die drei nicht dargestellten Größen. X-/Zeitachse und die ausgewählte Y-Achse werden wie bisher berechnet. Bei Wechsel der Messgröße wird die neu gewählte Achse beim nächsten Zeichnen aufbereitet. MENU/Graph und andere Aufrufer behalten das bisherige Verhalten.

Graph-Host-Test erfolgreich, ergänzt um Vergleich aller vier ausgewählten Größen mit der bisherigen vollständigen Berechnung, unveränderte ungewählte Achsen und Erhaltung manueller Skalierung. Pico-Build erfolgreich: 85.872 Byte statisches RAM (unverändert), 165.908 Byte Flash (+64 Byte). Diagnose bleibt aktiv, SPI unverändert 8 MHz, alle Kurvenpunkte bleiben erhalten. Logdateien: tmp/logs/main-autoscale-host.log und tmp/logs/main-autoscale-build.log. Zeitgewinn noch nicht auf dem Gerät gemessen; Referenz plot_prepare 47,126 ms bei vollem Main-Graph. Kein Upload durchgeführt.

## Nachmessung: Main skaliert nur die ausgewählte Größe – 27.11.2026

COM6, voller Main-Graph ohne Wischen. Acht vollständige Fenster nach Verwerfen des ersten Fensters, 120 Bilder, durchgehend 1024 Punkte und 12.719 Nutzbytes pro Bild.

| Messgröße | Vorher Ø | Nachher Ø | Nachher Maximum |
|---|---:|---:|---:|
| Graph-/Achsenaufbereitung | 47,126 ms | 10,847 ms | 11,246 ms |
| Rasterisierung | 96,637 ms | 95,648 ms | 99,233 ms |
| Pixeltransfer | 26,270 ms | 26,270 ms | 26,283 ms |
| Gesamtes Zeichnen | 174,787 ms | 137,486 ms | 141,467 ms |
| Touch-Abstand | 176,027 ms | 138,634 ms | 142,727 ms |

Achsenaufbereitung um 36,279 ms / etwa 77 % reduziert; gesamtes Zeichnen etwa 21 % kürzer. Rasterisierung weitgehend unverändert, Transfer unverändert. Weiterhin liegen alle 120 Touch-Abstände über 20 ms. Die Änderung ist messbar wirksam, erreicht aber noch keine flüssige Bedienung. Nächster Kandidat für gemeinsame Entscheidung: konstante Skalierungsfaktoren in PlotRaster außerhalb der Punkteschleife berechnen, anschließend Bild-/Clippingverhalten prüfen und erneut messen; noch nicht umgesetzt.

Rohdaten: `tmp/logs/touch-graph-selected-only.log`, Zusammenfassung: `tmp/logs/touch-graph-selected-only.json`.

## Zweite Optimierung: Skalierungsfaktoren einmal pro Kurvenbild – 27.11.2026

PlotRaster berechnet X-/Y-Spannen und deren Kehrwerte vor der Punkteschleife. Die Normierung verwendet bei normalen Bereichen Multiplikationen statt zwei Divisionen pro Messpunkt. Alle Punkte, Linien und Clipping bleiben erhalten; ClipPlotLine wurde nicht geändert. Exakte obere Bereichsgrenzen bleiben exakt 1, damit Kehrwert-Rundung Randpunkte nicht ausblendet. Bei extrem kleinen Spannen mit nicht endlichem Kehrwert bleibt Division als Rückfall erhalten; nicht endliche Spannen werden abgelehnt.

Graph-Host-Test erfolgreich, zusätzlich geprüft: Eckpunkte bei Dezimalspannen, sehr kleine Spannen mit Kehrwertüberlauf, Pixel-Rundungsmittelpunkt und Speichergrenzen. Bestehende Clipping-/Lückentests bestehen. Kein Anspruch auf bitidentische Bilder für jeden numerischen Grenzfall: Multiplikation mit Kehrwert kann nahe Rundungsgrenzen anders runden als Division.

Pico-Build erfolgreich: RAM unverändert 85.872 Byte, Flash 166.212 Byte (+304 Byte). Keine zusätzliche dauerhafte Pufferung, nur lokale Skalierungswerte. Diagnose bleibt aktiv, SPI unverändert. Vorherige Rasterzeit am Gerät: 95,648 ms; Nachmessung ausstehend. Logs: tmp/logs/raster-scale-host.log und tmp/logs/raster-scale-build.log. Kein Upload durchgeführt.

## Nachmessung: vorberechnete Raster-Skalierungsfaktoren – 27.11.2026

COM6, voller Main-Graph ohne Bedienung. Neun vollständige Fenster, 135 Bilder, durchgehend 1024 Punkte und 12.719 Nutzbytes. Erstes Fenster verworfen; Mittelwerte nach Aufrufzahl gewichtet.

| Kennzahl | Vorher (nur ausgewählte Achse) | Jetzt | Maximum jetzt |
|---|---:|---:|---:|
| Aufbereitung | 10,847 ms | 10,852 ms | 11,269 ms |
| Rasterisierung | 95,648 ms | 96,433 ms | 100,193 ms |
| Pixeltransfer | 26,270 ms | 26,269 ms | 26,279 ms |
| Gesamtes Zeichnen | 137,486 ms | 138,334 ms | 142,571 ms |
| Touch-Abstand | 138,634 ms | 139,448 ms | 143,687 ms |

Kein erkennbarer Geschwindigkeitsgewinn durch die zweite Änderung; die mittlere Rasterzeit ist etwa 0,8 ms höher. Diese Aufnahmen allein trennen Mess-/Datenvariation und eine kleine tatsächliche Regression nicht sicher. Keinen Erfolg daraus ableiten. Die erste Verbesserung (nur sichtbare Achse aufbereiten) bleibt erhalten. Noch keine weitere Implementierung oder Rücknahme vorgenommen.

Weiterer Kandidat aus dem Quellcode: ClipPlotLine führt für jede gültige Verbindung eine allgemeine Clipping-Berechnung aus, auch wenn beide Endpunkte bereits innerhalb des Bildes liegen. Ein Innenbereich-Schnellpfad könnte diese Arbeit vermeiden; sein Zeitanteil und Nutzen sind noch nicht gemessen.

Rohdaten: tmp/logs/touch-graph-scaled.log; Zusammenfassung: tmp/logs/touch-graph-scaled.json.

## Wiederholung nach erneutem Upload – 27.11.2026

Nutzer hat nochmals hochgeladen. Gleiche Aufnahme ohne weitere Codeänderung: voller Main-Graph, neun Fenster, 135 Bilder, 1024 Punkte und 12.719 Nutzbytes pro Bild. Mittelwerte: Aufbereitung 10,845 ms; Rasterisierung 95,529 ms (Maximum 99,188 ms); Transfer 26,270 ms; gesamtes Zeichnen 137,431 ms; Touch-Abstand 138,601 ms (Maximum 142,345 ms). Alle 135 Touch-Abstände >20 ms.

Die Rasterzeit liegt praktisch auf der Referenz vor der zweiten Optimierung (95,648 ms), ebenso die gesamte Zeichenzeit (137,486 ms). Somit auch nach erneutem Upload kein erkennbarer Nutzen der vorberechneten Skalierungsfaktoren. Keine Aussage über die Ursache (Compileroptimierung oder anderer dominierender Rasteraufwand) ohne weitere Untersuchung. Keine zusätzliche Änderung vorgenommen.

Rohdaten: tmp/logs/touch-graph-scaled-reupload.log; Zusammenfassung: tmp/logs/touch-graph-scaled-reupload.json.

## Main / Lines: EVE-Vektoren – 27.11.2026

Vom Nutzer freigegebener erster Schritt: ausschließlich Main im Stil Lines bevorzugt EVE LINE_STRIP mit VERTEX2F. Alle Messpunkte werden synchron aus dem bestehenden BufferView verarbeitet; keine Datenreduktion. Der Pico berechnet Bildschirmkoordinaten und schneidet Linien am Diagrammrand ab, die EVE übernimmt die Rasterisierung. Ungültige Werte und Ausflüge aus dem sichtbaren Bereich unterbrechen die Linie korrekt. Leichte Unterschiede durch die EVE-Linienglättung gegenüber dem bisherigen 1-Bit-Bild sind möglich.

GDClass teilt den vorhandenen 25.600-Byte-Arbeitspuffer zwischen Raster und Vektorkoordinaten. Kein zusätzlicher Messwertpuffer und keine Änderung der Core-/SPI-Zuordnung oder des SPI-Takts. Die gesamte sichtbare Linie wird bei jedem Bild erneut als Vektoren gesendet; noch kein inkrementelles Update einzelner Segmente.

Schutzmaßnahmen: nach Abschluss der bisherigen Coprozessorbefehle wird REG_CMD_DL gelesen. Die Vektoren müssen einschließlich BEGIN/END und Grafikzustand in die 8-KiB-Displayliste passen; Main reserviert zusätzlich 2 KiB für nachfolgende Navigation, MeasurePanel und DISPLAY. Bei zu vielen Teilstücken, unzureichendem Budget oder noch beschäftigtem Coprozessor bleibt der bisherige Bitmap-Pfad aktiv. Sichtbare zusätzliche Root-Flächen/Dialoge sperren den Vektorpfad. MENU/Graph, Points, Both und Tabellen-Preview verwenden weiterhin Bitmaps. Beim Vektortransfer wird der freie Command-FIFO-Platz in Blöcken von höchstens 64 Befehlen kontrolliert; Warten ist auf jeweils 2 ms begrenzt. Bei Transferfehlern wird das unvollständige Bild nicht per SWAP sichtbar geschaltet.

1024 zusammenhängende Punkte benötigen 1034 Displaylisten-Wörter / 4136 Nutzbytes einschließlich Grafikzustand; gegenüber 12.719 Bitmap-Nutzbytes. Das ist kein vollständiger SPI-Buszähler: Adressbytes und Registerabfragen kommen hinzu. Die bestehende Ausgabe EVE DL peak bleibt für die Kontrolle am Gerät erhalten. Die reservierten 2 KiB müssen bei künftigen Änderungen der Main-Widgets erneut überprüft werden.

Diagnose ergänzt: plot_mode=vector bzw. bitmap, plot_vector für Koordinatenaufbereitung, plot_commands für Befehlsübertragung einschließlich FIFO-Abfragen. plot_bytes bezeichnet je nach Modus Vektorbefehle oder Bitmap-Nutzdaten. plot_raster/plot_upload bleiben dem Bitmap-Pfad vorbehalten. Die Mode-/Bytefelder beschreiben den letzten Plot im Berichtsfenster, die Zeitstatistiken alle Aufrufe im Fenster; für den Vergleich ausschließlich stabile Fenster ohne Dialog-/Moduswechsel verwenden.

Validierung: Host-Graphsuite inklusive Vektorkapazität, vollständiger Punktzahl, Lücken, Clipping, Wiedereintritt, NaN, Einzelpunkt, Randkoordinaten, exakter Displaybudget-Grenze und Main/Lines-Auswahl bestanden. Pico-Build erfolgreich: RAM 85.916 Byte (+44), Flash 168.164 Byte (+1952 gegenüber der letzten Raster-Version). Separater Core-1-Stack weiterhin zusätzlich 8 KiB zur Laufzeit. Symbolprüfung bestätigt genau einen Grafik-Arbeitspuffer mit 25.600 Byte. Logs: tmp/logs/graph-vectors-host.log und tmp/logs/graph-vectors-build.log. Kein Upload und noch keine Laufzeit-/Bildprüfung am Gerät durchgeführt.

Nächste Messung nach Nutzer-Upload: Main / Graph / Lines, SIM ON und voller Buffer; zuerst ohne Bedienung für den Zeitvergleich, danach Source → Graph → Statistik und Measure Voltage sowie ein Dialog über dem Graphen. Prüfen: plot_mode=vector, DL-Peak unter 8192, keine Coprozessorfehler, korrekte Kurve und verbesserter Touch-Abstand. Referenz: gesamtes Zeichnen 137,431 ms und Touch-Abstand 138,601 ms. Kein Geschwindigkeitsgewinn zugesichert, bevor diese Nachmessung vorliegt.

## Erste Aufnahme nach Vektor-Upload – 27.11.2026

COM6, 23 Sekunden aufgenommen; erstes Berichtsfenster verworfen, danach 10 Fenster / 150 Bilder mit 1024 Punkten. Neue Diagnosefelder vorhanden, aber durchgehend plot_mode=bitmap, plot_bytes=12719 und null Aufrufe für plot_vector/plot_commands. Gesamtes Zeichnen 137,537 ms, Touch-Abstand 138,669 ms, Raster 95,420 ms, Pixeltransfer 26,347 ms. Somit noch keine Messung des Vektorpfads und kein belegter Geschwindigkeitsgewinn.

Die Eintrittsbedingung des Vektorpfads ist nicht erfüllt; ein Displaylisten-Budget-Fallback allein erklärt das nicht, da dann bereits plot_vector-Aufrufe vorlägen. Nutzer gebeten, unter MENU → Graph → DATA den Stil Lines zu prüfen und anschließend Main → Graph zu öffnen. Noch nicht belegt, ob der Stil oder eine andere Freigabebedingung die Ursache ist. Keine Firmwareänderung. Rohdaten: tmp/logs/touch-graph-vectors.log; Auswertung: tmp/logs/touch-graph-vectors.json.

## Vektorpfad aktiv, schwarze Streifen gemeldet – 27.11.2026

Nutzer meldet Darstellungsfehler (schwarze Streifen), sobald der Buffer voll ist. Erneute COM6-Aufnahme über 23 Sekunden: erstes Fenster verworfen; 11 Fenster / 377 Bilder, durchgehend 1024 Messpunkte und plot_mode=vector. Keine Raster-/Bitmaptransfer-Aufrufe. Zeichenzeit im Mittel 57,942 ms (Maximum 59,085), Touch-Abstand 58,930 ms (Maximum 60,179). Referenz letzter Raster-Reupload: 137,431 bzw. 138,601 ms. Damit etwa 58 % kürzere Zeichenzeit, jedoch wegen der gemeldeten Bildfehler noch kein funktional erfolgreicher Abschluss.

Teilzeiten: Achsenaufbereitung 10,894 ms, Vektorkoordinaten 29,750 ms, Befehlsübertragung inklusive FIFO-Abfragen 12,659 ms. Angezeigte Vektornutzbytes je letztem Bild im Fenster 3704 bis 3924 (Clipping beeinflusst die Zahl ausgegebener Vertices). In dieser Aufnahme keine Coprozessor-Fault-Meldung und kein neuer DL-Peak; der aktuelle vollständige Displaylistenverbrauch ist daher nicht aus dem Log bestimmbar. Alle 377 Touch-Abstände liegen weiterhin über 20 ms.

Ursache der schwarzen Streifen offen. Ein mögliches Problem ist das Renderzeitbudget je Bildschirmzeile, das von der reinen 8-KiB-Speichergrenze zu unterscheiden ist: Laut FT81x Programming Guide 2.10.7 baut die EVE jede Zeile während der Ausgabe auf und hat dafür begrenzte Zeit. Das ist eine Hypothese, keine am Gerät bestätigte Diagnose. Nutzer nach Ausrichtung/Ausdehnung der Streifen und sauberer Darstellung nach Wechsel zu Source/Statistik gefragt. Keine Änderung des SPI-/Displaytakts oder der Firmware vorgenommen. Nächste Diagnose sollte vollständige DL-Größe und Bildfehler räumlich zuordnen, bevor Punkte reduziert oder Timings geändert werden.

Quelle: https://brtchip.com/wp-content/uploads/2025/02/BRT_AN_088_FT81x_BT88x-Programming-Guide.pdf (Abschnitt 2.10.7). Rohdaten: tmp/logs/touch-graph-vectors-lines.log; Auswertung: tmp/logs/touch-graph-vectors-lines.json.

## Gezielte Prüfung des Rasterwechsels – 27.11.2026

Nutzer präzisiert: Streifen nur im Main-Graph einschließlich Y-Achse, Auftreten beim Verschieben um ein Raster. Keine Reduktion der Messpunkte und keine Taktänderung gewünscht. Die Last-Hypothese ist damit weiterhin unbewiesen; Schwerpunkt auf unserer Darstellung beim X-Ursprungswechsel.

Neue Hostprüfung CheckVectorScroll verwendet den tatsächlichen BufferView, GraphState::Resolve und PlotVector::Build über jeweils dreifache Ringkapazität mit 50-ms-Abstand. Zwei Zeitursprünge testen normale Zeit und uint32-Timestamp-Überlauf. Geprüft werden sämtliche ausgegebenen Koordinatengrenzen, chronologische X-Reihenfolge sowie jeder sichtbare Linienabschnitt gegen die Normalisierung/Clipping-Berechnung des alten Rasterpfads (maximal ein Pixel Rundungsabweichung). Rasterwechsel vor und nach Ringüberschreiben werden durchlaufen. Test und bestehende Graphsuite bestanden; Fehler damit nicht reproduziert und keine Ursache behauptet.

Für die nächste Geräteaufnahme Diagnose ergänzt, ohne Zeichenpfadänderung: x0 = letzter X-Ursprung in Sekunden; shifts = Zahl der Änderungen im Berichtsfenster; vn/vs = Vertex-/Teilstückzahl des letzten Vektorplots; vx/vy = minimale/maximale lokale Diagrammkoordinaten über das Fenster; outside = Anzahl außerhalb der Diagrammgrenzen ausgegebener Vertices; dlmax = maximale vollständig abgearbeitete Displaylistengröße über das Fenster, aus dem ohnehin vorhandenen Registerlesen. Bei zeitlichem Vergleich beachten: DL-Wert gehört zum zuvor abgeschlossenen Bild; kein atomarer Frame-Snapshot. Werte erlauben Einordnung vor/nach dem Sprung. Kein zusätzlicher Messdatenpuffer, Ausgabe weiterhin über begrenzten USB-Diagnosepuffer.

Pico-Diagnosebuild erfolgreich: RAM 85.964 Byte (+48), Flash 168.500 Byte. Noch nicht hochgeladen. Logs: tmp/logs/vector-scroll-host.log und tmp/logs/vector-scroll-build.log. Nächster Schritt: Nutzer-Upload, Main/Lines mit neu geleertem Buffer öffnen und Aufnahme vor dem ersten Rasterwechsel starten; Zeitpunkt des sichtbaren Fehlers melden. Darstellungsfehler bleibt bis zu dieser Prüfung offen.

## Geräteaufnahme der Rasterwechsel – 27.11.2026

55 Sekunden COM6-Aufnahme nach Nutzer-Upload: 28 Berichtsfenster, beginnend bei 742 Messpunkten und X-Ursprung 0. Insgesamt 12 Rasterwechsel bis X-Ursprung 48 Sekunden erfasst. Der erste Wechsel auf 4 Sekunden liegt im Fenster bis Gerätezeit 71.659 ms; dort Buffer voll, letzter Plot 946 Vertices, ein Linienzug, lokale Koordinaten X 0..624 und Y 3..160, outside=0 und DL-Maximum 6504 Byte.

Über die gesamte Aufnahme: keine ausgegebenen Vertices außerhalb der Diagrammgrenzen, durchgehend genau ein Linienzug, maximale vollständig abgearbeitete Displayliste 6508 Byte (unter 8192). Kein Hinweis auf einen Koordinatenausreißer oder Displaylisten-Speicherüberlauf in dieser Aufnahme. Diese Messung prüft weder den tatsächlichen Bildinhalt noch sämtliche Grafikzustände oder die korrekte Übertragung jedes Befehls und beweist daher keine Fehlerfreiheit der Darstellung.

22 Fenster mit am Fensterende vollem Buffer / 759 Bilder: Zeichenzeit 58,007 ms, Touch-Abstand 59,003 ms. Das erste dieser Fenster kann noch Bilder während des letzten Auffüllens enthalten. Während der Aufnahme noch kein vom Nutzer gemeldeter genauer Zeitpunkt des sichtbaren Fehlers. Keine weitere Firmwareänderung. Für die räumliche Zuordnung ist ein Foto des gestörten Graphen hilfreich. Rohdaten: tmp/logs/touch-graph-scroll.log; Auswertung: tmp/logs/touch-graph-scroll.json.

## Main / Lines: jeden zweiten Messwert darstellen – 27.11.2026

Nutzer korrigiert die Beobachtung: Streifen beginnen bereits bei ungefähr 80 % der Linien, vor dem ersten Rasterwechsel. Anschließend ausdrücklich gewünschter Versuch: jeden zweiten Messwert darstellen. Main / Lines nutzt jetzt einen synchronen, nicht besitzenden PlotEverySecondSource-Adapter: chronologische Indizes 0,2,4,...; der letzte Messwert wird zusätzlich berücksichtigt, wenn er sonst entfiele. Bei 1024 gespeicherten Werten sind das 513 an den Renderer übergebene Punkte, vor Clipping. plot_samples meldet daher nun die reduzierte Anzahl, nicht den tatsächlichen Bufferfüllstand.

Keine Änderung an Erfassung, Ringbufferkapazität, Statistiken, Achsenskalierung, SPI- oder Displaytakt. Kein neuer dauerhafter Puffer. Andere Ansichten sowie Points/Both bleiben unverändert. Ungültige übersprungene Werte unterbrechen konservativ die Linie am nächsten ausgewählten Punkt, damit keine Messlücke überbrückt wird. Einfaches Auslassen ist keine Min/Max-Darstellung: Spitzen in übersprungenen Messwerten können im Graphen fehlen, bleiben aber in Daten und Skalierung berücksichtigt.

Host-Graphsuite bestanden, einschließlich leerer/kleiner/gerader/ungerader Punktzahlen, letztem Endpunkt, ungültigem Zwischenwert und voller Main-Kurve mit unverändertem Buffer. Pico-Build erfolgreich: RAM unverändert 85.964 Byte, Flash 168.708 Byte. Logs: tmp/logs/graph-every-second-host.log und tmp/logs/graph-every-second-build.log. Kein Upload; Wirkung auf Streifen und Laufzeit erst nach Geräteprüfung bewertbar.

## Korrektur: stabile Messpunktauswahl statt wechselnder Ringpositionen – 27.11.2026

Nutzer beobachtet kurz erscheinende/verschwindende Punkte und Flackern bei vollem Diagramm. Zwei konkrete Fehler in der vorherigen Reduktion identifiziert: (1) Indizes 0,2,4 relativ zum ältesten Ringwert wechseln bei jedem Überschreiben zwischen geraden und ungeraden Erfassungsnummern. Dadurch wird die ganze ausgewählte Kurve ausgetauscht. (2) Der stets zusätzlich eingeblendete letzte Punkt kann beim nächsten Messwert wieder aus der Auswahl fallen.

Korrigiert: Auswahl bleibt an geraden nullbasierten Erfassungsnummern seit Buffer-Reset verankert. Der erste Index der aktuellen View ergibt sich aus GetTotalSamples()-Count; dessen Parität bestimmt den lokalen Startoffset. Kein zusätzlicher letzter Zwischenpunkt. Der Graph wächst alle zwei Messungen, liegt höchstens eine Messperiode hinter der neuesten Messung und erhält bei vollem Buffer 512 Punkte vor Clipping. Datenhaltung, Statistik, Skalierung, Takte und andere Darstellungsstile unverändert.

Regressionstest zeichnet über drei Bufferkapazitäten und prüft nach jeder neuen Messung alle ausgewählten Erfassungsnummern einschließlich Reset. Neue und bestehende Graphsuite bestanden. Pico-Build erfolgreich: RAM unverändert 85.964 Byte, Flash 168.724 Byte. Logs: tmp/logs/graph-stable-stride-host.log und tmp/logs/graph-stable-stride-build.log. Fehler der Punktauswahl behoben; Wirkung auf tatsächliches Flackern und die zuvor gemeldeten schwarzen Streifen noch am Gerät zu bestätigen. Diese Korrektur erklärt nicht rückwirkend die Streifen im ursprünglichen Vektorpfad ohne Reduktion.

## Main / Linien + Punkte – 27.11.2026

Nutzer bestätigt korrekte Darstellung nach stabiler Auswahl und wünscht ergänzende Punkte für den nächsten Test. Main im Stil Both (Linien + Punkte) verwendet nun dieselbe stabile Halbierung wie Lines und bevorzugt ebenfalls EVE-Vektoren. Linienzug und runde Punkte (3 Pixel Durchmesser) werden aus denselben ausgewählten Messwerten erzeugt. Punkte liegen nur an gültigen, sichtbaren Messwerten, nicht an künstlichen Schnittpunkten am Diagrammrand. Ein einzelner gültiger Messwert bleibt als Punkt sichtbar. Die bestehende konservative Unterbrechung bei übersprungenen ungültigen Werten gilt auch hier.

Koordinaten für Linien und Punkte werden in einem Durchlauf berechnet und teilen sich den vorhandenen 25.600-Byte-Grafikpuffer. Die Budgetprüfung berücksichtigt zusätzlich POINT_SIZE, BEGIN/END und alle Punktvertices; Bitmap-Fallback bleibt erhalten. Bei 512 zusammenhängenden sichtbaren Messwerten sind maximal 4148 Vektor-Nutzbytes erforderlich. Reiner Points-Stil und MENU-Ansichten unverändert. Diagnose vn/vs beschreibt weiterhin die Linienvertices/-teilstücke; plot_bytes enthält im Vektor-Both-Modus zusätzlich die Punktbefehle.

Tests bestanden: stabile Auswahl bei Wechsel Lines/Both über drei Bufferkapazitäten, Einzelpunkt, keine Punkte an Clipping-Grenzen, Punktpufferkapazität und Displaylistenbudget einschließlich Punkte. Pico-Build erfolgreich: RAM unverändert 85.964 Byte, Flash 169.028 Byte. Logs: tmp/logs/graph-vector-dots-host.log und tmp/logs/graph-vector-dots-build.log. Kein Upload. Nächster Test nach Upload: MENU → Graph → DATA → Both, anschließend Main → Graph, voller Buffer; Bildstabilität und Zeiten erneut prüfen.

## Nachmessung Main / Linien + Punkte – 27.11.2026

Nutzer bestätigt funktionierende Darstellung und gibt die Messung frei. COM6 für 23 Sekunden aufgenommen, erstes Berichtsfenster verworfen: 10 Fenster / 390 Bilder, durchgehend Vektorpfad und 512 ausgewählte Messwerte. Fünf Rasterwechsel erfasst. Keine Raster-/Bitmaptransfer-Aufrufe, keine Fault-Meldung in der Aufnahme, keine Linienvertices außerhalb des Diagramms; vollständige Displayliste maximal 6520 von 8192 Byte. Bildkorrektheit beruht auf Nutzerbestätigung, nicht auf einer visuellen Prüfung durch den Agenten.

Mittelwerte / Maxima: gesamtes Zeichnen 50,582 / 51,912 ms; Touch-Abstand 51,589 / 53,309 ms; Graph gesamt 48,620 / 49,932 ms; Achsenaufbereitung 10,871 / 11,322 ms; Vektorkoordinaten 22,128 / 22,395 ms; Befehlsübertragung inklusive FIFO-Abfragen 12,951 / 13,673 ms. Entspricht ungefähr 19,4 Main-Loop-/Touch-Abfragen pro Sekunde während des Zeichnens; keine Messung der physischen LCD-Bildfrequenz oder einer Touchgeste.

Gegenüber der früheren Raster-Referenz mit 1024 Punkten und Lines (137,431 ms Zeichenzeit, 138,601 ms Touch-Abstand) rund 63 % kürzere Zeichenzeit. Kein isolierter Vergleich der Punktmarker-Kosten: Referenz und aktueller Stand unterscheiden sich auch in Punktzahl, Renderingpfad und Stil. Eine eigene Aufnahme der stabil reduzierten Lines-Variante liegt noch nicht vor. Keine weitere Firmwareänderung. Rohdaten: tmp/logs/touch-graph-vector-both.log; Auswertung: tmp/logs/touch-graph-vector-both.json.

## Gemeinsames Diagramm und Full Buffer – 27.11.2026

Main und MENU rufen nun dieselbe GraphPlot-Zeichenfunktion für Raster, X-/Y-Achsen, sichtbare Kurven und Punkte auf. Die Seiten liefern Position, Größe, Farben und kompakten Achsenstil; MENU behält Legende, Mehrkurvenwahl, Cursor und Einstellungen. Die Auswahl jedes zweiten Erfassungswerts liegt nur noch an einer Stelle. GraphState startet mit Full Buffer (scrolling=false); Step scrolling kann im MENU weiterhin ausdrücklich gewählt werden. Bei vollem Ring zeigt Full Buffer die gesamte noch vorhandene Historie und verschiebt das Zeitfenster mit dem ältesten Messwert, ohne Rasterstufen. Main und MENU erhalten denselben Standard, führen aber weiterhin eigene Zoom-/Pan-Zustände.

Histogramm-AUTO-SCALE geprüft: Aktion setzt die HistogramScale-Methode zurück auf SmartScale. Sie ist nach manuell eingestellten Minimum/Maximum/Bins/Y-Maximum oder anderem Verfahren sinnvoll und bleibt erhalten. Es handelt sich um eine eigene Histogrammfunktion, nicht um den GraphState-AutoScale.

Host-Graphsuite mit angepassten Erwartungen für Full Buffer bestanden; expliziter Step-scrolling-Pfad ebenfalls geprüft. Pico-Build erfolgreich: RAM 85.964 Byte unverändert, Flash 169.436 Byte. Logs: tmp/logs/shared-graph-host.log und tmp/logs/shared-graph-build.log. Kein Upload; Layout und Reaktionszeit müssen am Gerät geprüft werden.

## Full Buffer ohne leeres rechtes Raster – 27.11.2026

Nutzer bestätigt gleiches Verhalten beider Diagramme, meldet aber ein leeres letztes Raster. Ursache im GraphState: Full Buffer rundete die benötigte Zeitspanne mit HistorySpan auf 1/2/5/10er Schritte auf. Dadurch lag der neueste gespeicherte Punkt je nach Füllstand links vom rechten Plotrand und das letzte Raster blieb leer. Full Buffer setzt nun X.start auf die Zeit des ältesten gespeicherten Messwerts und die Spannweite exakt auf dessen Abstand zum neuesten Wert (Mindestspanne 1 ms bei Einzelpunkt). Die X-Achse nutzt weiterhin zehn Teilungen, deren Zahlen dadurch weniger „runde“ Werte haben können. Beim Ringüberschreiben wandert der Ursprung kontinuierlich, wie für Full Buffer zu erwarten; wer die frühere schrittweise Verschiebung möchte, wählt im MENU Step scrolling.

Gezielter Hosttest prüft nach jeder Messung über zwei Bufferkapazitäten, dass die rechte Achsengrenze dem neuesten Messwert entspricht, auch nach Ringüberschreiben. Bestehende Host-Graphsuite nach Anpassung der Erwartungen bestanden. Pico-Build erfolgreich: RAM 85.964 Byte unverändert, Flash 169.324 Byte. Logs: tmp/logs/full-buffer-edge-host.log und tmp/logs/full-buffer-edge-build.log. Kein Upload; Geräteprüfung offen.

## Korrektur der Nutzerpräferenz: feste X-Achse, Rasterverschiebung – 27.11.2026

Nutzer präzisiert den gewünschten Standard: bei t=0 beginnen, Diagramm mit fester X-Skalierung füllen und erst am rechten Rand um genau eine Rasterteilung nach links schieben. Die vorherige Umstellung auf Full Buffer als Standard war eine Fehlinterpretation; sie veränderte die Skalierung während des Füllens und passte nicht zum gewünschten ruhigen Bild. GraphState startet nun wieder mit scrolling=true, sodass Main und MENU durch den gemeinsamen Renderer dieselbe schrittweise Zeitachse nutzen. MENU bietet Full Buffer weiterhin optional an. Die vorherige Korrektur des optionalen Full-Buffer-Modus bleibt erhalten. Plotdaten, stabile Halbierung, EVE-Ausgabe und Takte unverändert.

Neuer Hosttest prüft über zwei Ringkapazitäten, dass die X-Teilung konstant bleibt, der Ursprung bei null startet und jeder Wechsel exakt einer Teilung entspricht. Bestehende Graphsuite bestanden. Pico-Build erfolgreich: RAM 85.964 Byte, Flash 169.324 Byte. Logs: tmp/logs/fixed-step-default-host.log und tmp/logs/fixed-step-default-build.log. Kein Upload; Verhalten am Gerät noch zu prüfen.

## Übertragung auf MENU → Graph – 27.11.2026

Nutzer wünscht dieselbe Optimierung im MENU-Graph. GraphPage nutzt jetzt für Lines und Both dieselben stabilen geraden Erfassungsnummern wie Main: 512 Punkte je Kurve bei vollem Buffer, unabhängig von Ringposition und auch für I(V). Alle Messwerte bleiben in Buffer, Statistik und Achsenaufbereitung erhalten. Reiner Points-Stil bleibt bei voller Punktzahl und Bitmap-Ausgabe.

EVE-Vektoren sind für die allein sichtbare GraphPage freigegeben. Andere sichtbare Root-Flächen deaktivieren die Freigabe. Der interne Zahleneditor zeichnet weiterhin den aufgenommenen Hintergrund und den Editor, keine darunterliegenden Kurven. Im Graph-Tab werden Bedienelemente und Information vor den Kurven gezeichnet (getrennte Bildschirmbereiche), sodass deren tatsächlicher Displaylistenverbrauch vor der Vektorentscheidung bekannt ist. Für weitere Bitmap-Kurven, Cursor und DISPLAY sind 1024 Byte reserviert; Main behält seine bisherige Reserve von 2048 Byte.

Jede sichtbare Kurve versucht einzeln den Vektorpfad. Bei unzureichendem Budget oder anderen bestehenden Fallback-Bedingungen wird die jeweilige Kurve als Bitmap ausgegeben, ebenfalls mit stabil halbierter Auswahl. Mehrkurvenansichten können deshalb gemischte Vektor-/Bitmapausgabe verwenden und langsamer bleiben als eine einzelne Kurve. Keine weitere Reduktion unter 512 oder pauschale Zusage gleicher Geschwindigkeit. PlotPrepare erfasst nun auch die MENU-Achsenaufbereitung; x0 bezieht sich auf die aktive X-Achse (Sekunden bzw. Volt bei I(V)). Mode-/Bytefelder beschreiben weiterhin die letzte Kurve, die Zeitmessungen summieren alle Aufrufe; PlotCommands und PlotUpload helfen gemischte Ausgabe zu erkennen.

Host-Graphsuite bestanden, erweitert um Main/MENU-Vergleich sämtlicher ausgewählter Punkte über drei Ringkapazitäten mit Lines/Both-Wechsel, vier aktive Kurven, reinen Points-Stil sowie I(V). Bestehende Zoom-, Cursor-, Dialog- und Navigationsprüfungen bestehen. Pico-Build erfolgreich: RAM unverändert 85.964 Byte, Flash 169.284 Byte. Logs: tmp/logs/menu-vector-host.log und tmp/logs/menu-vector-build.log. Kein Upload. Nächster Gerätetest: zuerst MENU → Graph mit einer Kurve in Lines/Both, danach mehrere Kurven, Cursor und Zahleneditor; erst danach Laufzeitvergleich bewerten.
