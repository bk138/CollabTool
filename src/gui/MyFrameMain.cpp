
#include "wx/aboutdlg.h"
#include "wx/socket.h"

#include "res/about.png.h"

#include "MyFrameMain.h"
#include "MyDialogSettings.h"
#include "../dfltcfg.h"
#include "../CollabToolApp.h"


#define DFLTPORT _T("5900")






// map recv of wxServDiscNOTIFY to list update method
BEGIN_EVENT_TABLE(MyFrameMain, FrameMain)
  EVT_COMMAND  (wxID_ANY, wxServDiscNOTIFY, MyFrameMain::onSDNotify)
END_EVENT_TABLE()




using namespace std;
 




/*
  This is a helper class to specify the handler for process termination
  taken from wxwidgets exec sample
*/
#include "wx/process.h"

class MyProcess : public wxProcess
{
protected:
  MyFrameMain *m_parent; 
  wxString m_cmd;
  void (MyFrameMain::*m_callback)(wxString&, int);

public:
  bool do_callback;

  MyProcess(MyFrameMain *parent, const wxString& cmd,
	    void (MyFrameMain::*callback)(wxString&, int))
    : wxProcess(parent), m_cmd(cmd)
  {
    m_parent = parent; // save our caller for later on
    m_callback = callback;
    do_callback = true;
  }


  void OnTerminate(int pid, int status)
  {
    // clean up after us
    if(do_callback)
      {
	cerr << "calling callback" << endl;
	(m_parent->*m_callback)(m_cmd, status);
      }
    
    // we're not needed any more
    delete this;
  }
};





MyFrameMain::MyFrameMain(wxWindow* parent, int id, const wxString& title, 
			 const wxPoint& pos,
			 const wxSize& size,
			 long style):
  FrameMain(parent, id, title, pos, size, style)	
{
  /*
    disable some menu items for a new frame
    unfortunately there seems to be a bug in wxMenu::FindItem(str): 
    it skips '&' characters,  but GTK uses '_' for accelerators and 
    these are not trimmed...
  */
  // this is "share window"
  frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(0)->Enable(false);
// this is "stop share window"
  frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(1)->Enable(false);
  // "end connection"
  frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(3)->Enable(false);


  // dont forget this! otherwise we'll be killin some innocent app
  client_pid = 0;
  client_proc = 0;
  
  windowshare_pid = 0;
  windowshare_proc = 0;

  // read in cmd templates
  read_config();

  // mdns service scanner
  servscan = new wxServDisc(this, wxT("_rfb._tcp.local."), QTYPE_PTR);

}




MyFrameMain::~MyFrameMain()
{
  kill_client();
  kill_windowshare();
}



/*
  private functions

*/

// config stuff
void MyFrameMain::read_config()
{
  // get default config object, created on demand if not exist
  wxConfigBase *pConfig = wxConfigBase::Get();

  // client template
  wxString c;
  is_client_cmd_rconly = false;
  if(pConfig->Read(K_CLIENTMODE, wxEmptyString) == V_RCONLY)
    {
      c = pConfig->Read(K_CUSTOMRCONLY, wxEmptyString);
      if(c != wxEmptyString)
	client_cmd_template = c;
      else
	client_cmd_template = V_DFLTRCONLY;

      is_client_cmd_rconly = true;
    }
  else
    if(pConfig->Read(K_CLIENTMODE, wxEmptyString) == V_VIEWER)
      {
	c = pConfig->Read(K_CUSTOMVIEWER, wxEmptyString);
	if(c != wxEmptyString)
	  client_cmd_template = c;
	else
	  client_cmd_template = V_DFLTVIEWER;
      }
    else //default
      {
	client_cmd_template = V_DFLTRCONLY;
	is_client_cmd_rconly = true;
      }


  // windowshare template
  windowshare_cmd_template = pConfig->Read(K_WINDOWSHARE, V_DFLTWINDOWSHARE);

}






// windowshare stuff
bool MyFrameMain::spawn_windowshare()
{
  wxBusyCursor busy;

  // right now x11vnc and tightvnc behave differently :-/
#ifdef __WIN32__
  wxString window = wxGetTextFromUser(_("Enter Name of Window to share:"), _("Share Window"));
  if(window == wxEmptyString)
    return false;
#else
  int answer = wxMessageBox(_("Press OK to select the window you want to share."),
			    _("Share Window"),
			    wxOK|wxCANCEL);
  if(answer == wxCANCEL)
    return false;
#endif


  SetStatusText(_("Sharing window with ") + addr);
 

  // handle %a and %p
  wxString cmd = windowshare_cmd_template;
  cmd.Replace(_T("%a"), addr);
  cmd.Replace(_T("%p"), port);
#ifdef __WIN32__
  cmd.Replace(_T("%w"), window);
#endif
  
  // terminate old one
  kill_windowshare();

  windowshare_proc = new MyProcess(this, cmd, &MyFrameMain::on_windowshare_term);
  windowshare_pid = wxExecute(cmd, wxEXEC_ASYNC|wxEXEC_MAKE_GROUP_LEADER, windowshare_proc);
  
  cerr << "spawn_windowshare() spawns " << windowshare_pid << endl; 
  if ( !windowshare_pid )
    {
      SetStatusText(_("Windowshare helper execution failed."));
      wxLogError( _("Error sharing window."), cmd.c_str());

      windowshare_proc = 0; // obj pointed to by windowshare_proc deletes itself

      return false;
    }
		
  // this is "share window"
  frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(0)->Enable(false);
  // this is "stop share window"
  frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(1)->Enable(true);

  return true;
}



void MyFrameMain::kill_windowshare()
{
  // avoid callback call
  if(wxProcess::Exists(windowshare_pid))
    if(windowshare_proc)
      {
	windowshare_proc->do_callback = false;
	windowshare_proc = 0; // obj pointed to by windowshare_proc deletes itself
      }
     

  cerr << "kill_windowshare() tries to kill " << windowshare_pid << endl;

  if(windowshare_pid)
    if(wxKill(windowshare_pid, wxSIGTERM, NULL, wxKILL_CHILDREN) == 0)
      {
	cerr << "kill_windowshare() zeros " << windowshare_pid << endl;
	windowshare_pid = 0;
	  
	// this is "share window"
	frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(0)->Enable(true);
	// this is "stop share window"
	frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(1)->Enable(false);
      }
}



void MyFrameMain::on_windowshare_term(wxString& cmd, int status)
{
  cerr << "on_windowshare_term() zeros " << windowshare_pid << endl;

  if(status == 0) 
    SetStatusText(_("Windowshare terminated gracefully."));
  else
    if(status == -1 || status == 1)
       SetStatusText( _("Connection terminated."));
    else
      {
	SetStatusText(_("Error running Windowshare."));
	wxLogError(_("Error running Windowshare."));
      }
   
   windowshare_pid = 0;
   windowshare_proc = 0;
   
   // this is "share window"
   frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(0)->Enable(true);
   // "stop window share"
   frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(1)->Enable(false);

}





// client stuff
bool MyFrameMain::spawn_client()
{
  SetStatusText(_("Connecting to ") + addr + _T(":") + port + _T("..."));
  wxBusyCursor busy;

  // handle %a and %p
  wxString cmd = client_cmd_template;
  cmd.Replace(_T("%a"), addr);
  cmd.Replace(_T("%p"), port);
  
  // terminate old one
  kill_windowshare();
  kill_client();

  client_proc = new MyProcess(this, cmd, &MyFrameMain::on_client_term);
  client_pid = wxExecute(cmd, wxEXEC_ASYNC|wxEXEC_MAKE_GROUP_LEADER, client_proc);
  
  cerr << "spawn_client() spawns " << client_pid << endl; 
  if ( !client_pid )
    {
      SetStatusText(_("Client execution failed."));
      wxLogError( _("Execution of '%s' failed."), cmd.c_str());

      client_proc = 0; // obj pointed to by client_proc deletes itself

      return false;
    }

  SetStatusText(_("Connected to ") + addr + _T(":") + port);

  // this is "share window"
  frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(0)->Enable(true);
  // "end connection"
  frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(3)->Enable(true);
  
  return true;
}


void MyFrameMain::kill_client()
{
  // avoid callback call
  if(wxProcess::Exists(client_pid))
    if(client_proc)
      {
	client_proc->do_callback = false;
	client_proc = 0; // obj pointed to by client_proc deletes itself
      }
     

  cerr << "kill_client() tries to kill " << client_pid << endl;

  if(client_pid)
    if(wxKill(client_pid, wxSIGTERM, NULL, wxKILL_CHILDREN) == 0)
    {
	cerr << "kill_client() zeros " << client_pid << endl;
	client_pid = 0;
	  
	// this is "share window"
	frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(0)->Enable(false);
	// this is "stop share window"
	frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(1)->Enable(false);
	// "end connection"
	frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(3)->Enable(false);
      }
}


void MyFrameMain::on_client_term(wxString& cmd, int status)
{
   cerr << "on_client_term() zeros " << client_pid << endl;

   if(status == 0) 
    SetStatusText(_("Client terminated gracefully."));
   else
     if(status == -1 || status == 1)
       {
	 SetStatusText( _("Connection failed."));
	 wxLogError( _("Connection failed."));
       }
    else
      {
	SetStatusText(_("Error running client."));
	wxLogError(_("Error running '%s'."), cmd.c_str());
      }
   
   client_pid = 0;
   client_proc = 0;
   
   // this is "share window"
   frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(0)->Enable(false);
   // this is "stop share window"
   frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(1)->Enable(false);
   // "end connection"
   frame_main_menubar->GetMenu(frame_main_menubar->FindMenu(wxT("Edit")))->FindItemByPosition(3)->Enable(false);
   
}






/*
  handler functions
*/



void MyFrameMain::file_exit(wxCommandEvent &event)
{
  Close(true);
}


void MyFrameMain::edit_connect(wxCommandEvent &event)
{
  wxString s = wxGetTextFromUser(_("Enter host to connect to:"),
				 _("Connect to specific host"));
  if(s != wxEmptyString)
    {
      wxIPV4address host_addr;
	    
      // get host part and port part
      wxString host_name, host_port;
      host_name = s.BeforeFirst(_T(':'));
      host_port = s.AfterFirst(_T(':'));
      
      // look up name
      if(! host_addr.Hostname(host_name))
	{
	  wxLogError(_("Invalid hostname or IP address."));
	  return;
	}
      else
#ifdef __WIN32__
	addr = host_addr.Hostname(); // wxwidgets bug, ah well ...
#else
        addr = host_addr.IPAddress();
#endif



      
      
      // and handle port
      if(host_addr.Service(host_port))
	port = wxString() << host_addr.Service();
      else
	port = DFLTPORT;


      spawn_client();
    }
}



void MyFrameMain::edit_endconn(wxCommandEvent &event)
{
  kill_windowshare();
  kill_client();  
  SetStatusText( _("Connection terminated."));
}


void MyFrameMain::edit_share(wxCommandEvent &event)
{
  spawn_windowshare();  
}



void MyFrameMain::edit_endshare(wxCommandEvent &event)
{
  kill_windowshare();  
  SetStatusText( _("Stopped window sharing."));
}



void MyFrameMain::edit_preferences(wxCommandEvent &event)
{
  MyDialogSettings dialog_settings(this, wxID_ANY, _("Preferences"));
 
  if(dialog_settings.ShowModal() == wxID_OK)
    {
      wxConfigBase *pConfig = wxConfigBase::Get();      

      pConfig->Write(K_CLIENTMODE, dialog_settings.getClientMode());
      pConfig->Write(K_CUSTOMRCONLY, dialog_settings.getRConly());
      pConfig->Write(K_CUSTOMVIEWER, dialog_settings.getViewer());

      read_config();
    }
}


void MyFrameMain::help_about(wxCommandEvent &event)
{	
  wxAboutDialogInfo info;
  wxIcon icon;
  icon.CopyFromBitmap(bitmapFromMem(about_png));

  info.SetIcon(icon);
  info.SetName(wxT("CollabKit Client"));
  info.SetVersion(wxT(VERSION));
  info.SetDescription(_("This is the CollabKit Client, a tool to connect to a CollabKit server and share windows."));
  info.SetCopyright(wxT(COPYRIGHT));
  
  wxAboutBox(info); 

}




void MyFrameMain::help_contents(wxCommandEvent &e)
{
  wxLogMessage(_("Select a host in the list to get more information, double click to connect.\n\nWhen in Remote Control Only Mode, move the pointer over the right screen edge to control the remote desktop."));
}









void MyFrameMain::onSDNotify(wxCommandEvent& event)
{
    if(event.GetEventObject() == servscan)
      {
	wxArrayString items; 
	
	// length of qeury plus leading dot
	size_t qlen =  servscan->getQuery().Len() + 1;
	
	vector<wxSDEntry> entries = servscan->getResults();
	vector<wxSDEntry>::const_iterator it; 
	for(it=entries.begin(); it != entries.end(); it++)
	  items.Add(it->name.Mid(0, it->name.Len() - qlen));
	
	list_box_services->Set(items, 0);
      }
}



void MyFrameMain::list_select(wxCommandEvent &event)
{
  int timeout;
  wxBusyCursor busy;
  int sel = event.GetInt();
 
  if(sel < 0) // seems this happens when we update the list
    return;

  SetStatusText(_("Looking up host address..."));


  // lookup hostname and port
  {
    wxServDisc namescan(0, servscan->getResults().at(sel).name, QTYPE_SRV);
  
    timeout = 3000;
    while(!namescan.getResultCount() && timeout > 0)
      {
	wxMilliSleep(25);
	timeout-=25;
      }
    if(timeout <= 0)
      {
	wxLogError(_("Timeout looking up hostname."));
	SetStatusText(_("Timeout looking up hostname."));
	hostname = addr = port = wxEmptyString;
	return;
      }
    hostname = namescan.getResults().at(0).name;
    port = wxString() << namescan.getResults().at(0).port;
  }

  
  // lookup ip address
  {
    wxServDisc addrscan(0, hostname, QTYPE_A);
  
    timeout = 3000;
    while(!addrscan.getResultCount() && timeout > 0)
      {
	wxMilliSleep(25);
	timeout-=25;
      }
    if(timeout <= 0)
      {
	wxLogError(_("Timeout looking up IP address."));
	SetStatusText(_("Timeout looking up IP address."));
	hostname = addr = port = wxEmptyString;
	return;
      }
    addr = addrscan.getResults().at(0).ip;
  }

  SetStatusText(hostname + wxT(" (") + addr + wxT(":") + port + wxT(")"));
}


void MyFrameMain::list_dclick(wxCommandEvent &event)
{
  list_select(event); // get the actual values
  spawn_client();
} 
 
