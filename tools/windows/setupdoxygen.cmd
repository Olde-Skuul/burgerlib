@echo off

:: System setup script for setting up a PC for installing Doxygen
:: Written by Rebecca Ann Heineman

:: First step, set up the location of Doxygen
if not "%DOXYGEN%" == "" goto doxygenvarpresent
	echo Setting DOXYGEN="%ProgramFiles%\doxygen"
	:: Testing if administrator privledges are present
	net session >nul 2>&1
	if not %errorLevel% == 0 goto requiredadministratorrights
	setx DOXYGEN "%ProgramFiles%\doxygen" /M
	set DOXYGEN=%ProgramFiles%\doxygen
:doxygenvarpresent

: Detect if Doxygen 1.8.6 is installed

if not exist "%DOXYGEN%\bin\doxygen.exe" goto installdoxygen
type "%DOXYGEN%\version.txt" | findstr /i /r "1.8.6" >nul
if %errorlevel% == 0 goto doxygeninstalled

:: Download the installer if needed
:installdoxygen
if exist "%~dp0doxygen-1.8.6-setup.exe" goto doxygendownloaded
call :createloader "http://ftp.stack.nl/pub/users/dimitri/doxygen-1.8.6-setup.exe" "%~dp0doxygen-1.8.6-setup.exe"
echo Downloading Doxygen, this may take a while...
cscript /b foo.vbs
del foo.vbs
:doxygendownloaded

:: Execute the installer
"%~dp0doxygen-1.8.6-setup.exe" /SP- /VERYSILENT /SUPPRESSMSGBOXES
if %errorlevel% == 0 goto doxygeninstalledokay
echo Error in installing Doxygen. Manually install %~dp0doxygen-1.8.6-setup.exe
exit /b 10

:: Dispose of the installer
:doxygeninstalledokay
del /q "%~dp0doxygen-1.8.6-setup.exe"

:: Exit okay!
:doxygeninstalled
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



