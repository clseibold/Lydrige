@echo off

set compiler=%1
set arch=%2
if /i "%compiler%"=="gcc" goto :gcc
if /i "%compiler%"=="clang" goto :clang
if /i "%compiler%"=="run" goto :run
if /i "%compiler%"=="clean" goto :clean
if /i "%compiler%"=="" echo Error: Please type in the compiler ('gcc', 'clang', or 'msvc') or a command ('run' or 'clean') [argument 1]
echo Error: Unknown command (argument 2)
goto :end


:gcc
if /i "%arch%"=="x86" goto :gccx86
if /i "%arch%"=="x86_64" goto :gccx64
echo Error: Unknown architecture. Must enter either 'x86' or 'x86_64' [argument 2]
goto :end

:gccx86
echo NEEDS UPDATED
mkdir build\debug
cd build\debug
rem GCC: Compile mpc
mkdir mpc
gcc -m32 -c -std=c99 -Wall -g ..\..\src\mpc\c\mpc.c -o mpc\mpc.o
ar -cvq libmpc.a mpc\*.o
rem GCC: Compile hashmap
mkdir hashmap
cd hashmap
gcc -m32 -c -std=c99 -Wall -g ..\..\..\src\hashmap\c\*.c
ar -cvq ..\libhashmap.a *.o
cd ..
rem GCC: Compile Lydrige
gcc -m32 -std=c99 -Wall -g -D_WIN32 ..\..\src\main\c\*.c libmpc.a libhashmap.a -o lydrige.exe
cd ..\..
goto :end

:gccx64
echo NEEDS UPDATED
mkdir build\debug
cd build\debug
rem GCC: Compile mpc
mkdir mpc
gcc -m64 -c -std=c99 -Wall -g ..\..\src\mpc\c\mpc.c -o mpc\mpc.o
ar -cvq libmpc.a mpc\*.o
rem GCC: Compile hashmap
mkdir hashmap
cd hashmap
gcc -m64 -c -std=c99 -Wall -g ..\..\..\src\hashmap\c\*.c
ar -cvq ..\libhashmap.a *.o
cd ..
rem GCC: Compile Lydrige
gcc -m64 -std=c99 -Wall -g -D_WIN32 ..\..\src\main\c\*.c libmpc.a libhashmap.a -o lydrige.exe
cd ..\..
goto :end


:clang
if /i "%arch%"=="x86" goto :clangx86
if /i "%arch%"=="x86_64" goto :clangx64
echo Error: Unknown architecture. Must enter either 'x86' or 'x86_64' [argument 2]
goto :end

:clangx86
mkdir build\debug
cd build\debug
rem Clang: Compiler mpc
mkdir mpc
clang -m32 -c -std=c99 -Wall -g ..\..\src\mpc\c\mpc.c -o mpc\mpc.o
llvm-ar rcs libmpc.a mpc\mpc.o
rem Clang: Compile hashmap
mkdir hashmap
cd hashmap
clang -m32 -c -std=c99 -Wall -g ..\..\..\src\hashmap\c\*.c
llvm-ar rcs ..\libhashmap.a bstrlib.o darray.o hashmap.o
cd ..
rem Clang: Compile Lydrige
clang -m32 -std=c99 -Wall -g -D_WIN32 ..\..\src\main\c\*.c libmpc.a libhashmap.a -o lydrige.exe
cd ..\..
goto :end

:clangx64
mkdir build\debug
cd build\debug
rem Clang: Compile mpc
mkdir mpc
clang -m64 -c -std=c99 -Wall -g ..\..\src\mpc\c\mpc.c -o mpc\mpc.o
llvm-ar rcs libmpc.a mpc\mpc.o
rem Clang: Compile hashmap
mkdir hashmap
cd hashmap
clang -m64 -c -std=c99 -Wall -g ..\..\..\src\hashmap\c\*.c
llvm-ar rcs ..\libhashmap.a bstrlib.o darray.o hashmap.o
cd ..
rem Clang: Compile Lydrige
clang -m64 -std=c99 -Wall -g -D_WIN32 ..\..\src\main\c\*.c libmpc.a libhashmap.a -o lydrige.exe
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
