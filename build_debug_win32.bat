@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
mkdir build
mkdir build\debug
cd build\debug

rem Compile mpc
cl /c ..\..\src\mpc\c\mpc.c
lib mpc.obj

rem Compile hashmap
cl /c ..\..\src\hashmap\c\hashmap.c ..\..\src\hashmap\c\bstrlib.c ..\..\src\hashmap\c\darray.c
lib hashmap.obj darray.obj bstrlib.obj

rem Compile Lydrige
cl /D_WIN32 ..\..\src\main\c\main.c ..\..\src\main\c\builtin.c ..\..\src\main\c\structure.c hashmap.lib mpc.lib /link /out:Lydrige.exe /SUBSYSTEM:CONSOLE

PAUSE