@echo off

if not exist rgb.exe (
	cl rgb.c
	if ERRORLEVEL 1 (
		echo Fail
		goto end
	)
)

for %%f in (*.cpp) do (
	call :compile %%f
	if ERRORLEVEL 1 (
		goto end
	)
)

:end
for %%f in (*.exe) do if not %%f==rgb.exe del %%f
del *.obj
pause
exit /b

:compile
	cl %* /W4 /sdl /EHsc
	if ERRORLEVEL 1 (
		rgb r
		echo Fail
		rgb rgb
		exit /b 1
	) else (
		rgb g
		echo Success
		rgb rgb
		exit /b 0
	)