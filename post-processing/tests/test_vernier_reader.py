# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
from pathlib import Path
import unittest
import sys

# pylint: disable=wrong-import-position
sys.path.append(str(Path(__file__).parent.parent))
from vernier.vernier_reader import VernierReader


class TestVernierReader(unittest.TestCase):
    """
    Tests for the VernierReader class
    """
    def setUp(self):
        self.test_data_dir = Path(__file__).parent / "data"

    def test_nonexistent_file(self):
        with self.assertRaises(ValueError):
            VernierReader(self.test_data_dir / "nonexistent-file").load()

    def test_junk_file(self):
        with self.assertRaises(ValueError):
            VernierReader(self.test_data_dir / "junk-data").load()

    def test_load_from_file(self):
        test_reader = VernierReader(self.test_data_dir / "vernier-output-test")
        loaded_data = test_reader.load()
        self.assertIn("__test_app__", loaded_data.data)
        self.assertIn("some_process", loaded_data.data)
        self.assertEqual(loaded_data.data["__test_app__"].n_calls, [1, 1])
        self.assertEqual(loaded_data.data["some_process"].n_calls, [2, 2])
        self.assertEqual(loaded_data.data["__test_app__"].rank, [0, 1])
        self.assertEqual(loaded_data.data["some_process"].rank, [0, 1])
        self.assertEqual(loaded_data.data["__test_app__"].thread, [0, 0])
        self.assertEqual(loaded_data.data["some_process"].thread, [0, 0])
        self.assertEqual(loaded_data.data["__test_app__"].time_percent, [44.130, 37.835])
        self.assertEqual(loaded_data.data["some_process"].time_percent, [34.563, 43.991])
        self.assertEqual(loaded_data.data["__test_app__"].cumul_time, [2.583, 5.596])
        self.assertEqual(loaded_data.data["some_process"].cumul_time, [4.606, 3.009])
        self.assertEqual(loaded_data.data["__test_app__"].self_time, [2.583, 2.588])
        self.assertEqual(loaded_data.data["some_process"].self_time, [2.023, 3.009])
        self.assertEqual(loaded_data.data["__test_app__"].total_time, [5.854, 6.839])
        self.assertEqual(loaded_data.data["some_process"].total_time,[2.077, 3.069])

    def test_load_from_directory(self):
        test_reader = VernierReader(self.test_data_dir / "vernier-output")
        loaded_data = test_reader.load()
        self.assertIn("__test_app__", loaded_data.data)
        self.assertIn("some_process", loaded_data.data)
        self.assertCountEqual(loaded_data.data["__test_app__"].n_calls, [1, 1])
        self.assertCountEqual(loaded_data.data["some_process"].n_calls, [2, 2])
        self.assertCountEqual(loaded_data.data["__test_app__"].rank, [0, 1])
        self.assertCountEqual(loaded_data.data["some_process"].rank, [0, 1])
        self.assertCountEqual(loaded_data.data["__test_app__"].thread, [0, 0])
        self.assertCountEqual(loaded_data.data["some_process"].thread, [0, 0])
        self.assertCountEqual(loaded_data.data["__test_app__"].time_percent, [44.130, 37.835])
        self.assertCountEqual(loaded_data.data["some_process"].time_percent, [34.563, 43.991])
        self.assertCountEqual(loaded_data.data["__test_app__"].cumul_time, [2.583, 5.596])
        self.assertCountEqual(loaded_data.data["some_process"].cumul_time, [4.606, 3.009])
        self.assertCountEqual(loaded_data.data["__test_app__"].self_time, [2.583, 2.588])
        self.assertCountEqual(loaded_data.data["some_process"].self_time, [2.023, 3.009])
        self.assertCountEqual(loaded_data.data["__test_app__"].total_time, [5.854, 6.839])
        self.assertCountEqual(loaded_data.data["some_process"].total_time,[2.077, 3.069])

    def test_load_threaded_data_from_directory(self):
        test_reader = VernierReader(self.test_data_dir / "vernier-output-threaded")
        loaded_data = test_reader.load()
        self.assertIn("FULL", loaded_data.data)
        self.assertIn("MAIN_SUB", loaded_data.data)
        self.assertIn("MAIN_SUB2", loaded_data.data)

        self.assertCountEqual(loaded_data.data["FULL"].n_calls, [1, 1])
        self.assertCountEqual(loaded_data.data["FULL"].time_percent, [40.0, 40.031])
        self.assertCountEqual(loaded_data.data["FULL"].self_time, [2.0, 2.004])
        self.assertCountEqual(loaded_data.data["FULL"].total_time, [5.0, 5.006])
        self.assertCountEqual(loaded_data.data["FULL"].cumul_time, [4.0, 2.004])
        self.assertCountEqual(loaded_data.data["FULL"].rank, [0, 1])
        self.assertCountEqual(loaded_data.data["FULL"].thread, [0, 0])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].n_calls, [2, 1, 1, 1, 2, 1, 1, 1])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].time_percent, [40.0, 20.0, 20.0, 20.0, 39.956, 19.978, 19.978, 19.977])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].self_time, [2.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].total_time, [3.0, 2.0, 2.0, 2.0, 3.002, 2.004, 2.002, 2.0])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].cumul_time, [2.0, 6.0, 10.001, 11.001, 4.004, 9.012, 10.012, 11.012])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].rank, [0, 0, 0, 0, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].thread, [0, 3, 2, 1, 0, 1, 3, 2])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].n_calls, [1, 1, 1, 1, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].time_percent, [20.0, 20.0, 20.0, 20.0, 20.056, 20.017, 20.012, 19.979])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].self_time, [1.0, 1.0, 1.0, 1.0, 1.004, 1.002, 1.002, 1.0])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].total_time, [1.0, 1.0, 1.0, 1.0, 1.004, 1.002, 1.002, 1.0])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].cumul_time, [5.0, 7.001, 8.001, 9.001, 5.008, 6.01, 7.012, 8.012])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].rank, [0, 0, 0, 0, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].thread, [3, 0, 2, 1, 1, 3, 0, 2])

if __name__ == '__main__':
    unittest.main()
