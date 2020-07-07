call vsdevcmd
call :test ir_math_test.cpp
if %TESTRESULT%==1 goto end
call :test ir_n2st_database_test.cpp
if %TESTRESULT%==1 goto end
call :test ir_neuro_test.cpp
if %TESTRESULT%==1 goto end
call :test ir_openmap_test.cpp
if %TESTRESULT%==1 goto end
call :test ir_plot_test.cpp user32.lib gdi32.lib
if %TESTRESULT%==1 goto end
call :test ir_resource_test.cpp
if %TESTRESULT%==1 goto end
call :test ir_s2st_database_test.cpp
if %TESTRESULT%==1 goto end
call :test ir_utf_test.cpp
if %TESTRESULT%==1 goto end
call :test ir_vector_test.cpp
if %TESTRESULT%==1 goto end
call :test ir_register_test.cpp
if %TESTRESULT%==1 goto end
:end
del *.exe
del *.obj
pause
exit /b

:test
	cl %* /I .. /W4 /sdl /EHsc
	if ERRORLEVEL 1 (
		rgb r
		echo Fail
		rgb rgb
		SET TESTRESULT=1
	) else (
		rgb g;
		echo Success
		rgb rgb
		SET TESTRESULT=0
	)
	exit /b