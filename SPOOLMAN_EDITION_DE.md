# Snapmaker Orca 2.4.0 Spoolman Edition

Basis: offizieller Tag v2.4.0 (b1831e5dcb464172de33783142425aafda834fbc).
Die zusätzlichen Änderungen wurden aus dem geprüften Stand d581fdea78d6e7cd1718abc0c7196a8919df239e übernommen.

Im Filamentprofil stehen weiterhin Spoolman URL und Spoolman filament ID zur Verfügung.
Die ID bezeichnet das Filament in Spoolman, nicht die einzelne Spule.
Die bisherigen Konfigurationsschlüssel und das Anwendungsverzeichnis bleiben gleich.
Der Auswahlknopf, dessen Entfernung den gemeldeten Dialogabsturz behoben hat, ist nicht enthalten.

Die Synchronisierung versucht eine Zuordnung über eine vom Drucker gelieferte Spulen-ID
oder über Material, Farbe und Hersteller zu einem verknüpften lokalen Profil.
Die weiterhin gemeldete Auswahl von Generic ist durch diesen Versionswechsel allein
nicht als behoben anzusehen. Für die weitere Diagnose fehlen noch die lokale Logdatei
und die gespeicherten Verbindungswerte.

Vor einem Versionswechsel den Konfigurationsordner sichern.
Die neue Basis enthält die offiziellen Änderungen an Profilen und Bibliotheken.
Der Windows-Build und ein Test auf dem Zielrechner sind zusätzlich zu den
Quellcodeprüfungen erforderlich.
