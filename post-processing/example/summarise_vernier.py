# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
from pathlib import Path
import argparse
import sys
sys.path.append(str(Path(__file__).parent.parent / "lib"))
from vernier import VernierReader


def process_args():
    """
    Take the vernier output path as a command-line argument.
    """
    parser = argparse.ArgumentParser(
        description="Process Vernier output file and generate summary."
    )
    parser.add_argument(
        "vernier_output",
        type=Path,
        help="Path to the Vernier output file or directory."
    )

    return parser.parse_args()


if __name__ == "__main__":

    args = process_args()
    timers = VernierReader(args.vernier_output).load()
    timers.write_txt_output()
