@echo off

set compiler=%1
if /i "%compiler%"=="msvc" goto :msvc
if /i "%compiler%"=="gcc" goto :gcc
if /i "%compiler%"=="run" goto :run
if /i "%compiler%"=="clean" goto :clean
if /i "%compiler%"=="" echo Error: Please type in the compiler, 'msvc'/'gcc', or type in 'run' or 'clean'
echo Error: Unknown command!
goto :end

:msvc
set arch=%2
if /i "%arch%"=="32" goto :msvc32
if /i "%arch%"=="64" goto :msvc64
echo Error: Please type in '32'/'64' after the compiler argument.
goto :end

:msvc32
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
goto :msvcgo

:msvc64
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
goto :msvcgo

:msvcgo
mkdir build\debug
cd build\debug
rem MSVC: Compile mpc
cl /c ..\..\src\mpc\c\mpc.c
lib mpc.obj

rem MSVC: Compile hashmap
cl /c ..\..\src\hashmap\c\hashmap.c ..\..\src\hashmap\c\bstrlib.c ..\..\src\hashmap\c\darray.c
lib hashmap.obj darray.obj bstrlib.obj

rem MSVC: Compile Lydrige
cl /D_WIN32 ..\..\src\main\c\main.c ..\..\src\main\c\builtin.c ..\..\src\main\c\structure.c hashmap.lib mpc.lib /link /out:lydrige.exe /SUBSYSTEM:CONSOLE
cd ..\..
goto :end

:gcc
mkdir build\debug
cd build\debug
rem GCC: Compile mpc
mkdir mpc
gcc -c -std=c99 -Wall -g ..\..\src\mpc\c\mpc.c -o mpc\mpc.o
ar -cvq libmpc.a mpc\*.o

rem GCC: Compile hashmap
mkdir hashmap
cd hashmap
gcc -c -std=c99 -Wall -g ..\..\..\src\hashmap\c\*.c
ar -cvq ..\libhashmap.a *.o
cd ..

rem GCC: Compile Lydrige
gcc -std=c99 -Wall -g -D_WIN32 ..\..\src\main\c\*.c libmpc.a libhashmap.a -o lydrige.exe
cd ..\..
goto :end

:run
if not exist "build\debug\lydrige.exe" goto :runerror
call build\debug\lydrige.exe
goto :end

:runerror
echo Error: Must compile program before you can run it!
goto :end

:clean
rmdir /S build

:end
