# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
"""
Module for storing the VernierData and VernierCalliper classes.
"""
from dataclasses import dataclass
from pathlib import Path
import statistics
import sys
from typing import Optional
from collections import OrderedDict


@dataclass(order=True)
class VernierCalliper():
    """
    Class to hold data for a single Vernier calliper, including arrays for
    each metric.

    """
    total_time: list[float]
    time_percent: list[float]
    self_time: list[float]
    cumul_time: list[float]
    n_calls: list[int]
    rank: list[int]
    thread: list[int]
    name: str

    def __init__(self, name: str):
        """
        Initialise the VernierCalliper instance with lists ready to be filled
        with calliper data.

        :param str name: The name of the VernierCalliper.

        """
        self.name = name
        self.rank = []
        self.thread = []
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
            len(self.self_time) == len(self.total_time) == len(self.n_calls)):
            result = len(self.time_percent)
        return result

    def _get_rank_indices(self, rank: int):
        """
        Return the indices of the data for a given rank.

        :param int rank: The rank number to extract indices for.

        :returns: A list of indices corresponding to the entries for the
                  provided rank.
        :rtype: list[int]

        """
        rank_indices = []
        start = 0
        # Incrementally iterate though list with List.index() to find all the
        # indices matching the chosen rank ID
        while True:
            try:
                # Find next instance of rank ID in rank list
                rank_index = self.rank.index(rank, start)
                rank_indices.append(rank_index)
                start = rank_index + 1
            except ValueError:
                return rank_indices

    def _get_thread_indices(self, thread: int):
        """
        Return the indices of the data for a given thread.

        :param int thread: The thread number to extract indices for.

        :returns: A list of indices corresponding to the entries for the
                  provided thread.
        :rtype: list[int]

        """
        thread_indices = []
        start = 0
        # Incrementally iterate though list with List.index() to find all the
        # indices matching the chosen thread ID
        while True:
            try:
                # Find the next instance of thread ID in the list
                thread_index = self.thread.index(thread, start)
                thread_indices.append(thread_index)
                start = thread_index + 1
            except ValueError:
                return thread_indices

    def _filter_by_indices(self, indices: list[int]):
        """
        Return a new VernierCalliper containing only the entries corresponding
        to the provided indices.

        :param list[int] indices: A list of indices to filter by.

        :returns: A new VernierCalliper instance containing only the entries
                  corresponding to the provided indices.
        :rtype: :py:class:`vernier.VernierCalliper`

        """
        filtered = VernierCalliper(self.name)
        for index in indices:
            filtered.rank.append(self.rank[index])
            filtered.thread.append(self.thread[index])
            filtered.time_percent.append(self.time_percent[index])
            filtered.cumul_time.append(self.cumul_time[index])
            filtered.self_time.append(self.self_time[index])
            filtered.total_time.append(self.total_time[index])
            filtered.n_calls.append(self.n_calls[index])

        return filtered

    def reduce(self) -> OrderedDict:
        """Reduces the data for this calliper to a single row of summary data.

        :returns: A OrderedDict containing the aggregate (mean) for each metric of the
                  VernierCalliper instance, and the min/max for total time and
                  self time.

        """

        return OrderedDict([
            ("Routine",self.name), # calliper name
            ("Total Min(s)", round(min(self.total_time), 5)),               # min total time across calls
            ("Total Mean(s)", round(statistics.mean(self.total_time), 5)),   # mean total time across calls
            ("Total Max(s)", round(max(self.total_time), 5)),               # max total time across calls
            ("Self Min(s)", round(min(self.self_time), 5)),                # min self time across calls
            ("Self Mean(s)", round(statistics.mean(self.self_time), 5)),    # mean self time across calls
            ("Self Max(s)", round(max(self.self_time), 5)),                # max self time across calls
            ("Max no. calls", max(self.n_calls)), # number of calls (should be the same for all entries, so just take the first)
            ("% time", round(statistics.mean(self.time_percent), 5)), # mean percentage of time across calls
            ("Time per call(s)", round(statistics.mean([t / n for t, n in zip(self.total_time, self.n_calls)]), 5)) # mean time per call
        ])


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

    def add_calliper(self, calliper_key: str):
        """
        Adds a new calliper to the data structure, with empty arrays for each
        metric.

        :param str calliper_key: The name of the calliper to be added, also
                                 used to access the calliper.

        """
        # Create empty data arrays
        self.data[calliper_key] = VernierCalliper(calliper_key)

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
        :rtype:  :py:class:`vernier.lib.vernierData`

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
        header_list = []

        # From reduce, grab all of the data and separate from the header keys
        header_pass=True
        for calliper in self.data.keys():
            reduce_row = []
            reduce_dict=self.data[calliper].reduce()
            # Work through each caliper key pair returned by reduce
            for caliper_key in reduce_dict:
                # Append the keys data/value to the row
                reduce_row.append(reduce_dict[caliper_key])
                # On the first pass capture the key for later as headers
                if header_pass:
                    header_list.append(caliper_key)
            txt_table.append(reduce_row)
            header_pass=False

        # sort by self time, descending
        txt_table = sorted(txt_table, key=lambda x: x[2], reverse=True)

        # Use the header key to add to the top of the output
        txt_table.insert(0, header_list)

        max_calliper_len = max([len(line[0]) for line in txt_table])

        # Write to stdout if no path provided, otherwise write to file
        if txt_path is None:
            out = sys.stdout
        else:
            out = open(txt_path, 'w')

        # For each line in the text table, format the string to be written
        for row in txt_table:
            row_output_string=''
            for index, column in enumerate(row):
                 # The first column, the caliper name uses the maximum size to align all lines
                if index == 0:
                    row_output_string=row_output_string+(
                        '| {:>{}} '.format(row[0], max_calliper_len))
                # Per each other element, align them to the header length, or where this is too small, 8
                else:
                    row_output_string=row_output_string+(
                        '| {:>{}} '.format(row[index], max(len(header_list[index]),8)))
            row_output_string=row_output_string+'|\n'
            
            # Write the string
            out.write(row_output_string)

        if txt_path is not None:
            out.close()

    def get(self, calliper_key: str, rank: Optional[int] = None, thread: Optional[int] = None) -> Optional[VernierCalliper]:
        """
        Return a VernierCalliper of the data for this calliper_key,
        or None if it does not exist.

        :param str calliper_key: The name of the VernierCalliper to extract
                                 from the VernierData object.
        :param int rank: The rank ID to extract data for.
        :param int thread: The thread ID to extract data for.

        :returns: A VernierCalliper instance containing the data of all
                  callipers matching the calliper_key
        :rtype: :py:class:`vernier.VernierCalliper`
        """
        # First get data for calliper key
        return_caliper = self.data[calliper_key]

        # If rank ID given as argument, filter only data indices where rank data
        # matches the rank ID given as argument
        if rank is not None:
            rank_indices = return_caliper._get_rank_indices(rank)
            if len(rank_indices) == 0:
                return None
            return_caliper = return_caliper._filter_by_indices(rank_indices)

        # Same above but for thread ID
        if thread is not None:
            thread_indices = return_caliper._get_thread_indices(thread)
            if len(thread_indices) == 0:
                return None
            return_caliper = return_caliper._filter_by_indices(thread_indices)

        return return_caliper

    def aggregate(self, vernier_data_list=None, internal_consistency=True):
        """
        Aggregates a list of VernierData objects into a single VernierData
        object by concatenating the data for each calliper across the input
        objects.

        :param vernier_data_list: A list of VernierData objects to combine.
        :type vernier_datalist: list[:py:class:`vernier.lib.vernierData`]

        :param bool internal_conistency: If set to True (default), callipers
                                         between all items in the
                                         vernier_data_list must be identical.

        :returns: A single VernierData object containing the data from all
                VernierData objects in vernier_data_list.
        :rtype: :py:class:`vernier.lib.vernierData`

        :raises ValueError: if internal_consistency is set to True and callipers
                            between items in vernier_data_list are not
                            identical.

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
                self.data[calliper].rank.extend(vernier_data.data[calliper].rank)
                self.data[calliper].thread.extend(vernier_data.data[calliper].thread)


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
        # notImplemented enforce consistent sizing of members?? needed?
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

    def get(self, calliper_key: str, rank: Optional[int] = None, thread: Optional[int] = None) -> Optional[VernierCalliper]:
        """
        Return a VernierCalliper of all the data from all collation members
        for this calliper_key, or None if it does not exist.

        :param str calliper_key: The name of the VernierCalliper to extract
                                 from the VernierData object.
        :param int rank: The rank ID to extract data for.
        :param int thread: The thread ID to extract data for.

        :returns: A VernierCalliper instance containing the data of all
                  callipers matching the calliper_key
        :rtype: :py:class:`vernier.lib.vernierCalliper`

        """
        if calliper_key not in self.calliper_list():
            return None
        self.internal_consistency()
        results = VernierCalliper(calliper_key)
        for _, vdata in self.vernier_data.items():
            data_to_add = vdata.get(calliper_key, rank, thread)
            if data_to_add is None:
                continue
            results.total_time += data_to_add.total_time
            results.time_percent += data_to_add.time_percent
            results.self_time += data_to_add.self_time
            results.cumul_time += data_to_add.cumul_time
            results.n_calls += data_to_add.n_calls

        return results
