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

#ifndef MEDIAPLAYER_CONTROL_PANEL_H
#define MEDIAPLAYER_CONTROL_PANEL_H

#include "mediaplayer_dialogbase.h"
// for context menu
#include <wx/menu.h>
// for splitting relative paths
#include <wx/tokenzr.h>
// not sure where an unordered map is used
#include <unordered_map>
// To determine relative paths
#include <filesystem>
namespace fs = std::filesystem;

extern bool mediaPlayerControlPanelVisible;
extern bool isMuted;
extern bool isOn;
extern bool isPaused;
extern double frequency;
extern wxString mediaSource;

extern const wxEventType wxEVT_MEDIAPLAYER_DIALOG_EVENT;
extern const int MEDIAPLAYER_ON;
extern const int MEDIAPLAYER_MUTE;
extern const int MEDIAPLAYER_NEXT;
extern const int MEDIAPLAYER_PREVIOUS;
extern const int MEDIAPLAYER_PAUSE;
extern const int MEDIAPLAYER_ZONE0;
extern const int MEDIAPLAYER_ZONE1;
extern const int MEDIAPLAYER_SOURCE;
extern const int MEDIAPLAYER_BASS;
extern const int MEDIAPLAYER_MIDRANGE;
extern const int MEDIAPLAYER_TREBLE;
extern const int MEDIAPLAYER_NEXT_FOLDER;
extern const int MEDIAPLAYER_PREVIOUS_FOLDER;
extern const int MEDIAPLAYER_CURRENT_FOLDER;
extern const int MEDIAPLAYER_FILE_PLAY;

#define ID_MENU_ITEM_PLAY 100

typedef enum folderType {
	VIRTUALFOLDER = 0x49,
	DEVICEFOLDER = 0x41,
	MTPFOLDER = 0x47,
	PHYSICALFOLDER = 0x4F,
	MUSICTRACK = 0x1F
} MEDIA_FILE_TYPE;

// Client data used for tree list control items
class MediaFileItem : public wxTreeItemData {
public:
	MediaFileItem(int type, int id, wxString path);
	~MediaFileItem();
	int GetItemType();
	int GetItemId();
	wxString GetItemPath();
	void SetItemPath(wxString path);
	void SetVisited(bool visit);
	bool GetVisited(void);

private:
	int itemType;
	int itemId;
	wxString itemPath;
	wxString mediaFileName;
	bool hasChildren;
	int count;
	bool visited;
};

class MediaPlayerDialog : public MediaPlayerDialogBase
{
	protected:
		void OnSourceChanged( wxCommandEvent& event );
		void OnZone0VolumeChanged( wxCommandEvent& event );
		void OnZone1VolumeChanged( wxCommandEvent& event );
		void OnButtonReverse( wxCommandEvent& event );
		void OnButtonPause( wxCommandEvent& event );
		void OnButtonForward( wxCommandEvent& event );
		void OnButtonMute( wxCommandEvent& event );
		void OnButtonPower(wxCommandEvent& event);
		void OnSelected(wxTreeEvent& event);
		void OnSelectedV1(wxTreeEvent& event);
		void OnSelectedV3(wxTreeEvent& event);
		void OnItemContextMenu(wxTreeEvent& event);
		void OnExpanding(wxTreeEvent& event);
		void OnExpanded(wxTreeEvent& event);
		
				
	public:
		MediaPlayerDialog( wxWindow* parent, wxEvtHandler *handler);
		~MediaPlayerDialog(void);
		// Getters & Setters
		void SetZone0Volume(unsigned int volume);
		void SetZone1Volume(unsigned int volume);
		void SetZone0Name(wxString name);
		void SetZone1Name(wxString name);
		void SetStatus(wxString statusMessage);
		void SetPlayState(bool playingState);
		void SetPowerState(bool powerState);
		void SetMuteState(bool muteState);
		void SetGaugeRange(unsigned int trackLength);
		void SetGaugeValue(unsigned int elapsedTime);
		void SetSource(wxString sourceName);
		void ResetDirectory(wxString rootFolderName, int fileType, int fileId);
		void AppendFolder(wxString folderName, int fileType, int fileId);
		void RenameFolder(wxString oldName, wxString newName);
		void AppendFile(wxString fileName, int fileId);

	private:
		wxEvtHandler *eventHandler;

		// BUG BUG DEBUG REMOVE
		void PopulateStuff(void);

		wxTreeItemId rootFolder;
		wxTreeItemId currentItem;
		wxTreeItemId previousItem;
				
};
#endif
