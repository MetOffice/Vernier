# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
from pathlib import Path
from io import StringIO
import tempfile
import unittest
import sys
sys.path.append(str(Path(__file__).parent.parent))
from vernier import VernierData, VernierDataAggregation

class TestVernierData(unittest.TestCase):
    """
    Tests for the VernierData class and aggregator function
    """
    def setUp(self):
        self.test_data = VernierData()

    def test_add_empty_calliper(self):
        self.test_data.add_calliper("test_calliper")
        self.assertIn("test_calliper", self.test_data.data)
        self.assertEqual(self.test_data.data["test_calliper"].time_percent, [])
        self.assertEqual(self.test_data.data["test_calliper"].cumul_time, [])
        self.assertEqual(self.test_data.data["test_calliper"].self_time, [])
        self.assertEqual(self.test_data.data["test_calliper"].total_time, [])
        self.assertEqual(self.test_data.data["test_calliper"].n_calls, [])

    def test_filter_calliper(self):
        self.test_data.add_calliper("timestep_calliper")
        self.test_data.add_calliper("other_calliper")
        filtered = self.test_data.filter(["timestep"])
        self.assertIn("timestep_calliper", filtered.data)
        self.assertNotIn("other_calliper", filtered.data)

    def test_filter_no_match(self):
        self.test_data.add_calliper("timestep_calliper")
        with self.assertRaises(ValueError):
            self.test_data.filter(["nonexistent"])

    def test_filter_multiple_matches(self):
        self.test_data.add_calliper("timestep_calliper_1")
        self.test_data.add_calliper("timestep_calliper_2")
        filtered = self.test_data.filter(["timestep"])
        self.assertIn("timestep_calliper_1", filtered.data)
        self.assertIn("timestep_calliper_2", filtered.data)

    def test_filter_empty_keys(self):
        self.test_data.add_calliper("timestep_calliper")
        with self.assertRaises(ValueError):
            self.test_data.filter([])

    def test_write_txt_output_file(self):
        self.test_data.add_calliper("test_calliper")
        self.test_data.data["test_calliper"].time_percent = [10.0, 20.0]
        self.test_data.data["test_calliper"].cumul_time = [30.0, 40.0]
        self.test_data.data["test_calliper"].self_time = [5.0, 15.0]
        self.test_data.data["test_calliper"].total_time = [25.0, 35.0]
        self.test_data.data["test_calliper"].n_calls = [2]

        with tempfile.NamedTemporaryFile(delete=False) as tmp_file:
            self.test_data.write_txt_output(Path(tmp_file.name))
            contents = Path(tmp_file.name).read_text().splitlines()
            self.assertEqual("|       Routine | Total time (s) |     Self (s) | Cumul time (s) | No. calls |   % time | Time per call (s) |", contents[0])
            self.assertEqual("| test_calliper |           30.0 |         10.0 |           35.0 |         2 |     15.0 |              15.0 |", contents[1])

    def test_write_txt_output_terminal(self):
        self.test_data.add_calliper("test_calliper")
        self.test_data.data["test_calliper"].time_percent = [50.0, 40.0]
        self.test_data.data["test_calliper"].cumul_time = [10.0, 12.0]
        self.test_data.data["test_calliper"].self_time = [3.0, 4.0]
        self.test_data.data["test_calliper"].total_time = [15.0, 55.0]
        self.test_data.data["test_calliper"].n_calls = [2]

        write_output = StringIO()
        sys.stdout = write_output
        self.test_data.write_txt_output()
        sys.stdout = sys.__stdout__

        self.assertEqual("|       Routine | Total time (s) |     Self (s) | Cumul time (s) | No. calls |   % time | Time per call (s) |", write_output.getvalue().splitlines()[0])
        self.assertEqual("| test_calliper |           35.0 |          3.5 |           11.0 |         2 |     45.0 |              17.5 |", write_output.getvalue().splitlines()[1])

    def test_aggregate(self):
        data1 = VernierData()
        data1.add_calliper("calliper_a")
        data1.data["calliper_a"].time_percent = [10.0, 20.0]
        data1.data["calliper_a"].cumul_time = [30.0, 40.0]
        data1.data["calliper_a"].self_time = [5.0, 15.0]
        data1.data["calliper_a"].total_time = [25.0, 35.0]
        data1.data["calliper_a"].n_calls = [2, 2]

        data2 = VernierData()
        data2.add_calliper("calliper_a")
        data2.data["calliper_a"].time_percent = [15.0, 25.0]
        data2.data["calliper_a"].cumul_time = [35.0, 45.0]
        data2.data["calliper_a"].self_time = [6.0, 16.0]
        data2.data["calliper_a"].total_time = [28.0, 38.0]
        data2.data["calliper_a"].n_calls = [3, 3]

        aggregated = VernierData()
        aggregated.aggregate([data1, data2])
        self.assertIn("calliper_a", aggregated.data)
        self.assertEqual(aggregated.data["calliper_a"].time_percent, [10.0, 20.0, 15.0, 25.0])
        self.assertEqual(aggregated.data["calliper_a"].cumul_time, [30.0, 40.0, 35.0, 45.0])
        self.assertEqual(aggregated.data["calliper_a"].self_time, [5.0, 15.0, 6.0, 16.0])
        self.assertEqual(aggregated.data["calliper_a"].total_time, [25.0, 35.0, 28.0, 38.0])
        self.assertEqual(aggregated.data["calliper_a"].n_calls, [2, 2, 3, 3])

    def test_aggregate_inconsistent(self):
        data1 = VernierData()
        data1.add_calliper("calliper_a")
        data1.data["calliper_a"].time_percent = [10.0, 20.0]
        data1.data["calliper_a"].cumul_time = [30.0, 40.0]
        data1.data["calliper_a"].self_time = [5.0, 15.0]
        data1.data["calliper_a"].total_time = [25.0, 35.0]
        data1.data["calliper_a"].n_calls = [2, 2]

        data2 = VernierData()
        data2.add_calliper("calliper_b")
        data2.data["calliper_b"].time_percent = [15.0, 25.0]
        data2.data["calliper_b"].cumul_time = [35.0, 45.0]
        data2.data["calliper_b"].self_time = [6.0, 16.0]
        data2.data["calliper_b"].total_time = [28.0, 38.0]
        data2.data["calliper_b"].n_calls = [3, 3]

        with self.assertRaises(ValueError):
            aggregated = VernierData()
            aggregated.aggregate([data1, data2])

    def test_aggregate_inconsistent_ok(self):
        data1 = VernierData()
        data1.add_calliper("calliper_a")

        data2 = VernierData()
        data2.add_calliper("calliper_b")

        aggregated = VernierData()
        aggregated.aggregate([data1, data2], internal_consistency=False)
        self.assertIn("calliper_a", aggregated.data)
        self.assertIn("calliper_b", aggregated.data)

    def test_get(self):
        data1 = VernierData()
        data1.add_calliper("calliper_a")
        data1.data["calliper_a"].time_percent = [10.0, 20.0]
        data1.data["calliper_a"].cumul_time = [30.0, 40.0]
        data1.data["calliper_a"].self_time = [5.0, 15.0]
        data1.data["calliper_a"].total_time = [25.0, 35.0]
        data1.data["calliper_a"].n_calls = [2, 2]
        self.assertEqual(len(data1.get("calliper_a")), 2)


class TestVernierAggregation(unittest.TestCase):
    """
    Tests for the VernierData Aggregation class.
    """
    def _add_data(self):
        self.aggregation = VernierDataAggregation()
        data1 = VernierData()
        data1.add_calliper("calliper_a")
        data1.data["calliper_a"].time_percent = [10.0, 20.0]
        data1.data["calliper_a"].cumul_time = [30.0, 40.0]
        data1.data["calliper_a"].self_time = [5.0, 15.0]
        data1.data["calliper_a"].total_time = [25.0, 35.0]
        data1.data["calliper_a"].n_calls = [2, 2]

        data2 = VernierData()
        data2.add_calliper("calliper_a")
        data2.data["calliper_a"].time_percent = [15.0, 25.0]
        data2.data["calliper_a"].cumul_time = [35.0, 45.0]
        data2.data["calliper_a"].self_time = [6.0, 16.0]
        data2.data["calliper_a"].total_time = [28.0, 38.0]
        data2.data["calliper_a"].n_calls = [3, 3]

        self.aggregation.add_data('test1', data1)
        self.aggregation.add_data('test2', data2)
        
    def test_add_data(self):
        self._add_data()
        self.assertEqual(len(self.aggregation), 2)

    def test_remove_data(self):
        self._add_data()
        self.aggregation.remove_data('test1')
        self.assertEqual(len(self.aggregation), 1)

    def test_get(self):
        self._add_data()
        calliper_a = self.aggregation.get("calliper_a")
        self.assertEqual(len(calliper_a), 4)

    def test_internal_consistency(self):
        self._add_data()
        data_inc = VernierData()
        data_inc.add_calliper("calliper_a")
        data_inc.data["calliper_a"].time_percent = [10.0, 20.0]
        data_inc.data["calliper_a"].cumul_time = [30.0, 40.0]
        data_inc.data["calliper_a"].self_time = [5.0, 15.0]
        data_inc.data["calliper_a"].total_time = [25.0, 35.0]
        data_inc.data["calliper_a"].n_calls = [2, 2]

        data_inc.add_calliper("calliper_b")
        data_inc.data["calliper_b"].time_percent = [15.0, 25.0]
        data_inc.data["calliper_b"].cumul_time = [35.0, 45.0]
        data_inc.data["calliper_b"].self_time = [6.0, 16.0]
        data_inc.data["calliper_b"].total_time = [28.0, 38.0]
        data_inc.data["calliper_b"].n_calls = [3, 3]

        with self.assertRaises(ValueError) as test_exception:
            self.aggregation.add_data('test3', data_inc)
        self.assertEqual(str(test_exception.exception),
                         "inconsistent callipers in new_vernier_data")


if __name__ == '__main__':
    unittest.main()
