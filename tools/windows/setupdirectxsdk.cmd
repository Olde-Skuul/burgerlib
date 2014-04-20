@echo off

:: System setup script for setting up a PC for using DirectX SDK June 2010
:: Written by Rebecca Ann Heineman

:: First step, set up the location of DirectX SDK
if not "%DXSDK_DIR%" == "" goto directxsdkvarpresent
	if exist "%SystemDrive%\Program Files (x86)" goto directxsdk64
	set DXSDK_DIR=%SystemDrive%\Program Files\Microsoft DirectX SDK (June 2010)
	goto directxsdk32
:directxsdk64
	set DXSDK_DIR=%SystemDrive%\Program Files (x86)\Microsoft DirectX SDK (June 2010)
:directxsdk32
	echo Setting DXSDK_DIR=%DXSDK_DIR%
	:: Testing if administrator privledges are present
	net session >nul 2>&1
	if %errorLevel% == 0 goto directxsdkadminokay
	set DXSDK_DIR=
	goto requiredadministratorrights
:directxsdkadminokay
	setx DXSDK_DIR "%DXSDK_DIR%" /M
:directxsdkvarpresent

:: Detect if DirectX is installed
if exist "%DXSDK_DIR%\Include\xnamath.h" goto directxsdkinstalled

:: Install DirectX SDK
:: Download the installer if needed
if exist "%~dp0DXSDK_Jun10.exe" goto directxsdkdownloaded
call :createloader "http://download.microsoft.com/download/A/E/7/AE743F1F-632B-4809-87A9-AA1BB3458E31/DXSDK_Jun10.exe" "%~dp0DXSDK_Jun10.exe"
echo Downloading DXSDK_Jun10.exe, this may take a while...
cscript /b foo.vbs
del foo.vbs
:directxsdkdownloaded

:: Execute the installer
"%~dp0DXSDK_Jun10.exe" /U
if %errorlevel% == 0 goto directxsdkinstalledokay
echo Error in installing Direct X SDK. Manually install %~dp0DXSDK_Jun10.exe
exit /b 10

:: Dispose of the installer
:directxsdkinstalledokay
del /q "%~dp0DXSDK_Jun10.exe"

:: Exit okay!
:directxsdkinstalled
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


