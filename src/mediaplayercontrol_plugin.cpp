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
// along with the mediaPlayerControl plugin for OpenCPN. If not, see <https://www.gnu.org/licenses/>.
//

//
// Project: Media Player Control Plugin
// Description: Remotely Control Fusion Media Players
// Owner: twocanplugin@hotmail.com
// Date: 6/1/2020
// Version History: 
// 1.0 Initial Release
//

#include "mediaplayercontrol_plugin.h"
#include "mediaplayercontrol_icon.h"

// Globally accessible variables used by the plugin
wxFileConfig *configSettings;

// Toolbar State
bool mediaPlayerControlWindowVisible;

// The class factories, used to create and destroy instances of the PlugIn
extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
	return new MediaPlayerControlPlugin(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) {
	delete p;
}

// Constructor
MediaPlayerControlPlugin::MediaPlayerControlPlugin(void *ppimgr) : opencpn_plugin_117(ppimgr), wxEvtHandler() {
	
	// Load the plugin bitmaps/icons 
	initialize_images();
}

// Destructor
MediaPlayerControlPlugin::~MediaPlayerControlPlugin(void) {
}

int MediaPlayerControlPlugin::Init(void) {
	// Maintain a reference to the OpenCPN window to use as the parent for the Race Start Window
	parentWindow = GetOCPNCanvasWindow();

	// Maintain a reference to the OpenCPN configuration object 
	// Not used, however could have a preference for the source or volume
	configSettings = GetOCPNConfigObject();

	// Load plugin icons
	
	// BUG BUG Can we automagically define this !! ${PACKAGE} + "_pi"
	wxString shareLocn = GetPluginDataDir(PLUGIN_PACKAGE_NAME) + wxFileName::GetPathSeparator() + _T("data") + wxFileName::GetPathSeparator();
	
	wxString normalIcon = shareLocn + _T("mediaplayer_icon_normal.svg");
	wxString toggledIcon = shareLocn + _T("mediaplayer_icon_toggled.svg");
	wxString rolloverIcon = shareLocn + _T("mediaplayer_icon_rollover.svg");

	// Insert the toolbar icons
	mediaPlayerControlToolbar = InsertPlugInToolSVG(_T(""), normalIcon, rolloverIcon, toggledIcon, wxITEM_CHECK, _("Media Player Control"), _T(""), NULL, -1, 0, this);


	// Wire up the event handler to receive events from the dialog
	// BUG BUG For some reason couldn't use wxAUI (Advanced User Interface), casting error ) to handle the close event  ??
	Connect(wxEVT_MEDIAPLAYERCONTROL_DIALOG_EVENT, wxCommandEventHandler(MediaPlayerControlPlugin::OnDialogEvent));

	
	mediaPlayerControlWindowVisible = false;

	// Notify OpenCPN what events we want to receive callbacks for
	return (WANTS_CONFIG | WANTS_TOOLBAR_CALLBACK | INSTALLS_TOOLBAR_TOOL | WANTS_PLUGIN_MESSAGING);
}

// OpenCPN is either closing down, or we have been disabled from the Preferences Dialog
bool MediaPlayerControlPlugin::DeInit(void) {
	Disconnect(wxEVT_MEDIAPLAYERCONTROL_DIALOG_EVENT, wxCommandEventHandler(MediaPlayerControlPlugin::OnDialogEvent));
	return TRUE;
}

// Indicate what version of the OpenCPN Plugin API we support
int MediaPlayerControlPlugin::GetAPIVersionMajor() {
	return OCPN_API_VERSION_MAJOR;
}

int MediaPlayerControlPlugin::GetAPIVersionMinor() {
	return OCPN_API_VERSION_MINOR;
}

// The plugin version numbers. 
int MediaPlayerControlPlugin::GetPlugInVersionMajor() {
	return PLUGIN_VERSION_MAJOR;
}

int MediaPlayerControlPlugin::GetPlugInVersionMinor() {
	return PLUGIN_VERSION_MINOR;
}

// Return descriptions for the Plugin
wxString MediaPlayerControlPlugin::GetCommonName() {
	return _T(PLUGIN_COMMON_NAME);
}

// BUG BUG Should use XML_SUMMARY & DESCRIPTION to avoid duplication
wxString MediaPlayerControlPlugin::GetShortDescription() {
	return _T(PLUGIN_SHORT_DESCRIPTION);
}

wxString MediaPlayerControlPlugin::GetLongDescription() {
	return _T(PLUGIN_LONG_DESCRIPTION);
}

// 32x32 pixel PNG file, use pgn2wx.pl perl script
wxBitmap* MediaPlayerControlPlugin::GetPlugInBitmap() {
		return mediaPlayerControlImage;
}

// We install one toolbar items
int MediaPlayerControlPlugin::GetToolbarToolCount(void) {
 return 1;
}

int MediaPlayerControlPlugin::GetToolbarItemId() { 
	return mediaPlayerControlToolbar; 
}

void MediaPlayerControlPlugin::SetDefaults(void) {
	// Is called when the plugin is enabled
	// Anything to do ?
}

void MediaPlayerControlPlugin::OnToolbarToolCallback(int id) {
	if (id == mediaPlayerControlToolbar) {

		// Display a non-modal Race Start Window
		if (!mediaPlayerControlWindowVisible) {
			mediaPlayerControlWindow = new MediaPlayerControlWindow(parentWindow, this);
			mediaPlayerControlWindowVisible = true;
			SetToolbarItemState(id, mediaPlayerControlWindowVisible);
			mediaPlayerControlWindow->Show(true);
		}
		else {
			mediaPlayerControlWindow->Close();
			delete mediaPlayerControlWindow;
			SetToolbarItemState(id, mediaPlayerControlWindowVisible);
		}
	}
}

void MediaPlayerControlPlugin::SetPluginMessage(wxString &message_id, wxString &message_body) {
	wxMessageBox(wxString::Format(_T("%s\n%s"),message_id, message_body.data()));
	return;
}

// Handle events from the Media Player Control Dialog
void MediaPlayerControlPlugin::OnDialogEvent(wxCommandEvent &event) {
	switch (event.GetId()) {
		// Keep the toolbar & canvas in sync with the display of the race start dialog
		case DIALOG_CLOSED:
			SetToolbarItemState(mediaPlayerControlToolbar, mediaPlayerControlWindowVisible);
			break;	
		default:
			event.Skip();
	}
}