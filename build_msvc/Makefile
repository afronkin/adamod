SRC_DIR=..\src
OBJS_GETOPT=getopt_long.obj
OBJS = adamod.obj messages.obj modify.obj $(OBJS_GETOPT)
PROGRAM = adamod.exe

PLATFORM_SDK=C:\Program Files\Microsoft SDKs\Windows\v7.1
ADALNK=D:\projects_lib\adalnk\V517

CFLAGS=/nologo /MT /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" \
  /FD /I "$(PLATFORM_SDK)\Include" /I "$(SRC_DIR)" /I "$(SRC_DIR)/getopt" /I "$(ADALNK)\inc"
LDFLAGS=/nologo /machine:i386
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

messages.obj: $(SRC_DIR)\messages.c
	cl /c $(CFLAGS) $**

modify.obj: $(SRC_DIR)\modify.c
	cl /c $(CFLAGS) $**

getopt_long.obj: $(SRC_DIR)\getopt\getopt_long.c
	cl /c $(CFLAGS) $**
