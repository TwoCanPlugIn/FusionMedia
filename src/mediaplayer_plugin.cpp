// Copyright(C) 2021 by Steven Adler
//
// This file is part of Fusion Media Player plugin for OpenCPN.
//
// Fusion Media Player plugin for OpenCPN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fusion Media Player plugin for OpenCPN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Fusion Media Player plugin for OpenCPN. If not, see <https://www.gnu.org/licenses/>.
//
// NMEA2000® is a registered trademark of the National Marine Electronics Association
// Fusion® is a registered trademark of Fusion Entertainment
//
// This software has been developed independently of Fusion Entertainment and is not
// sanctioned nor supported by them.
//
// Project: Fusion Media Player Plugin
// Description: Remotely Control Fusion Media Players (via TwoCan plugin)
// Owner: twocanplugin@hotmail.com
// Date: 10/10/2021
// Version History:
// 1.0 Initial Release
//
// BUG BUG To Do
// Pre-set radio stations, Tone controls, Bluetooth pairing with media devices.

#include "mediaplayer_plugin.h"
#include "mediaplayer_icon.h"

// BUG BUG REMOVE
#include <wx/socket.h>

// Some globals accessed by both the plugin and the dialog

// Mute/Unmute
bool isMuted = false;

// Power On/Off
bool isOn = false;

// Pause/Play
bool isPaused = false;

// The name of the source input (from the radio boxes)
wxString mediaSource;

// BUG BUG REMOVE 
// Debug spew via UDP
wxDatagramSocket *debugSocket;
wxIPV4address addrLocal;
wxIPV4address addrPeer;

// The class factories, used to create and destroy instances of the PlugIn
extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
	return new MediaPlayerPlugin(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) {
	delete p;
}

// Constructor
MediaPlayerPlugin::MediaPlayerPlugin(void *ppimgr) : opencpn_plugin_117(ppimgr), wxEvtHandler() {
	
	// Load the plugin icon
	// refer to mediaplayer_icons.cpp
	initialize_images();

	// Initialize Advanced User Interface Manager (AUI)
	auiManager = GetFrameAuiManager();
}

// Destructor
MediaPlayerPlugin::~MediaPlayerPlugin(void) {
	delete mediaPlayerLogo;
}

int MediaPlayerPlugin::Init(void) {
	// Maintain a reference to the OpenCPN window to use as the parent
	parentWindow = GetOCPNCanvasWindow();

	// Maintain a reference to the OpenCPN configuration object 
	configSettings = GetOCPNConfigObject();

	// Load Configuration Setings
	// BUG BUG Add Presets for the Radio Stations ??
	if (configSettings) {
		configSettings->SetPath(_T("/PlugIns/MediaPlayer"));
		configSettings->Read(_T("Source"), &mediaSource, "fm");
		configSettings->Read(_T("Zone0Volume"), &zone0Volume, 3);
		configSettings->Read(_T("Zone1Volume"), &zone1Volume, 3);
		configSettings->Read(_T("Visible"), &mediaPlayerDialogVisible, false);
	}
	else {
		mediaSource = "fm";
		zone0Volume = 3;
		zone1Volume = 3;
	}

	// Load toolbar icons
	wxString shareLocn = GetPluginDataDir(PLUGIN_PACKAGE_NAME) + wxFileName::GetPathSeparator() + _T("data") + wxFileName::GetPathSeparator();
	
	wxString normalIcon = shareLocn + _T("mediaplayer_icon_normal.svg");
	wxString toggledIcon = shareLocn + _T("mediaplayer_icon_toggled.svg");
	wxString rolloverIcon = shareLocn + _T("mediaplayer_icon_rollover.svg");

	// Insert the toolbar icons
	mediaPlayerToolbar = InsertPlugInToolSVG(_T(""), normalIcon, rolloverIcon, toggledIcon, wxITEM_CHECK, _("Fusion Media Player"), _T(""), NULL, -1, 0, this);

	// Instantiate the media player dialog
	mediaPlayerDialog = new  MediaPlayerDialog(parentWindow, this);

	// Display the default/previously saved values
	mediaPlayerDialog->SetZone0Volume(zone0Volume);
	mediaPlayerDialog->SetZone1Volume(zone1Volume);
	mediaPlayerDialog->SetSource(mediaSource);


	// Wire up the event handler to receive events from the dialog
	Connect(wxEVT_MEDIAPLAYER_DIALOG_EVENT, wxCommandEventHandler(MediaPlayerPlugin::OnDialogEvent));
	
	// BUG BUG REMOVE
	// Initialize UDP socket for debug spew
	addrLocal.Hostname();
	addrPeer.Hostname("127.0.0.1");
	addrPeer.Service(3001);

	debugSocket = new wxDatagramSocket(addrLocal, wxSOCKET_NONE);
	
	if (!debugSocket->IsOk()) {
		wxLogMessage(_T("ERROR: failed to create UDP peer socket"));
	}

	folderId = 0;

	// Notify OpenCPN what events we want to receive callbacks for
	return (WANTS_CONFIG | WANTS_TOOLBAR_CALLBACK | INSTALLS_TOOLBAR_TOOL | WANTS_PLUGIN_MESSAGING | USES_AUI_MANAGER | WANTS_LATE_INIT);
}

void MediaPlayerPlugin::LateInit(void) {
	// For some reason unbeknownst to me, the aui manager fails to wire up correctly if done
	// in the constructor or init. Seems to wire up correctly here though....

	// Load our dialog into the AUI Manager
	wxAuiPaneInfo paneInfo;
	paneInfo.Name(_T("Fusion Media Player"));
	paneInfo.Caption("Fusion Remote Control Plugin for OpenCPN");
	paneInfo.CloseButton(true);
	paneInfo.Float();
	paneInfo.Dockable(false);
	auiManager->AddPane(mediaPlayerDialog, paneInfo);
	auiManager->Connect(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(MediaPlayerPlugin::OnPaneClose), NULL, this);
	auiManager->Update();

	//paneInfo = auiManager->GetPane(_T("Fusion Media Player"));
	//if (paneInfo.IsOk()) {
		//auiManager->Connect(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(MediaPlayerPlugin::OnPaneClose), NULL, this);
	//}
}

// OpenCPN is either closing down, or we have been disabled from the Preferences Dialog
bool MediaPlayerPlugin::DeInit(void) {
	Disconnect(wxEVT_MEDIAPLAYER_DIALOG_EVENT, wxCommandEventHandler(MediaPlayerPlugin::OnDialogEvent));
	auiManager->Disconnect(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(MediaPlayerPlugin::OnPaneClose), NULL, this);
	auiManager->UnInit();
	auiManager->DetachPane(mediaPlayerDialog);
	delete mediaPlayerDialog;	

	if (configSettings) {
		configSettings->SetPath(_T("/PlugIns/MediaPlayer"));
		configSettings->Write(_T("Source"), mediaSource);
		configSettings->Write(_T("Zone0Volume"), zone0Volume);
		configSettings->Write(_T("Zone1Volume"), zone1Volume);
		configSettings->Write(_T("Visible"), mediaPlayerDialogVisible);
	}

	// BUG BUG DEBUG REMOVE
	debugSocket->Close();
	return true;
}

// Overridden OpenCPN methods

// Indicate what version of the OpenCPN Plugin API we support
int MediaPlayerPlugin::GetAPIVersionMajor() {
	return OCPN_API_VERSION_MAJOR;
}

int MediaPlayerPlugin::GetAPIVersionMinor() {
	return OCPN_API_VERSION_MINOR;
}

// The Media Player plugin version numbers. 
int MediaPlayerPlugin::GetPlugInVersionMajor() {
	return PLUGIN_VERSION_MAJOR;
}

int MediaPlayerPlugin::GetPlugInVersionMinor() {
	return PLUGIN_VERSION_MINOR;
}

// Descriptions for the Media Player plugin
wxString MediaPlayerPlugin::GetCommonName() {
	return _T(PLUGIN_COMMON_NAME);
}

wxString MediaPlayerPlugin::GetShortDescription() {
	return _T(PLUGIN_SHORT_DESCRIPTION);
}

wxString MediaPlayerPlugin::GetLongDescription() {
	return _T(PLUGIN_LONG_DESCRIPTION);
}

// Media Player plugin icon
// 32x32 pixel PNG file, use pgn2wx.pl perl script
wxBitmap* MediaPlayerPlugin::GetPlugInBitmap() {
		return mediaPlayerLogo;
}

// We install one toolbar items
int MediaPlayerPlugin::GetToolbarToolCount(void) {
 return 1;
}

int MediaPlayerPlugin::GetToolbarItemId() { 
	return mediaPlayerToolbar; 
}

void MediaPlayerPlugin::SetDefaults(void) {
	// Is called when the plugin is enabled
	// Anything to do ?
}

// UpdateAUI Status is invoked by OpenCPN when the saved AUI perspective is loaded
void MediaPlayerPlugin::UpdateAuiStatus(void) {
	wxAuiPaneInfo paneInfo;
	paneInfo = auiManager->GetPane(_T("Fusion Media Player"));
	mediaPlayerDialogVisible = paneInfo.IsShown();
	SetToolbarItemState(mediaPlayerToolbar, mediaPlayerDialogVisible);
}

// Toggle the display of dialog as appropriate when the toolbar button is pressed
void MediaPlayerPlugin::OnToolbarToolCallback(int id) {
	if (id == mediaPlayerToolbar) {
		mediaPlayerDialogVisible = !mediaPlayerDialogVisible;
		auiManager->GetPane(_T("Fusion Media Player")).Show(mediaPlayerDialogVisible);
		auiManager->Update();
		SetToolbarItemState(id, mediaPlayerDialogVisible);
	}
}

// Keep the toolbar in synch with the pane state (user has closed the dialog from the "x" button
void MediaPlayerPlugin::OnPaneClose(wxAuiManagerEvent& event) {
	// BUG BUG Do I need to check that the event belongs to our pane/window ?
	mediaPlayerDialogVisible = false;
	SetToolbarItemState(mediaPlayerToolbar, mediaPlayerDialogVisible);
}

// Receive the OpenCPN Plugin Messages. Only interested in message id "TWOCAN_MEDIA_PLAYER"
void MediaPlayerPlugin::SetPluginMessage(wxString &message_id, wxString &message_body) {

	// Brief description of the JSON schema used by TwoCan
	// Device Name
	// root["entertainment"]["device"]["name"]

	// Input source
	// root["entertainment"]["device"]["source"]["name"]
	// root["entertainment"]["device"]["source"]["sourceid"]

	// Current session
	// root["entertainment"]["device"]["source"]["ssessionid"]

	// Volume
	// root["entertainment"]["device"]["zone0"]["volume"]
	// root["entertainment"]["device"]["zone1"]["volume"]
	// root["entertainment"]["device"]["zone2"]["volume"]
	
	// Tone Settings
	// root["entertainment"]["device"]["tone"]["bass"]
	// root["entertainment"]["device"]["tone"]["midrange"]
	// root["entertainment"]["device"]["tone"]["treble"]

	// Balance
	// root["entertainment"]["device"]["zone0"]["balance"]
	// root["entertainment"]["device"]["zone1"]["balance"]
	// root["entertainment"]["device"]["zone2]["balance"]

	// Radio
	// root["entertainment"]["device"]["radio"]["name"]
	// root["entertainment"]["device"]["radio"]["frequency"] 

	// Music Tracks
	// root["entertainment"]["device"]["track"]["name"]
	// root["entertainment"]["device"]["track"]["trackid"]
	// root["entertainment"]["device"]["track"]["tracks"]
	// root["entertainment"]["device"]["track"]["length"] 
	// root["entertainment"]["device"]["track"]["elapsedtime"]
	
	// Device Status
	// root["entertainment"]["device"]["power"]
	// root["entertainment"]["device"]["playing"]
	// root["entertainment"]["device"]["repeat"]
	// root["entertainment"]["device"]["shuffle"]
	// root["entertainment"]["device"]["pause"]
	// root["entertainment"]["device"]["next"]
	// root["entertainment"]["device"]["previous"]
	// root["entertainment"]["device"]["mute"]

	// Media
	// root["entertainment"]["device"]["media"]["folderid"]
	// root["entertainment"]["device"]["media"]["sessionid"]

	wxJSONReader jsonReader;
	wxJSONWriter jsonWriter;
	wxJSONValue root;

	if (message_id == _T("TWOCAN_MEDIA_PLAYER")) {
		if (jsonReader.Parse(message_body, &root) > 0) {
			// Save the erroneous json text for debugging
			wxLogMessage("TwoCan Media Player, JSON Error in following text:");
			wxLogMessage("%s", message_body);
			wxArrayString jsonErrors = jsonReader.GetErrors();
			for (auto it : jsonErrors) {
				wxLogMessage(it);
			}
			return;
		}

		// BUG BUG DEBUG REMOVE
		//debugSocket->SendTo(addrPeer, message_body.data(), message_body.Length());

		// Display the name of the device
		if (root["entertainment"]["device"].HasMember("name")) {
			auiManager->GetPane("Fusion Media Player").caption = root["entertainment"]["device"]["name"].AsString();
			auiManager->Update();
		}

		// Display the power on/off state
		if (root["entertainment"]["device"].HasMember("power")) {
			mediaPlayerDialog->SetPowerState(root["entertainment"]["device"]["power"].AsBool());
		}

		// BUG BUG Check if used
		if (root["entertainment"]["device"]["source"].HasMember("count")) {
			totalSources = root["entertainment"]["device"]["source"]["count"].AsInt();
		}

		// Display the media source input
		if (root["entertainment"]["device"]["source"].HasMember("name")) {
			sessionId = root["entertainment"]["device"]["source"]["sessionid"].AsInt();
			mediaSource = root["entertainment"]["device"]["source"]["name"].AsString().Lower();
			mediaPlayerDialog->SetSource(mediaSource);
		}

		// Disply the mute/unmute status
		if (root["entertainment"]["device"].HasMember("mute")) {
			mediaPlayerDialog->SetMuteState(root["entertainment"]["device"]["mute"].AsBool());
		}

		// Display the playing/paused state
		if (root["entertainment"]["device"].HasMember("playing")) {
			mediaPlayerDialog->SetPlayState(root["entertainment"]["device"]["playing"].AsBool());
			
			// if we've switched to a USB device, retrieve and display the media folders and music track names
			if (rqstSourceChange == true) {
				rqstSourceChange = false;
				//request the folders
				// 0 just gets message 16, ?
				// 1 is get current folder,
				// 2 is next
				// 3 is previous
				root.Clear();
				root["entertainment"]["device"]["media"]["request"] = 1;
				root["entertainment"]["device"]["media"]["foldersessionid"] = sessionId;
				root["entertainment"]["device"]["media"]["folderid"] = folderId;
				jsonWriter.Write(root, message_body);
				SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);

				// BUG BUG DEBUG REMOVE
				debugSocket->SendTo(addrPeer, message_body.data(), message_body.Length());
			}
		}

		// If playing media from a USB device, populate the progress bar
		// If an AM or FM radio, use the progress bar to display the tuned frequency
		if (root["entertainment"]["device"]["track"].HasMember("length")) {
			mediaPlayerDialog->SetGaugeRange(root["entertainment"]["device"]["track"]["length"].AsInt());
		}

		// Set the progress bar value
		if (root["entertainment"]["device"]["track"].HasMember("elapsedtime")) {
			mediaPlayerDialog->SetGaugeValue(root["entertainment"]["device"]["track"]["elapsedtime"].AsInt());
		}

		// And display tha track title
		if (root["entertainment"]["device"]["track"].HasMember("name")) {
			mediaPlayerDialog->SetStatus(root["entertainment"]["device"]["track"]["name"].AsString());
		}

		// Similarly if a radio, display the station name (if available)
		if (root["entertainment"]["device"]["radio"].HasMember("name")) {
			mediaPlayerDialog->SetStatus(root["entertainment"]["device"]["radio"]["name"].AsString());
		}

		// Save the radio frequency for tuning up and down
		if (root["entertainment"]["device"]["radio"].HasMember("frequency")) {
			frequency = root["entertainment"]["device"]["radio"]["frequency"].AsDouble();
		}

		// Set zone values
		// BUG BUG wxStaticBoxSizer labels can't be dynamically changed ??
		// BUG BUG To Do balance
		if (root["entertainment"]["device"]["zone0"].HasMember("name")) {
			mediaPlayerDialog->SetZone0Name(root["entertainment"]["device"]["zone0"]["name"].AsString());
		}

		if (root["entertainment"]["device"]["zone0"].HasMember("volume")) {
			mediaPlayerDialog->SetZone0Volume(root["entertainment"]["device"]["zone0"]["volume"].AsInt());
		}
			
		if (root["entertainment"]["device"]["zone1"].HasMember("name")) {
			mediaPlayerDialog->SetZone1Name(root["entertainment"]["device"]["zone1"]["name"].AsString());
		}

		if (root["entertainment"]["device"]["zone1"].HasMember("volume")) {
			mediaPlayerDialog->SetZone1Volume(root["entertainment"]["device"]["zone1"]["volume"].AsInt());
		}
		
		// This sequence begins from source change to usb.
		// A sequence of acks/confirms
		if (root["entertainment"]["device"]["media"].HasMember("rootfolder")) {
			folderSessionId = root["entertainment"]["device"]["media"]["foldersessionid"].AsInt();
			folderId = root["entertainment"]["device"]["media"]["folderid"].AsInt();
			bool isRootFolder = root["entertainment"]["device"]["media"]["rootfolder"].AsBool();
	
			// Send an ack
			root.Clear();
			root["entertainment"]["device"]["media"]["ack"] = true;
			jsonWriter.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);

			// BUG BUG DEBUG REMOVE
			debugSocket->SendTo(addrPeer, message_body.data(), message_body.Length());
		}

		if (root["entertainment"]["device"]["media"].HasMember("count")) {
			expectedRecords = root["entertainment"]["device"]["media"]["count"].AsInt();
			// BUG BUG Should check the folderSessionId matches 
			// Send a confirm
			root.Clear();
			root["entertainment"]["device"]["media"]["confirm"] = true;
			root["entertainment"]["device"]["media"]["folderid"] = folderId;
			root["entertainment"]["device"]["media"]["recordsreceived"] = expectedRecords;
			jsonWriter.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);

			// BUG BUG DEBUG REMOVE
			debugSocket->SendTo(addrPeer, message_body.data(), message_body.Length());
		}

		if (root["entertainment"]["device"]["media"].HasMember("foldername")) {
			wxString folderName = root["entertainment"]["device"]["media"]["foldername"].AsString();
			folderId = root["entertainment"]["device"]["media"]["folderid"].AsInt();
			int folderType = root["entertainment"]["device"]["media"]["foldertype"].AsInt();
			
			switch (folderType) {
				case MEDIA_FILE_TYPE::MTPFOLDER: // The top level device folder, aka root folder
					mediaPlayerDialog->ResetDirectory(folderName, folderType, folderId);
					break;
				case MEDIA_FILE_TYPE::DEVICEFOLDER: // physical or virtual folders
				case MEDIA_FILE_TYPE::PHYSICALFOLDER:
					mediaPlayerDialog->AppendFolder(folderName, folderType, folderId);
					// BUG BUG DEBUG REMOVE
					{wxString debugMessage = wxString::Format("JSON: Folder Name: %s, Type: 0x%02X, Id: %d",
						folderName.ToAscii().data(), folderType, folderId); 
					debugSocket->SendTo(addrPeer, debugMessage.data(), debugMessage.Length());
					}
					break;
				case MEDIA_FILE_TYPE::MUSICTRACK: // self explanatory
					mediaPlayerDialog->AppendFile(folderName, folderId);
					// BUG BUG DEBUG REMOVE
					{wxString debugMessage = wxString::Format("JSON: Track Name: %s", folderName.ToAscii().data());
					debugSocket->SendTo(addrPeer, debugMessage.data(), debugMessage.Length());
					}
					break;
			}

			receivedRecords++;
			// BUG BUG Should check the folderSessionId matches 

			// We've received all of the records, do we send a confirm ??
			// or if we haven't received all the records, request them again
			// do we need a timer ??
			if (expectedRecords == receivedRecords) { 
			//	root.Clear();
			//	root["entertainment"]["device"]["media"]["confirm"] = true;
			//	root["entertainment"]["device"]["media"]["folderid"] = folderId;
			//	root["entertainment"]["device"]["media"]["recordsreceived"] = receivedRecords;
			//	jsonWriter.Write(root, message_body);
			//	SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			}
		}
	}
}

// Handle events from the dialog
// Encode the JSON commands to send to the twocan media device so it can generate the NMEA 2000 messages
void MediaPlayerPlugin::OnDialogEvent(wxCommandEvent &event) {
	wxString message_body;
	wxJSONValue root;
	wxJSONWriter writer;
	switch (event.GetId()) {
		case MEDIAPLAYER_MUTE: 
			root["entertainment"]["device"]["mute"] = (event.GetString() == "Mute") ? true : false;
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			break;

		case MEDIAPLAYER_ON: 
			root["entertainment"]["device"]["power"] = (event.GetString() == "On") ? true : false;
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			break;

		case MEDIAPLAYER_ZONE0: 
			zone0Volume = std::atoi(event.GetString());
			root["entertainment"]["device"]["zone0"]["volume"] = zone0Volume;
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			break;

		case MEDIAPLAYER_ZONE1:
			zone1Volume = std::atoi(event.GetString());
			root["entertainment"]["device"]["zone1"]["volume"] = zone1Volume;
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			break;

		case MEDIAPLAYER_BASS:
			root["entertainment"]["device"]["tone"]["bass"] = std::atoi(event.GetString());
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			break;

		case MEDIAPLAYER_MIDRANGE:
			root["entertainment"]["device"]["tone"]["midrange"] = std::atoi(event.GetString());
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			break;

		case MEDIAPLAYER_TREBLE:
			root["entertainment"]["device"]["tone"]["treble"] = std::atoi(event.GetString());
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			break;

		case MEDIAPLAYER_NEXT:
			root["entertainment"]["device"]["next"] = true;
			// for radios need to send a frequency
			// fm steps are 5Mhz, am are 9 KHz, howver it looks like it does this automagically
			if ((mediaSource == "am") || (mediaSource == "fm")) {
				root["entertainment"]["device"]["frequency"] = frequency;
			}
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			break;

		case MEDIAPLAYER_PAUSE:
			root["entertainment"]["device"]["play"] = event.GetString() == "Play" ? true : false;
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);

			break;

		case MEDIAPLAYER_PREVIOUS:
			root["entertainment"]["device"]["previous"] = true;
			if ((mediaSource == "am") || (mediaSource == "fm")) {
				root["entertainment"]["device"]["frequency"] = frequency;
			}
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			break;

		case MEDIAPLAYER_NEXT_FOLDER:
			root["entertainment"]["device"]["media"]["request"] = 2;
			root["entertainment"]["device"]["media"]["folderid"] = std::atoi(event.GetString());
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			
			// BUG BUG DEBUG REMOVE
			// debugSocket->SendTo(addrPeer, message_body, message_body.Length());
			break;

		case MEDIAPLAYER_PREVIOUS_FOLDER:
			root["entertainment"]["device"]["media"]["request"] = 3;
			root["entertainment"]["device"]["media"]["folderid"] = 0; //std::atoi(event.GetString());
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);

			// BUG BUG DEBUG REMOVE
			debugSocket->SendTo(addrPeer, message_body, message_body.Length());
			break;
		
		case MEDIAPLAYER_FILE_PLAY:
			root["entertainment"]["device"]["track"] = event.GetString();
			root["entertainment"]["device"]["play"] = true;
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);
			break;

		case MEDIAPLAYER_SOURCE:
			root["entertainment"]["device"]["source"] =  event.GetString();
			writer.Write(root, message_body);
			SendPluginMessage(_T("TWOCAN_MEDIA_CONTROL"), message_body);

			// If we have changed the source to usb/mtp/ipod, update the media tree control
			if (event.GetString().Lower() == "usb") {
				rqstSourceChange = true;
			}
			else {
				// need to empty the tree control
			}
			
			break;
		
		default:
			event.Skip();
	}
}