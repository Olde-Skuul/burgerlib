@echo off

:: Enable Visual studio to pull microsoft symbols from the symbol server
:: Written by Rebecca Ann Heineman

if not "%_NT_SYMBOL_PATH%" == "" goto symbolsfound

	if exist "%temp%\localsymbols" goto nolocalsymbolsfolder
		echo Creating "%temp%\localsymbols"
		md "%temp%\localsymbols"
	:nolocalsymbolsfolder

	echo Setting _NT_SYMBOL_PATH="symsrv*symsrv.dll*%temp%\localsymbols*http://msdl.microsoft.com/download/symbols"

	:: Testing if administrator privledges are present

	net session >nul 2>&1
	if not %errorLevel% == 0 goto requiredadministratorrights
	setx _NT_SYMBOL_PATH "symsrv*symsrv.dll*%temp%\localsymbols*http://msdl.microsoft.com/download/symbols" /M
	set _NT_SYMBOL_PATH=symsrv*symsrv.dll*%temp%\localsymbols*http://msdl.microsoft.com/download/symbols
:symbolsfound
exit /b 0

:: Epic fail in case an operation was needed that needed administrator rights
:requiredadministratorrights
echo This operation requires administrator rights to execute. Please re-run this
echo batch file from a console with administrator rights.
exit /b 10

