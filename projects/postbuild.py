#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import subprocess
import shutil
import sys
import burger
	
#
# Build the burgerlib headers
#

def main(workingDir):

	# Was Burgerlib already installed?
	
	sdks = burger.getsdksfolder()

	# XCode and CodeWarrior Mac only runs under Mac OSX
	# Abort on Windows/Linux/MacOSX Intel only hosts

	# Update the codewarrior files
	
	windowslibfiles = [
		'burgerlibcw9w32rel.lib',
		'burgerlibcw9w32dbg.lib',
		'burgerlibcw9w32int.lib' ]

	windowsburgerbase = os.path.join(sdks,'windows','burgerlib')
	outputfolder = os.path.join(workingDir,'bin')
	for windowslib in windowslibfiles:
		error = burger.copyfileandcheckoutifneeded(os.path.join(outputfolder,windowslib),os.path.join(windowsburgerbase,windowslib))
		if error!=0:
			break
	
	return error
		
# 
# If called as a function and not a class,
# call my main
#

if __name__ == "__main__":
	sys.exit(main(os.path.dirname(os.path.abspath(__file__))))

