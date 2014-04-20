@echo off
if not "%PYTHON%" == "" goto pythonvarpresent
	set PYTHON=%SystemDrive%\Python27
:pythonvarpresent
"%PYTHON%\python.exe" "%SDKS%\python\makeprojects.py" %*