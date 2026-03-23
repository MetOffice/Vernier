# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
from concurrent import futures
from pathlib import Path
import os
from vernier.vernier_data import VernierData


class VernierReader():
    """
    Class handling the reading of Vernier output files, and converting them
    into a VernierData object.

    """
    def __init__(self, vernier_path: Path):
        """
        Initialise the VernierReader instance and set the associated path as
        an attribute.

        :param vernier_path: Path to the Vernier data file.
        :type vernier_path: :py:class:`pathlib.Path`
        """
        self.path = vernier_path

    def _load_from_file(self) -> VernierData:
        """
        Loads Vernier data from a single file, and returns it as a VernierData
        object.

        """
        loaded = VernierData()

        # Populate data
        contents = self.path.read_text().splitlines()
        for line in contents:
            sline = line.split()
            if len(sline) > 0: # Line contains data
                if "Task" in sline:
                    rank = int(sline[-1]) # Extract rank number from the data line

                if sline[0].isdigit(): # Calliper lines start with a digit
                    calliper, thread = sline[-1].split('@')
                    if not calliper in loaded.data:
                        loaded.add_calliper(calliper)

                    loaded.data[calliper].rank.append(int(rank))
                    loaded.data[calliper].thread.append(int(thread))
                    loaded.data[calliper].time_percent.append(float(sline[1]))
                    loaded.data[calliper].cumul_time.append(float(sline[2]))
                    loaded.data[calliper].self_time.append(float(sline[3]))
                    loaded.data[calliper].total_time.append(float(sline[4]))
                    loaded.data[calliper].n_calls.append(int(sline[5]))

        if not loaded.data:
            raise ValueError(f"No calliper data found in file '{self.path}'.")

        return loaded

    def _load_from_directory(self) -> VernierData:
        """
        Loads Vernier data from a directory of files, and returns it as a
        VernierData object.

        """
        vernier_files = [f for f in os.listdir(self.path) if
                         f.startswith("vernier-output")]

        with futures.ThreadPoolExecutor() as pool:
            vernier_datasets = list(
                pool.map(lambda f:
                         VernierReader(
                             self.path / f)._load_from_file(),
                             vernier_files)
                             )

        result = VernierData()
        result.aggregate(vernier_datasets)
        return result

    def load(self) -> VernierData:
        """Generic load routine for Vernier data, aiming to handle both single
        files and directories of files."""

        if self.path.is_file():
            return self._load_from_file()

        elif self.path.is_dir():
            return self._load_from_directory()

        else:
            raise ValueError(f"Provided path '{self.path}' is neither a file "
                             f"nor a directory.")
