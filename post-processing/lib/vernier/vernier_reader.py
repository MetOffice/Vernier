from pathlib import Path
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

        handle = open(self.path, 'r')

        loaded = VernierData()

        # Populate data
        contents = handle.readlines()
        for line in contents:
            sline = line.split()
            if len(sline) > 0:
                if sline[0].isdigit():

                    caliper = sline[-1]
                    if not caliper in loaded.data:
                        loaded.add_caliper(caliper)

                    loaded.data[caliper]["%time"].append(float(sline[1]))
                    loaded.data[caliper]["cumul"].append(float(sline[2]))
                    loaded.data[caliper]["self"].append(float(sline[3]))
                    loaded.data[caliper]["total"].append(float(sline[4]))
                    if not int(sline[5]) in loaded.data[caliper]["n_calls"]:
                        loaded.data[caliper]["n_calls"].append(int(sline[5]))

        return loaded


    def load(self) -> VernierData:
        """Generic load routine for Vernier data, aiming to handle both single
        files and directories of files."""

        if self.path.is_file():
            return self._load_from_file()

        elif self.path.is_dir():
            raise NotImplementedError("Loading from a directory of Vernier output files is not yet implemented.")