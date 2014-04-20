@echo off

:: System setup script for setting up a PC for using HTML Help Workshop
:: Written by Rebecca Ann Heineman

:: First step, set up the location of Help Workshop
if not "%HTMLHELP%" == "" goto htmlhelpvarpresent
	if exist "%SystemDrive%\Program Files (x86)" goto htmlhelp64
	set HTMLHELP=%SystemDrive%\Program Files\HTML Help Workshop
	goto htmlhelp32
:htmlhelp64
	set HTMLHELP=%SystemDrive%\Program Files (x86)\HTML Help Workshop
:htmlhelp32
	echo Setting HTMLHELP=%HTMLHELP%
	:: Testing if administrator privledges are present
	net session >nul 2>&1
	if %errorLevel% == 0 goto htmlhelpadminokay
	set HTMLHELP=
	goto requiredadministratorrights
:htmlhelpadminokay
	setx HTMLHELP "%HTMLHELP%" /M
:htmlhelpvarpresent

:: Detect if HTML Help Workshop is installed
if exist "%HTMLHELP%\hhc.exe" goto htmlhelpinstalled

:: Install HTML Help Workshop
:: Download the installer if needed
if exist "%~dp0htmlhelp.exe" goto htmlhelpdownloaded
call :createloader "http://download.microsoft.com/download/0/A/9/0A939EF6-E31C-430F-A3DF-DFAE7960D564/htmlhelp.exe" "%~dp0htmlhelp.exe"
echo Downloading htmlhelp.exe, this may take a while...
cscript /b foo.vbs
del foo.vbs
:htmlhelpdownloaded

:: Execute the installer
"%~dp0htmlhelp.exe" /Q:A
if %errorlevel% == 0 goto htmlinstalledokay
echo Error in installing HTML Help Workshop. Manually install %~dp0htmlhelp.exe
exit /b 10

:: Dispose of the installer
:htmlinstalledokay
del /q "%~dp0htmlhelp.exe"

:: Exit okay!
:htmlhelpinstalled
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


