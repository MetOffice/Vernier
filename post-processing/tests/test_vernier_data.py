# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
"""
Module to hold tests for VernierData class and aggregate function.
"""
from pathlib import Path
from io import StringIO
import numpy as np
import tempfile
import unittest
import sys

# pylint: disable=wrong-import-position
sys.path.append(str(Path(__file__).parent.parent))
from vernier import VernierData, VernierDataCollation


class TestVernierData(unittest.TestCase):
    """
    Tests for the VernierData class and aggregator function.
    """
    def setUp(self):
        """
        Initialise useful attributes for testing.
        """
        self.test_data = VernierData()

    def test_add_empty_calliper(self):
        """
        Tests that a VernierCalliper instance is correctly added to the
        VernierData instance.
        """
        self.test_data.add_calliper("test_calliper", 1, 1)
        self.assertIn("test_calliper", self.test_data.data)

    def test_filter_calliper(self):
        """
        Tests that callipers that do not match the pattern pased to
        `VernierData.filter()` are not returned.
        """
        self.test_data.add_calliper("timestep_calliper", 1, 1)
        self.test_data.add_calliper("other_calliper", 1, 1)
        filtered = self.test_data.filter(["timestep"])
        self.assertIn("timestep_calliper", filtered.data)
        self.assertNotIn("other_calliper", filtered.data)

    def test_filter_no_match(self):
        """
        Test that an error is raised when no callipers are found when calling
        the `VernierData.filter()` method.
        """
        self.test_data.add_calliper("timestep_calliper", 1, 1)
        with self.assertRaises(ValueError):
            self.test_data.filter(["nonexistent"])

    def test_filter_multiple_matches(self):
        """
        Tests that filter correctly returns all callipers with a substring
        equal to the pattern passed to `VernierData.filter()`
        """
        self.test_data.add_calliper("timestep_calliper_1", 1, 1)
        self.test_data.add_calliper("timestep_calliper_2", 1, 1)
        filtered = self.test_data.filter(["timestep"])
        self.assertIn("timestep_calliper_1", filtered.data)
        self.assertIn("timestep_calliper_2", filtered.data)

    def test_filter_empty_keys(self):
        """
        Test that an error is raised when no filters are passed to the
        `VernierData.filter()` method.
        """
        self.test_data.add_calliper("timestep_calliper", 1, 1)
        with self.assertRaises(ValueError):
            self.test_data.filter([])

    def test_write_txt_output_file(self):
        """
        Test that the formatting of write_txt_ouput is as expected when writing
        to a file.
        """
        self.test_data.add_calliper("test_calliper", 2, 1)
        self.test_data.data["test_calliper"].time_percent = np.array([10.0, 20.0])
        self.test_data.data["test_calliper"].cumul_time = np.array([30.0, 40.0])
        self.test_data.data["test_calliper"].self_time = np.array([5.0, 15.0])
        self.test_data.data["test_calliper"].total_time = np.array([25.0, 35.0])
        self.test_data.data["test_calliper"].n_calls = np.array([2, 2])

        # pylint: disable=unspecified-encoding
        with tempfile.NamedTemporaryFile(delete=False) as tmp_file:
            self.test_data.write_txt_output(Path(tmp_file.name))
            contents = Path(tmp_file.name).read_text().splitlines()
            # pylint: disable=line-too-long
            self.assertEqual("|       Routine | Total time (s) |     Self (s) | Cumul time (s) | Max no. calls |   % time | Time per call (s) |", contents[0])
            self.assertEqual("| test_calliper |           30.0 |         10.0 |           35.0 |             2 |     15.0 |              15.0 |", contents[1])

    def test_write_txt_output_terminal(self):
        """
        Test that the formatting of write_txt_ouput is as expected when writing
        to the terminal.
        """
        self.test_data.add_calliper("test_calliper", 2, 1)
        self.test_data.data["test_calliper"].time_percent = np.array([50.0, 40.0])
        self.test_data.data["test_calliper"].cumul_time = np.array([10.0, 12.0])
        self.test_data.data["test_calliper"].self_time = np.array([3.0, 4.0])
        self.test_data.data["test_calliper"].total_time = np.array([15.0, 55.0])
        self.test_data.data["test_calliper"].n_calls = np.array([2, 2])

        write_output = StringIO()
        sys.stdout = write_output
        self.test_data.write_txt_output()
        sys.stdout = sys.__stdout__

        # pylint: disable=line-too-long
        self.assertEqual("|       Routine | Total time (s) |     Self (s) | Cumul time (s) | Max no. calls |   % time | Time per call (s) |", write_output.getvalue().splitlines()[0])
        self.assertEqual("| test_calliper |           35.0 |          3.5 |           11.0 |             2 |     45.0 |              17.5 |", write_output.getvalue().splitlines()[1])


    def test_get(self):
        """
        Test that the get method of the VernierData class works as expected.
        """
        data1 = VernierData()
        data1.add_calliper("calliper_a", 2, 1)
        data1.data["calliper_a"].time_percent = np.array([10.0, 20.0])
        data1.data["calliper_a"].cumul_time = np.array([30.0, 40.0])
        data1.data["calliper_a"].self_time = np.array([5.0, 15.0])
        data1.data["calliper_a"].total_time = np.array([25.0, 35.0])
        data1.data["calliper_a"].n_calls = np.array([2, 2])
        self.assertEqual(len(data1.get("calliper_a")), 2)


class TestVernierCollation(unittest.TestCase):
    """
    Tests for the VernierData Collation class.
    """
    def _add_data(self):
        """
        Setup for testing the VernierCollation object.
        """
        self.collation = VernierDataCollation()
        data1 = VernierData()
        data1.add_calliper("calliper_a", 2, 1)
        data1.data["calliper_a"].time_percent = np.array([[10.0], [20.0]])
        data1.data["calliper_a"].cumul_time = np.array([[30.0], [40.0]])
        data1.data["calliper_a"].self_time = np.array([[5.0], [15.0]])
        data1.data["calliper_a"].total_time = np.array([[25.0], [35.0]])
        data1.data["calliper_a"].n_calls = np.array([[2], [2]])

        data2 = VernierData()
        data2.add_calliper("calliper_a", 2, 1)
        data2.data["calliper_a"].time_percent = np.array([[15.0], [25.0]])
        data2.data["calliper_a"].cumul_time = np.array([[35.0], [45.0]])
        data2.data["calliper_a"].self_time = np.array([[6.0], [16.0]])
        data2.data["calliper_a"].total_time = np.array([[28.0], [38.0]])
        data2.data["calliper_a"].n_calls = np.array([[3], [3]])

        self.collation.add_data('test1', data1)
        self.collation.add_data('test2', data2)

    def test_add_data(self):
        """
        Test that the add_data method adds VernierData objects to the collation
        attribute.
        """
        self._add_data()
        self.assertEqual(len(self.collation), 2)

    def test_remove_data(self):
        """
        Test that the remove_data method drops the correct VernierData object
        from the collation attribute.
        """
        self._add_data()
        self.collation.remove_data('test1')
        self.assertEqual(len(self.collation), 1)

    def test_get(self):
        """
        Test that the get method of VernierCollation returns the expected
        VernierData instance.
        """
        self._add_data()
        calliper_a = self.collation.get("calliper_a")
        self.assertEqual(len(calliper_a), 4)

    def test_internal_consistency(self):
        """
        Test that the internal_consistency method of VernierCollation returns
        the expected error type and message.
        """
        self._add_data()
        data_inc = VernierData()
        data_inc.add_calliper("calliper_a", 2, 1)
        data_inc.data["calliper_a"].time_percent = np.array([[10.0], [20.0]])
        data_inc.data["calliper_a"].cumul_time = np.array([[30.0], [40.0]])
        data_inc.data["calliper_a"].self_time = np.array([[5.0], [15.0]])
        data_inc.data["calliper_a"].total_time = np.array([[25.0], [35.0]])
        data_inc.data["calliper_a"].n_calls = np.array([[2], [2]])

        data_inc.add_calliper("calliper_b", 2, 1)
        data_inc.data["calliper_b"].time_percent = np.array([[15.0], [25.0]])
        data_inc.data["calliper_b"].cumul_time = np.array([[35.0], [45.0]])
        data_inc.data["calliper_b"].self_time = np.array([[6.0], [16.0]])
        data_inc.data["calliper_b"].total_time = np.array([[28.0], [38.0]])
        data_inc.data["calliper_b"].n_calls = np.array([[3], [3]])

        with self.assertRaises(ValueError) as test_exception:
            self.collation.add_data('test3', data_inc)
        self.assertEqual(str(test_exception.exception),
                         "Inconsistent callipers in new_vernier_data: "
                         "['calliper_a', 'calliper_b'] detected as unmatched")


if __name__ == '__main__':
    unittest.main()
