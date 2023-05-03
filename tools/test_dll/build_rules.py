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
# pylint: disable=consider-using-f-string
# pylint: disable=global-statement

from __future__ import absolute_import, print_function, unicode_literals

import sys
from burger import clean_directories, clean_files, is_under_git_control

from makeprojects.enums import ProjectTypes

# Check if git is around
_GIT_FOUND = None

# Name of the project, default is the directory name
PROJECT_NAME = "test_dll"

# Type of the project, default is ProjectTypes.tool
PROJECT_TYPE = ProjectTypes.sharedlibrary

# Recommended IDE for the project. Default is IDETypes.default()
# PROJECT_IDE = IDETypes.vs2022

# Recommend target platform for the project.
# PROJECT_PLATFORM = PlatformTypes.windows

# If set to True, Don't parse directories in this folder when ``-r``
# is active.
# Can be overridden above
NO_RECURSE = True

# If any IDE file is present, cleanme and buildme will process them.
# Can be overridden above
PROCESS_PROJECT_FILES = False

########################################


def is_git(working_directory):
    """
    Detect if perforce or git is source control
    """

    global _GIT_FOUND

    if _GIT_FOUND is None:
        _GIT_FOUND = is_under_git_control(working_directory)
    return _GIT_FOUND

########################################


def clean(working_directory):
    """
    Delete temporary files.

    This function is called by ``cleanme`` to remove temporary files.

    On exit, return 0 for no error, or a non zero error code if there was an
    error to report.

    Args:
        working_directory
            Directory this script resides in.

    Returns:
        None if not implemented, otherwise an integer error code.
    """

    clean_directories(working_directory, (".vscode",
                                          "appfolder",
                                          "temp",
                                          "ipch",
                                          "bin",
                                          ".vs",
                                          "*_Data",
                                          "* Data",
                                          "__pycache__"))

    clean_files(working_directory, (".DS_Store",
                                    "*.suo",
                                    "*.user",
                                    "*.ncb",
                                    "*.err",
                                    "*.sdf",
                                    "*.layout.cbTemp",
                                    "*.VC.db",
                                    "*.pyc",
                                    "*.pyo"))

########################################


def configuration_list(platform, ide):
    """
    Return names of configurations.

    The default is to generate configuration names of "Release",
    "Debug", etc. If an override is desired, return a list of strings
    containing the names of the default configurations.

    Args:
        platform: PlatformTypes of the platform being built
        ide: IDE project being generated for.
    Returns:
        None or list of configuration names.
    """

    # Only generate for the Release configuration
    # return ["Release"]
    if platform.is_windows():
        return ["Release_LTCG"]
    return ["Release"]

########################################


def project_settings(project):
    """
    Set up defines and default libraries.

    Adjust the default settings for the project to generate. Usually it's
    setting the location of source code or perforce support.

    Args:
        project: Project record to update.

    Returns:
        None, to continue processing, zero is no error and stop processing,
        any other number is an error code.
    """

    project.solution.perforce = not is_git(project.working_directory)

    platform = project.platform
    project.include_folders_list.append("../../source/lowlevel")

    project.deploy_folder = \
        "../../data/test_dlls/{}".format(platform.get_platform_folder())


# If called as a command line, replace 0 with a call the function
# for the default action. Return a numeric error code, or zero.
if __name__ == "__main__":
    sys.exit(0)
