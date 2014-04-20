@echo off

:: System setup script for setting up a PC for Python 2.7.6
:: Written by Rebecca Ann Heineman

:: Install Python
:: First step, set up the location of Python

if not "%PYTHON%" == "" goto pythonvarpresent
	echo Setting PYTHON="%SystemDrive%\Python27"
	:: Testing if administrator privledges are present
	net session >nul 2>&1
	if not %errorLevel% == 0 goto requiredadministratorrights
	setx PYTHON "%SystemDrive%\Python27" /M
	set PYTHON=%SystemDrive%\Python27
:pythonvarpresent

:: Detect if python 2.7.x is installed

if not exist "%PYTHON%\python.exe" goto installpython
"%python%\python.exe" -V 2>&1 | findstr /i /r "^Python\ 2.7" >nul
if %errorlevel% == 0 goto pythoninstalled

:: Install python 2.7.6

:: 32 or 64 bit version of Python?
:installpython
if exist "%SystemDrive%\Program Files (x86)" goto python64
set PYTHONINSTALLER=python-2.7.6.msi
goto python32
:python64
set PYTHONINSTALLER=python-2.7.6.amd64.msi
:python32

:: Download the installer if needed
if exist "%~dp0%PYTHONINSTALLER%" goto pythondownloaded
call :createloader "http://www.python.org/ftp/python/2.7.6/%PYTHONINSTALLER%" "%~dp0%PYTHONINSTALLER%"
echo Downloading Python 2.7.6, this may take a while...
cscript /b foo.vbs
del foo.vbs
:pythondownloaded

:: Execute the installer
msiexec /passive /i "%~dp0%PYTHONINSTALLER%"
if %errorlevel% == 0 goto pythoninstalledokay
echo Error in installing Python. Manually install %~dp0%PYTHONINSTALLER%
exit /b 10

:: Dispose of the installer
:pythoninstalledokay
del /q "%~dp0%PYTHONINSTALLER%"

:: Exit okay!
:pythoninstalled
exit /b


:: Create a visual basic script that will load a file from the internet
:createloader
echo TheFileURL = %1 > foo.vbs
echo Output = %2 >> foo.vbs
echo Set objXMLHTTP = CreateObject("WinHttp.WinHttpRequest.5.1") >> foo.vbs
echo objXMLHTTP.open "GET", TheFileURL, false >> foo.vbs
echo objXMLHTTP.send() >> foo.vbs
echo If objXMLHTTP.Status = 200 Then >> foo.vbs
echo   Set objADOStream = CreateObject("ADODB.Stream") >> foo.vbs
echo   objADOStream.Open >> foo.vbs
echo   objADOStream.Type = 1  >> foo.vbs
echo   objADOStream.Write objXMLHTTP.ResponseBody >> foo.vbs
echo   objADOStream.Position = 0  >> foo.vbs
echo   Set objFSO = Createobject("Scripting.FileSystemObject") >> foo.vbs
echo   If objFSO.Fileexists(Output) Then objFSO.DeleteFile Output >> foo.vbs
echo   Set objFSO = Nothing >> foo.vbs
echo   objADOStream.SaveToFile Output >> foo.vbs
echo   objADOStream.Close >> foo.vbs
echo   Set objADOStream = Nothing >> foo.vbs
echo End if >> foo.vbs
echo Set objXMLHTTP = Nothing >> foo.vbs
exit /b

:: Epic fail in case an operation was needed that needed administrator rights
:requiredadministratorrights
echo This operation requires administrator rights to execute. Please re-run this
echo batch file from a console with administrator rights.
exit /b 10

