import os
import pandas as pd
from io import StringIO as sio
import re
import sys
import argparse
from pathlib import Path
import glob

def parse_cli_arguments(arguments: list[str] = None) -> argparse.ArgumentParser:

    """ Parses command line arguments for running the script in the terminal """
    parser = argparse.ArgumentParser()
    parser.add_argument("-path",          type=Path,  default=(os.getcwd()),                help="Path to Vernier output files")
    parser.add_argument("-outputname",    type=str,   default=str("vernier-merged-output"), help="Name of file to write to.")
    parser.add_argument("-inputname",     type=str,   default=str("vernier-output-"),       help="Vernier files to read from.")

    return parser.parse_args(args=arguments)

def read_mpi_ranks(current_dir_path: Path, input_name: str) -> int:

    """ Reads the number of vernier-output files from the given directory to determine no. of ranks to use """
    files = glob.glob(f"{current_dir_path}/{input_name}*")

    return len(files)

def read_and_pre_process(file_path: Path,
                         rank: int, 
                         input_name: str,
                     ) -> pd.DataFrame:
    """ Reads a vernier-output and processes it 
    
    Reads in the current vernier-output file for a given rank before removing
    whitespace and formatting into a pandas dataframe.

    Args:
        file_path:  The path where the vernier outputs are located
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

def merge_and_analyse(file_path: Path, mpiranks: int, input_name: str) -> pd.DataFrame:

    """ Reads in the files and merges them """
    print(f"Path to open: {file_path}")
    print(f"Detected {mpiranks} files.")

    for rank in range(0,mpiranks):

        """ Open the file, read it, workout where it actually starts """
        dataframe = read_and_pre_process(file_path, rank, input_name)

        if rank == 0:     

            prev_df = dataframe.copy()
            min_df = dataframe.copy()
            max_df = dataframe.copy()
                    
        else:

            """ Adds the new loaded dataframe to the previous one, resorting and reorganising the indices every time """
            new_df = prev_df.add(dataframe)
            new_df["Routine"] = prev_df["Routine"]           
            prev_df = new_df.copy()
                
    """ Averages the summed dataframe """    
    mean_df = prev_df.drop(columns=["Routine"]) / int(mpiranks)
    mean_df["Routine"] = prev_df["Routine"]

    return mean_df

def main():

    """ Read in command line arguments, assigning them to variables. Determine how many outputs to merge """
    args = parse_cli_arguments()
    file_path = args.path
    merged_file_name = args.outputname
    input_name = args.inputname
    mpiranks = read_mpi_ranks(file_path, input_name)

    if mpiranks == 0:

        print("Error, no vernier-outputs detected")
        print("Searched in: ", file_path)

    else:

        print("\nReading and Merging...")

        merged_frame = merge_and_analyse(file_path, int(mpiranks), input_name)


        print("\nWriting...")
        with open(f"{merged_file_name}", 'w') as f:
                  f.write(merged_frame.to_string(index=False, col_space=10))

        print(f"Merged outputs written to {merged_file_name}\n")


if __name__ == '__main__':
    main()
