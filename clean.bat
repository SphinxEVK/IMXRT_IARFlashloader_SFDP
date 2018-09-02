@echo off

::-----::
set iar_path=%~dp0\iar\
set param="*.browse" and "*.linf" and "*.o" and "*.pbi" and "*.pbd" and "*.xcl" and "*.out" and "*.hex" and "*.bin" and "*.pbw"
rem 不能误删除flashloader，将flashloader视作*.out的例外
::-----::

echo Cleanning up repository...
echo.

for /f "delims=" %%i in ('dir iar_path /a-d/b/s %param%') do (
if not "%%i"=="%except%" (
del %%i
echo Deleted: %%i!
)
)

echo.
echo Done!

pause