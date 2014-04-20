@echo off

:: System setup script for setting up a PC for using GraphViz
:: Written by Rebecca Ann Heineman

:: First step, set up the location of GraphViz
if not "%GRAPHVIZ%" == "" goto graphvizvarpresent
	if exist "%SystemDrive%\Program Files (x86)" goto graphviz64
	set GRAPHVIZ=%SystemDrive%\Program Files\GraphViz2.36
	goto graphviz32
:graphviz64
	set GRAPHVIZ=%SystemDrive%\Program Files (x86)\GraphViz2.36
:graphviz32
	echo Setting GRAPHVIZ=%GRAPHVIZ%
	:: Testing if administrator privledges are present
	net session >nul 2>&1
	if %errorLevel% == 0 goto graphvizadminokay
	set GRAPHVIZ=
	goto requiredadministratorrights
:graphvizadminokay
	setx GRAPHVIZ "%GRAPHVIZ%" /M
:graphvizvarpresent

:: Detect if GraphViz is installed
if exist "%GRAPHVIZ%\bin\dot.exe" goto graphvizinstalled

:: Install GraphViz
:: Download the installer if needed
if exist "%~dp0graphviz-2.36.msi" goto graphvizdownloaded
call :createloader "http://graphviz.org/pub/graphviz/stable/windows/graphviz-2.36.msi" "%~dp0graphviz-2.36.msi"
echo Downloading graphviz-2.36.msi, this may take a while...
cscript /b foo.vbs
del foo.vbs
:graphvizdownloaded

:: Execute the installer
msiexec /passive /i "%~dp0graphviz-2.36.msi"
if %errorlevel% == 0 goto graphvizinstalledokay
echo Error in installing GraphViz. Manually install %~dp0graphviz-2.36.msi
exit /b 10

:: Dispose of the installer
:graphvizinstalledokay
del /q "%~dp0graphviz-2.36.msi"

:: Exit okay!
:graphvizinstalled
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


