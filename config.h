/*
  applicationwide defines for config keys and values 
*/

#ifndef CONFIG_H
#define CONFIG_H


// client stuff
#define K_CLIENTMODE _T("ClientMode")
#define V_DFLTCLIENTMODE _T("RConly")
#define V_RCONLY _T("RConly")
#define V_VIEWER _T("Viewer")

#define K_CUSTOMRCONLY _T("CustomClientRCOnly")
#define K_CUSTOMVIEWER _T("CustomClientViewer")

#ifdef __WIN32__
#define V_DFLTRCONLY  _T("win2vnc.exe %a")
#define V_DFLTVIEWER  _T("vncviewer.exe %a::%p")
#else
#define V_DFLTRCONLY  _T("x2vnc -noreconnect %a:0")
#define V_DFLTVIEWER  _T("vncviewer %a::%p")
#endif


// windowshare stuff
#define K_WINDOWSHARE _T("CustomWindowShare")
#ifdef __WIN32__
#define V_DFLTWINDOWSHARE _T("winvnc.exe -oneshot -sharewindow \"%w\" -connect %a")
#else
#define V_DFLTWINDOWSHARE _T("x11vnc -repeat -id pick -xrandr -connect %a")
#endif


#endif // CONFIG_H
