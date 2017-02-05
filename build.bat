@echo off

where cl >NUL 2>NUL
if %ERRORLEVEL%==0 goto :do
echo [Error] Please run vsvarsall.bat
goto :end

:do
set command=%1
if /i "%command%"=="debug" goto :debug
if /i "%command%"=="release" goto :release
if /i "%command%"=="" goto :debug
echo [Error] Unknown command
goto :end

:debug
mkdir build\debug >NUL 2>NUL
cd build\debug
echo Uses Debug
cl /nologo /Oi /GM- /MP /FC /fp:fast /fp:except- ..\..\src\main\c\*.c ..\..\src\hashmap\c\*.c ..\..\src\mpc\c\*.c /I..\..\src\main\headers\ /I..\..\src\hashmap\headers\ /I..\..\src\mpc\headers\ /link -OUT:lydrige.exe -incremental:no -opt:ref -subsystem:console
cd ..\..
goto :end

:release
mkdir build\release >NUL 2>NUL
cd build\release
echo MSVC Release
cl /nologo /O3 /GM- /MP /FC /fp:fast /fp:except- ..\..\src\main\c\*.c ..\..\src\hashmap\c\*.c ..\..\src\mpc\c\*.c /I..\..\src\main\headers\ /I..\..\src\hashmap\headers\ /I..\..\src\mpc\headers\ /link -OUT:lydrige.exe -incremental:no -opt:ref -subsystem:console
cd ..\..
goto :end

:end
