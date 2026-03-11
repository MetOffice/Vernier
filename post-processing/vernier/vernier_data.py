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

@dataclass(order=True)
class VernierCalliper():
    """Class to hold data for a single Vernier calliper, including arrays for each metric."""

    total_time: list[float]
    time_percent: list[float]
    self_time: list[float]
    cumul_time: list[float]
    n_calls: list[int]
    name: str

    def __init__(self, name: str):

        self.name = name
        self.time_percent = []
        self.cumul_time = []
        self.self_time = []
        self.total_time = []
        self.n_calls = []

        return

    def __len__(self):
        """
        Return None if caliper elements differ in length,
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
            round(np.mean(self.total_time), 5), # mean total time across calls
            round(np.mean(self.self_time), 5), # mean self time across calls
            round(np.mean(self.cumul_time), 5), # mean cumulative time across calls
            self.n_calls[0], # number of calls (should be the same for all entries, so just take the first)
            round(np.mean(self.time_percent), 5), # mean percentage of time across calls
            round(np.mean(self.total_time) / self.n_calls[0], 5) # mean time per call
        ]

    @classmethod
    def labels(self):
        return ["Routine", "Total time (s)", "Self (s)", "Cumul time (s)",
                "No. calls", "% time", "Time per call (s)"]


class VernierData():
    """
    Class to hold Vernier data from a single instrumented job in a structured way.
    Provides methods for filtering and outputting the data.

    """

    def __init__(self):

        self.data = {}

        return


    def add_calliper(self, calliper_key: str):
        """Adds a new calliper to the data structure, with empty arrays for each metric."""

        # Create empty data arrays
        self.data[calliper_key] = VernierCalliper(calliper_key)

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


    def aggregate(self, vernier_data_list=None, internal_consistency=True):
        """
        Aggregates a list of VernierData objects into a single VernierData
        object, by concatenating the data for each calliper across the input
        objects.
        """

        if vernier_data_list is None:
            vernier_data_list = []
        if internal_consistency:
            # Check that all input VernierData objects have the same
            # set of callipers
            calliper_sets = [set(vernier_data.data.keys()) for vernier_data in
                             vernier_data_list]
            if not all(calliper_set == calliper_sets[0] for
                       calliper_set in calliper_sets):
                raise ValueError("Input VernierData objects do not have the "
                                 "same set of callipers, but "
                                 "internal_consistency is set to True.")

        for vernier_data in vernier_data_list:
            for calliper in vernier_data.data.keys():
                if not calliper in self.data:
                    self.add_calliper(calliper)

                self.data[calliper].time_percent.extend(vernier_data.data[calliper].time_percent)
                self.data[calliper].cumul_time.extend(vernier_data.data[calliper].cumul_time)
                self.data[calliper].self_time.extend(vernier_data.data[calliper].self_time)
                self.data[calliper].total_time.extend(vernier_data.data[calliper].total_time)
                self.data[calliper].n_calls.extend(vernier_data.data[calliper].n_calls)


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
