@echo off
if not defined ACLSDK (echo ACLSDK must be defined & exit 2)

call "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86 2> nul:
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86 2> nul:

nmake /f Makefile %*
