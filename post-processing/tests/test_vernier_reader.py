# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
from pathlib import Path
import unittest
import numpy as np
import sys
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
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].n_calls, np.array([1, 1])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].n_calls, np.array([2, 2])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].time_percent, np.array([44.130, 37.835])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].time_percent, np.array([34.563, 43.991])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].cumul_time, np.array([2.583, 5.596])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].cumul_time, np.array([4.606, 3.009])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].self_time, np.array([2.583, 2.588])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].self_time, np.array([2.023, 3.009])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].total_time, np.array([5.854, 6.839])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].total_time, np.array([2.077, 3.069])))

    def test_load_from_directory(self):
        test_reader = VernierReader(self.test_data_dir / "vernier-output")
        loaded_data = test_reader.load()
        self.assertIn("__test_app__", loaded_data.data)
        self.assertIn("some_process", loaded_data.data)
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].n_calls, np.array([1, 1])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].n_calls, np.array([2, 2])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].time_percent, np.array([44.130, 37.835])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].time_percent, np.array([34.563, 43.991])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].cumul_time, np.array([2.583, 5.596])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].cumul_time, np.array([4.606, 3.009])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].self_time, np.array([2.583, 2.588])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].self_time, np.array([2.023, 3.009])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].total_time, np.array([5.854, 6.839])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].total_time, np.array([2.077, 3.069])))


if __name__ == '__main__':
    unittest.main()
