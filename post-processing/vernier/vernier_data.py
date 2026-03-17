# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
"""
Module for storing the VernierData and VernierCalliper classes.
"""
from dataclasses import dataclass
import sys
import numpy as np
from pathlib import Path
from typing import Optional

class VernierCalliper():
    """
    Class to hold data for a single Vernier calliper, including arrays for
    each metric.
    """

    self_time: np.ndarray
    total_time: np.ndarray
    time_percent: np.ndarray
    cumul_time: np.ndarray
    n_calls: np.ndarray
    n_ranks: int
    n_threads: int
    name: str

    def __init__(self, name: str, n_ranks: int, n_threads: int = 1):
        """
        Initialise the VernierCalliper instance with lists ready to be filled
        with calliper data.

        :param str name: The name of the VernierCalliper.

        """
        self.name = name
        self.ranks = n_ranks
        self.threads = n_threads
        self.time_percent = np.full((n_ranks, n_threads), np.nan)
        self.cumul_time = np.full((n_ranks, n_threads), np.nan)
        self.self_time = np.full((n_ranks, n_threads), np.nan)
        self.total_time = np.full((n_ranks, n_threads), np.nan)
        self.n_calls = np.full((n_ranks, n_threads), np.nan)

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
        """Reduces the data for this calliper to a single row of summary data.

        :returns: A list containing the aggregate (mean) for each metric of the
                  VernierCalliper instance.
        :rtype: list[str, float, float, float, int, float, float]

        """
        return [
            self.name, # calliper name
            # We use nanmean to avoid NaN's where array shapes are non-conformant
            # due to collated data possibly having different numbers of threads
            round(np.nanmean(self.total_time), 5), # mean total time across calls
            round(np.nanmean(self.self_time), 5), # mean self time across calls
            round(np.nanmean(self.cumul_time), 5), # mean cumulative time across calls
            int(np.nanmax(self.n_calls))    , # number of calls
            round(np.nanmean(self.time_percent), 5), # mean percentage of time across calls
            round(np.nanmean(self.total_time / self.n_calls), 5) # mean time per call
        ]

    @classmethod
    def labels(self):
        return ["Routine", "Total time (s)", "Self (s)", "Cumul time (s)",
                "Max no. calls", "% time", "Time per call (s)"]

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
    Class to hold Vernier data from a single instrumented job in a structured
    way.

    Provides methods for filtering and outputting the data.

    """
    def __init__(self):
        """
        Initialises the VernierData instance with an empty dictionary for
        storing VernierCalliper objects.
        """
        self.data = {}

    def add_calliper(self, calliper_key: str, n_ranks: int, n_threads: int):
        """
        Adds a new calliper to the data structure, with empty arrays for each
        metric.

        :param str calliper_key: The name of the calliper to be added, also
                                 used to access the calliper.

        """
        # Create empty data arrays
        self.data[calliper_key] = VernierCalliper(calliper_key, n_ranks, n_threads)

    def filter(self, calliper_keys: list[str]):
        """
        Filters the Vernier data to include only callipers matching the
        provided keys. The filtering is done in a glob-like fashion, so an
        input key of "timestep" will match any calliper with "timestep" in
        its name.

        :param list[str] calliper_keys: A list of keys to extract from the
                                        callipers owned by the VernierData
                                        instance.

        :returns: A new VernierData object containing only the
                                filtered callipers.
        :rtype:  :py:class:`vernier.VernierData`

        :raises ValueError: if no callipers are found that match calliper_keys.

        """
        filtered_data = VernierData()

        # Filter data for a given calliper key
        for timer in self.data.keys():
            if any(calliper_key in timer for calliper_key in calliper_keys):
                filtered_data.data[timer] = self.data[timer]

        if len(filtered_data.data) == 0:
            raise ValueError(f"No callipers found matching the provided keys: "
                             f"{calliper_keys}")

        return filtered_data

    def write_txt_output(self, txt_path: Optional[Path] = None):
        """
        Writes the Vernier data to a text output in a human-readable table
        format. If an output path is provided, the table is written to that
        file. Otherwise, it is printed to the terminal.

        :param txt_path: The file path that the text output will be written to.
        :type txt_path: :py:class:`pathlib.Path`

        """
        txt_table = []
        for calliper in self.data.keys():
            txt_table.append(self.data[calliper].reduce())
        # sort by self time, descending
        txt_table = sorted(txt_table, key=lambda x: x[2], reverse=True)

        txt_table.insert(0, VernierCalliper.labels())

        max_calliper_len = max([len(line[0]) for line in txt_table])

        # Write to stdout if no path provided, otherwise write to file
        if txt_path is None:
            out = sys.stdout
        else:
            out = open(txt_path, 'w')

        for row in txt_table:
            out.write('| {:>{}} | {:>14} | {:>12} | {:>14} | {:>13} | {:>8} | {:>17} |\n'.format(row[0], max_calliper_len, *row[1:]))

        if txt_path is not None:
            out.close()

    def get(self, calliper_key) -> VernierCalliper:
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
        """
        Initialise an empty dictionary for storing VernierData objects.
        """
        self.vernier_data = {}

    def __len__(self):
        """
        Gets the number of VernierData objects held by the VernierDataCollation
        object.

        :returns: The number of VernierData objects.
        :rtype: int

        """
        return len(self.vernier_data)

    def add_data(self, label, vernier_data):
        """
        Add a VernierData object to the collation.

        """
        if label in self.vernier_data:
            raise ValueError(f'The label {label} already exists in this '
                             f'collation. Please use a different label or '
                             f'remove the existing entry.')
        if not isinstance(vernier_data, VernierData):
            raise TypeError(f'The provided vernier_data is not a VernierData '
                            f'object.')
        # Check for consistency
        self.internal_consistency(vernier_data)
        # Add the new data
        self.vernier_data[label] = vernier_data

    def remove_data(self, label):
        """
        Removes a VernierData object with a matching label from the collation
        object.

        :param str label: The label of the VernierData object to be removed.
        """
        if label not in self.vernier_data:
            raise ValueError(f'The label {label} does not exist in this '
                             'collation.')

        # Drop the VernierData object from the collation.
        self.vernier_data.pop(label)

    def internal_consistency(self, new_vernier_data=None):
        """
        Enforce internal consistency, with the same callipers for all members.

        :param new_vernier_data: A VernierData object being tested for
                                 consistent callipers.

        """
        callipers = []
        # Loop over VernierData objects
        for _, vdata in self.vernier_data.items():
            # Get a list of the VernierData's callipers, sorted by name
            loop_callipers = sorted(list(vdata.data.keys()))
            # If no callipers checked yet, set these as the truth callipers
            if len(callipers) == 0:
                callipers = loop_callipers
            else:
                # Check callipers against 'truth'
                if loop_callipers != callipers:
                    # Extract callipers that were mismatched using Python XOR
                    # Note that this works both ways so all callipers not in the
                    # others list are included.
                    mismatched = set(loop_callipers) ^ set(callipers)
                    raise ValueError(f'Inconsistent callipers in contents: '
                                     f'{mismatched} detected as unmatched')
        if new_vernier_data is not None:
            if not isinstance(new_vernier_data, VernierData):
                raise TypeError(f'The provided vernier_data is not a '
                                'VernierData object.')
            check_callipers = sorted(list(new_vernier_data.data.keys()))
            if callipers and check_callipers != callipers:
                # Extract callipers that were mismatched using Python XOR
                mismatched = set(check_callipers) ^ set(callipers)
                raise ValueError(f'Inconsistent callipers in new_vernier_data: '
                                 f'{check_callipers} detected as unmatched')

    def calliper_list(self):
        """
        Return the list of callipers in this collation.

        :returns: A list of all callipers held by the collation
        :rtype: list[str]

        """
        result = []
        self.internal_consistency()

        for _, vdata in self.vernier_data.items():
            result = sorted(list(vdata.data.keys()))
            break
        return result

    def get(self, calliper_key) -> VernierCalliper:
        """
        Return a VernierCalliper of all the data from all collation members
        for this calliper_key, or None if it does not exist.

        :param str calliper_key: The name of the VernierCalliper to extract
                                 from the VernierData object.

        :returns: A VernierCalliper instance containing the data of all
                  callipers matching the calliper_key
        :rtype: :py:class:`vernier.VernierCalliper`

        """
        if calliper_key not in self.calliper_list():
            return None
        self.internal_consistency()

        n_ranks = sum([vdata.data[calliper_key].ranks for _, vdata in self.vernier_data.items()])
        n_threads = max([vdata.data[calliper_key].threads for _, vdata in self.vernier_data.items()])
        results = VernierCalliper(calliper_key, n_ranks, n_threads)
        rank_begin_index = 0
        for _, vdata in self.vernier_data.items():
            rank_end_index = rank_begin_index + vdata.data[calliper_key].ranks
            results.total_time[rank_begin_index:rank_end_index][:] = vdata.data[calliper_key].total_time
            results.time_percent[rank_begin_index:rank_end_index][:] = vdata.data[calliper_key].time_percent
            results.self_time[rank_begin_index:rank_end_index][:] = vdata.data[calliper_key].self_time
            results.cumul_time[rank_begin_index:rank_end_index][:] = vdata.data[calliper_key].cumul_time
            results.n_calls[rank_begin_index:rank_end_index][:] = vdata.data[calliper_key].n_calls
            rank_begin_index = rank_end_index

        return results
