# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
"""
Module to hold tests for CLI tools.
"""
from pathlib import Path
import unittest
import subprocess


class TestCLITools(unittest.TestCase):
    """
    Unittest class for testing CLI tools.
    """
    def setUp(self):
        """
        Initialise useful attributes for testing.
        """
        self.tools_dir = Path(__file__).parent.parent / 'vernier' / 'tools'
        self.test_data_dir = Path(__file__).parent / 'data'
        # pylint: disable=line-too-long
        self.test_data_kgo = (
            '|      Routine |   Total Min(s) |  Total Mean(s) |   Total Max(s) |    Self Min(s) |   Self Mean(s) |    Self Max(s) | Max no. calls |   % time  |  Time per call(s) |\n' +
            '| __test_app__ |          5.854 |         6.3465 |          6.839 |          2.583 |         2.5855 |          2.588 |             1 |  40.9825  |            6.3465 |\n' +
            '| some_process |          2.077 |          2.573 |          3.069 |          2.023 |          2.516 |          3.009 |             2 |   39.277  |            1.2865 |\n'
            )
        return super().setUp()

    def test_summarise_vernier_file(self):
        """
        Tests the summarise_vernier python script on a single file and
        compares the output to a KGO.
        """
        result = subprocess.run(
            [str(self.tools_dir / 'summarise_vernier.py'),
             str(self.test_data_dir / 'vernier-output-test')],
            capture_output=True,
            text=True,
            check=False
        )
        self.assertEqual(result.returncode, 0)
        self.assertEqual(self.test_data_kgo, result.stdout)

    def test_summarise_vernier_dir(self):
        """
        Tests the summarise_vernier python script on a directory and compares
        the output to a KGO.
        """
        result = subprocess.run(
            [str(self.tools_dir / 'summarise_vernier.py'),
             str(self.test_data_dir / 'vernier-output')],
            capture_output=True,
            text=True,
            check=False
        )
        self.assertEqual(result.returncode, 0)
        self.assertEqual(self.test_data_kgo, result.stdout)

    def test_summarise_vernier_noinput(self):
        """
        Tests that the correct error is raised when calling the
        summarise_vernier python script on an empty file.
        """
        result = subprocess.run(
            [str(self.tools_dir / 'summarise_vernier.py')],
            capture_output=True,
            text=True,
            check=False
        )
        self.assertEqual(result.returncode, 2)
