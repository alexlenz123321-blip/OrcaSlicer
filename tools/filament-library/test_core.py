import copy
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import zipfile

import core

HERE = Path(__file__).parent


class LibraryTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.catalog = core.load_catalog(HERE / "catalog.json.gz")
        cls.row = next(r for r in cls.catalog["profiles"] if r["brand_slug"] == "anycubic" and r["inherits"] == "Generic PLA @System")
        cls.bases = {name: {} for name in ("Generic PLA", "Generic PLA @U1 0.6 nozzle", "Generic PETG", "Generic PLA-CF")}

    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        self.target = self.root / "user" / "default" / "filament"
        self.profile = core.adapt_profile(self.row, self.bases)

    def tearDown(self):
        self.temp.cleanup()

    def test_snapshot_all_records_are_valid(self):
        self.assertGreater(len(self.catalog["profiles"]), 2000)
        self.assertGreater(len({r['brand'] for r in self.catalog['profiles']}), 150)
        core.validate_catalog(self.catalog)

    def test_snapmaker_parent_and_metadata(self):
        before = copy.deepcopy(self.row)
        self.assertEqual(self.profile['inherits'], 'Generic PLA')
        self.assertEqual(self.profile['filament_vendor'], self.row['preset']['filament_vendor'])
        self.assertEqual(self.profile['nozzle_temperature'], self.row['preset']['nozzle_temperature'])
        self.assertEqual(self.profile['filament_settings_id'], [self.profile['name']])
        for key in ('type', 'setting_id', 'filament_id', 'compatible_printers', 'spoolman_url'):
            self.assertNotIn(key, self.profile)
        self.assertEqual(before, self.row)

    def test_nozzle_is_exact(self):
        self.assertEqual(core.choose_base(self.row, self.bases, '0.6'), 'Generic PLA @U1 0.6 nozzle')
        self.assertIsNone(core.choose_base(self.row, self.bases, '0.8'))

    def test_fibre_material_never_falls_back_to_plain(self):
        row = dict(self.row, inherits='Generic PETG-CF @System')
        self.assertIsNone(core.choose_base(row, self.bases))

    def test_wrong_diameter_rejected(self):
        row = copy.deepcopy(self.row)
        row['preset']['filament_diameter'] = ['2.85']
        with self.assertRaises(ValueError):
            core.adapt_profile(row, self.bases)

    def test_same_product_names_have_distinct_stable_names(self):
        row = dict(self.row, path=self.row['path'] + '-different')
        self.assertNotEqual(core.adapt_profile(row, self.bases)['name'], self.profile['name'])
        self.assertEqual(core.adapt_profile(self.row, self.bases)['name'], self.profile['name'])

    def test_active_signed_in_user_config(self):
        (self.root / 'Snapmaker_Orca.conf').write_text('{"": {"preset_folder": "12345"}}\n# checksum', encoding='utf-8')
        self.assertEqual(core.active_filament_dir(self.root), self.root / 'user' / '12345' / 'filament')

    def test_ini_and_default_config(self):
        self.assertEqual(core.active_filament_dir(self.root), self.target)
        (self.root / 'Snapmaker_Orca.conf').write_text('preset_folder = alex\n', encoding='utf-8')
        self.assertEqual(core.active_filament_dir(self.root), self.root / 'user' / 'alex' / 'filament')

    def test_path_traversal_rejected(self):
        (self.root / 'Snapmaker_Orca.conf').write_text('{"preset_folder":"../escape"}', encoding='utf-8')
        with self.assertRaises(ValueError):
            core.active_filament_dir(self.root)
        invalid = copy.deepcopy(self.catalog)
        invalid['profiles'][0]['path'] = '../bad.json'
        with self.assertRaises(ValueError):
            core.validate_catalog(invalid)

    def test_first_import_and_repeat(self):
        first = core.install_profiles([self.profile], self.target)
        self.assertEqual(first['installed'], 1)
        self.assertEqual(core.install_profiles([self.profile], self.target)['skipped'], 1)
        self.assertEqual(len(list(self.target.glob('*.json'))), 1)
        self.assertTrue((Path(first['backup']) / 'import-log.json').exists())
        self.assertFalse(core.MANIFEST.endswith('.json'))

    def test_user_edited_profile_is_protected_even_in_update_mode(self):
        core.install_profiles([self.profile], self.target)
        file = next(self.target.glob('*.json'))
        edited = json.loads(file.read_text(encoding='utf-8'))
        edited['filament_flow_ratio'] = ['0.91']
        file.write_bytes(core.json_bytes(edited))
        before = file.read_bytes()
        changed = dict(self.profile, nozzle_temperature=['215'])
        report = core.install_profiles([changed], self.target, update=True)
        self.assertEqual(report['protected'], 1)
        self.assertEqual(file.read_bytes(), before)

    def test_opt_in_updates_are_backed_up(self):
        core.install_profiles([self.profile], self.target)
        file = next(self.target.glob('*.json'))
        original = file.read_bytes()
        changed = dict(self.profile, nozzle_temperature=['215'])
        self.assertEqual(core.install_profiles([changed], self.target)['protected'], 1)
        report = core.install_profiles([changed], self.target, update=True)
        self.assertEqual(report['updated'], 1)
        self.assertEqual((Path(report['backup']) / file.name).read_bytes(), original)
        self.assertEqual(core.read_json(file)['nozzle_temperature'], ['215'])

    def test_unmanaged_files_never_overwritten(self):
        self.target.mkdir(parents=True)
        file = self.target / (core.safe_name(self.profile['name']) + '.json')
        file.write_text('custom data', encoding='utf-8')
        self.assertEqual(core.install_profiles([self.profile], self.target, update=True)['protected'], 1)
        self.assertEqual(file.read_text(), 'custom data')

    def test_failed_write_rolls_back_batch(self):
        core.install_profiles([self.profile], self.target)
        file = next(self.target.glob('*.json'))
        original = file.read_bytes()
        original_manifest = (self.target / core.MANIFEST).read_bytes()
        changed = dict(self.profile, nozzle_temperature=['215'])
        second = dict(self.profile, name=self.profile['name'] + ' second')
        real = core.atomic_write
        failed = False
        def fail_once(path, data):
            nonlocal failed
            if path.parent == self.target and 'second' in path.name and not failed:
                failed = True
                raise OSError('simulated disk failure')
            return real(path, data)
        with patch('core.atomic_write', side_effect=fail_once):
            with self.assertRaises(OSError):
                core.install_profiles([changed, second], self.target, update=True)
        self.assertEqual(file.read_bytes(), original)
        self.assertEqual((self.target / core.MANIFEST).read_bytes(), original_manifest)
        self.assertEqual(len(list(self.target.glob('*.json'))), 1)

    def test_running_orca_blocks_import(self):
        with patch('core.slicer_running', return_value=True):
            with self.assertRaises(RuntimeError):
                core.install_profiles([self.profile], self.target)
        self.assertFalse(self.target.exists())

    def test_base_profiles_read_installed_resources(self):
        parent = self.root / 'resources' / 'profiles' / 'Snapmaker' / 'filament'
        parent.mkdir(parents=True)
        (parent / 'Generic PLA.json').write_text('{"name":"Generic PLA", "instantiation":"true"}', encoding='utf-8')
        self.assertIn('Generic PLA', core.base_profiles(self.root / 'snapmaker-orca.exe', self.root))

    def test_refresh_uses_bulk_zip_and_handles_duplicate_names(self):
        row = {k: v for k, v in self.row.items() if k != 'preset'}
        index = {'profiles': [row], 'all_bundle': 'bundles/all.zip'}
        original = core.safe_name(row['profile_name'])
        buf = io.BytesIO()
        with zipfile.ZipFile(buf, 'w') as z:
            z.writestr(f"{row['brand_slug']}/{original}.json", core.json_bytes(self.row['preset']))
        def fake_download(path, *args):
            return core.json_bytes(index) if path == 'index.json' else buf.getvalue()
        with patch('core.download', side_effect=fake_download):
            new = core.refresh_catalog()
        self.assertEqual(new['profiles'][0]['preset'], self.row['preset'])

    def test_every_snapshot_profile_maps_without_name_collisions(self):
        # Matches stock Snapmaker U1 base names; no silent generic replacements.
        bases = {f'Generic {s}': {} for s in ('PLA', 'PLA Silk', 'PLA High Speed', 'PLA-CF', 'PETG', 'PETG HF',
                 'PETG-CF', 'PETG-GF', 'ABS', 'ASA', 'TPU', 'TPU 95A HF', 'PA', 'PA-CF', 'PC', 'PCTG', 'PVA', 'BVOH', 'Support For PLA')}
        names = set()
        count = 0
        for row in self.catalog['profiles']:
            try:
                value = core.adapt_profile(row, bases)
            except ValueError:
                continue
            self.assertNotIn(value['name'].casefold(), names)
            names.add(value['name'].casefold())
            self.assertEqual(value['filament_diameter'], ['1.75'])
            count += 1
        self.assertGreater(count, 1500)


if __name__ == '__main__':
    unittest.main()
