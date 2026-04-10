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
        self.test_data.add_calliper("test_calliper")
        self.assertIn("test_calliper", self.test_data.data)
        self.assertEqual(self.test_data.data["test_calliper"].time_percent, [])
        self.assertEqual(self.test_data.data["test_calliper"].self_time, [])
        self.assertEqual(self.test_data.data["test_calliper"].total_time, [])
        self.assertEqual(self.test_data.data["test_calliper"].n_calls, [])
        self.assertEqual(self.test_data.data["test_calliper"].rank, [])
        self.assertEqual(self.test_data.data["test_calliper"].thread, [])

    def test_get(self):
        """
        Tests that the get method of VernierData returns the expected data.
        """
        self.test_data.add_calliper("test_calliper")
        self.test_data.data["test_calliper"].time_percent = [10.0, 20.0]
        self.test_data.data["test_calliper"].self_time = [5.0, 15.0]
        self.test_data.data["test_calliper"].total_time = [25.0, 35.0]
        self.test_data.data["test_calliper"].n_calls = [2, 2]
        self.test_data.data["test_calliper"].rank = [0, 1]
        self.test_data.data["test_calliper"].thread = [0, 0]
        calliper_data = self.test_data.get("test_calliper")
        self.assertEqual(calliper_data.time_percent, [10.0, 20.0])
        self.assertEqual(calliper_data.self_time, [5.0, 15.0])
        self.assertEqual(calliper_data.total_time, [25.0, 35.0])
        self.assertEqual(calliper_data.n_calls, [2, 2])
        self.assertEqual(calliper_data.rank, [0, 1])
        self.assertEqual(calliper_data.thread, [0, 0])

    def test_get_rank(self):
        """
        Tests getter for data from a single rank
        """
        self.test_data.add_calliper("test_calliper")
        self.test_data.data["test_calliper"].time_percent = [10.0, 20.0]
        self.test_data.data["test_calliper"].self_time = [5.0, 15.0]
        self.test_data.data["test_calliper"].total_time = [25.0, 35.0]
        self.test_data.data["test_calliper"].n_calls = [2, 2]
        self.test_data.data["test_calliper"].rank = [0, 1]
        self.test_data.data["test_calliper"].thread = [0, 0]
        calliper_data = self.test_data.get("test_calliper", rank=1)
        self.assertEqual(calliper_data.time_percent, [20.0])
        self.assertEqual(calliper_data.self_time, [15.0])
        self.assertEqual(calliper_data.total_time, [35.0])
        self.assertEqual(calliper_data.n_calls, [2])
        self.assertEqual(calliper_data.rank, [1])
        self.assertEqual(calliper_data.thread, [0])

    def test_get_thread(self):
        """
        Tests getter for data from a single thread
        """
        self.test_data.add_calliper("test_calliper")
        self.test_data.data["test_calliper"].time_percent = [10.0, 20.0]
        self.test_data.data["test_calliper"].self_time = [5.0, 15.0]
        self.test_data.data["test_calliper"].total_time = [25.0, 35.0]
        self.test_data.data["test_calliper"].n_calls = [2, 2]
        self.test_data.data["test_calliper"].rank = [0, 0]
        self.test_data.data["test_calliper"].thread = [0, 1]
        calliper_data = self.test_data.get("test_calliper", thread=1)
        self.assertEqual(calliper_data.time_percent, [20.0])
        self.assertEqual(calliper_data.self_time, [15.0])
        self.assertEqual(calliper_data.total_time, [35.0])
        self.assertEqual(calliper_data.n_calls, [2])
        self.assertEqual(calliper_data.rank, [0])
        self.assertEqual(calliper_data.thread, [1])

    def test_get_rank_and_thread(self):
        self.test_data.add_calliper("test_calliper")
        self.test_data.data["test_calliper"].time_percent = [10.0, 20.0, 10.5, 20.5]
        self.test_data.data["test_calliper"].self_time = [5.0, 15.0, 5.5, 15.5]
        self.test_data.data["test_calliper"].total_time = [25.0, 35.0, 25.5, 35.5]
        self.test_data.data["test_calliper"].n_calls = [2, 2, 2, 2]
        self.test_data.data["test_calliper"].rank = [0, 0, 1, 1]
        self.test_data.data["test_calliper"].thread = [0, 1, 0, 1]
        calliper_data = self.test_data.get("test_calliper", thread=1, rank=1)
        self.assertEqual(calliper_data.time_percent, [20.5])
        self.assertEqual(calliper_data.self_time, [15.5])
        self.assertEqual(calliper_data.total_time, [35.5])
        self.assertEqual(calliper_data.n_calls, [2])
        self.assertEqual(calliper_data.rank, [1])
        self.assertEqual(calliper_data.thread, [1])

    def test_filter_calliper(self):
        """
        Tests that callipers that do not match the pattern pased to
        `VernierData.filter()` are not returned.
        """
        self.test_data.add_calliper("timestep_calliper")
        self.test_data.add_calliper("other_calliper")
        filtered = self.test_data.filter(["timestep"])
        self.assertIn("timestep_calliper", filtered.data)
        self.assertNotIn("other_calliper", filtered.data)

    def test_filter_no_match(self):
        """
        Test that an error is raised when no callipers are found when calling
        the `VernierData.filter()` method.
        """
        self.test_data.add_calliper("timestep_calliper")
        with self.assertRaises(ValueError):
            self.test_data.filter(["nonexistent"])

    def test_filter_multiple_matches(self):
        """
        Tests that filter correctly returns all callipers with a substring
        equal to the pattern passed to `VernierData.filter()`
        """
        self.test_data.add_calliper("timestep_calliper_1")
        self.test_data.add_calliper("timestep_calliper_2")
        filtered = self.test_data.filter(["timestep"])
        self.assertIn("timestep_calliper_1", filtered.data)
        self.assertIn("timestep_calliper_2", filtered.data)

    def test_filter_empty_keys(self):
        """
        Test that an error is raised when no filters are passed to the
        `VernierData.filter()` method.
        """
        self.test_data.add_calliper("timestep_calliper")
        with self.assertRaises(ValueError):
            self.test_data.filter([])

    def test_write_txt_output_file(self):
        """
        Test that the formatting of write_txt_ouput is as expected when writing
        to a file.
        """
        self.test_data.add_calliper("test_calliper")
        self.test_data.data["test_calliper"].time_percent = [10.0, 20.0]
        self.test_data.data["test_calliper"].self_time = [5.0, 15.0]
        self.test_data.data["test_calliper"].total_time = [25.0, 35.0]
        self.test_data.data["test_calliper"].n_calls = [2]
        self.test_data.data["test_calliper"].rank = [0, 1]
        self.test_data.data["test_calliper"].thread = [0, 0]

        # pylint: disable=unspecified-encoding
        with tempfile.NamedTemporaryFile(delete=False) as tmp_file:
            self.test_data.write_txt_output(Path(tmp_file.name))
            contents = Path(tmp_file.name).read_text().splitlines()
            # pylint: disable=line-too-long
            self.assertEqual("|       Routine | Total time (s) |     Self (s) | Max no. calls |   % time | Time per call (s) |", contents[0])
            self.assertEqual("| test_calliper |           30.0 |         10.0 |             2 |     15.0 |              15.0 |", contents[1])

    def test_write_txt_output_terminal(self):
        """
        Test that the formatting of write_txt_ouput is as expected when writing
        to the terminal.
        """
        self.test_data.add_calliper("test_calliper")
        self.test_data.data["test_calliper"].time_percent = [50.0, 40.0]
        self.test_data.data["test_calliper"].self_time = [3.0, 4.0]
        self.test_data.data["test_calliper"].total_time = [15.0, 55.0]
        self.test_data.data["test_calliper"].n_calls = [2]
        self.test_data.data["test_calliper"].rank = [0, 1]
        self.test_data.data["test_calliper"].thread = [0, 0]

        write_output = StringIO()
        sys.stdout = write_output
        self.test_data.write_txt_output()
        sys.stdout = sys.__stdout__

        # pylint: disable=line-too-long
        self.assertEqual("|       Routine | Total time (s) |     Self (s) | Max no. calls |   % time | Time per call (s) |", write_output.getvalue().splitlines()[0])
        self.assertEqual("| test_calliper |           35.0 |          3.5 |             2 |     45.0 |              17.5 |", write_output.getvalue().splitlines()[1])

    def test_aggregate(self):
        """
        Tests that aggregate returns a single VernierData object with all
        expected callipers and data.
        """
        data1 = VernierData()
        data1.add_calliper("calliper_a")
        data1.data["calliper_a"].time_percent = [10.0, 20.0]
        data1.data["calliper_a"].self_time = [5.0, 15.0]
        data1.data["calliper_a"].total_time = [25.0, 35.0]
        data1.data["calliper_a"].n_calls = [2, 2]
        data1.data["calliper_a"].rank = [0, 1]
        data1.data["calliper_a"].thread = [0, 0]

        data2 = VernierData()
        data2.add_calliper("calliper_a")
        data2.data["calliper_a"].time_percent = [15.0, 25.0]
        data2.data["calliper_a"].self_time = [6.0, 16.0]
        data2.data["calliper_a"].total_time = [28.0, 38.0]
        data2.data["calliper_a"].n_calls = [3, 3]
        data2.data["calliper_a"].rank = [0, 1]
        data2.data["calliper_a"].thread = [0, 0]

        aggregated = VernierData()
        aggregated.aggregate([data1, data2])
        self.assertIn("calliper_a", aggregated.data)
        self.assertEqual(
            aggregated.data["calliper_a"].time_percent,
            [10.0, 20.0, 15.0, 25.0])
        self.assertEqual(
            aggregated.data["calliper_a"].self_time,
            [5.0, 15.0, 6.0, 16.0])
        self.assertEqual(
            aggregated.data["calliper_a"].total_time,
            [25.0, 35.0, 28.0, 38.0])
        self.assertEqual(
            aggregated.data["calliper_a"].n_calls,
            [2, 2, 3, 3])

    def test_aggregate_inconsistent(self):
        """
        Tests that an error is raised when VernierData objects with mismatched
        callipers are passed to aggregate with internal consistency constraints
        turned on.
        """
        data1 = VernierData()
        data1.add_calliper("calliper_a")
        data1.data["calliper_a"].time_percent = [10.0, 20.0]
        data1.data["calliper_a"].self_time = [5.0, 15.0]
        data1.data["calliper_a"].total_time = [25.0, 35.0]
        data1.data["calliper_a"].n_calls = [2, 2]
        data1.data["calliper_a"].rank = [0, 1]
        data1.data["calliper_a"].thread = [0, 0]

        data2 = VernierData()
        data2.add_calliper("calliper_b")
        data2.data["calliper_b"].time_percent = [15.0, 25.0]
        data2.data["calliper_b"].self_time = [6.0, 16.0]
        data2.data["calliper_b"].total_time = [28.0, 38.0]
        data2.data["calliper_b"].n_calls = [3, 3]
        data2.data["calliper_b"].rank = [0, 1]
        data2.data["calliper_b"].thread = [0, 0]

        with self.assertRaises(ValueError):
            aggregated = VernierData()
            aggregated.aggregate([data1, data2])

    def test_aggregate_inconsistent_ok(self):
        """
        Test that combining VernierData objects without forced consistency
        of callipers works as expected (both callipers present in output).
        """
        data1 = VernierData()
        data1.add_calliper("calliper_a")

        data2 = VernierData()
        data2.add_calliper("calliper_b")

        aggregated = VernierData()
        aggregated.aggregate([data1, data2], internal_consistency=False)
        self.assertIn("calliper_a", aggregated.data)
        self.assertIn("calliper_b", aggregated.data)


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
        data1.add_calliper("calliper_a")
        data1.data["calliper_a"].time_percent = [10.0, 20.0]
        data1.data["calliper_a"].self_time = [5.0, 15.0]
        data1.data["calliper_a"].total_time = [25.0, 35.0]
        data1.data["calliper_a"].n_calls = [2, 2]
        data1.data["calliper_a"].rank = [0, 1]
        data1.data["calliper_a"].thread = [0, 0]

        data2 = VernierData()
        data2.add_calliper("calliper_a")
        data2.data["calliper_a"].time_percent = [15.0, 25.0]
        data2.data["calliper_a"].self_time = [6.0, 16.0]
        data2.data["calliper_a"].total_time = [28.0, 38.0]
        data2.data["calliper_a"].n_calls = [3, 3]
        data2.data["calliper_a"].rank = [0, 1]
        data2.data["calliper_a"].thread = [0, 0]

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

    def test_get__collation(self):
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
        data_inc.add_calliper("calliper_a")
        data_inc.data["calliper_a"].time_percent = [10.0, 20.0]
        data_inc.data["calliper_a"].self_time = [5.0, 15.0]
        data_inc.data["calliper_a"].total_time = [25.0, 35.0]
        data_inc.data["calliper_a"].n_calls = [2, 2]
        data_inc.data["calliper_a"].rank = [0, 1]
        data_inc.data["calliper_a"].thread = [0, 0]

        data_inc.add_calliper("calliper_b")
        data_inc.data["calliper_b"].time_percent = [15.0, 25.0]
        data_inc.data["calliper_b"].self_time = [6.0, 16.0]
        data_inc.data["calliper_b"].total_time = [28.0, 38.0]
        data_inc.data["calliper_b"].n_calls = [3, 3]
        data_inc.data["calliper_b"].rank = [0, 1]
        data_inc.data["calliper_b"].thread = [0, 0]

        with self.assertRaises(ValueError) as test_exception:
            self.collation.add_data('test3', data_inc)
        self.assertEqual(str(test_exception.exception),
                         "Inconsistent callipers in new_vernier_data: "
                         "['calliper_a', 'calliper_b'] detected as unmatched")


if __name__ == '__main__':
    unittest.main()
