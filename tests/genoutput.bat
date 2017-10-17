@echo off
for /l %%i in (1, 1, 40) do (
	if not exist %%i (
		md %%i
		echo.>%%i\%%i.txt
	)
	copy "%~dp0..\..\Debug\PascalCompiler.exe" "%~dp0\%%i" >nul
	pushd %%i 
	PascalCompiler.exe -l %%i.txt
	fc /b tokens.log res.txt >nul
	if not errorlevel 1 (
		echo Test %%i: Passed
	) else (
		echo Test %%i: Failed
	)
	popd
)
pause