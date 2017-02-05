@echo off
if not exist build\release\lydrige.exe (
	echo Please Run 'build release'
	goto :end
)
.\build\release\lydrige.exe %*

:end
