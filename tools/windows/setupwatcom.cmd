@echo off

:: System setup script for setting up a PC for Open Watcom 1.9
:: Written by Rebecca Ann Heineman

:: First step, set up the location of Watcom

if not "%WATCOM%" == "" goto watcomvarpresent
	echo Setting WATCOM="%SystemDrive%\WATCOM"
	:: Testing if administrator privledges are present
	net session >nul 2>&1
	if not %errorLevel% == 0 goto requiredadministratorrights
	setx WATCOM "%SystemDrive%\WATCOM" /M
	set WATCOM=%SystemDrive%\WATCOM
:watcomvarpresent

:: Detect if Open Watcom 1.9 is installed

if not exist "%WATCOM%\binnt\exe2bin.exe" goto installwatcom
"%WATCOM%\binnt\exe2bin.exe" -V 2>&1 | findstr /i /r "Version\ 1.9" >nul
if %errorlevel% == 0 goto watcominstalled

:: Install Open Watcom 1.9
:: Download the installer if needed
:installwatcom
if exist "%~dp0open-watcom-c-win32-1.9.exe" goto watcomdownloaded
call :createloader "http://openwatcom.mirrors.pair.com/open-watcom-c-win32-1.9.exe" "%~dp0open-watcom-c-win32-1.9.exe"
echo Downloading Open Watcom 1.9, this may take a while...
cscript /b foo.vbs
del foo.vbs
:watcomdownloaded

:: Execute the installer
"%~dp0open-watcom-c-win32-1.9.exe" -ns -i -s -dFullInstall=1
if %errorlevel% == 0 goto watcominstalledokay
echo Error in installing Watcom. Manually install %~dp0open-watcom-c-win32-1.9.exe
exit /b 10

:: Dispose of the installer
:watcominstalledokay
del /q "%~dp0open-watcom-c-win32-1.9.exe"

:: Exit okay!
:watcominstalled
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

