import os
import pandas as pd
from io import StringIO as sio
import re
import sys
import argparse
from pathlib import Path
import glob

""" Global Variables """

file_name = "vernier-output-"

""" Main funcs """

def parse_cli_arguments(arguments: list[str] = None):

    """ Parses command line arguments for running the script in the terminal """

    parser = argparse.ArgumentParser()
    parser.add_argument("-path",          type=Path,  default=(os.getcwd()),                help="Path to Vernier output files")
    parser.add_argument("-outputname",    type=str,   default=str("vernier-merged-output"), help="Name of file to write to.")

    return parser.parse_args(args=arguments)

def read_mpi_ranks(current_dir_path):

    """ Reads the number of vernier-output files from the given directory to determine no. of ranks to use """

    files = glob.glob(f"{current_dir_path}/{file_name}*")

    return len(files)

def read_and_pre_process(file_path, rank):

    """ Reads in the current file """

    file = open(f'{file_path}/{file_name}{rank}')
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

def merge_and_analyse(file_path, mpiranks):

    """ Reads in the files and performs analysis on them """

    print(f"Path to open: {file_path}")
    print(f"Detected {mpiranks} files.")

    for rank in range(0,mpiranks):

        """ Open the file, read it, workout where it actually starts """

        dataframe = read_and_pre_process(file_path, rank)

        if rank == 0:     

            prev_df = dataframe.copy()

        else:

            """ Adds the new loaded dataframe to the previous one, resorting and reorganising the indices every time """

            new_df = prev_df.add(dataframe)              
            new_df["Routine"] = prev_df["Routine"]                  
            new_df=new_df.sort_values(by="Routine")
            new_df = new_df.reset_index(drop=True)

            prev_df = new_df.copy()
                
    """ Averages the summed dataframe """
        
    mean_df = prev_df.drop(columns=["Routine"]) / int(mpiranks)
    mean_df["Routine"] = prev_df["Routine"]

    return mean_df


def crop_numbers(value):

    """ Provides a cropping function for the final average output """

    if isinstance(value, (int, float)):
        return float(str(value)[:5])
    
    return value

   
    
def main():

    args = parse_cli_arguments()
    file_path = args.path
    merged_file_name = args.outputname

    mpiranks = read_mpi_ranks(f'{file_path}')

    if mpiranks == 0:

        print("Error, no vernier-outputs detected")
        print("Searched in: ", file_path)

    else:

        print("\nReading and Merging...")

        merged_frame = merge_and_analyse(file_path, int(mpiranks))

        
 


        print("\nWriting...")
        with open(f"{merged_file_name}", 'w') as f:
                  f.write(merged_frame.to_string(index=False, col_space=10))

        print(f"Merged outputs written to {merged_file_name}\n")


main()








