# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
import unittest
import numpy as np
from pathlib import Path
import sys
sys.path.append(str(Path(__file__).parent.parent))
from vernier.vernier_data import VernierCalliper

class TestVernierCalliper(unittest.TestCase):

    def setUp(self):
        self.calliper_a = VernierCalliper("test_calliper_a", 2, 1)
        self.calliper_b = VernierCalliper("test_calliper_b", 2, 1)

    def test_init(self):
        self.assertEqual(self.calliper_a.name, "test_calliper_a")
        self.assertTrue(np.array_equal(self.calliper_a.time_percent.flatten(), np.array([0.0, 0.0])))
        self.assertTrue(np.array_equal(self.calliper_a.cumul_time.flatten(), np.array([0.0, 0.0])))
        self.assertTrue(np.array_equal(self.calliper_a.self_time.flatten(), np.array([0.0, 0.0])))
        self.assertTrue(np.array_equal(self.calliper_a.total_time.flatten(), np.array([0.0, 0.0])))
        self.assertTrue(np.array_equal(self.calliper_a.n_calls.flatten(), np.array([0.0, 0.0])))

    def test_reduce(self):
        self.calliper_a.time_percent = np.array([10.0, 20.0])
        self.calliper_a.cumul_time = np.array([30.0, 40.0])
        self.calliper_a.self_time = np.array([5.0, 15.0])
        self.calliper_a.total_time = np.array([25.0, 35.0])
        self.calliper_a.n_calls = np.array([2, 2])

        reduced_data = self.calliper_a.reduce()
        self.assertEqual(reduced_data[0], "test_calliper_a")
        self.assertEqual(reduced_data[1], 30.0)
        self.assertEqual(reduced_data[2], 10.0)
        self.assertEqual(reduced_data[3], 35.0)
        self.assertEqual(reduced_data[4], 2)
        self.assertEqual(reduced_data[5], 15.0)
        self.assertEqual(reduced_data[6], 15.0)

    def test_compare(self):
        self.calliper_a.time_percent = np.array([10.0, 20.0])
        self.calliper_a.cumul_time = np.array([30.0, 40.0])
        self.calliper_a.self_time = np.array([5.0, 15.0])
        self.calliper_a.total_time = np.array([25.0, 35.0])
        self.calliper_a.n_calls = np.array([2, 2])

        self.calliper_b.time_percent = np.array([12.0, 25.0])
        self.calliper_b.cumul_time = np.array([35.0, 46.0])
        self.calliper_b.self_time = np.array([6.0, 19.0])
        self.calliper_b.total_time = np.array([28.0, 39.0])
        self.calliper_b.n_calls = np.array([2, 2])

        self.assertTrue(self.calliper_a < self.calliper_b)
        self.assertFalse(self.calliper_a > self.calliper_b)
        self.assertFalse(self.calliper_a == self.calliper_b)

if __name__ == '__main__':
    unittest.main()
