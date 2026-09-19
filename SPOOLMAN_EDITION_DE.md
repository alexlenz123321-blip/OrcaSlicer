# Snapmaker Orca Spoolman Edition

Diese Variante basiert auf Snapmaker Orca 2.3.6 und ergänzt eine optionale,
dauerhafte Verbindung zwischen einem Orca-Filamentprofil und einem
Spoolman-Filament.

## Felder im Filamentprofil

Unter **Filamenteinstellungen → Filament → Spoolman profile link** stehen zwei
zusätzliche Felder:

- **Spoolman URL**: vollständige Adresse des Servers, beispielsweise
  `http://192.168.178.81:7912`.
- **Spoolman filament ID (optional)**: numerische ID des Filaments in Spoolman.

Bleibt die Filament-ID leer, wird keine feste Zuordnung geschrieben. Die
vorhandene automatische Namenszuordnung bleibt dann aktiv.

Ist eine ID eingetragen, verbindet Orca das Profil beim Speichern mit diesem
Spoolman-Filament. Dazu werden am Filament die Spoolman-Zusatzfelder
`orca_profile` und `variant` angelegt beziehungsweise aktualisiert. Ein bereits
vorhandenes `extra`-Feld wird dabei erhalten.

Der Profilname muss dem Muster

`<Hersteller> <Material> <Variante> @Snapmaker U1 0.4 nozzle`

folgen, zum Beispiel:

`Anycubic PLA Basic Black @Snapmaker U1 0.4 nozzle`

Kann Spoolman nicht erreicht werden oder ist die ID ungültig, bleibt das lokal
gespeicherte Orca-Profil erhalten und Orca zeigt eine Warnung an.

## Filamentsynchronisierung

Beim Synchronisieren der U1-Filamente berücksichtigt diese Edition exakte,
kompatible Benutzerprofile. Der generische Fallback bleibt auf Systemprofile
beschränkt.
