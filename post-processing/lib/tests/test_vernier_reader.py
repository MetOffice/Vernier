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
        test_data = Path(__file__).parent / "data/vernier-output-test"
        self.test_reader = VernierReader(test_data)

    def test_nonexistent_file(self):
        with self.assertRaises(ValueError):
            VernierReader(Path(__file__).parent / "data/nonexistent-file").load()

    def test_load_from_file(self):
        loaded_data = self.test_reader.load()
        self.assertIn("__test_app__", loaded_data.data)
        self.assertIn("some_process", loaded_data.data)
        self.assertEqual(loaded_data.data["__test_app__"]["n_calls"], [1])
        self.assertEqual(loaded_data.data["some_process"]["n_calls"], [2])
        self.assertEqual(loaded_data.data["__test_app__"]["%time"], [44.130, 37.835])
        self.assertEqual(loaded_data.data["some_process"]["%time"], [34.563, 43.991])
        self.assertEqual(loaded_data.data["__test_app__"]["cumul"], [2.583, 5.596])
        self.assertEqual(loaded_data.data["some_process"]["cumul"], [4.606, 3.009])
        self.assertEqual(loaded_data.data["__test_app__"]["self"], [2.583, 2.588])
        self.assertEqual(loaded_data.data["some_process"]["self"], [2.023, 3.009])
        self.assertEqual(loaded_data.data["__test_app__"]["total"], [5.854, 6.839])
        self.assertEqual(loaded_data.data["some_process"]["total"],[2.077, 3.069])

    def test_load_from_directory(self):
        with self.assertRaises(NotImplementedError):
            VernierReader(Path(__file__).parent / "data").load()


if __name__ == '__main__':
    unittest.main()