/*


*/

#include <iostream>


#include "CollabToolApp.h"
#include "gui/MyFrameMain.h"



using namespace std;



// this also sets up main()
IMPLEMENT_APP(CollabToolApp);




bool CollabToolApp::OnInit()
{
  locale = 0;
  
  setLocale(wxLANGUAGE_DEFAULT);

  // wxConfig:
  // application and vendor name are used by wxConfig to construct the name
  // of the config file/registry key and must be set before the first call
  // to Get() if you want to override the default values (the application
  // name is the name of the executable and the vendor name is the same)
  SetVendorName(_T("CollabKit"));
 
  // greetings to anyone who made it...
  cout << "\n:::  this is collabtool  :::\n\n";
  cout << COPYRIGHT << ".\n";
  cout << "collabtool is free software, licensed unter the GPL.\n\n";

  // wx stuff
  wxInitAllImageHandlers();
  MyFrameMain* frame_main = new MyFrameMain(NULL, wxID_ANY, wxEmptyString);
  SetTopWindow(frame_main);
  frame_main->Show();

  return true;
}


int CollabToolApp::OnExit()
{
  // clean up: Set() returns the active config object as Get() does, but unlike
  // Get() it doesn't try to create one if there is none (definitely not what
  // we want here!)
  delete wxConfigBase::Set((wxConfigBase *) NULL);

  return 0;
}



bool CollabToolApp::setLocale(int language)
{
  delete locale;
     
  locale = new wxLocale;

  // don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return 
  // false just because it failed to load wxstd catalog                                 
  if(! locale->Init(language, wxLOCALE_CONV_ENCODING) )                      
    {  
      wxLogError(_("This language is not supported by the system.")); 
      return false;    
    }            

  // normally this wouldn't be necessary as the catalog files would be found  
  // in the default locations, but when the program is not installed the
  // catalogs are in the build directory where we wouldn't find them by  
  // default
  wxLocale::AddCatalogLookupPathPrefix(wxT("."));            
           
  // Initialize the catalogs we'll be using  
  locale->AddCatalog(wxT("collabtool"));  

  return true;
}
