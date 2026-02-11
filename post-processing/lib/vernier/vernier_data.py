import numpy as np
from pathlib import Path
from typing import Optional

class VernierData():
    """Class to hold Vernier data in a structured way, and provide methods for filtering and outputting the data."""

    def __init__(self):

        self.data = {}

        return


    def add_caliper(self, caliper_key):
        """Adds a new caliper to the data structure, with empty arrays for each metric."""

        # Create empty data arrays
        self.data[caliper_key] = {
            "%time"   : [],
            "cumul"   : [],
            "self"    : [],
            "total"   : [],
            "n_calls" : []
        }


    def filter(self, caliper_keys: list[str]):
        """Filters the Vernier data to include only calipers matching the provided keys.
        The filtering is done in a glob-like fashion, so an input key of "timestep"
        will match any caliper with "timestep" in its name."""

        filtered_data = VernierData()

        # Filter data for a given caliper key
        for timer in self.data.keys():
            if any(caliper_key in timer for caliper_key in caliper_keys):
                filtered_data.data[timer] = self.data[timer]

        if len(filtered_data.data) == 0:
            raise ValueError(f"No calipers found matching the provided keys: {caliper_keys}")

        return filtered_data


    def write_txt_output(self, txt_path: Optional[Path] = None):
        """Writes the Vernier data to a text output in a human-readable table format.
        If an output path is provided, the table is written to that file. Otherwise,
        it is printed to the terminal."""

        txt_table = []
        txt_table.append(["Routine", "Total time (s)", "Self (s)", "No. calls", "% time", "Time per call (s)"])
        for caliper in self.data.keys():
            txt_table.append([
                f"{caliper.replace('@0', '')}",
                f"{round(np.mean(self.data[caliper]['total']), 5)}",
                f"{round(np.mean(self.data[caliper]['self']), 5)}",
                f"{self.data[caliper]['n_calls'][0]}",
                f"{round(np.mean(self.data[caliper]['%time']), 5)}",
                f"{round(np.mean(self.data[caliper]['total']) / self.data[caliper]['n_calls'][0], 5)}"
            ])

        if txt_path is None:
            for row in txt_table:
                print('| {:>32} | {:>16} | {:>12} | {:>10} | {:>10} | {:>18} |'.format(*row))
            print("\n")
        else:
            with open(txt_path, 'w') as f:
                for row in txt_table:
                    f.write('| {:>32} | {:>16} | {:>12} | {:>10} | {:>10} | {:>18} |\n'.format(*row))