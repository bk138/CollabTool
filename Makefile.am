## Process this file with automake to produce Makefile.in


SUBDIRS = \
	src


collabtooldocdir = ${datadir}/doc/collabtool
collabtooldoc_DATA = \
	README\
	AUTHORS\
	ChangeLog\
	NEWS

Applicationsdir = $(datadir)/applications/
Applications_DATA = src/collabtool.desktop

pixmapdir = $(datadir)/pixmaps
pixmap_DATA = src/gui/res/collabtool.xpm


EXTRA_DIST = $(collabtooldoc_DATA) \
             $(Applications_DATA) \
             $(pixmap_DATA)


if DARWIN
bundle_contents = CollabTool.app/Contents
appbundle: src/collabtool
	mkdir -p $(bundle_contents)/MacOS
	mkdir -p $(bundle_contents)/Resources
	echo "APPL????" > $(bundle_contents)/PkgInfo
	$(INSTALL_PROGRAM) $< $(bundle_contents)/MacOS/
	cp src/gui/res/icon.icns $(bundle_contents)/Resources
	cp README  $(bundle_contents)/Resources
	cp NEWS    $(bundle_contents)/Resources
	cp COPYING $(bundle_contents)/Resources
	cp AUTHORS $(bundle_contents)/Resources
	echo \
	"<?xml version="1.0" encoding="UTF-8"?>\
	<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd"\
	<plist version="1.0">\
	<dict>\
	<key>CFBundleDevelopmentRegion</key>\
	<string>English</string>\
	<key>CFBundleExecutable</key>\
	<string>@PACKAGE_NAME@</string>\
	<key>CFBundleIconFile</key>\
	<string>icon.icns</string>\
	<key>CFBundleGetInfoString</key>\
	<string>CollabTool @PACKAGE_VERSION@, © Christian Beier (dontmind@freeshell.org), 2008-2010 </string>\
	<key>CFBundleName</key>  \
	<string>CollabTool</string> \
	<key>CFBundlePackageType</key> \
	<string>APPL</string>          \
	<key>CFBundleShortVersionString</key> \
	<string>@PACKAGE_VERSION@</string> \
	<key>CFBundleVersion</key> \
	<string>@PACKAGE_VERSION@</string> \
	</dict>   \
	</plist> \
	" >  $(bundle_contents)/Info.plist
endif




if MINGW
nsis_installer: all collabtool.nsi README NEWS COPYING
	cat README | unix2dos > README.TXT
	cat NEWS | unix2dos > NEWS.TXT
	cat COPYING | unix2dos > COPYING.TXT
	$(STRIP) src/collabtool.exe
	makensis collabtool.nsi 
endif
