# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
"""
Module for testing the VernierCalliper class.
"""

import unittest
from pathlib import Path
import sys

# pylint: disable=wrong-import-position
sys.path.append(str(Path(__file__).parent.parent))
from vernier.vernier_data import VernierCalliper


class TestVernierCalliper(unittest.TestCase):
    """
    Unittest class for holding tests related to the VernierCalliper class.
    """
    def setUp(self):
        """
        Initialise useful attributes for testing.
        """
        self.calliper_a = VernierCalliper("test_calliper_a")
        self.calliper_b = VernierCalliper("test_calliper_b")

    def test_init(self):
        """
        Test that the class initialises as expected (empty lists)
        """
        self.assertEqual(self.calliper_a.name, "test_calliper_a")
        self.assertEqual(self.calliper_a.time_percent, [])
        self.assertEqual(self.calliper_a.cumul_time, [])
        self.assertEqual(self.calliper_a.self_time, [])
        self.assertEqual(self.calliper_a.total_time, [])
        self.assertEqual(self.calliper_a.n_calls, [])

    def test_reduce(self):
        """
        Test that the reduce() method returns the expected results.
        """
        # pylint: disable=pointless-statement
        self.calliper_a.time_percent = [10.0, 20.0]
        self.calliper_a.cumul_time = [30.0, 40.0]
        self.calliper_a.self_time = [5.0, 15.0]
        self.calliper_a.total_time = [25.0, 35.0]
        self.calliper_a.n_calls = [2, 2]

        reduced_data = self.calliper_a.reduce()
        self.assertEqual(reduced_data[0], "test_calliper_a")
        self.assertEqual(reduced_data[1], 30.0)
        self.assertEqual(reduced_data[2], 10.0)
        self.assertEqual(reduced_data[3], 35.0)
        self.assertEqual(reduced_data[4], 2)
        self.assertEqual(reduced_data[5], 15.0)
        self.assertEqual(reduced_data[6], 15.0)

    def test_compare(self):
        """
        Test that the comparison of dataclasses works as expected for the
        VernierCalliper class.
        """
        # pylint: disable=pointless-statement
        self.calliper_a.time_percent = [10.0, 20.0]
        self.calliper_a.cumul_time = [30.0, 40.0]
        self.calliper_a.self_time = [5.0, 15.0]
        self.calliper_a.total_time = [25.0, 35.0]
        self.calliper_a.n_calls = [2, 2]

        self.calliper_b.time_percent = [12.0, 25.0]
        self.calliper_b.cumul_time = [35.0, 46.0]
        self.calliper_b.self_time = [6.0, 19.0]
        self.calliper_b.total_time = [28.0, 39.0]
        self.calliper_b.n_calls = [2, 2]

        self.assertTrue(self.calliper_a < self.calliper_b)
        self.assertFalse(self.calliper_a > self.calliper_b)
        self.assertFalse(self.calliper_a == self.calliper_b)


if __name__ == '__main__':
    unittest.main()
