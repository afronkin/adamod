call "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd" /Release /x64 /2008
nmake /f Makefile.64 %*
