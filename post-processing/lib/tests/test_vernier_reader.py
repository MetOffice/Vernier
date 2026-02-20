from pathlib import Path
import unittest
import sys
sys.path.append('../vernier')
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
        self.assertEqual(loaded_data.data["__test_app__"].n_calls, [1, 1])
        self.assertEqual(loaded_data.data["some_process"].n_calls, [2, 2])
        self.assertEqual(loaded_data.data["__test_app__"].time_percent, [44.130, 37.835])
        self.assertEqual(loaded_data.data["some_process"].time_percent, [34.563, 43.991])
        self.assertEqual(loaded_data.data["__test_app__"].cumul_time, [2.583, 5.596])
        self.assertEqual(loaded_data.data["some_process"].cumul_time, [4.606, 3.009])
        self.assertEqual(loaded_data.data["__test_app__"].self_time, [2.583, 2.588])
        self.assertEqual(loaded_data.data["some_process"].self_time, [2.023, 3.009])
        self.assertEqual(loaded_data.data["__test_app__"].total_time, [5.854, 6.839])
        self.assertEqual(loaded_data.data["some_process"].total_time,[2.077, 3.069])


if __name__ == '__main__':
    unittest.main()
