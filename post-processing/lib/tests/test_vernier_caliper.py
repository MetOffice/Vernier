# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
import unittest
import sys
sys.path.append('../vernier')
from vernier.vernier_data import VernierCaliper

class TestVernierCaliper(unittest.TestCase):

    def setUp(self):
        self.caliper_a = VernierCaliper("test_caliper_a")
        self.caliper_b = VernierCaliper("test_caliper_b")

    def test_init(self):
        self.assertEqual(self.caliper_a.name, "test_caliper_a")
        self.assertEqual(self.caliper_a.time_percent, [])
        self.assertEqual(self.caliper_a.cumul_time, [])
        self.assertEqual(self.caliper_a.self_time, [])
        self.assertEqual(self.caliper_a.total_time, [])
        self.assertEqual(self.caliper_a.n_calls, [])

    def test_reduce(self):
        self.caliper_a.time_percent = [10.0, 20.0]
        self.caliper_a.cumul_time = [30.0, 40.0]
        self.caliper_a.self_time = [5.0, 15.0]
        self.caliper_a.total_time = [25.0, 35.0]
        self.caliper_a.n_calls = [2, 2]

        reduced_data = self.caliper_a.reduce()
        self.assertEqual(reduced_data[0], "test_caliper_a")
        self.assertEqual(reduced_data[1], 30.0)
        self.assertEqual(reduced_data[2], 10.0)
        self.assertEqual(reduced_data[3], 35.0)
        self.assertEqual(reduced_data[4], 2)
        self.assertEqual(reduced_data[5], 15.0)
        self.assertEqual(reduced_data[6], 15.0)

    def test_compare(self):
        self.caliper_a.time_percent = [10.0, 20.0]
        self.caliper_a.cumul_time = [30.0, 40.0]
        self.caliper_a.self_time = [5.0, 15.0]
        self.caliper_a.total_time = [25.0, 35.0]
        self.caliper_a.n_calls = [2, 2]

        self.caliper_b.time_percent = [12.0, 25.0]
        self.caliper_b.cumul_time = [35.0, 46.0]
        self.caliper_b.self_time = [6.0, 19.0]
        self.caliper_b.total_time = [28.0, 39.0]
        self.caliper_b.n_calls = [2, 2]

        self.assertTrue(self.caliper_a < self.caliper_b)
        self.assertFalse(self.caliper_a > self.caliper_b)
        self.assertFalse(self.caliper_a == self.caliper_b)

if __name__ == '__main__':
    unittest.main()
