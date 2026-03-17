# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
from pathlib import Path
import unittest
import numpy as np
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
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].n_calls.flatten(), np.array([1, 1])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].n_calls.flatten(), np.array([2, 2])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].time_percent.flatten(), np.array([44.130, 37.835])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].time_percent.flatten(), np.array([34.563, 43.991])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].cumul_time.flatten(), np.array([2.583, 5.596])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].cumul_time.flatten(), np.array([4.606, 3.009])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].self_time.flatten(), np.array([2.583, 2.588])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].self_time.flatten(), np.array([2.023, 3.009])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].total_time.flatten(), np.array([5.854, 6.839])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].total_time.flatten(), np.array([2.077, 3.069])))

    def test_load_from_directory(self):
        test_reader = VernierReader(self.test_data_dir / "vernier-output")
        loaded_data = test_reader.load()
        self.assertIn("__test_app__", loaded_data.data)
        self.assertIn("some_process", loaded_data.data)
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].n_calls.flatten(), np.array([1, 1])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].n_calls.flatten(), np.array([2, 2])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].time_percent.flatten(), np.array([44.130, 37.835])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].time_percent.flatten(), np.array([34.563, 43.991])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].cumul_time.flatten(), np.array([2.583, 5.596])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].cumul_time.flatten(), np.array([4.606, 3.009])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].self_time.flatten(), np.array([2.583, 2.588])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].self_time.flatten(), np.array([2.023, 3.009])))
        self.assertTrue(np.array_equal(loaded_data.data["__test_app__"].total_time.flatten(), np.array([5.854, 6.839])))
        self.assertTrue(np.array_equal(loaded_data.data["some_process"].total_time.flatten(), np.array([2.077, 3.069])))

    def test_load_threaded_data_from_directory(self):
        test_reader = VernierReader(self.test_data_dir / "vernier-output-threaded")
        loaded_data = test_reader.load()
        self.assertIn("FULL", loaded_data.data)
        self.assertIn("MAIN_SUB", loaded_data.data)
        self.assertIn("MAIN_SUB2", loaded_data.data)

        self.assertTrue(np.array_equal(loaded_data.data["FULL"].n_calls, np.array([[1], [1]])))
        self.assertTrue(np.array_equal(loaded_data.data["FULL"].time_percent, np.array([[40.0], [40.031]])))
        self.assertTrue(np.array_equal(loaded_data.data["FULL"].self_time, np.array([[2.0], [2.004]])))
        self.assertTrue(np.array_equal(loaded_data.data["FULL"].total_time, np.array([[5.0], [5.006]])))
        self.assertTrue(np.array_equal(loaded_data.data["FULL"].cumul_time, np.array([[4.0], [2.004]])))
        self.assertTrue(np.array_equal(loaded_data.data["MAIN_SUB"].n_calls, np.array([[2, 1, 1, 1], [2, 1, 1, 1]])))
        self.assertTrue(np.array_equal(loaded_data.data["MAIN_SUB"].time_percent, np.array([[40.0, 20.0, 20.0, 20.0], [39.956, 19.978, 19.977, 19.978]])))
        self.assertTrue(np.array_equal(loaded_data.data["MAIN_SUB"].self_time, np.array([[2.0, 1.0, 1.0, 1.0], [2.0, 1.0, 1.0, 1.0]])))
        self.assertTrue(np.array_equal(loaded_data.data["MAIN_SUB"].total_time, np.array([[3.0, 2.0, 2.0, 2.0], [3.002, 2.004, 2.0, 2.002]])))
        self.assertTrue(np.array_equal(loaded_data.data["MAIN_SUB"].cumul_time, np.array([[2.0, 11.001, 10.001, 6.0], [4.004, 9.012, 11.012, 10.012]])))
        self.assertTrue(np.array_equal(loaded_data.data["MAIN_SUB2"].n_calls, np.array([[1, 1, 1, 1], [1, 1, 1, 1]])))
        self.assertTrue(np.array_equal(loaded_data.data["MAIN_SUB2"].time_percent, np.array([[20.0, 20.0, 20.0, 20.0], [20.012, 20.056, 19.979, 20.017]])))
        self.assertTrue(np.array_equal(loaded_data.data["MAIN_SUB2"].self_time, np.array([[1.0, 1.0, 1.0, 1.0], [1.002, 1.004, 1.0, 1.002]])))
        self.assertTrue(np.array_equal(loaded_data.data["MAIN_SUB2"].total_time, np.array([[1.0, 1.0, 1.0, 1.0], [1.002, 1.004, 1.0, 1.002]])))
        self.assertTrue(np.array_equal(loaded_data.data["MAIN_SUB2"].cumul_time, np.array([[7.001, 9.001, 8.001, 5.0], [7.012, 5.008, 8.012, 6.01]])))

if __name__ == '__main__':
    unittest.main()
