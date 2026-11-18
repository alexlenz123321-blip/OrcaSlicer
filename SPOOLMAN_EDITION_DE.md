# Snapmaker Orca 2.4.2-alpha Spoolman Edition

Basis ist der offizielle Tag v2.4.2-alpha (19df6ce6516ef68b5bb30a543af954e69d6d7844)
mit den Spoolman-Anpassungen der zuvor erstellten Version 2.4.1.
Dies ist eine Vorabversion; die bisherige stabile Version bleibt separat verfügbar.
Die Filamentprofile enthalten Spoolman URL und Spoolman filament ID.
Beim Speichern wird das zugehörige Profil am Spoolman-Filament hinterlegt.
Der Sync wählt kompatible eigene Profile über die gemeldete Spulen-ID aus.
Fehlt die Spulen-ID, verwendet er Material, Farbe, Hersteller und die Filament-ID.
Die Serveradresse wird unter Einstellungen > Spoolman > Server URL gespeichert.
Das Feld ist anfangs leer. Ein Hostname oder eine IP mit Port genügt, HTTP wird ergänzt.
Die globale Adresse gilt für Sync und Profilverknüpfung.
Bei leerer globaler Adresse werden vorhandene Profiladressen weiter verwendet.
Die Gateway-Geräteanbindung übernimmt auch filament_spool_id.
Änderungen der Spulen-ID aktualisieren die Slotdaten.
Die Anwendung verwendet das bisherige Konfigurationsverzeichnis Snapmaker_Orca.

Der Windows-Workflow erstellt Installer und portable Anwendung als Build-Artefakte.
Der Funktionstest auf dem Zielrechner umfasst Profil speichern, Sync und Farben
sowie die Kontrolle des Flussverhältnisses und Pressure Advance nach dem Sync.
