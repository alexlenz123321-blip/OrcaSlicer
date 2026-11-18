"""OFD catalogue, Snapmaker profile adaptation and non-destructive installation."""
from __future__ import annotations

import copy
import ctypes
import gzip
import hashlib
import io
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import subprocess
import tempfile
from datetime import datetime, timezone
from urllib.parse import urljoin, urlsplit
from urllib.request import Request, urlopen
import zipfile

API = "https://api.openfilamentdatabase.org/orcaslicer/"
VERSION = "1.0.0"
MANIFEST = ".ofd-library-manifest"


def read_json(path: Path):
    return json.loads(path.read_text(encoding="utf-8-sig"))


def atomic_write(path: Path, data: bytes):
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, name = tempfile.mkstemp(prefix=".ofd-", suffix=".tmp", dir=path.parent)
    try:
        with os.fdopen(fd, "wb") as f:
            f.write(data)
            f.flush()
            os.fsync(f.fileno())
        os.replace(name, path)
    finally:
        if os.path.exists(name):
            os.unlink(name)


def json_bytes(value):
    return (json.dumps(value, ensure_ascii=False, indent=2) + "\n").encode("utf-8")


def digest(data: bytes):
    return hashlib.sha256(data).hexdigest()


def safe_name(name: str):
    cleaned = re.sub(r'[<>:"/\\|?*\x00-\x1f]', "_", name).strip().rstrip(". ")
    if not cleaned or cleaned.upper().split(".")[0] in {
        "CON", "PRN", "AUX", "NUL", *[f"COM{i}" for i in range(1, 10)], *[f"LPT{i}" for i in range(1, 10)]
    }:
        cleaned = "OFD_" + cleaned
    return cleaned


def download(relative: str, limit=25_000_000):
    url = urljoin(API, relative)
    if urlsplit(url).netloc != "api.openfilamentdatabase.org" or not url.startswith(API):
        raise ValueError("Ungültige Datenbank-Adresse")
    req = Request(url, headers={"User-Agent": f"Snapmaker-Filament-Library/{VERSION}"})
    with urlopen(req, timeout=40) as response:
        if urlsplit(response.url).scheme != "https":
            raise ValueError("Unsichere Weiterleitung")
        data = response.read(limit + 1)
    if len(data) > limit:
        raise ValueError("Die Datenbank-Datei überschreitet die Größenbegrenzung")
    return data


def validate_catalog(data):
    if not isinstance(data, dict) or not isinstance(data.get("profiles"), list):
        raise ValueError("Ungültiger OFD-Katalog")
    paths = set()
    for row in data["profiles"]:
        if not all(isinstance(row.get(k), str) and row[k] for k in
                   ("brand", "brand_slug", "material", "filament", "profile_name", "inherits", "path")):
            raise ValueError("Unvollständiger Katalogeintrag")
        path = PurePosixPath(row["path"])
        if path.is_absolute() or ".." in path.parts or row["path"] in paths:
            raise ValueError("Ungültiger oder doppelter Profilpfad")
        paths.add(row["path"])
        profile = row.get("preset")
        if not isinstance(profile, dict) or profile.get("name") != row["profile_name"]:
            raise ValueError("Profildaten fehlen oder stimmen nicht mit dem Katalog überein")
    return data


def load_catalog(path: Path):
    with gzip.open(path, "rt", encoding="utf-8") as f:
        return validate_catalog(json.load(f))


def refresh_catalog(progress=lambda text: None):
    progress("Aktuellen OFD-Katalog laden …")
    index = json.loads(download("index.json"))
    if not isinstance(index.get("profiles"), list) or len(index["profiles"]) > 20000:
        raise ValueError("Unerwartetes Katalogformat")
    progress("Alle verfügbaren Herstellerprofile herunterladen …")
    archive = download(index.get("all_bundle", "bundles/all.zip"))
    with zipfile.ZipFile(io.BytesIO(archive)) as z:
        infos = z.infolist()
        if sum(i.file_size for i in infos) > 150_000_000:
            raise ValueError("Das Profilarchiv ist unerwartet groß")
        by_name = {}
        for info in infos:
            by_name.setdefault(info.filename, []).append(info)
        for n, row in enumerate(index["profiles"]):
            # OFD bundles are grouped by brand and use sanitized preset names.
            original = re.sub(r'[<>:"/\\|?*]', "_", row["profile_name"]).strip().rstrip(".")
            key = f"{row['brand_slug']}/{original}.json"
            matches = by_name.get(key, [])
            if len(matches) == 1:
                if matches[0].file_size > 1_000_000:
                    raise ValueError("Einzelprofil ist unerwartet groß")
                row["preset"] = json.loads(z.read(matches[0]))
            else:
                # A few OFD products share a name. Fetch their unambiguous API path.
                row["preset"] = json.loads(download(row["path"], 1_000_000))
            if n % 200 == 0:
                progress(f"Profile prüfen: {n + 1} / {len(index['profiles'])}")
    return validate_catalog(index)


def discover_executable():
    candidates = []
    if os.name == "nt":
        import winreg
        for hive in (winreg.HKEY_LOCAL_MACHINE, winreg.HKEY_CURRENT_USER):
            for key_name in (r"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall",
                             r"SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall"):
                try:
                    with winreg.OpenKey(hive, key_name) as key:
                        for i in range(winreg.QueryInfoKey(key)[0]):
                            try:
                                with winreg.OpenKey(key, winreg.EnumKey(key, i)) as child:
                                    label = str(winreg.QueryValueEx(child, "DisplayName")[0])
                                    if "snapmaker" in label.lower() and "orca" in label.lower():
                                        root = Path(winreg.QueryValueEx(child, "InstallLocation")[0])
                                        candidates.append(root / "snapmaker-orca.exe")
                            except OSError:
                                continue
                except OSError:
                    continue
    for variable in ("ProgramFiles", "ProgramFiles(x86)", "LOCALAPPDATA"):
        if os.environ.get(variable):
            root = Path(os.environ[variable])
            candidates += [root / "Snapmaker_Orca" / "snapmaker-orca.exe",
                           root / "Programs" / "Snapmaker_Orca" / "snapmaker-orca.exe"]
    return next((p for p in candidates if p.is_file()), None)


def default_data_dir():
    return Path(os.environ.get("APPDATA", str(Path.home() / ".config"))) / "Snapmaker_Orca"


def active_filament_dir(data_dir: Path):
    folder = "default"
    config = data_dir / "Snapmaker_Orca.conf"
    if config.exists():
        text = config.read_text(encoding="utf-8-sig")
        try:
            obj, _ = json.JSONDecoder().raw_decode(text.lstrip())
            folder = (obj.get("preset_folder") or obj.get("", {}).get("preset_folder")
                      or obj.get("app", {}).get("preset_folder") or "default")
        except (ValueError, AttributeError):
            match = re.search(r"(?m)^\s*preset_folder\s*=\s*(.*?)\s*$", text)
            if match:
                folder = match.group(1) or "default"
    if not isinstance(folder, str) or folder in (".", "..") or re.search(r'[/\\:\x00]', folder):
        raise ValueError("Der aktive Orca-Profilordner ist ungültig; bitte den Filamentordner manuell wählen.")
    return data_dir / "user" / folder / "filament"


def base_profiles(executable: Path | None, data_dir: Path):
    """Read actual installed U1 bases rather than assuming upstream @System exists."""
    result = {}
    roots = []
    if executable:
        roots.append(executable.parent / "resources" / "profiles" / "Snapmaker" / "filament")
    roots.append(data_dir / "system" / "Snapmaker" / "filament")
    for root in roots:
        if root.is_dir():
            for file in root.glob("Generic*.json"):
                try:
                    value = read_json(file)
                    if str(value.get("instantiation", "")).lower() == "true":
                        result[value["name"]] = value
                except (ValueError, OSError, KeyError):
                    continue
    return result


def choose_base(row, bases, nozzle="0.4"):
    source = row["inherits"]
    generic = source.removesuffix(" @System")
    # Explicit semantic equivalents. No CF/GF -> unfilled or PA -> PLA fallback.
    aliases = {"Generic PLA Matte": "Generic PLA", "Generic PLA High Speed": "Generic PLA High Speed"}
    generic = aliases.get(generic, generic)
    names = [generic] if nozzle == "0.4" else []
    names += [f"{generic} @U1 {nozzle} nozzle"]
    for name in names:
        if name in bases:
            return name
    return None


def adapt_profile(row, bases, nozzle="0.4"):
    base = choose_base(row, bases, nozzle)
    if not base:
        raise ValueError(f"Keine passende U1-Basis für {row['inherits']} / {nozzle} mm")
    source = row["preset"]
    diameter = source.get("filament_diameter", ["1.75"])
    if any(abs(float(d) - 1.75) > 0.05 for d in diameter):
        raise ValueError("Profil ist für einen anderen Filamentdurchmesser als 1,75 mm")
    result = copy.deepcopy(source)
    for key in ("type", "instantiation", "compatible_printers", "setting_id", "filament_id",
                "printer_settings_id", "print_settings_id", "spoolman_url", "spoolman_filament_id"):
        result.pop(key, None)
    identity = hashlib.sha256(row["path"].encode()).hexdigest()[:8]
    title = row["profile_name"].removesuffix(" (OFD)")
    # Include material and stable ID: OFD contains a few duplicate product names.
    name = safe_name(f"{title[:115]} · {row['material']} · {identity} (OFD U1 {nozzle})")
    result.update(name=name, inherits=base, **{"from": "User", "version": "2.0.2.0",
                                           "filament_settings_id": [name]})
    result["filament_diameter"] = ["1.75"]
    notes = source.get("filament_notes", [])
    if isinstance(notes, str):
        notes = [notes]
    result["filament_notes"] = ["\n".join(notes) + f"\n\nSnapmaker Filament Library {VERSION}\n"
                                f"U1-Basis: {base}\n"
                                "Hersteller-/Datenbankwerte; Fluss und Pressure Advance nicht individuell kalibriert.\n"
                                "Farben/Spulen und Spoolman-Verknüpfungen werden nicht automatisch angelegt."]
    return result


def slicer_running():
    if os.name != "nt":
        return False
    # Toolhelp avoids localized tasklist output, console windows and a psutil dependency.
    from ctypes import wintypes
    class Entry(ctypes.Structure):
        _fields_ = [("dwSize", wintypes.DWORD), ("cntUsage", wintypes.DWORD),
                    ("th32ProcessID", wintypes.DWORD), ("th32DefaultHeapID", ctypes.c_size_t),
                    ("th32ModuleID", wintypes.DWORD), ("cntThreads", wintypes.DWORD),
                    ("th32ParentProcessID", wintypes.DWORD), ("pcPriClassBase", wintypes.LONG),
                    ("dwFlags", wintypes.DWORD), ("szExeFile", wintypes.WCHAR * 260)]
    kernel = ctypes.WinDLL("kernel32", use_last_error=True)
    kernel.CreateToolhelp32Snapshot.restype = wintypes.HANDLE
    kernel.CreateToolhelp32Snapshot.argtypes = [wintypes.DWORD, wintypes.DWORD]
    kernel.Process32FirstW.argtypes = [wintypes.HANDLE, ctypes.POINTER(Entry)]
    kernel.Process32NextW.argtypes = [wintypes.HANDLE, ctypes.POINTER(Entry)]
    kernel.CloseHandle.argtypes = [wintypes.HANDLE]
    handle = kernel.CreateToolhelp32Snapshot(2, 0)
    if handle == ctypes.c_void_p(-1).value:
        raise OSError("Windows-Prozessliste konnte nicht geprüft werden")
    try:
        entry = Entry()
        entry.dwSize = ctypes.sizeof(entry)
        more = kernel.Process32FirstW(handle, ctypes.byref(entry))
        while more:
            if entry.szExeFile.lower() in ("snapmaker-orca.exe", "snapmaker_orca.exe"):
                return True
            more = kernel.Process32NextW(handle, ctypes.byref(entry))
    finally:
        kernel.CloseHandle(handle)
    return False


def install_profiles(profiles, target: Path, update=False, progress=lambda text: None):
    """Only update byte-identical files previously written by this app. Roll back on error."""
    if slicer_running():
        raise RuntimeError("Snapmaker Orca bitte vollständig schließen und erneut importieren. Ungespeicherte Projekte vorher speichern.")
    if not profiles:
        raise ValueError("Keine kompatiblen Profile ausgewählt")
    target.mkdir(parents=True, exist_ok=True)
    manifest_path = target / MANIFEST
    manifest = read_json(manifest_path) if manifest_path.exists() else {"files": {}}
    if not isinstance(manifest.get("files"), dict):
        raise ValueError("Importverwaltung ist beschädigt. Es wurde nichts geändert.")
    original_manifest = manifest_path.read_bytes() if manifest_path.exists() else None
    report = {"installed": 0, "updated": 0, "skipped": 0, "protected": 0, "backup": ""}
    planned = []
    seen = set()
    for profile in profiles:
        name = profile["name"]
        filename = safe_name(name) + ".json"
        if filename.casefold() in seen:
            raise ValueError("Doppelte Profilnamen in der Auswahl")
        seen.add(filename.casefold())
        path = target / filename
        if path.is_symlink():
            raise ValueError("Verknüpfung im Zielordner: " + filename)
        data = json_bytes(profile)
        before = path.read_bytes() if path.exists() else None
        if before == data:
            report["skipped"] += 1
            continue
        if before is not None:
            owned = manifest["files"].get(filename)
            if not update or not isinstance(owned, dict) or digest(before) != owned.get("sha256"):
                report["protected"] += 1
                continue
        planned.append((path, data, before))
    if not planned:
        return report
    backup = target.parent / "ofd-library-backups" / datetime.now().strftime("%Y%m%d-%H%M%S-%f")
    backup.mkdir(parents=True)
    if original_manifest:
        atomic_write(backup / MANIFEST, original_manifest)
    journal = {"target": str(target), "created": [], "replaced": []}
    for path, data, before in planned:
        if before is None:
            journal["created"].append(path.name)
        else:
            atomic_write(backup / path.name, before)
            if path.with_suffix(".info").exists():
                shutil.copy2(path.with_suffix(".info"), backup / path.with_suffix(".info").name)
            journal["replaced"].append(path.name)
    atomic_write(backup / "import-log.json", json_bytes(journal))
    applied = []
    try:
        for n, (path, data, before) in enumerate(planned):
            if slicer_running():
                raise RuntimeError("Orca wurde während des Imports gestartet. Der Import wird zurückgesetzt.")
            # Recheck immediately before writing to protect concurrent user edits.
            current = path.read_bytes() if path.exists() else None
            if current != before:
                raise RuntimeError("Eine Profildatei wurde zwischenzeitlich geändert. Import zurückgesetzt.")
            atomic_write(path, data)
            applied.append((path, before))
            manifest["files"][path.name] = {"sha256": digest(data), "name": profile_name(data)}
            report["installed" if before is None else "updated"] += 1
            progress(f"Importiere {n + 1} / {len(planned)} …")
        atomic_write(manifest_path, json_bytes(manifest))
    except Exception:
        for path, before in reversed(applied):
            if before is None:
                path.unlink(missing_ok=True)
            else:
                atomic_write(path, before)
        if original_manifest is not None:
            atomic_write(manifest_path, original_manifest)
        else:
            manifest_path.unlink(missing_ok=True)
        raise
    report["backup"] = str(backup)
    return report


def profile_name(data):
    return json.loads(data)["name"]


def export_profiles(profiles, destination: Path):
    destination.mkdir(parents=True, exist_ok=True)
    count = 0
    for profile in profiles:
        target = destination / (safe_name(profile["name"]) + ".json")
        if target.exists():
            raise FileExistsError("Exportdatei existiert bereits: " + target.name)
        atomic_write(target, json_bytes(profile))
        count += 1
    return count
