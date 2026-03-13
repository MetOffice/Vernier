# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
from dataclasses import dataclass
import sys
import numpy as np
from pathlib import Path
from typing import Optional

class VernierCalliper():
    """Class to hold data for a single Vernier calliper, including arrays for each metric."""

    self_time: np.ndarray
    total_time: np.ndarray
    time_percent: np.ndarray
    cumul_time: np.ndarray
    n_calls: np.ndarray
    name: str

    def __init__(self, name: str, n_ranks: int):

        self.name = name
        self.time_percent = np.zeros(n_ranks)
        self.cumul_time = np.zeros(n_ranks)
        self.self_time = np.zeros(n_ranks)
        self.total_time = np.zeros(n_ranks)
        self.n_calls = np.zeros(n_ranks)

        return

    def __len__(self):
        """
        Return None if calliper elements differ in length,
        otherwise return element lengths.
        """
        result = None
        if (len(self.time_percent) == len(self.cumul_time) ==
            len(self.self_time) == len(self.total_time) ==  len(self.n_calls)):
            result = len(self.time_percent)
        return result

    def reduce(self) -> list:
        """Reduces the data for this calliper to a single row of summary data."""

        return [
            self.name.replace('@0', ''), # calliper name
            round(self.total_time.mean(), 5), # mean total time across calls
            round(self.self_time.mean(), 5), # mean self time across calls
            round(self.cumul_time.mean(), 5), # mean cumulative time across calls
            int(self.n_calls[0])    , # number of calls (should be the same for all entries, so just take the first)
            round(self.time_percent.mean(), 5), # mean percentage of time across calls
            round(self.total_time.mean() / self.n_calls[0], 5) # mean time per call
        ]

    @classmethod
    def labels(self):
        return ["Routine", "Total time (s)", "Self (s)", "Cumul time (s)",
                "No. calls", "% time", "Time per call (s)"]

    def __lt__(self, other):
        """Comparison method for sorting callipers by self time."""
        return self.self_time.sum() < other.self_time.sum()

    def __eq__(self, other):
        """Equality method for comparing callipers by self time."""
        return self.self_time.sum() == other.self_time.sum()

    def __gt__(self, other):
        """Comparison method for sorting callipers by self time."""
        return self.self_time.sum() > other.self_time.sum()


class VernierData():
    """
    Class to hold Vernier data from a single instrumented job in a structured way.
    Provides methods for filtering and outputting the data.

    """

    def __init__(self):

        self.data = {}

        return


    def add_calliper(self, calliper_key: str, n_ranks: int):
        """Adds a new calliper to the data structure, with empty arrays for each metric."""

        # Create empty data arrays
        self.data[calliper_key] = VernierCalliper(calliper_key, n_ranks)

    def filter(self, calliper_keys: list[str]):
        """Filters the Vernier data to include only callipers matching the provided keys.
        The filtering is done in a glob-like fashion, so an input key of "timestep"
        will match any calliper with "timestep" in its name."""

        filtered_data = VernierData()

        # Filter data for a given calliper key
        for timer in self.data.keys():
            if any(calliper_key in timer for calliper_key in calliper_keys):
                filtered_data.data[timer] = self.data[timer]

        if len(filtered_data.data) == 0:
            raise ValueError(f"No callipers found matching the provided keys: {calliper_keys}")

        return filtered_data


    def write_txt_output(self, txt_path: Optional[Path] = None):
        """Writes the Vernier data to a text output in a human-readable table format.
        If an output path is provided, the table is written to that file. Otherwise,
        it is printed to the terminal."""

        txt_table = []
        for calliper in self.data.keys():
            txt_table.append(self.data[calliper].reduce())
        txt_table = sorted(txt_table, key=lambda x: x[2], reverse=True) # sort by self time, descending

        txt_table.insert(0, ["Routine", "Total time (s)", "Self (s)", "Cumul time (s)", "No. calls", "% time", "Time per call (s)"])

        max_calliper_len = max([len(line[0]) for line in txt_table])

        # Write to stdout if no path provided, otherwise write to file
        if txt_path is None:
            out = sys.stdout
        else:
            out = open(txt_path, 'w')

        for row in txt_table:
            out.write('| {:>{}} | {:>14} | {:>12} | {:>14} | {:>9} | {:>8} | {:>17} |\n'.format(row[0], max_calliper_len, *row[1:]))

        if txt_path is not None:
            out.close()

    def get(self, calliper_key):
        """
        Return a VernierCalliper of the data for this calliper_key,
        or None if it does not exist.
        """
        return self.data.get(calliper_key, None)


class VernierDataCollation():
    """
    Class to hold an collation of VernierData instances.
    Instances are asserted to be consistent in terms enforced by the
    interal_consistency method.

    """
    def __init__(self):
        self.vernier_data = {}
        return

    def __len__(self):
        return len(self.vernier_data)

    def add_data(self, label, vernier_data):
        if label in self.vernier_data:
            raise ValueError(f'The label {label} already exists in this '
                             'collation. Please use a different label or '
                             'remove the existing entry.')
        if not isinstance(vernier_data, VernierData):
            raise TypeError(f'The provided vernier_data is not a VernierData '
                            'object.')
        self.internal_consistency(vernier_data)
        self.vernier_data[label] = vernier_data

    def remove_data(self, label):
        if label not in self.vernier_data:
            raise ValueError(f'The label {label} does not exist in this '
                             'collation.')
        discarded = self.vernier_data.pop(label)

    def internal_consistency(self, new_vernier_data=None):
        """
        Enforce internal consistency, with the same callipers for all members.
        """
        # notImplemented enforce consistent sizing of members?? needed?
        callipers = []
        for k, vdata in self.vernier_data.items():
            loop_callipers = sorted(list(vdata.data.keys()))
            if len(callipers) == 0:
                callipers = loop_callipers
            else:
                if loop_callipers != callipers:
                    raise ValueError('inconsistent callipers in contents')
        if new_vernier_data is not None:
            if not isinstance(new_vernier_data, VernierData):
                raise TypeError(f'The provided vernier_data is not a '
                                'VernierData object.')
            check_callipers = sorted(list(new_vernier_data.data.keys()))
            if callipers and check_callipers != callipers:
                raise ValueError('inconsistent callipers in new_vernier_data')

    def calliper_list(self):
        """Return the list of callipers in this collation."""
        result = []
        self.internal_consistency()

        for k, vdata in self.vernier_data.items():
            result = sorted(list(vdata.data.keys()))
            break
        return result

    def get(self, calliper_key):
        """
        Return a VernierCalliper of all the data from all collation members
        for this calliper_key, or None if it does not exist.

        """
        if calliper_key not in self.calliper_list():
            return None
        self.internal_consistency()
        results = VernierCalliper(calliper_key)
        for akey, vdata in self.vernier_data.items():
            results.total_time += vdata.data[calliper_key].total_time
            results.time_percent += vdata.data[calliper_key].time_percent
            results.self_time += vdata.data[calliper_key].self_time
            results.cumul_time += vdata.data[calliper_key].cumul_time
            results.n_calls += vdata.data[calliper_key].n_calls

        return results
