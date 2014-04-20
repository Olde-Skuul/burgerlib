@echo off

:: System setup script for setting up a PC for CodeBlocks
:: Written by Rebecca Ann Heineman

:: First step, set up the location of CodeBlocks

if not "%CODEBLOCKS%" == "" goto codeblocksvarpresent
	if exist "%SystemDrive%\Program Files (x86)" goto codeblocks64
	set CODEBLOCKS=%SystemDrive%\Program Files\CodeBlocks
	goto codeblocks32
:codeblocks64
	set CODEBLOCKS=%SystemDrive%\Program Files (x86)\CodeBlocks
:codeblocks32
	echo Setting CODEBLOCKS=%CODEBLOCKS%
	:: Testing if administrator privledges are present
	net session >nul 2>&1
	if %errorLevel% == 0 goto codeblocksadminokay
	set CODEBLOCKS=
	goto requiredadministratorrights
:codeblocksadminokay
	setx CODEBLOCKS "%CODEBLOCKS%" /M
:codeblocksvarpresent

:: Detect if CodeBlocks is installed
if exist "%CODEBLOCKS%\codeblocks.exe" goto codeblocksinstalled

:: Install Codeblocks
:: Download the installer if needed
if exist "%~dp0codeblocks-13.12-setup.exe" goto codeblocksdownloaded
call :createloader "http://iweb.dl.sourceforge.net/project/codeblocks/Binaries/13.12/Windows/codeblocks-13.12-setup.exe" "%~dp0codeblocks-13.12-setup.exe"
echo Downloading codeblocks-13.12, this may take a while...
cscript /b foo.vbs
del foo.vbs
:codeblocksdownloaded

:: Execute the installer
"%~dp0codeblocks-13.12-setup.exe" /S
if %errorlevel% == 0 goto codeblocksinstalledokay
:: For some odd reason, the CodeBlocks installer returns 2 on a good install
if %errorlevel% == 2 goto codeblocksinstalledokay
echo Error in installing Codeblocks. Manually install %~dp0codeblocks-13.12-setup.exe
exit /b 10

:: Dispose of the installer
:codeblocksinstalledokay
del /q "%~dp0codeblocks-13.12-setup.exe"

:: Exit okay!
:codeblocksinstalled
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

