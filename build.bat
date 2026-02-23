@echo on
echo Setting up a Qt64 environment...
 
set QTDIR=C:\Qt\Qt5.9.1\5.9.1\msvc2015_64
set PATH=%QTDIR%\bin;%PATH%
set QMAKESPEC=win32-msvc2015
 
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
 
"C:\Program Files (x86)\MSBuild\14.0\Bin\amd64\msbuild.exe" %* /t:Rebuild /p:Configuration=Release /p:Platform=x64