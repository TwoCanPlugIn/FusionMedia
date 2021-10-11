// Copyright(C) 2018-2020 by Steven Adler
//
// This file is part of Media Player Control plugin for OpenCPN.
//
// Media Player Control plugin for OpenCPN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Media Player Control plugin for OpenCPN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Media Player Control plugin for OpenCPN. If not, see <https://www.gnu.org/licenses/>.
//
// NMEA2000® is a registered trademark of the National Marine Electronics Association
// Fusion® is a registered trademark of Fusion


#ifndef MEDIAPLAYERCONTROL_PLUGIN_H
#define MEDIAPLAYERCONTROL_H

// Pre compiled headers 
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

// Defines version numbers, names etc. for this plugin
// This is automagically constructed via version.h.in from CMakeLists.txt, personally I think this is convoluted
#include "version.h"

// OpenCPN include file
#include "ocpn_plugin.h"

// Media Player Control Dialog (non modal)
#include "mediaplayercontrol_window.h"

// Plugin receives events from the Media Player Control Window
const wxEventType wxEVT_MEDIAPLAYERCONTROL_DIALOG_EVENT = wxNewEventType();
const int DIALOG_CLOSED = wxID_HIGHEST + 1;
const int DIALOG_ON = wxID_HIGHEST + 2;
const int DIALOG_OFF = wxID_HIGHEST + 3;

// The Media Player Control plugin
class MediaPlayerControlPlugin : public opencpn_plugin_117, public wxEvtHandler {

public:
	// The constructor
	MediaPlayerControlPlugin(void *ppimgr);
	
	// and destructor
	~MediaPlayerControlPlugin(void);

	// Overridden OpenCPN plugin methods
	int Init(void);
	bool DeInit(void);
	int GetAPIVersionMajor();
	int GetAPIVersionMinor();
	int GetPlugInVersionMajor();
	int GetPlugInVersionMinor();
	wxString GetCommonName();
	wxString GetShortDescription();
	wxString GetLongDescription();
	wxBitmap *GetPlugInBitmap();
	int GetToolbarToolCount(void);
	int GetToolbarItemId(void);
	void OnToolbarToolCallback(int id);
	void SetDefaults(void);
	void SetPluginMessage(wxString &message_id, wxString &message_body);
	
	// Event Handler
	void OnPluginEvent(wxCommandEvent &event);


	// Event Handler for events received from the Media Player Control Window
	void OnDialogEvent(wxCommandEvent &event);
		
private: 
	// Media Player Control modeless dialog
	MediaPlayerControlWindow *mediaPlayerControlWindow;

	// Plugin bitmap
	wxBitmap pluginBitmap;

	// Reference to the OpenCPN window handle
	wxWindow *parentWindow;
	
	// Toolbar id's
	int mediaPlayerControlToolbar;

};

#endif 

