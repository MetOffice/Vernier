# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
from pathlib import Path
import unittest
import sys

# pylint: disable=wrong-import-position
sys.path.append(str(Path(__file__).parent.parent))
from vernier.lib.vernier_reader import VernierReader


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

    def test_load_from_file_threads_format(self):
        test_reader = VernierReader(self.test_data_dir / "vernier-output-threads-collated")
        loaded_data = test_reader.load()
        self.assertIn("FULL", loaded_data.data)
        self.assertIn("MAIN_SUB", loaded_data.data)
        self.assertIn("MAIN_SUB2", loaded_data.data)

        self.assertCountEqual(loaded_data.data['FULL'].time_percent, [39.99956002903808, 40.10755822077782])
        self.assertCountEqual(loaded_data.data['FULL'].self_time, [2.00011, 2.00914])
        self.assertCountEqual(loaded_data.data['FULL'].total_time, [5.00033, 5.00938])
        self.assertCountEqual(loaded_data.data['FULL'].cumul_time, [4.000249999999999, 2.00914])
        self.assertCountEqual(loaded_data.data['FULL'].rank, [0, 1])
        self.assertCountEqual(loaded_data.data['FULL'].thread, [0, 0])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].n_calls, [2, 1, 1, 1, 2, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].time_percent, [40.0001599894407, 20.00027998152122, 20.00027998152122, 20.00007999472035, 39.928094893978894, 19.96394763423817, 19.96394763423817, 19.96374800873561])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].self_time, [2.00014, 1.00008, 1.00008, 1.00007, 2.00015, 1.00007, 1.00007, 1.00006])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].total_time, [3.00022, 2.00016, 2.00015, 2.00013, 3.00023, 2.00526, 2.00895, 2.00148])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].cumul_time, [2.00014, 5.00033, 6.0004100000000005, 9.000620000000001, 4.00929, 9.024909999999998, 10.02498, 11.025039999999999])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].rank, [0, 0, 0, 0, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].thread, [0, 1, 2, 3, 0, 3, 1, 2])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].n_calls, [1, 1, 1, 1, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].time_percent, [20.00007999472035, 20.00007999472035, 19.999880007919476, 19.999880007919476, 20.139817701991063, 20.066155891547456, 19.990697451580832, 19.96394763423817])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].self_time, [1.00007, 1.00007, 1.00006, 1.00006, 1.00888, 1.00519, 1.00141, 1.00007])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].total_time, [1.00007, 1.00007, 1.00006, 1.00006, 1.00888, 1.00519, 1.00141, 1.00007])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].cumul_time, [7.0004800000000005, 8.00055, 10.000680000000001, 11.00074, 5.01817, 6.023359999999999, 7.024769999999999, 8.02484])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].rank, [0, 0, 0, 0, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].thread, [0, 2, 1, 3, 1, 3, 2, 0])


    def test_load_from_file_drhook_format(self):
        test_reader = VernierReader(self.test_data_dir / "vernier-output-drhook-collated")
        loaded_data = test_reader.load()
        self.assertIn("FULL", loaded_data.data)
        self.assertIn("MAIN_SUB", loaded_data.data)
        self.assertIn("MAIN_SUB2", loaded_data.data)

        self.assertCountEqual(loaded_data.data['FULL'].n_calls, [1, 1])
        self.assertCountEqual(loaded_data.data['FULL'].time_percent, [39.999, 40.106])
        self.assertCountEqual(loaded_data.data['FULL'].self_time, [2.0, 2.009])
        self.assertCountEqual(loaded_data.data['FULL'].total_time, [5.0, 5.009])
        self.assertCountEqual(loaded_data.data['FULL'].cumul_time, [4.0, 2.009])
        self.assertCountEqual(loaded_data.data['FULL'].rank, [0, 1])
        self.assertCountEqual(loaded_data.data['FULL'].thread, [0, 0])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].n_calls, [2, 1, 1, 1, 2, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].time_percent, [40.0, 20.0, 20.0, 20.0, 39.929, 19.965, 19.965, 19.965])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].self_time, [2.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].total_time, [3.0, 2.0, 2.0, 2.0, 3.0, 2.004, 2.007, 2.009])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].cumul_time, [2.0, 5.0, 6.0, 9.001, 4.009, 9.029, 10.03, 11.03])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].rank, [0, 0, 0, 0, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].thread, [0, 2, 3, 1, 0, 1, 3, 2])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].n_calls, [1, 1, 1, 1, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].time_percent, [20.0, 20.0, 20.0, 19.999, 20.137, 20.11, 20.046, 19.965])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].self_time, [1.0, 1.0, 1.0, 1.0, 1.009, 1.007, 1.004, 1.0])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].total_time, [1.0, 1.0, 1.0, 1.0, 1.009, 1.007, 1.004, 1.0])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].cumul_time, [7.0, 8.001, 10.001, 11.001, 5.018, 6.025, 7.029, 8.029])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].rank, [0, 0, 0, 0, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].thread, [1, 0, 2, 3, 2, 3, 1, 0])

    def test_load_from_directory_threads_format(self):
        test_reader = VernierReader(self.test_data_dir / "vernier-output-threads-format")
        loaded_data = test_reader.load()
        self.assertIn("FULL", loaded_data.data)
        self.assertIn("MAIN_SUB", loaded_data.data)
        self.assertIn("MAIN_SUB2", loaded_data.data)

        self.assertCountEqual(loaded_data.data['FULL'].n_calls, [1, 1])
        self.assertCountEqual(loaded_data.data['FULL'].time_percent, [39.999280048956685, 40.09476250471716])
        self.assertCountEqual(loaded_data.data['FULL'].self_time, [2.0001, 2.00807])
        self.assertCountEqual(loaded_data.data['FULL'].total_time, [5.00034, 5.00831])
        self.assertCountEqual(loaded_data.data['FULL'].cumul_time, [4.00025, 2.00807])
        self.assertCountEqual(loaded_data.data['FULL'].rank, [0, 1])
        self.assertCountEqual(loaded_data.data['FULL'].thread, [0, 0])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].n_calls, [2, 1, 1, 1, 2, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].time_percent, [40.0002799809613, 20.000439970082034, 20.000039997280187, 20.000039997280187, 39.93682499685523, 19.968212830276084, 19.968212830276084, 19.968212830276084])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].self_time, [2.00015, 1.00009, 1.00007, 1.00007, 2.00016, 1.00007, 1.00007, 1.00007])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].total_time, [3.00024, 2.00016, 2.00015, 2.00017, 3.00023, 2.00784, 2.00049, 2.00063])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].cumul_time, [2.00015, 5.0003400000000005, 8.00057, 9.00064, 4.00823, 8.017040000000001, 9.017110000000002, 11.017250000000004])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].rank, [0, 0, 0, 0, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB'].thread, [0, 2, 3, 1, 0, 3, 1, 2])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].n_calls, [1, 1, 1, 1, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].time_percent, [20.000239983681112, 20.000239983681112, 20.000039997280187, 19.99984001087926, 20.121757638804308, 19.977996569701155, 19.97520121557971, 19.968212830276084])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].self_time, [1.00008, 1.00008, 1.00007, 1.00006, 1.00776, 1.00056, 1.00042, 1.00007])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].total_time, [1.00008, 1.00008, 1.00007, 1.00006, 1.00776, 1.00056, 1.00042, 1.00007])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].cumul_time, [6.00042, 7.000500000000001, 10.000710000000002, 11.000770000000001, 5.01599, 6.0165500000000005, 7.016970000000001, 10.017180000000003])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].rank, [0, 0, 0, 0, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data['MAIN_SUB2'].thread, [3, 0, 1, 2, 3, 2, 1, 0])

    def test_load_from_directory_drhook_format(self):
        test_reader = VernierReader(self.test_data_dir / "vernier-output-drhook-format")
        loaded_data = test_reader.load()
        self.assertIn("FULL", loaded_data.data)
        self.assertIn("MAIN_SUB", loaded_data.data)
        self.assertIn("MAIN_SUB2", loaded_data.data)

        self.assertCountEqual(loaded_data.data["FULL"].n_calls, [1, 1])
        self.assertCountEqual(loaded_data.data["FULL"].time_percent, [40.0, 40.031])
        self.assertCountEqual(loaded_data.data["FULL"].self_time, [2.0, 2.004])
        self.assertCountEqual(loaded_data.data["FULL"].total_time, [5.0, 5.006])
        self.assertCountEqual(loaded_data.data["FULL"].cumul_time, [4.0, 2.004])
        self.assertCountEqual(loaded_data.data["FULL"].rank, [0, 1])
        self.assertCountEqual(loaded_data.data["FULL"].thread, [0, 0])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].n_calls, [2, 1, 1, 1, 2, 1, 1, 1])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].time_percent, [40.0, 20.0, 20.0, 20.0, 39.956, 19.978, 19.978, 19.977])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].self_time, [2.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].total_time, [3.0, 2.0, 2.0, 2.0, 3.002, 2.004, 2.002, 2.0])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].cumul_time, [2.0, 6.0, 10.001, 11.001, 4.004, 9.012, 10.012, 11.012])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].rank, [0, 0, 0, 0, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data["MAIN_SUB"].thread, [0, 3, 2, 1, 0, 1, 3, 2])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].n_calls, [1, 1, 1, 1, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].time_percent, [20.0, 20.0, 20.0, 20.0, 20.056, 20.017, 20.012, 19.979])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].self_time, [1.0, 1.0, 1.0, 1.0, 1.004, 1.002, 1.002, 1.0])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].total_time, [1.0, 1.0, 1.0, 1.0, 1.004, 1.002, 1.002, 1.0])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].cumul_time, [5.0, 7.001, 8.001, 9.001, 5.008, 6.01, 7.012, 8.012])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].rank, [0, 0, 0, 0, 1, 1, 1, 1])
        self.assertCountEqual(loaded_data.data["MAIN_SUB2"].thread, [3, 0, 2, 1, 1, 3, 0, 2])

if __name__ == '__main__':
    unittest.main()
