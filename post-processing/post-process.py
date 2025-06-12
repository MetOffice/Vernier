#!/usr/bin/env python3
import os
import pandas as pd
from io import StringIO as sio
import re
import argparse
from pathlib import Path
import glob

""" 
README

For documentation on this script, please see the post-processing section of the user guide documentation.

README
""" 

def parse_cli_arguments(input_arguments: list[str] = None,
                        ) -> argparse.ArgumentParser:
    """ Parses command line arguments

    Reads in arguments used in the command line and passes them into the parser object.

    Args:
        input_arguments: A list of command-line arguments to be parsed

    Returns:
        The 'parser' object. This contains the arguments to be read into variables for later use.
    """

    parser = argparse.ArgumentParser(description="This script is for merging the outputs from a test that uses Vernier callipers into one file. For full documentation please see the post-processing section of the user guide.")

    parser.add_argument("-p", "--path",         type=Path,  default=(os.getcwd()),                 help="Path to Vernier output files")
    parser.add_argument("-o", "--output_name",   type=str,   default=str("vernier-merged-output"), help="Name of file to write to")
    parser.add_argument("-i", "--input_name",    type=str,   default=str("vernier-output-"),       help="Vernier files to read from")
    parser.add_argument("-d", "--decimals",     type=int,   default=3,                             help="Number of decimal places calculated results will be reported to")
    parser.add_argument("-b", "--basic_output", action="store_true", default=False,                help="Outputs only mean values across MPI ranks")


    return parser.parse_args(args=input_arguments)

def read_mpi_ranks(directory_path: Path,
                    input_name: str,
                ) -> int:
    """ Returns the no. of output files

    Reads the directory containing the output files to determine how many there are.

    Args:
        directory_path: The path to the directory containing the files to be opened later.
        input_name:     The name of the vernier output files.    

    Returns:
        The length of the 'files' list, which is equal to the number of vernier outputs in the directory         
    """

    files = glob.glob(f"{directory_path}/{input_name}*")

    return len(files)

def read_and_pre_process(file_path: Path,
                         rank: int, 
                         input_name: str,
                     ) -> pd.DataFrame:
    """ Reads a vernier-output and processes it 

    Reads in the current vernier-output file for a given rank before removing
    whitespace and formatting into a pandas dataframe.

    Args:
        file_path:  The path where the vernier outputs are located.
        rank:       The current output file to open, as different output files 
                    are ordered according to MPI rank.
        input_name: The name of the vernier output files without the rank.

    Returns:
        A Pandas dataframe containing the processed vernier output data.
        In the current implementation it contains only the name of the routine
        and corresponding 'Self' and 'Total' values.
    """

    """ Reads in the current file """
    file = open(f'{file_path}/{input_name}{rank}')
    content = file.read()

    """ Removes information from beginning of file """
    del_index = re.search("  1", content).start()
    content=content[del_index:]

    """ Create a new dataframe based on the pruned output file """
    header_names = ["index", "%Time", "Cumul", "Self", "Total", "Calls", "Se/cl", "To/cl", "Routine"]
    dataframe = pd.read_csv(sio(content), sep=r"\s+", engine="python", names=header_names)

    """ Organises the new dataframe """
    dataframe = dataframe.sort_values(by="Routine")
    dataframe = dataframe.reset_index(drop=True)
    temp_dataframe = dataframe[["Total", "Self", "Routine"]]

    return temp_dataframe

def merge_and_analyse(file_path: Path,
                      mpiranks: int,
                      input_name: str,
                      basic_output_bool: bool
                  ) -> pd.DataFrame:
    """ Reads in the files and merges them 

    Iterates the 'rank' variable, opening all of the vernier outputs using the 'read_and_pre_process'
    function. It will copy the first output file to the prev_df dataframe, then add all the other files
    before averaging them.

    Args:
        file_path:         The path where the vernier outputs are located.
        mpiranks:          The number of mpi ranks (equivalent to the number of files) to iterate through.
        input_name:        The name of the vernier output files without the rank.
        basic_output_bool: A boolean which if set to True will not calculate minimum/ maximum values. 

    Returns:
        The merged dataframe, containing the routine names and the mean 'Self' and 'Total' values across all outputs.
    """

    print(f"Path to open: {file_path}")
    print(f"Detected {mpiranks} files.")

    for rank in range(0,mpiranks):

        """ Open the file, read it, workout where it actually starts """
        dataframe = read_and_pre_process(file_path, rank, input_name)

        if rank == 0:     

            prev_df = dataframe.copy()
            if not(basic_output_bool):
                min_df  = dataframe.copy()
                max_df  = dataframe.copy()

        else:

            """ Adds the new loaded dataframe to the previous one """
            new_df = prev_df.add(dataframe)
            new_df["Routine"] = prev_df["Routine"]

            """ Calculates new min/ max values """
            if not(basic_output_bool):
                for column in dataframe.columns:
                    min_df[column] = min_df[column].where(min_df[column] < dataframe[column], dataframe[column])
                    max_df[column] = max_df[column].where(max_df[column] > dataframe[column], dataframe[column])

            prev_df = new_df.copy()

    """ Averages the summed dataframe """    
    mean_df = prev_df.drop(columns=["Routine"]) / int(mpiranks)
    mean_df["Routine"] = prev_df["Routine"]

    """ Adds the min/ max values to the mean dataframe and renames columns """

    for column in mean_df.drop(columns=["Routine"]):
        mean_df[f"Mean_{column}"] = mean_df[column]
        if not(basic_output_bool):
            mean_df[f"Min_{column}"]  = min_df[column]
            mean_df[f"Max_{column}"]  = max_df[column]
        mean_df = mean_df.drop(columns=[f"{column}"])  

    return mean_df

def main():

    """ Read in command line arguments, assigning them to variables. Determine how many outputs to merge """
    args = parse_cli_arguments()
    file_path = args.path
    merged_file_name = args.output_name
    input_name = args.input_name
    basic_output_bool = args.basic_output
    decimals = args.decimals
    mpiranks = read_mpi_ranks(file_path, input_name)

    if decimals >= 4:

        print("WARNING: Vernier typically reports values to 3 d.p, so calculated values may not be accurate or representative at higher precisions and may not display correctly")

    if mpiranks == 0:

        print("Error, no vernier-outputs detected")
        print("Searched in: ", file_path)

    else:

        print("\nReading and Merging...")


        merged_frame = merge_and_analyse(file_path, int(mpiranks), input_name, basic_output_bool)

        thread_string = "@0" 
        merged_frame["Routine"] = merged_frame["Routine"].str.replace(thread_string, '')
        merged_frame = merged_frame.round(decimals)
        print("\nWriting...")
        with open(f"{merged_file_name}", 'w') as f:
                  f.write(merged_frame.to_string(index=False, col_space=10))

        print(f"Merged outputs written to {merged_file_name}\n")


if __name__ == '__main__':
    main()
