# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
from concurrent import futures
from pathlib import Path
import os
from .vernier_data import VernierData

class VernierReader():
    """Class handling the reading of Vernier output files, and converting them into a VernierData object."""

    def __init__(self, vernier_path: Path):

        self.path = vernier_path

        return


    def _load_from_file(self) -> VernierData:
        """
        Loads Vernier data from a single file, and returns it as a VernierData object.
        """

        loaded = VernierData()
        n_ranks_total = None
        rank = 0

        # Populate data
        file_contents = self.path.read_text()
        n_ranks_in_file = file_contents.count("Task") # Number of ranks worth of data in file only countable this way
        calliper_ids = [line.split()[-1] for line in file_contents.splitlines() if line.split() and line.split()[0].isdigit()]

        file_data = file_contents.splitlines()
        for line in file_data:
            sline = line.split()
            if len(sline) > 0: # Line contains data
                if "Task" in sline:
                    if n_ranks_total is None:
                        n_ranks_total = int(sline[3])
                        if not n_ranks_in_file in [1, n_ranks_total]:
                            raise ValueError(f"Input data not consistent with either collated or multi-file vernier output.")
                    rank = int(sline[-1]) # Extract rank number from the data line
                if sline[0].isdigit(): # Calliper lines start with a digit

                    if n_ranks_in_file == 1:
                        rank_index = 0
                    else:
                        rank_index = rank

                    calliper, thread = sline[-1].split('@')
                    thread_id = int(thread)

                    n_threads = int(len([cal_id for cal_id in calliper_ids if f"{calliper}@" in cal_id]) / n_ranks_in_file)

                    if not calliper in loaded.data:
                        loaded.add_calliper(calliper, n_ranks_in_file, n_threads)

                    loaded.data[calliper].time_percent[rank_index, thread_id] = float(sline[1])
                    loaded.data[calliper].cumul_time[rank_index, thread_id] = float(sline[2])
                    loaded.data[calliper].self_time[rank_index, thread_id] = float(sline[3])
                    loaded.data[calliper].total_time[rank_index, thread_id] = float(sline[4])
                    loaded.data[calliper].n_calls[rank_index, thread_id] = int(sline[5])

        if not loaded.data:
            raise ValueError(f"No calliper data found in file '{self.path}'.")

        return loaded


    def _load_from_directory(self) -> VernierData:
        """Loads Vernier data from a directory of files, and returns it as a VernierData object."""

        vernier_files = [f for f in os.listdir(self.path) if f.startswith("vernier-output")]

        with futures.ThreadPoolExecutor() as pool:
            vernier_datasets = list(pool.map(lambda f: VernierReader(self.path / f)._load_from_file(), vernier_files))

        result = VernierData()

        # Check that all input VernierData objects have the same
        # set of callipers
        calliper_sets = [set(vernier_data.data.keys()) for vernier_data in
                            vernier_datasets]
        if not all(calliper_set == calliper_sets[0] for
                    calliper_set in calliper_sets):
            raise ValueError("Input VernierData objects do not have the "
                                "same set of callipers, but "
                                "internal_consistency is set to True.")

        for i, vernier_data in enumerate(vernier_datasets):
            for calliper in vernier_data.data.keys():
                if not calliper in result.data:
                    _, n_threads = vernier_data.data[calliper].self_time.shape
                    result.add_calliper(calliper, len(vernier_datasets), n_threads)

                result.data[calliper].time_percent[i] = vernier_data.data[calliper].time_percent[0]
                result.data[calliper].cumul_time[i] = vernier_data.data[calliper].cumul_time[0]
                result.data[calliper].self_time[i] = vernier_data.data[calliper].self_time[0]
                result.data[calliper].total_time[i] = vernier_data.data[calliper].total_time[0]
                result.data[calliper].n_calls[i] = vernier_data.data[calliper].n_calls[0]

        return result


    def load(self) -> VernierData:
        """Generic load routine for Vernier data, aiming to handle both single
        files and directories of files."""

        if self.path.is_file():
            return self._load_from_file()

        elif self.path.is_dir():
            return self._load_from_directory()

        else:
            raise ValueError(f"Provided path '{self.path}' is neither a file nor a directory.")
