#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Configuration file on how to build and clean projects in a specific folder.

This file is parsed by the cleanme, buildme, rebuildme and makeprojects
command line tools to clean, build and generate project files.
"""

from __future__ import absolute_import, print_function, unicode_literals

import sys
import os

from burger import import_py_script

########################################


def do_prebuild(working_directory):
    """ Create the super header for extracting documentation.

    :Args:
        working_dir: Directory where the headers to copy are located

    Returns:
        Zero on no error, non-zero on error

    """

    # Update the changelist header
    root_folder = os.path.dirname(working_directory)
    script = import_py_script(os.path.join(root_folder, 'build_rules.py'))
    if not script:
        return 10

    error = script.do_prebuild(root_folder)
    return error

########################################


def rules(command, working_directory, **kargs):
    """
    Main entry point for build_rules.py.

    When ``makeprojects``, ``cleanme``, or ``buildme`` is executed, they will
    call this function to perform the actions required for build customization.

    The parameter ``working_directory`` is required, and if it has no default
    parameter, this function will only be called with the folder that this
    file resides in. If there is a default parameter of ``None``, it will be
    called with any folder that it is invoked on. If the default parameter is a
    directory, this function will only be called if that directory is desired.

    The optional parameter of ``root``` alerts the tool if subsequent processing
    of other ``build_rules.py`` files are needed or if set to have a default
    parameter of ``True``, processing will end once the calls to this
    ``rules()`` function are completed.

    Commands are 'build', 'clean', 'prebuild', 'postbuild', 'project',
    'configurations'

    Arg:
        command: Command to execute.
        working_directory: Directory for this function to clean
        root: If set to True, exit cleaning upon completion of this function
    Return:
        Zero on success, non-zero on failure, and a list for 'configurations'

    """

    # Unused arguments
    # pylint: disable=W0613

    # Perform the action based on the command.

    if command == 'prebuild':
        # Perform actions before building any IDE based projects
        # Return non zero integer on error.
        return do_prebuild(working_directory)

    return 0

########################################


# If called as a command line and not a class, perform the build
if __name__ == "__main__":
    WORKING_DIRECOTORY = os.path.dirname(os.path.abspath(__file__))
    sys.exit(do_prebuild(WORKING_DIRECOTORY))
