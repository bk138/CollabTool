// -*- C++ -*- 

#ifndef MYFRAMEMAIN_H
#define MYFRAMEMAIN_H


#include "FrameMain.h"
#include "../wxServDisc/wxServDisc.h"



class MyProcess;

class MyFrameMain: public FrameMain
{
  friend class MyProcess; // to allow callback

  // main service scanner
  wxServDisc* servscan;

  // to temporarily store these
  wxString hostname;
  wxString addr;
  wxString port;


  void read_config();

  
  // the client command to be executed and its process id
  bool is_client_cmd_rconly;
  wxString client_cmd_template;
  long client_pid;
  MyProcess *client_proc;
  bool spawn_client();
  void kill_client();
  void on_client_term(wxString& cmd, int status); // callback if client exits on its own


  // the windowshare command to be executed and its process id
  wxString windowshare_cmd_template;
  long windowshare_pid;
  MyProcess *windowshare_proc;
  bool spawn_windowshare();
  void kill_windowshare();
  // callback if windowshare exits on its own
  void on_windowshare_term(wxString& cmd, int status); 

  
protected:
  DECLARE_EVENT_TABLE();
 
public:
  MyFrameMain(wxWindow* parent, int id, const wxString& title, 
	      const wxPoint& pos=wxDefaultPosition, 
	      const wxSize& size=wxDefaultSize, 
	      long style=wxDEFAULT_FRAME_STYLE);
  ~MyFrameMain();

  
  // handlers
  void onSDNotify(wxCommandEvent& event);
  void list_dclick(wxCommandEvent &event); 
  void list_select(wxCommandEvent &event); 

  void file_exit(wxCommandEvent &event);

  void edit_preferences(wxCommandEvent &event);
  void edit_connect(wxCommandEvent &event);
  void edit_endconn(wxCommandEvent &event);
  void edit_share(wxCommandEvent &event);
  void edit_endshare(wxCommandEvent &event);

  void help_about(wxCommandEvent &event);
  void help_contents(wxCommandEvent &event);

};



#endif
