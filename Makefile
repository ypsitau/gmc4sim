# This makefile works with Borland make tool.

!if !$d(WXVER)
WXVER = 2.8.10
!endif

VERSION = 1.38
TARGET = gmc4sim.exe
TARGET_CON = gmc4tool.exe

SRCS = GMC4Sim.cpp ConfigDialog.cpp Config.cpp
SRCS = $(SRCS) RegisterWatcher.cpp ProgramMonitor.cpp
SRCS = $(SRCS) Board.cpp AssemblerEditor.cpp DumpPresenter.cpp
SRCS = $(SRCS) VirtualMachineGUI.cpp
SRCS = $(SRCS) Parser.cpp Generator.cpp MmlParser.cpp
SRCS = $(SRCS) VirtualMachine.cpp Operator.cpp

SRCS_CON = GMC4Tool.cpp
SRCS_CON = $(SRCS_CON) VirtualMachineConsole.cpp
SRCS_CON = $(SRCS_CON) Parser.cpp Generator.cpp MmlParser.cpp
SRCS_CON = $(SRCS_CON) VirtualMachine.cpp Operator.cpp

ZIPFILES = gmc4sim.exe gmc4tool.exe GMC4Sim.pdf
ZIPFILES = $(ZIPFILES) Sample\Gakken002\*.asm
ZIPFILES = $(ZIPFILES) Sample\Gakken008-021\*.hex
ZIPFILES = $(ZIPFILES) Sample\Gakken022-042\*.hex
ZIPFILES = $(ZIPFILES) Sample\Gakken043-066\*.hex
ZIPFILES = $(ZIPFILES) Sample\Gakken067-100\*.hex

RCFILE = GMC4Sim.rc

#------------------------------------------------------------------------------
WXDIR = $(MAKEDIR)\..\wxWidgets-$(WXVER)

CFLAGS = -v -P -Od -a8 -g0 \
	-DVERSION=\"$(VERSION)\" \
	-D__WXMSW__ -DNOPCH \
	-w-8057 \
	-I. -I.. \
	-I$(MAKEDIR)\..\Include \
	-I$(WXDIR)\lib\bcc_lib\msw \
	-I$(WXDIR)\include

RCFLAGS = -32 -r -d__WXMSW__  \
	-i$(MAKEDIR)\..\include \
	-i$(WXDIR)\lib\bcc_lib\msw \
	-i$(WXDIR)\include

LDFLAGS = -Tpe -q -v -aa

LIBPATHFLAGS = \
	-L$(MAKEDIR)\..\Lib \
	-L$(MAKEDIR)\..\Lib\psdk \
	-L$(WXDIR)\lib\bcc_lib 

OBJS = $(SRCS:.cpp=.obj)
OBJS_CON = $(SRCS_CON:.cpp=.obj)
RESFILE = $(RCFILE:.rc=.res)

LIBS = wxbase28.lib wxmsw28_adv.lib wxmsw28_core.lib wxmsw28_html.lib wxmsw28_richtext.lib \
	wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib \
	ole2w32.lib oleacc.lib odbc32.lib import32.lib cw32mt.lib

.cpp.obj:
	@bcc32 -q -c $(CFLAGS) -o$@ $<

.rc.res:
	@brcc32 $(RCFLAGS) -fo$@ $<

all: $(TARGET) $(TARGET_CON)

$(TARGET): $(OBJS) $(RESFILE)
	@echo Generating $@ ...
	@ilink32 $(LDFLAGS) $(LIBPATHFLAGS) c0w32.obj $(OBJS),$@,,$(LIBS),,$(RESFILE)

$(TARGET_CON): $(OBJS_CON)
	@echo Generating $@ ...
	@bcc32 -q -e$@ -L$(MAKEDIR)\..\Lib -L$(MAKEDIR)\..\Lib\PSDK $**

clean:
	del $(TARGET) $(OBJS) $(RESFILE) \
	$(TARGET:.exe=.tds) $(TARGET:.exe=.map) \
	$(TARGET:.exe=.ils) $(TARGET:.exe=.ilf) \
	$(TARGET:.exe=.ild) $(TARGET:.exe=.ilc) \
	$(TARGET:.exe=.tr2) $(TARGET:.exe=.td2) \
	$(TARGET_CON) $(TARGET_CON:.exe=.tds) $(OBJS_CON)

dist: $(TARGET)
	@7z a -tzip ../GMC4Sim-$(VERSION).zip $(ZIPFILES)
	@7z a -tzip ../GMC4Sim-src-$(VERSION).zip Makefile *.cpp *.h *.rc *.ico

pack:
	@svn cleanup
	@svn ls -R > svnlist.txt
	@7z a -tzip ../GMC4Sim-src-$(VERSION).zip @svnlist.txt
	@del svnlist.txt
