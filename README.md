## TO BE TESTED:
### HIGH Priority:
- Input Capture und Output Capture + Auslesen der Entfernung aus dem Ultraschallsensor
<br>

## TODO:
### MEDIUM Priority
- Für zweite Zeile: Durch Button lässt sich zwischen den Funktionen wie in einem Menü umherspringen (Interrupt mit Button)
  - optischer Entfernungswarner (auf dem Display als Progress-Bar)  ==> EASY
    - Anzahl der verfügbaren Zeichen auf Display anschauen
    - 0 bedeutet sehr nah, x (x ist die maximale Zahl) bedeutet sehr weit weg
    - neue Funktion berechnet wie viele Elemente angezeigt werden (zwischen 0 und x)
  - Berechnung der Geschwindigkeit  ==> HARD
    - benötigt wird Distanz und Zeit
    - Ermittlung von Distanz kein Problem
    - Ermittlung von Zeit ist schwieriger
  - Speicherung des Signals und Differenzmessung ==> MEDIUM
    - Knopf wird gedrückt (mit Interrupt), dadurch wird die aktuelle Distanz gespeichert (in Buffer-Variable)
    - Knopf wird weiteres Mal gedrückt, dadurch wird der zweite Messwert mit dem ersten verglichen und die Differenz ausgerechnet
    - kann beliebig oft gemacht werden
- Datum anzeigen lassen (und Uhrzeit) ==> MEDIUM (mit Datum und/oder Uhrzeit einstellen ==> HARD) 
  - eingestelltes Datum wird angezeigt
<br>
