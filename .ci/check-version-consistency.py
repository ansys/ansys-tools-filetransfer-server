#!/usr/bin/env python

# Copyright (C) 2022 - 2025 ANSYS, Inc. and/or its affiliates.
# SPDX-License-Identifier: MIT
#
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""Checks that the version number of the project is consistent everywhere.

The version of the project is required in multiple places, without
an easy way to single-source it. To ensure consistency between the version
numbers, this script checks that they are all the same.

Note that this does not include the vtkEven version used in ACP, which can
be different.
"""

import sys
import re
import pathlib

ROOT_DIR = pathlib.Path(__file__).resolve().parent.parent
VERSION_FILE = ROOT_DIR / "VERSION"
DOXYGEN_CONF = ROOT_DIR / "doc" / "doxygen" / "Doxyfile"

ALL_FILES = [VERSION_FILE, DOXYGEN_CONF]


def check_files_exist():
    inexistent_files = [str(f) for f in ALL_FILES if not f.exists()]
    if inexistent_files:
        print(
            f"The following expected files do not exist: {', '.join(inexistent_files)}."
        )
        sys.exit(1)


def check_versions_match():
    regex_by_file = {
        VERSION_FILE: r"([0-9]*).([0-9]*).([0-9a-z\.]*)",
        DOXYGEN_CONF: r"PROJECT_NUMBER[\s]*=[\s]*\"v([0-9]*).([0-9]*).([0-9a-z\.]*)\"",
    }
    version_by_file = {}
    for file, regex_pattern in regex_by_file.items():
        with open(file, "r", encoding="utf-8") as in_f:
            file_content = in_f.read()
        match_obj = re.search(regex_pattern, file_content)
        if match_obj is None:
            print(f"Could not determine version for {file}")
            sys.exit(1)
        version_by_file[file] = (
            f"{match_obj.group(1)}.{match_obj.group(2)}.{match_obj.group(3)}"
        )

    if len(set(version_by_file.values())) > 1:
        msg = "Versions are not equal:\n"
        for file, version in version_by_file.items():
            msg += f"{version} from {file}\n"
        print(msg)
        sys.exit(1)


def main():
    check_files_exist()
    check_versions_match()


if __name__ == "__main__":
    main()
