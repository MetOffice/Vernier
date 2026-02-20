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
    parser.add_argument(
        "-f", "--filters",
        nargs="+",
        help="Filters to apply to the timers."
    )
    parser.add_argument(
        "-o", "--output",
        type=Path,
        default=None,
        help="Path to the output summary file."
    )

    return parser.parse_args()


if __name__ == "__main__":

    args = process_args()
    timers = VernierReader(args.vernier_output).load()

    if args.filters:
        timers = timers.filter(args.filters)

    timers.write_txt_output(args.output)