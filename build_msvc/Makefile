SRC_DIR=..\src
OBJS = adamod.obj
PROGRAM = adamod.exe

PLATFORM_SDK=C:\Program Files\Microsoft SDKs\Windows\v7.1
ADALNK=D:\projects_lib\adalnk\V517

CFLAGS=/nologo /MT /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" \
  /FD /I "$(PLATFORM_SDK)\Include" /I "..\src" /I "$(ADALNK)\inc"
LDFLAGS=/nologo /machine:I386
LIBS=$(ADALNK)\lib\adalnkx.lib

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	cl $** /link $(LDFLAGS) /out:$@ $(LIBS)

clean:
	del *.exe *.idb *.obj

verify:
	$(PROGRAM) -h

adamod.obj: $(SRC_DIR)\adamod.c
	cl /c $(CFLAGS) $**
