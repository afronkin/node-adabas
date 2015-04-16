@echo off
setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x64

set ACLSDK=%ADADIR64%\..

set NODE_GYP="C:\Program Files\nodejs\node_modules\npm\bin\node-gyp-bin\node-gyp"
if "%1" == "clean" (
  %NODE_GYP% clean
) else (
  %NODE_GYP% configure build
)
