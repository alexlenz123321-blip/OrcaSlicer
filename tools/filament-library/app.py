"""Windows desktop catalogue for the Open Filament Database and Snapmaker Orca."""
from __future__ import annotations

import gzip
import json
import logging
import os
from pathlib import Path
import queue
import subprocess
import sys
import threading
import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import webbrowser

import core

APP_NAME = "Snapmaker Filament Library"
APP_DIR = Path(os.environ.get("LOCALAPPDATA", str(Path.home() / ".cache"))) / "SnapmakerFilamentLibrary"
ASSETS = Path(getattr(sys, "_MEIPASS", Path(__file__).resolve().parent))


class LibraryApp:
    def __init__(self, root: tk.Tk, smoke=False):
        self.root = root
        self.smoke = smoke
        self.events = queue.Queue()
        self.busy = False
        self.closing = False
        self.selected = set()
        self.rows = []
        self.visible = []
        self.catalog = {}
        self.bases = {}
        self.settings_path = APP_DIR / "settings.json"
        APP_DIR.mkdir(parents=True, exist_ok=True)
        logging.basicConfig(filename=APP_DIR / "app.log", encoding="utf-8", level=logging.INFO,
                            format="%(asctime)s %(levelname)s %(message)s")
        try:
            settings = core.read_json(self.settings_path)
        except (OSError, ValueError):
            settings = {}
        executable = settings.get("executable") or str(core.discover_executable() or "")
        data_dir = core.default_data_dir()
        try:
            target = core.active_filament_dir(data_dir)
        except (ValueError, OSError) as exc:
            target = data_dir / "user" / "default" / "filament"
            logging.warning("Profilordner nicht erkannt: %s", exc)
        self.executable = tk.StringVar(value=executable)
        self.target = tk.StringVar(value=settings.get("target") or str(target))
        self.nozzle = tk.StringVar(value=settings.get("nozzle", "0.4"))
        self.query = tk.StringVar()
        self.status = tk.StringVar(value="Lade mitgelieferten Katalog …")
        self.summary = tk.StringVar()
        self.selection_text = tk.StringVar(value="0 Profile ausgewählt")
        self.update_existing = tk.BooleanVar(value=False)
        self.launch_after = tk.BooleanVar(value=True)
        root.title(f"{APP_NAME}  {core.VERSION}")
        root.geometry("1220x820")
        root.minsize(1000, 700)
        root.configure(bg="#101821")
        self.style()
        self.build_ui()
        self.reload_bases()
        cached = APP_DIR / "catalog.json.gz"
        try:
            catalog = core.load_catalog(cached if cached.is_file() else ASSETS / "catalog.json.gz")
        except (OSError, ValueError, EOFError):
            catalog = core.load_catalog(ASSETS / "catalog.json.gz")
        self.set_catalog(catalog)
        self.query.trace_add("write", lambda *_: self.filter_rows())
        self.nozzle.trace_add("write", lambda *_: self.filter_rows())
        root.after(100, self.poll)
        root.protocol("WM_DELETE_WINDOW", self.close)

    def style(self):
        s = ttk.Style()
        s.theme_use("clam")
        s.configure(".", background="#101821", foreground="#e6edf5", font=("Segoe UI", 10))
        s.configure("TFrame", background="#101821")
        s.configure("TLabel", background="#101821")
        s.configure("Muted.TLabel", foreground="#9bb0c4")
        s.configure("Title.TLabel", font=("Segoe UI", 24, "bold"))
        s.configure("TButton", padding=(12, 8), background="#243444", borderwidth=0)
        s.map("TButton", background=[("active", "#354a5d")], foreground=[("disabled", "#718296")])
        s.configure("Accent.TButton", background="#087f83", foreground="white")
        s.map("Accent.TButton", background=[("active", "#0d999d")])
        s.configure("TEntry", fieldbackground="#1b2836", foreground="#e6edf5", insertcolor="white", padding=6)
        s.configure("TCombobox", fieldbackground="#1b2836", foreground="#e6edf5", padding=5)
        s.map("TCombobox", fieldbackground=[("readonly", "#1b2836")], foreground=[("readonly", "#e6edf5")])
        s.configure("Treeview", background="#172431", fieldbackground="#172431", foreground="#e6edf5",
                    rowheight=30, borderwidth=0)
        s.configure("Treeview.Heading", background="#233445", foreground="#a8c0d4", padding=8)
        s.map("Treeview", background=[("selected", "#235368")])
        s.configure("TCheckbutton", background="#101821")
        s.map("TCheckbutton", background=[("active", "#101821")])
        s.configure("Horizontal.TProgressbar", background="#31c9bd", troughcolor="#243444")
        self.root.option_add("*TCombobox*Listbox.background", "#1b2836")
        self.root.option_add("*TCombobox*Listbox.foreground", "#e6edf5")

    def build_ui(self):
        outer = ttk.Frame(self.root, padding=22)
        outer.pack(fill="both", expand=True)
        header = ttk.Frame(outer)
        header.pack(fill="x")
        titles = ttk.Frame(header)
        titles.pack(side="left")
        ttk.Label(titles, text="Deine Filamentbibliothek", style="Title.TLabel").pack(anchor="w")
        ttk.Label(titles, text="Open Filament Database  →  Snapmaker Orca", style="Muted.TLabel").pack(anchor="w", pady=(3, 12))
        self.refresh_button = ttk.Button(header, text="Datenbank aktualisieren", command=self.refresh)
        self.refresh_button.pack(side="right")
        ttk.Label(outer, textvariable=self.summary, style="Muted.TLabel").pack(anchor="w", pady=(0, 12))

        paths = ttk.Frame(outer)
        paths.pack(fill="x", pady=(0, 15))
        paths.columnconfigure(1, weight=1)
        ttk.Label(paths, text="Snapmaker Orca:").grid(row=0, column=0, sticky="w", padx=(0, 12))
        ttk.Entry(paths, textvariable=self.executable).grid(row=0, column=1, sticky="ew")
        ttk.Button(paths, text="EXE wählen", command=self.browse_exe).grid(row=0, column=2, padx=(8, 0))
        ttk.Label(paths, text="Filamentordner:").grid(row=1, column=0, sticky="w", padx=(0, 12), pady=(6, 0))
        ttk.Entry(paths, textvariable=self.target).grid(row=1, column=1, sticky="ew", pady=(6, 0))
        ttk.Button(paths, text="Ordner wählen", command=self.browse_target).grid(row=1, column=2, padx=(8, 0), pady=(6, 0))

        body = ttk.Frame(outer)
        body.pack(fill="both", expand=True)
        side = ttk.Frame(body, width=210)
        side.pack(side="left", fill="y", padx=(0, 18))
        ttk.Label(side, text="MARKEN", style="Muted.TLabel").pack(anchor="w")
        ttk.Label(side, text="Mehrfachauswahl: Strg / Umschalt", style="Muted.TLabel", font=("Segoe UI", 8)).pack(anchor="w", pady=(2, 5))
        self.brand_list = self.listbox(side, height=10)
        ttk.Button(side, text="Alle Marken", command=lambda: self.reset_list(self.brand_list)).pack(fill="x", pady=5)
        ttk.Label(side, text="MATERIALARTEN", style="Muted.TLabel").pack(anchor="w", pady=(10, 5))
        self.material_list = self.listbox(side, height=6)
        ttk.Button(side, text="Alle Materialien", command=lambda: self.reset_list(self.material_list)).pack(fill="x", pady=5)

        main = ttk.Frame(body)
        main.pack(side="left", fill="both", expand=True)
        search = ttk.Frame(main)
        search.pack(fill="x", pady=(0, 10))
        ttk.Label(search, text="Suchen").pack(side="left", padx=(0, 8))
        ttk.Entry(search, textvariable=self.query).pack(side="left", fill="x", expand=True)
        ttk.Label(search, text="U1-Düse").pack(side="left", padx=(15, 8))
        ttk.Combobox(search, textvariable=self.nozzle, values=("0.2", "0.4", "0.6", "0.8"), width=5,
                     state="readonly").pack(side="left")
        ttk.Label(search, text="mm").pack(side="left", padx=4)
        table_frame = ttk.Frame(main)
        table_frame.pack(fill="both", expand=True)
        self.tree = ttk.Treeview(table_frame, columns=("check", "brand", "filament", "material", "base"),
                                 show="headings", selectmode="extended")
        for key, title, width in (("check", "✓", 38), ("brand", "Marke", 130), ("filament", "Filament", 245),
                                  ("material", "Material", 80), ("base", "U1-Kompatibilität", 190)):
            self.tree.heading(key, text=title)
            self.tree.column(key, width=width, minwidth=width if key == "check" else 60, stretch=key != "check")
        self.tree.pack(side="left", fill="both", expand=True)
        bar = ttk.Scrollbar(table_frame, orient="vertical", command=self.tree.yview)
        bar.pack(side="right", fill="y")
        self.tree.configure(yscrollcommand=bar.set)
        self.tree.tag_configure("unsupported", foreground="#9b9d9e")
        self.tree.bind("<Button-1>", self.tree_click)
        self.tree.bind("<space>", self.toggle_highlighted)
        self.tree.bind("<Double-1>", self.details)
        tools = ttk.Frame(main)
        tools.pack(fill="x", pady=(10, 0))
        ttk.Button(tools, text="Sichtbare passende auswählen", command=self.select_visible).pack(side="left")
        ttk.Button(tools, text="Auswahl leeren", command=self.clear_selection).pack(side="left", padx=8)
        ttk.Label(tools, textvariable=self.selection_text, style="Muted.TLabel").pack(side="right")

        ttk.Label(outer, text="Herstellerdaten als Ausgangspunkt. Fluss, Pressure Advance und Geschwindigkeit am eigenen Filament kalibrieren.",
                  style="Muted.TLabel").pack(anchor="w", pady=(16, 8))
        options = ttk.Frame(outer)
        options.pack(fill="x")
        ttk.Checkbutton(options, text="Unveränderte App-Importe aktualisieren (bearbeitete Profile bleiben geschützt)",
                        variable=self.update_existing).pack(side="left")
        ttk.Checkbutton(options, text="Orca danach öffnen", variable=self.launch_after).pack(side="right")
        actions = ttk.Frame(outer)
        actions.pack(fill="x", pady=(12, 8))
        self.import_button = ttk.Button(actions, text="Auswahl in Snapmaker Orca importieren", style="Accent.TButton", command=self.import_selected)
        self.import_button.pack(side="right")
        self.export_button = ttk.Button(actions, text="JSON-Dateien exportieren", command=self.export_selected)
        self.export_button.pack(side="right", padx=10)
        ttk.Button(actions, text="Quelle & Hilfe", command=self.help).pack(side="left")
        self.progress = ttk.Progressbar(outer, mode="indeterminate")
        self.progress.pack(fill="x", pady=(2, 7))
        ttk.Label(outer, textvariable=self.status, style="Muted.TLabel", wraplength=1100).pack(anchor="w")

    def listbox(self, parent, height):
        frame = ttk.Frame(parent)
        frame.pack(fill="both", expand=True)
        box = tk.Listbox(frame, selectmode="extended", exportselection=False, height=height, width=24,
                         bg="#172431", fg="#e6edf5", selectbackground="#235368", relief="flat",
                         highlightthickness=0, font=("Segoe UI", 10))
        box.pack(side="left", fill="both", expand=True)
        bar = ttk.Scrollbar(frame, orient="vertical", command=box.yview)
        bar.pack(side="right", fill="y")
        box.configure(yscrollcommand=bar.set)
        box.bind("<<ListboxSelect>>", lambda e: self.filter_rows())
        return box

    def set_catalog(self, catalog):
        self.catalog = catalog
        self.rows = catalog["profiles"]
        self.selected.clear()
        for box, key in ((self.brand_list, "brand"), (self.material_list, "material")):
            box.delete(0, "end")
            for value in sorted({r[key] for r in self.rows}, key=str.casefold):
                box.insert("end", value)
        unavailable = sum(catalog.get("skipped_materials", {}).values())
        self.summary.set(f"Stand {catalog.get('version', '?')}  ·  {len(self.rows):,} Produktprofile  ·  "
                         f"{len({r['brand'] for r in self.rows})} Marken  ·  {unavailable} weitere Produkte ohne OFD-Orca-Profil")
        self.filter_rows()
        self.status.set("Bereit. Marken/Material wählen, Profile anhaken und importieren. Orca muss dabei geschlossen sein.")

    def reload_bases(self):
        exe = Path(self.executable.get()) if self.executable.get() else None
        data_root = core.default_data_dir()
        # A custom profile directory still uses this installation's U1 base profiles.
        self.bases = core.base_profiles(exe, data_root)

    def compatibility(self, row):
        try:
            p = core.adapt_profile(row, self.bases, self.nozzle.get())
            return p["inherits"]
        except (ValueError, TypeError):
            return None

    def filter_rows(self):
        if not hasattr(self, "tree"):
            return
        brands = {self.brand_list.get(i) for i in self.brand_list.curselection()}
        materials = {self.material_list.get(i) for i in self.material_list.curselection()}
        query = self.query.get().casefold().strip()
        self.tree.delete(*self.tree.get_children())
        self.visible = []
        compatible_count = 0
        for i, row in enumerate(self.rows):
            if brands and row["brand"] not in brands or materials and row["material"] not in materials:
                continue
            if query and query not in f"{row['brand']} {row['filament']} {row['material']}".casefold():
                continue
            base = self.compatibility(row)
            if not base:
                self.selected.discard(row["path"])
            else:
                compatible_count += 1
            self.visible.append(i)
            self.tree.insert("", "end", iid=str(i), values=("☑" if row["path"] in self.selected else "☐",
                row["brand"], row["filament"], row["material"], base or "Keine passende U1-Basis"),
                tags=() if base else ("unsupported",))
        self.selection_text.set(f"{len(self.selected)} ausgewählt · {compatible_count}/{len(self.visible)} sichtbar passend")

    def reset_list(self, box):
        box.selection_clear(0, "end")
        self.filter_rows()

    def tree_click(self, event):
        if self.tree.identify_column(event.x) == "#1":
            iid = self.tree.identify_row(event.y)
            if iid:
                self.toggle([iid])
                return "break"

    def toggle_highlighted(self, _event):
        self.toggle(self.tree.selection())
        return "break"

    def toggle(self, ids):
        for iid in ids:
            row = self.rows[int(iid)]
            if self.compatibility(row):
                if row["path"] in self.selected:
                    self.selected.remove(row["path"])
                else:
                    self.selected.add(row["path"])
        self.filter_rows()

    def select_visible(self):
        self.selected.update(self.rows[i]["path"] for i in self.visible if self.compatibility(self.rows[i]))
        self.filter_rows()

    def clear_selection(self):
        self.selected.clear()
        self.filter_rows()

    def details(self, event=None):
        items = self.tree.selection()
        if not items:
            return
        row = self.rows[int(items[0])]
        try:
            preset = core.adapt_profile(row, self.bases, self.nozzle.get())
        except ValueError as exc:
            messagebox.showinfo("Profil nicht kompatibel", str(exc), parent=self.root)
            return
        window = tk.Toplevel(self.root)
        window.title(row["profile_name"])
        window.geometry("760x600")
        view = tk.Text(window, wrap="word", bg="#172431", fg="#e6edf5", font=("Consolas", 10), padx=15, pady=15)
        view.pack(fill="both", expand=True)
        view.insert("1.0", json.dumps(preset, ensure_ascii=False, indent=2))
        view.configure(state="disabled")

    def browse_exe(self):
        path = filedialog.askopenfilename(title="Snapmaker Orca auswählen", filetypes=[("Snapmaker Orca", "*.exe")])
        if path:
            self.executable.set(path)
            self.reload_bases()
            self.filter_rows()
            self.save_settings()

    def browse_target(self):
        path = filedialog.askdirectory(title="Aktiven Orca-Filamentordner wählen (user / Benutzer / filament)")
        if path:
            self.target.set(path)
            self.save_settings()

    def save_settings(self):
        core.atomic_write(self.settings_path, core.json_bytes({"executable": self.executable.get(),
                          "target": self.target.get(), "nozzle": self.nozzle.get()}))

    def run(self, work, done):
        if self.busy:
            return
        self.busy = True
        self.progress.start(12)
        for button in (self.import_button, self.export_button, self.refresh_button):
            button.configure(state="disabled")
        def worker():
            try:
                self.events.put(("done", done, work()))
            except Exception as exc:
                logging.exception("Vorgang fehlgeschlagen")
                self.events.put(("error", str(exc)))
        threading.Thread(target=worker, daemon=True).start()

    def progress_text(self, text):
        self.events.put(("status", text))

    def poll(self):
        try:
            # Coalesce frequent import progress messages to keep the UI responsive.
            status = None
            for _ in range(500):
                event = self.events.get_nowait()
                if event[0] == "status":
                    status = event[1]
                else:
                    self.busy = False
                    self.progress.stop()
                    for button in (self.import_button, self.export_button, self.refresh_button):
                        button.configure(state="normal")
                    if event[0] == "error":
                        self.status.set("Vorgang fehlgeschlagen: " + event[1])
                        messagebox.showerror(APP_NAME, event[1] + f"\n\nDetails: {APP_DIR / 'app.log'}", parent=self.root)
                    else:
                        try:
                            event[1](event[2])
                        except Exception as exc:
                            logging.exception("Ergebnis anzeigen fehlgeschlagen")
                            messagebox.showerror(APP_NAME, str(exc), parent=self.root)
                    status = None
        except queue.Empty:
            pass
        if status:
            self.status.set(status)
        if not self.closing:
            self.root.after(100, self.poll)

    def refresh(self):
        def work():
            catalog = core.refresh_catalog(self.progress_text)
            core.atomic_write(APP_DIR / "catalog.json.gz", gzip.compress(core.json_bytes(catalog)))
            return catalog
        self.run(work, self.set_catalog)

    def selected_profiles(self):
        self.reload_bases()
        profiles = []
        skipped = 0
        for row in self.rows:
            if row["path"] in self.selected:
                try:
                    profiles.append(core.adapt_profile(row, self.bases, self.nozzle.get()))
                except ValueError:
                    skipped += 1
        if not profiles:
            messagebox.showinfo(APP_NAME, "Bitte passende Profile anhaken. Falls keine passen: zuerst die Snapmaker-Orca-EXE wählen.", parent=self.root)
        elif skipped:
            messagebox.showinfo(APP_NAME, f"{skipped} ausgewählte Profile passen nicht zur Düse und werden ausgelassen.", parent=self.root)
        return profiles

    def import_selected(self):
        if self.busy:
            return
        try:
            self.save_settings()
            profiles = self.selected_profiles()
            if not profiles:
                return
            if core.slicer_running():
                messagebox.showinfo(APP_NAME, "Bitte das Projekt in Orca speichern und Snapmaker Orca vollständig schließen. Danach erneut auf Importieren klicken.", parent=self.root)
                return
            target = Path(self.target.get()).expanduser()
            if not target.is_absolute() or target.name.lower() != "filament" or not (target.parent / "machine").is_dir():
                raise ValueError("Bitte den vorhandenen aktiven Filamentordner wählen. Er liegt neben dem Ordner 'machine'. Snapmaker Orca zuvor einmal starten und einrichten.")
            update = self.update_existing.get()
            launch = self.launch_after.get()
            executable = Path(self.executable.get()) if self.executable.get() else None
            def done(report):
                text = (f"{report['installed']} neu · {report['updated']} aktualisiert · "
                        f"{report['skipped']} identisch · {report['protected']} vorhandene Profile geschützt")
                self.status.set(text)
                extra = f"\n\nImportprotokoll / Sicherung:\n{report['backup']}" if report["backup"] else ""
                messagebox.showinfo("Import abgeschlossen", text + "\n\nDie Profile stehen nach dem Start unter Benutzer-Filamenten bereit." + extra, parent=self.root)
                if launch and executable and executable.is_file():
                    subprocess.Popen([str(executable)], cwd=executable.parent)
            self.run(lambda: core.install_profiles(profiles, target, update, self.progress_text), done)
        except Exception as exc:
            messagebox.showerror(APP_NAME, str(exc), parent=self.root)

    def export_selected(self):
        profiles = self.selected_profiles()
        if not profiles:
            return
        parent = filedialog.askdirectory(title="Export-Zielordner wählen")
        if not parent:
            return
        from datetime import datetime
        destination = Path(parent) / ("OFD-U1-" + datetime.now().strftime("%Y%m%d-%H%M%S-%f"))
        self.run(lambda: core.export_profiles(profiles, destination),
                 lambda count: messagebox.showinfo("Export abgeschlossen", f"{count} Profile exportiert nach:\n{destination}\n\nIn Orca: Datei → Importieren → Konfigurationen importieren; JSON-Dateien auswählen.", parent=self.root))

    def help(self):
        messagebox.showinfo(APP_NAME,
            "1. Snapmaker-Orca-EXE prüfen und U1-Düse wählen.\n"
            "2. Marken/Materialien auswählen. Keine Filterauswahl bedeutet alle.\n"
            "3. Profile über das Kästchen anhaken; Doppelklick zeigt die Werte.\n"
            "4. Orca schließen und importieren. Danach Orca neu öffnen.\n\n"
            "Vorhandene Profile werden standardmäßig übersprungen. Mit der Update-Option werden nur unveränderte frühere App-Importe aktualisiert. "
            "Eigene Änderungen bleiben geschützt. Eine Sicherung und ein Importprotokoll entstehen vor dem Schreiben.\n\n"
            "Der Katalog enthält Produktprofile, keine einzelnen Farbspulen. Nicht unterstützte Material-/Düsenkombinationen werden nicht importiert. "
            "Spoolman-Zuordnungen bleiben separat.\n\n"
            "Daten: Open Filament Collective / SimplyPrint, MIT-Lizenz.\n"
            "https://openfilamentdatabase.org/brands\n"
            "Keine Anmeldung, kein API-Schlüssel und keine Administratorrechte erforderlich.", parent=self.root)

    def close(self):
        if self.busy:
            messagebox.showinfo(APP_NAME, "Der aktuelle Vorgang läuft noch. Bitte kurz warten, damit der Import vollständig abgeschlossen wird.", parent=self.root)
            return
        self.save_settings()
        self.closing = True
        self.root.destroy()


def main():
    if os.name == "nt":
        try:
            import ctypes
            ctypes.windll.shcore.SetProcessDpiAwareness(1)
        except (OSError, AttributeError):
            pass
    root = tk.Tk()
    app = LibraryApp(root, smoke="--smoke-test" in sys.argv)
    if app.smoke:
        root.update()
        assert len(app.rows) > 1000
        assert app.tree.get_children()
        root.destroy()
        return
    root.mainloop()


if __name__ == "__main__":
    main()
