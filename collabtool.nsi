!define VERSION "0.1"

Name "CollabTool ${VERSION}"

OutFile "collabtool_${VERSION}-setup.exe"

InstallDir $PROGRAMFILES\CollabTool

Page license
LicenseData COPYING.TXT

Page directory

Page instfiles

Section ""

  SetOutPath $INSTDIR

  File src\collabtool.exe
  File src\mingwm10.dll
  File NEWS.TXT	
  File README.TXT
  File src\contrib\win2vnc.exe
  File src\contrib\vncviewer.exe
  File src\contrib\WinVNC.exe
  File src\contrib\VNCHooks.dll
  File src\contrib\README-contrib.txt
  
  writeUninstaller $INSTDIR\collabtool-uninstall.exe

  # now the shortcuts
  CreateDirectory "$SMPROGRAMS\CollabTool"
  createShortCut  "$SMPROGRAMS\CollabTool\CollabTool.lnk" "$INSTDIR\collabtool.exe"
  createShortCut  "$SMPROGRAMS\CollabTool\Readme.lnk" "$INSTDIR\README.TXT"
  createShortCut  "$SMPROGRAMS\CollabTool\News.lnk" "$INSTDIR\NEWS.TXT"
  createShortCut  "$SMPROGRAMS\CollabTool\Uninstall CollabTool.lnk" "$INSTDIR\collabtool-uninstall.exe"

SectionEnd 

section "Uninstall"
 
  # Always delete uninstaller first
  delete $INSTDIR\collabtool-uninstall.exe

 
  # now delete installed files
  delete $INSTDIR\collabtool.exe
  delete $INSTDIR\mingwm10.dll
  delete $INSTDIR\NEWS.TXT
  delete $INSTDIR\README.TXT	
  delete $INSTDIR\win2vnc.exe
  delete $INSTDIR\vncviewer.exe
  delete $INSTDIR\WinVNC.exe
  delete $INSTDIR\VNCHooks.dll
  delete $INSTDIR\README-contrib.txt
  
  RMDir  $INSTDIR
 
  # delete shortcuts
  delete "$SMPROGRAMS\CollabTool\CollabTool.lnk"
  delete "$SMPROGRAMS\CollabTool\Readme.lnk"
  delete "$SMPROGRAMS\CollabTool\News.lnk"
  delete "$SMPROGRAMS\CollabTool\Uninstall CollabTool.lnk"
  RMDir  "$SMPROGRAMS\CollabTool"
  
sectionEnd

Function un.onInit
    MessageBox MB_YESNO "This will uninstall CollabTool. Continue?" IDYES NoAbort
      Abort ; causes uninstaller to quit.
    NoAbort:
FunctionEnd
