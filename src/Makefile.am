## Process this file with automake to produce Makefile.in

SUBDIRS = \
	wxServDisc


AM_CPPFLAGS = \
	$(WX_CXXFLAGS)

AM_CFLAGS =\
	 -Wall \
	 -g \
	 $(WX_CPPFLAGS)

bin_PROGRAMS = collabtool

collabtool_SOURCES = \
	CollabToolApp.cpp   \
	CollabToolApp.h   \
	config.h \
	gui/FrameMain.cpp \
	gui/FrameMain.h \
	gui/MyFrameMain.cpp \
	gui/MyFrameMain.h \
	gui/MyDialogSettings.cpp \
	gui/MyDialogSettings.h 

if MINGW
collabtool_SOURCES += gui/winres.rc
.rc.o:
	 $(WX_RESCOMP) -o $@ $<
endif


collabtool_LDFLAGS = 

collabtool_LDADD = \
	wxServDisc/libwxservdisc.a \
	$(WX_LIBS)



