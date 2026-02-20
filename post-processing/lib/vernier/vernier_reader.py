from pathlib import Path
import os
from .vernier_data import VernierData, aggregate

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

        # Populate data
        contents = self.path.read_text().splitlines()
        for line in contents:
            sline = line.split()
            if len(sline) > 0: # Line contains data
                if sline[0].isdigit(): # Caliper lines start with a digit

                    caliper = sline[-1]
                    if not caliper in loaded.data:
                        loaded.add_caliper(caliper)

                    loaded.data[caliper].time_percent.append(float(sline[1]))
                    loaded.data[caliper].cumul_time.append(float(sline[2]))
                    loaded.data[caliper].self_time.append(float(sline[3]))
                    loaded.data[caliper].total_time.append(float(sline[4]))
                    loaded.data[caliper].n_calls.append(int(sline[5]))

        if not loaded.data:
            raise ValueError(f"No caliper data found in file '{self.path}'.")

        return loaded


    def _load_from_directory(self) -> VernierData:
        """Loads Vernier data from a directory of files, and returns it as a VernierData object."""

        vernier_files = [f for f in os.listdir(self.path) if f.startswith("vernier-output")]
        vernier_datasets = [VernierReader(self.path / vernier_file)._load_from_file() for vernier_file in vernier_files]

        return aggregate(vernier_datasets)


    def load(self) -> VernierData:
        """Generic load routine for Vernier data, aiming to handle both single
        files and directories of files."""

        if self.path.is_file():
            return self._load_from_file()

        elif self.path.is_dir():
            return self._load_from_directory()

        else:
            raise ValueError(f"Provided path '{self.path}' is neither a file nor a directory.")
