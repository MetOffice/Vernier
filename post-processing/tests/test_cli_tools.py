# ------------------------------------------------------------------------------
#  (c) Crown copyright Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
from pathlib import Path
import unittest
import subprocess
import sys

class TestCLITools(unittest.TestCase):

    def setUp(self):
        self.tools_dir = Path(__file__).parent.parent / 'tools'
        self.test_data_dir = Path(__file__).parent / 'data'
        self.test_data_kgo = (
            '|      Routine | Total time (s) |     Self (s) | Cumul time (s) | No. calls |   % time | Time per call (s) |\n' +
            '| __test_app__ |         6.3465 |       2.5855 |         4.0895 |         1 |  40.9825 |            6.3465 |\n' +
            '| some_process |          2.573 |        2.516 |         3.8075 |         2 |   39.277 |            1.2865 |\n'
            )
        return super().setUp()

    def test_summarise_vernier_file(self):
        result = subprocess.run(
            [str(self.tools_dir / 'summarise_vernier.py'),
             str(self.test_data_dir / 'vernier-output-test')],
            capture_output=True,
            text=True
        )
        self.assertEqual(result.returncode, 0)
        self.assertEqual(self.test_data_kgo, result.stdout)

    def test_summarise_vernier_dir(self):
        result = subprocess.run(
            [str(self.tools_dir / 'summarise_vernier.py'),
             str(self.test_data_dir / 'vernier-output')],
            capture_output=True,
            text=True
        )
        self.assertEqual(result.returncode, 0)
        self.assertEqual(self.test_data_kgo, result.stdout)

    def test_summarise_vernier_noinput(self):
        result = subprocess.run(
            [str(self.tools_dir / 'summarise_vernier.py')],
            capture_output=True,
            text=True
        )
        self.assertEqual(result.returncode, 2)