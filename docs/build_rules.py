#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Build rules for the makeprojects suite of build tools.

This file is parsed by the cleanme, buildme, rebuildme and makeprojects
command line tools to clean, build and generate project files.

When any of these tools are invoked, this file is loaded and parsed to
determine special rules on how to handle building the code and / or data.
"""

# pylint: disable=unused-argument

from __future__ import absolute_import, print_function, unicode_literals

import sys
import os

from burger import import_py_script

# If set to True, ``buildme -r``` will not parse directories in this folder.
BUILDME_NO_RECURSE = True

# ``buildme``` will build these files and folders first.
BUILDME_DEPENDENCIES = []

# If set to True, ``cleanme -r``` will not parse directories in this folder.
CLEANME_NO_RECURSE = True

# ``cleanme`` will clean the listed folders using their rules before cleaning.
# this folder.
CLEANME_DEPENDENCIES = []

########################################


def prebuild(working_directory, configuration):
    """
    Perform actions before building any IDE based projects.

    This function is called before any IDE or other script is invoked. This is
    perfect for creating headers or other data that the other build projects
    need before being invoked.

    On exit, return 0 for no error, or a non zero error code if there was an
    error to report.

    Args:
        working_directory
            Directory this script resides in.

        configuration
            Configuration to build, ``all`` if no configuration was requested.

    Returns:
        None if not implemented, otherwise an integer error code.
    """

    # Create the super header for extracting documentation.
    # Update the changelist header
    root_folder = os.path.dirname(working_directory)
    script = import_py_script(os.path.join(root_folder, 'build_rules.py'))
    if not script:
        return 10

    error = script.prebuild(root_folder, 'all')
    return error

########################################


# If called as a command line and not a class, perform the build
if __name__ == "__main__":
    WORKING_DIRECOTORY = os.path.dirname(os.path.abspath(__file__))
    sys.exit(prebuild(WORKING_DIRECOTORY, 'all'))
