@echo off

mkdir build\debug >NUL 2>NUL
cd build\debug
echo Uses MSVC
cl /D_CRT_SECURE_NO_WARNINGS /nologo /Oi /GM- /MP /FC /fp:fast /fp:except- ..\..\src\main\c\*.c ..\..\src\hashmap\c\*.c ..\..\src\mpc\c\*.c /I..\..\src\main\headers\ /I..\..\src\hashmap\headers\ /I..\..\src\mpc\headers\ /link -OUT:lydrige.exe -incremental:no -opt:ref -subsystem:console
cd ..\..