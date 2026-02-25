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
sys.path.append('../vernier')
from vernier.vernier_data import VernierData, aggregate

class TestVernierData(unittest.TestCase):
    """
    Tests for the VernierData class and aggregator function
    """
    def setUp(self):
        self.test_data = VernierData()

    def test_add_empty_caliper(self):
        self.test_data.add_caliper("test_caliper")
        self.assertIn("test_caliper", self.test_data.data)
        self.assertEqual(self.test_data.data["test_caliper"].time_percent, [])
        self.assertEqual(self.test_data.data["test_caliper"].cumul_time, [])
        self.assertEqual(self.test_data.data["test_caliper"].self_time, [])
        self.assertEqual(self.test_data.data["test_caliper"].total_time, [])
        self.assertEqual(self.test_data.data["test_caliper"].n_calls, [])

    def test_filter_caliper(self):
        self.test_data.add_caliper("timestep_caliper")
        self.test_data.add_caliper("other_caliper")
        filtered = self.test_data.filter(["timestep"])
        self.assertIn("timestep_caliper", filtered.data)
        self.assertNotIn("other_caliper", filtered.data)

    def test_filter_no_match(self):
        self.test_data.add_caliper("timestep_caliper")
        with self.assertRaises(ValueError):
            self.test_data.filter(["nonexistent"])

    def test_filter_multiple_matches(self):
        self.test_data.add_caliper("timestep_caliper_1")
        self.test_data.add_caliper("timestep_caliper_2")
        filtered = self.test_data.filter(["timestep"])
        self.assertIn("timestep_caliper_1", filtered.data)
        self.assertIn("timestep_caliper_2", filtered.data)

    def test_filter_empty_keys(self):
        self.test_data.add_caliper("timestep_caliper")
        with self.assertRaises(ValueError):
            self.test_data.filter([])

    def test_write_txt_output_file(self):
        self.test_data.add_caliper("test_caliper")
        self.test_data.data["test_caliper"].time_percent = [10.0, 20.0]
        self.test_data.data["test_caliper"].cumul_time = [30.0, 40.0]
        self.test_data.data["test_caliper"].self_time = [5.0, 15.0]
        self.test_data.data["test_caliper"].total_time = [25.0, 35.0]
        self.test_data.data["test_caliper"].n_calls = [2]

        with tempfile.NamedTemporaryFile(delete=False) as tmp_file:
            self.test_data.write_txt_output(Path(tmp_file.name))
            contents = Path(tmp_file.name).read_text().splitlines()
            self.assertEqual("|      Routine | Total time (s) |     Self (s) | Cumul time (s) | No. calls |   % time | Time per call (s) |", contents[0])
            self.assertEqual("| test_caliper |           30.0 |         10.0 |           35.0 |         2 |     15.0 |              15.0 |", contents[1])

    def test_write_txt_output_terminal(self):
        self.test_data.add_caliper("test_caliper")
        self.test_data.data["test_caliper"].time_percent = [50.0, 40.0]
        self.test_data.data["test_caliper"].cumul_time = [10.0, 12.0]
        self.test_data.data["test_caliper"].self_time = [3.0, 4.0]
        self.test_data.data["test_caliper"].total_time = [15.0, 55.0]
        self.test_data.data["test_caliper"].n_calls = [2]

        write_output = StringIO()
        sys.stdout = write_output
        self.test_data.write_txt_output()
        sys.stdout = sys.__stdout__

        self.assertEqual("|      Routine | Total time (s) |     Self (s) | Cumul time (s) | No. calls |   % time | Time per call (s) |", write_output.getvalue().splitlines()[0])
        self.assertEqual("| test_caliper |           35.0 |          3.5 |           11.0 |         2 |     45.0 |              17.5 |", write_output.getvalue().splitlines()[1])

    def test_aggregate(self):
        data1 = VernierData()
        data1.add_caliper("caliper_a")
        data1.data["caliper_a"].time_percent = [10.0, 20.0]
        data1.data["caliper_a"].cumul_time = [30.0, 40.0]
        data1.data["caliper_a"].self_time = [5.0, 15.0]
        data1.data["caliper_a"].total_time = [25.0, 35.0]
        data1.data["caliper_a"].n_calls = [2, 2]

        data2 = VernierData()
        data2.add_caliper("caliper_a")
        data2.data["caliper_a"].time_percent = [15.0, 25.0]
        data2.data["caliper_a"].cumul_time = [35.0, 45.0]
        data2.data["caliper_a"].self_time = [6.0, 16.0]
        data2.data["caliper_a"].total_time = [28.0, 38.0]
        data2.data["caliper_a"].n_calls = [3, 3]

        aggregated = aggregate([data1, data2])
        self.assertIn("caliper_a", aggregated.data)
        self.assertEqual(aggregated.data["caliper_a"].time_percent, [10.0, 20.0, 15.0, 25.0])
        self.assertEqual(aggregated.data["caliper_a"].cumul_time, [30.0, 40.0, 35.0, 45.0])
        self.assertEqual(aggregated.data["caliper_a"].self_time, [5.0, 15.0, 6.0, 16.0])
        self.assertEqual(aggregated.data["caliper_a"].total_time, [25.0, 35.0, 28.0, 38.0])
        self.assertEqual(aggregated.data["caliper_a"].n_calls, [2, 2, 3, 3])

    def test_aggregate_inconsistent(self):
        data1 = VernierData()
        data1.add_caliper("caliper_a")
        data1.data["caliper_a"].time_percent = [10.0, 20.0]
        data1.data["caliper_a"].cumul_time = [30.0, 40.0]
        data1.data["caliper_a"].self_time = [5.0, 15.0]
        data1.data["caliper_a"].total_time = [25.0, 35.0]
        data1.data["caliper_a"].n_calls = [2, 2]

        data2 = VernierData()
        data2.add_caliper("caliper_b")
        data2.data["caliper_b"].time_percent = [15.0, 25.0]
        data2.data["caliper_b"].cumul_time = [35.0, 45.0]
        data2.data["caliper_b"].self_time = [6.0, 16.0]
        data2.data["caliper_b"].total_time = [28.0, 38.0]
        data2.data["caliper_b"].n_calls = [3, 3]

        with self.assertRaises(ValueError):
            aggregate([data1, data2])

    def test_aggregate_inconsistent_ok(self):
        data1 = VernierData()
        data1.add_caliper("caliper_a")

        data2 = VernierData()
        data2.add_caliper("caliper_b")

        agg_data = aggregate([data1, data2], internal_consistency=False)
        self.assertIn("caliper_a", agg_data.data)
        self.assertIn("caliper_b", agg_data.data)

if __name__ == '__main__':
    unittest.main()
