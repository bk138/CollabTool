
#ifndef COLLABTOOLAPP_H
#define COLLABTOOLAPP_H


#include <wx/wxprec.h>
#ifndef WX_PRECOMP 
#include "wx/wx.h"       
#endif  
#include "wx/config.h"



#define VERSION "0.1"
#define COPYRIGHT "Copyright (C) 2009-2010 Christian Beier <dontmind@freeshell.org>"


class CollabToolApp: public wxApp 
{
  wxLocale *locale;
public:
  
  bool OnInit();
  int  OnExit();
  bool setLocale(int language);
};


DECLARE_APP(CollabToolApp);





#endif // COLLABTOOLAPP_H
