# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
from concurrent import futures
from enum import Enum, auto
from pathlib import Path
import os
from vernier.lib.vernier_data import VernierData


class VernierFileFormat(Enum):
    """
    Enums for different Vernier output file formats.

    """
    THREADS = auto()
    DRHOOK = auto()
    INVALID = auto()


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

    def _get_file_format(self, file_header) -> VernierFileFormat:
        """
        Determines file format based on file contents - looks for the presence
        of characteristic string fragments in a provided section of the file
        contents.

        """

        if "region_name@thread_id" in file_header:
            return VernierFileFormat.THREADS
        elif any([line.startswith('Profiling on') for line in file_header]):
            return VernierFileFormat.DRHOOK
        else:
            return VernierFileFormat.INVALID

    def _parse_threadsfile_data(self, file_contents) -> VernierData:
        """
        Parses the contents of a Vernier output file in the 'threads' format
        into a VernierData object.
        """

        loaded = VernierData()

        calliper_data_section = False
        calc_time_percent = False

        # Add EOF line to ensure percentage calculation is triggered at end of
        # file
        file_contents.append("\n")

        # Populate data
        for line in file_contents:
            sline = line.split()
            if len(sline) > 0: # Line contains data
                if sline[0] == "Task":
                    rank = int(sline[-1]) # Extract rank number from the data line
                    continue

                # If line matches the beginning of a calliper data section of
                # the file, set this switch accordingly and reset temporaries
                if "--------------" in sline:
                    calliper_data_section = True
                    pc_self_times = {}
                    max_tot_time = 0
                    cumul_self_time = 0
                    continue

                if calliper_data_section:
                    calliper, thread = sline[0].split('@')
                    if not calliper in loaded.data:
                        loaded.add_calliper(calliper)

                    # Add values to percentage calculation temporaries
                    if float(sline[2]) > max_tot_time:
                        max_tot_time = float(sline[2])
                    pc_self_times[sline[0]] = float(sline[1])

                    cumul_self_time += float(sline[1])

                    loaded.data[calliper].rank.append(int(rank))
                    loaded.data[calliper].thread.append(int(thread))
                    loaded.data[calliper].self_time.append(float(sline[1]))
                    loaded.data[calliper].total_time.append(float(sline[2]))
                    loaded.data[calliper].n_calls.append(int(sline[4]))

                    loaded.data[calliper].cumul_time.append(cumul_self_time)


            elif len(sline) == 0: # End of calliper data section
                if calliper_data_section:
                    for key in pc_self_times.keys():
                        calliper = key.split('@')[0]
                        loaded.data[calliper].time_percent.append((pc_self_times[key]/max_tot_time)*100)
                calliper_data_section = False

        if not loaded.data:
            raise ValueError(f"No calliper data found in file '{self.path}'.")

        return loaded

    def _parse_drhook_data(self, file_contents) -> VernierData:
        """
        Parses the contents of a Vernier output file in the 'drhook' format
        into a VernierData object.
        """

        loaded = VernierData()

        # Populate data
        for line in file_contents:
            sline = line.split()
            if len(sline) > 0: # Line contains data
                if sline[0] == "Task":
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

    def _load_from_file(self) -> VernierData:
        """
        Loads Vernier data from a single file, and returns it as a VernierData
        object.

        """

        contents = self.path.read_text().splitlines()

        header = contents[0:10]

        # Match file format and populate data
        match self._get_file_format(header):
            case VernierFileFormat.THREADS:
                loaded = self._parse_threadsfile_data(contents)
            case VernierFileFormat.DRHOOK:
                loaded = self._parse_drhook_data(contents)
            case VernierFileFormat.INVALID:
                raise ValueError("Invalid file format - cannot load Vernier data.")

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
