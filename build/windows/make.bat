@echo off
if not defined ACLSDK (echo ACLSDK must be defined & exit 2)

call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd" /Release /x64 /2003 > nul:
nmake /f Makefile %*
