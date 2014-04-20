@echo off

:: System setup script for setting up a PC for developing with Burgerlib
:: Written by Rebecca Ann Heineman

:: Point to the SDKS folder

if not "%SDKS%" == "" goto sdkspresent
	echo Setting SDKS="%~dp0sdks"

	:: Testing if administrator privledges are present
	net session >nul 2>&1
	if not %errorLevel% == 0 goto requiredadministratorrights
	setx SDKS "%~dp0sdks" /M
	set SDKS="%~dp0sdks"
:sdkspresent


:: Enable Visual studio to pull microsoft symbols from the symbol server
:: Invoke from a standalone command

call "%~dp0tools\windows\setupsymbolpath.cmd"
if not %errorLevel% == 0 goto scriptfailure

:: Install Python 2.7.4

call "%~dp0tools\windows\setuppython27.cmd"
if not %errorLevel% == 0 goto scriptfailure

:: Install CodeBlocks

call "%~dp0tools\windows\setupcodeblocks.cmd"
if not %errorLevel% == 0 goto scriptfailure

:: Install Perforce Client

call "%~dp0tools\windows\setupperforce.cmd"
if not %errorLevel% == 0 goto scriptfailure

:: Install Open Watcom

call "%~dp0tools\windows\setupwatcom.cmd"
if not %errorLevel% == 0 goto scriptfailure

:: Install Graphviz

call "%~dp0tools\windows\setupgraphviz.cmd"
if not %errorLevel% == 0 goto scriptfailure

:: Install Doxygen

call "%~dp0tools\windows\setupdoxygen.cmd"
if not %errorLevel% == 0 goto scriptfailure

:: Install HTML Help Workshop

call "%~dp0tools\windows\setuphtmlhelp.cmd"
if not %errorLevel% == 0 goto scriptfailure

:: Install DirectX SDK June 2010

call "%~dp0tools\windows\setupdirectxsdk.cmd"
if not %errorLevel% == 0 goto scriptfailure


::http://gitextensions.googlecode.com/files/GitExtensions244SetupComplete.msi

:: Notify Python that it should have access to Burgerlib python libraries

if not "%PYTHONPATH%" == "" goto pythonpathvarpresent
	echo Setting PYTHONPATH="%sdks%\python"
	:: Testing if administrator privledges are present
	net session >nul 2>&1
	if not %errorLevel% == 0 goto requiredadministratorrights
	setx PYTHONPATH "%sdks%\python" /M
	set PYTHONPATH=%sdks%\python
:pythonpathvarpresent

echo Your system is all set for building Burgerlib based code!

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

:: Script failure?
:scriptfailure
pause
exit /b 10


:: Detect if on a 64 bit (amd64) machine

if exist "%SystemDrive%\Program Files (x86)" goto code64

:: Install 32 bit specific code
echo 32 bit OS detected


:: Set the location of Codewarrior 9.4 (If present)
set CWFolder=%ProgramFiles%\Metrowerks\Codewarrior
setx CWFolder "%ProgramFiles%\Metrowerks\Codewarrior"

setx path "%%sdks%%\w32burgertools"

goto :aftercode64





:code64
:: Install 64 bit specific code
echo 64 bit OS detected

:: Set the location of Codewarrior 9.4 (If present)
set CWFolder=%ProgramFiles(x86)%\Metrowerks\Codewarrior
setx CWFolder "%ProgramFiles(x86)%\Metrowerks\Codewarrior"

setx path "%%sdks%%\w64burgertools;%%sdks%%\w32burgertools"

:aftercode64


:: Install Codewarrior 9.0
if not exist "%CWFolder%\Bin\ide.exe" goto foundcw9
::call :createloader "http://www.burgerbecky.com/hosted/cw_windows_9.2.exe" "%~dp0cw_windows_9.2.exe"
echo Downloading Codewarrion 9.2
::cscript foo.vbs
::del foo.vbs
::del "%~dp0cw_windows_9.2.exe"
:foundcw9


