@echo off

:: System setup script for setting up a PC with Perforce
:: Written by Rebecca Ann Heineman

:: First step, set up the location of Perforce

if not "%PERFORCE%" == "" goto perforcevarpresent
	echo Setting PERFORCE="%ProgramFiles%\Perforce"
	:: Testing if administrator privledges are present
	net session >nul 2>&1
	if not %errorLevel% == 0 goto requiredadministratorrights
	setx PERFORCE "%ProgramFiles%\Perforce" /M
	set PERFORCE=%ProgramFiles%\Perforce
:perforcevarpresent

:: Set up a global for perforce to force it to use p4config files.

if not "%P4CONFIG%" == "" goto p4configvarpresent
	echo Setting P4CONFIG="p4config"
	:: Testing if administrator privledges are present
	net session >nul 2>&1
	if not %errorLevel% == 0 goto requiredadministratorrights
	setx P4CONFIG "p4config" /M
	set P4CONFIG=p4config
:p4configvarpresent

: Detect if Perforce 2013 is installed

if not exist "%PERFORCE%\p4.exe" goto installperforce
"%PERFORCE%\p4.exe" -V 2>&1 | findstr /i /r "\/2014." >nul
if %errorlevel% == 0 goto perforceinstalled

:: 32 or 64 bit version of Perforce?
:installperforce
if exist "%SystemDrive%\Program Files (x86)" goto perforce64
set PERFORCEINSTALLER=p4vinst.exe
set PERFORCEHTTP=http://filehost.perforce.com/perforce/r14.1/bin.ntx86/p4vinst.exe
goto perforce32
:perforce64
set PERFORCEINSTALLER=p4vinst64.exe
set PERFORCEHTTP=http://filehost.perforce.com/perforce/r14.1/bin.ntx64/p4vinst64.exe
:perforce32

:: Download the installer if needed
if exist "%~dp0%PERFORCEINSTALLER%" goto perforcedownloaded
call :createloader "%PERFORCEHTTP%" "%~dp0%PERFORCEINSTALLER%"
echo Downloading Perforce, this may take a while...
cscript /b foo.vbs
del foo.vbs
:perforcedownloaded

:: Execute the installer
"%~dp0%PERFORCEINSTALLER%" /s /v"/qn"
if %errorlevel% == 0 goto perforceinstalledokay
echo Error in installing Perforce. Manually install %~dp0%PERFORCEINSTALLER%
exit /b 10

:: Dispose of the installer
:perforceinstalledokay
del /q "%~dp0%PERFORCEINSTALLER%"

:: Exit okay!
:perforceinstalled
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
