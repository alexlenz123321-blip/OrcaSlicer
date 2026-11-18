# Snapmaker Filament Library

Windows-App zum Auswählen und Importieren von Open-Filament-Database-Produktprofilen in Snapmaker Orca für den U1.

## Benutzung

1. ZIP entpacken und `Snapmaker-Filament-Library.exe` starten. Python und Administratorrechte sind nicht nötig.
2. Die App erkennt die übliche Snapmaker-Orca-Installation und den aktiven Benutzerordner. Falls nötig über **EXE wählen** und **Ordner wählen** korrigieren. Der Filamentordner liegt normalerweise unter `%APPDATA%\Snapmaker_Orca\user\<Benutzer>\filament`, neben `machine` und `process`. Bei portabler Installation den tatsächlich verwendeten Datenordner wählen.
3. U1-Düsengröße wählen. Marken und Materialien mit Strg/Umschalt mehrfach auswählen. Keine Filterauswahl bedeutet alle. Die Suche filtert zusätzlich nach Produktnamen.
4. Einzelne Profile über das Kästchen anhaken oder **Sichtbare passende auswählen** verwenden. Die Auswahl bleibt beim Wechsel der Filter erhalten. Ein Doppelklick zeigt die exportierten Einstellungen.
5. Projekte in Orca speichern und Orca vollständig schließen. Dann **Auswahl in Snapmaker Orca importieren** anklicken. Orca wird bei aktivierter Option anschließend geöffnet. Die Profile erscheinen unter den Benutzerfilamenten.

Die App beendet Orca nicht selbst. Der Import schreibt ausschließlich neue, eindeutig benannte OFD-Filamentprofile. Bereits vorhandene Dateien werden standardmäßig geschützt. Die optionale Aktualisierung ersetzt nur App-Importe, deren Inhalt seit dem letzten Import unverändert ist. Kalibrierte oder anderweitig bearbeitete Profile werden auch dann übersprungen. Vor jeder Änderung entstehen ein Importprotokoll und Sicherungen ersetzter Dateien im benachbarten Ordner `ofd-library-backups`. Bei einem Schreibfehler wird der aktuelle Import zurückgesetzt. Ein Absturz oder Stromausfall kann anhand dieses Protokolls manuell rückgängig gemacht werden: Orca schließen, im Protokoll `created` genannte Dateien entfernen und unter `replaced` genannte Sicherungen zurückkopieren.

Alternativ **JSON-Dateien exportieren** verwenden. Die Dateien können in Orca über Datei → Importieren → Konfigurationen importieren ausgewählt werden.

## Daten und Grenzen

- Der mitgelieferte Offlinekatalog wurde am 18.11.2026 aus dem OFD-Quellstand `016c0da92f7687758bd6eba4a52b8e770d6e2119` mit dessen offiziellem Orca-Exporter erzeugt: 2.122 Produktprofile von 167 Marken. Weitere 93 Produkte exportiert OFD mangels Orca-Basis nicht. Die App zeigt diese Zahl an.
- **Datenbank aktualisieren** lädt den aktuellen offiziellen OFD-Orca-Katalog und das Gesamtarchiv per HTTPS. Ohne Netzwerk bleibt der mitgelieferte oder zuletzt gespeicherte Katalog nutzbar. Dabei werden keine lokalen Filamentdaten, Druckerinformationen oder Pfade hochgeladen.
- Die App passt die Profilvererbung an die tatsächlich installierten Snapmaker-U1-Basisprofile und die gewählte Düse an. Nicht verfügbare Basen und abweichende Filamentdurchmesser werden nicht importiert. PLA Matte darf die normale PLA-Basis verwenden; fasergefüllte Materialien werden niemals still auf ungefülltes Material umgestellt.
- Das sind Ausgangsprofile mit den in der Datenbank hinterlegten Einstellungen. Sie sind keine individuell kalibrierten U1-Profile. Fehlende Werte stammen vom gewählten U1-Basisprofil. Herstellerdaten können unvollständig oder falsch sein; vor dem Druck Temperatur und Materialeignung prüfen.
- Es handelt sich um Produktlinien, nicht um ein Profil für jede Farb-/Gewichtsvariante. Farben und physische Spulen sowie Spoolman-Zuordnungen werden nicht automatisch angelegt. Vorhandene Spoolman-Profile werden nicht bearbeitet.
- Einstellungen, Cache und Fehlerprotokoll liegen in `%LOCALAPPDATA%\SnapmakerFilamentLibrary`. Es ist keine Anmeldung erforderlich.
- Der direkte Import ist für Snapmaker Orca 2.4.1 und dessen Profilformat entwickelt. Laufzeitprüfung am echten U1 bleibt erforderlich.

## Entwicklung und Tests

Python 3.12, ausschließlich Standardbibliothek zur Laufzeit. Tests:

```powershell
cd tools/filament-library
python -m unittest -v
python app.py --smoke-test
```

Windows-Build:

```powershell
python -m pip install pyinstaller==6.11.1
python -m PyInstaller --noconfirm --clean --onefile --windowed --name Snapmaker-Filament-Library --add-data "catalog.json.gz;." --add-data "LICENSE-OFD.txt;." app.py
```

Der Workflow `.github/workflows/build-filament-library.yml` führt Tests und GUI-Startprüfungen aus, baut die EXE und stellt eine ZIP mit EXE, Anleitung und Lizenz bereit. Die App importiert keine ausführbaren Inhalte aus der Datenbank; heruntergeladene JSON-Profile werden als Daten verarbeitet.

## Quellen

- https://openfilamentdatabase.org/brands
- https://api.openfilamentdatabase.org/orcaslicer/index.json
- https://github.com/OpenFilamentCollective/open-filament-database/blob/main/docs/orcaslicer.md
- OFD-Daten und Exporter: Open Filament Collective / SimplyPrint, MIT; Lizenz in `LICENSE-OFD.txt`.

Diese Anwendung ist ein unabhängiges Hilfsprogramm und kein offizielles Snapmaker-Produkt.
