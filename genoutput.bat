@echo off
for /l %%i in (0, 1, 46) do (
	copy "%~dp0..\..\..\Debug\PascalCompiler.exe" "%~dp0" >nul
	PascalCompiler.exe -l %%i.in
	fc /b tokens.log %%i.out >nul
	if not errorlevel 1 (
		echo Test %%i: Passed
	) else (
		echo Test %%i: Failed
	)
)
pause
