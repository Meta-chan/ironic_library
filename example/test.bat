@echo off

if not exist rgb.exe (
	cl rgb.c
	if ERRORLEVEL 1 (
		echo Fail
		goto end
	)
)

for %%f in (*.c *.cpp) do (
	if not %%f==rgb.c (
		call :test %%f
		if "%RESULT%"=="1" ( goto end )
	)
)

:end
for %%f in (*.exe) do if not %%f==rgb.exe del %%f
del *.obj
pause
exit /b

:test
	if %*==ir_plot_test.cpp (
		set LIBS=user32.lib gdi32.lib
	) else (
		set LIBS=
	)
	
	cl %* %LIBS% /I .. /W4 /sdl /EHsc
	if ERRORLEVEL 1 (
		rgb r
		echo Fail
		rgb rgb
		SET RESULT=1
	) else (
		rgb g
		echo Success
		rgb rgb
		SET RESULT=0
	)
	exit /b