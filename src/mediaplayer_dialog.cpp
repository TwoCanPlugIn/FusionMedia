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

#include "mediaplayer_dialog.h"

// BUG BUG DEBUG REMOVE
#include <wx/socket.h>
#include <wx/msgdlg.h>

extern wxDatagramSocket *debugSocket;
extern wxIPV4address addrPeer;
wxString msg;

// Tree Control Client Item Data used for folders & music tracks from media devices
MediaFileItem::MediaFileItem(int type, int id, wxString path) {
	itemType = type;
	itemId = id;
	itemPath = path;
	visited = false;
}

MediaFileItem::~MediaFileItem() {
}

int MediaFileItem::GetItemId() {
	return itemId;
}

int MediaFileItem::GetItemType() {
	return itemType;
}

wxString MediaFileItem::GetItemPath() {
	return itemPath;
}

bool MediaFileItem::GetVisited(void) {
	return visited;
}

void MediaFileItem::SetVisited(bool visit) {
	visited = visit;
}

void MediaFileItem::SetItemPath(wxString path) {
	itemPath = path;
}

// The plugin dialog, inherits from a base class developed using wxFormBuilder
MediaPlayerDialog::MediaPlayerDialog( wxWindow* parent, wxEvtHandler *handler) : MediaPlayerDialogBase(parent) {
	eventHandler = handler;
	Layout();
}

MediaPlayerDialog::~MediaPlayerDialog(void) {
}

// Event Handlers

// Handle changes from the radio buttons used to select different media sources
void MediaPlayerDialog::OnSourceChanged(wxCommandEvent& event) {
	wxCommandEvent *commandEvent;
	commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_SOURCE);
	commandEvent->SetString(radioBoxSource->GetString(radioBoxSource->GetSelection()));
	wxQueueEvent(eventHandler, commandEvent);
	// Only a usb media device has the concept of pause/play
	if (radioBoxSource->GetString(radioBoxSource->GetSelection()).Lower() == "usb") {
		buttonPause->Enable(true);
	}
	else {
		buttonPause->Enable(false);
	}
}

// BUG BUG REMOVE Test stuff
void MediaPlayerDialog::PopulateStuff(void) {
	mediaTreeList->DeleteAllItems();
	wxTreeItemId root = mediaTreeList->AddRoot(wxT("Root"));
	MediaFileItem *rootItem = new MediaFileItem(MEDIA_FILE_TYPE::DEVICEFOLDER, 0, std::to_string(0));
	rootItem->SetVisited(true);
	mediaTreeList->SetItemData(root, rootItem);
	
	for (int i = 0; i < 5; i++) {
		wxTreeItemId parent = mediaTreeList->AppendItem(root, wxString::Format("Parent %d", i), 0, 0);
		MediaFileItem *parentItem = new MediaFileItem(MEDIA_FILE_TYPE::MTPFOLDER, i, wxString::Format("%s/%s", std::to_string(rootItem->GetItemId()), std::to_string(i)));
		parentItem->SetVisited(true);
		mediaTreeList->SetItemData(parent, parentItem);

		for (int k = 0; k < 2; k++) {
			wxTreeItemId child = mediaTreeList->AppendItem(parent, wxString::Format("Child %d", k), 0, 0);
			MediaFileItem *childItem = new MediaFileItem(MEDIA_FILE_TYPE::VIRTUALFOLDER, k, wxString::Format("%s/%s", parentItem->GetItemPath(), std::to_string(k)));
			childItem->SetVisited(true);
			mediaTreeList->SetItemData(child, childItem);
			for (int j = 0; j < 10; j++) {
				wxTreeItemId grandChild = mediaTreeList->AppendItem(child, wxString::Format("Grand Child %d of Child %d", j, k), 1, 1);
				mediaTreeList->SetItemData(grandChild, new MediaFileItem(MEDIA_FILE_TYPE::MUSICTRACK, j, wxString::Format("%s/%s", childItem->GetItemPath(), std::to_string(j))));
			}
		}
	}

	// On Expansion, we get a selection changed event to the first item, not to the one we are expanding
	//wxTreeItemIdValue cookie;
	//mediaTreeList->SelectItem(mediaTreeList->GetFirstChild(root, cookie));
	//mediaTreeList->ExpandAll();
	
}

// BUG BUG REMOVE, Not used
void MediaPlayerDialog::OnExpanding(wxTreeEvent& event) {
}

// If we've expanded an item, ensure it has been selected
void MediaPlayerDialog::OnExpanded(wxTreeEvent& event) {
	mediaTreeList->SelectItem(event.GetItem());
}

// Context Menu for music tracks
void MediaPlayerDialog::OnItemContextMenu(wxTreeEvent& event) {
	MediaFileItem *itemData;
	itemData = (MediaFileItem *)mediaTreeList->GetItemData(event.GetItem());
	if (itemData->GetItemType() == MEDIA_FILE_TYPE::MUSICTRACK) {
		wxMenu menu;
		menu.Append(ID_MENU_ITEM_PLAY, "&Play");

		if (mediaTreeList->GetPopupMenuSelectionFromUser(menu) == ID_MENU_ITEM_PLAY) {
			wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_FILE_PLAY);
			commandEvent->SetString(std::to_string(itemData->GetItemId()));
			wxQueueEvent(eventHandler, commandEvent);
		}
	}
}

void MediaPlayerDialog::OnZone0VolumeChanged(wxCommandEvent& event) {
	wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_ZONE0);
	commandEvent->SetString(std::to_string(sliderZone0Volume->GetValue()));
	wxQueueEvent(eventHandler, commandEvent);	
}

void MediaPlayerDialog::OnZone1VolumeChanged(wxCommandEvent& event) {
	wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_ZONE1);
	commandEvent->SetString(std::to_string(sliderZone1Volume->GetValue()));
	wxQueueEvent(eventHandler, commandEvent);
}

void MediaPlayerDialog::OnButtonReverse(wxCommandEvent& event) {
	wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_PREVIOUS);
	commandEvent->SetString(_T("Previous"));
	wxQueueEvent(eventHandler, commandEvent);
}

void MediaPlayerDialog::OnButtonPause(wxCommandEvent& event) {
	wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_PAUSE);
	commandEvent->SetString(buttonPause->GetLabel());
	wxQueueEvent(eventHandler, commandEvent);
	isPaused = !isPaused;
	buttonPause->SetLabel(isPaused ? "Play" :"Pause");
} 

void MediaPlayerDialog::OnButtonForward(wxCommandEvent& event) {
	wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_NEXT);
	commandEvent->SetString(_T("Next"));
	wxQueueEvent(eventHandler, commandEvent);
}

// We need to use relative paths to navigate through the music folders & tracks
void MediaPlayerDialog::OnSelectedV1(wxTreeEvent& event) {
	// Need to save a reference to the previous item, so that we can construct a relative path
	previousItem = currentItem;
	MediaFileItem *previousItemData = (MediaFileItem *)mediaTreeList->GetItemData(previousItem);
	// Mark as having visited this so we don't retrive the child items again
	//previousItemData->SetVisited(true);

	currentItem = mediaTreeList->GetFocusedItem(); // Why not event.GetItem() ??
	if (currentItem.IsOk()) {
		mediaTreeList->SelectItem(currentItem);
		MediaFileItem *currentItemData = (MediaFileItem *)mediaTreeList->GetItemData(currentItem);

		if (currentItemData->GetItemType() != MEDIA_FILE_TYPE::MUSICTRACK) {

			if (currentItemData->GetVisited() == false) {

				// We have yet to visit this folder, so need to populate it
				// previousItem != mediaTreeList->GetRootItem()
				// and it is not the root item 
				// if it was, currentItem = previous item)
				if (previousItem != mediaTreeList->GetRootItem()) {
					wxString relPath;
#if (defined (__APPLE__) && defined (__MACH__))
					std::experimental::filesystem::path a = previousItemData->GetItemPath().ToStdString();
					std::experimental::filesystem::path b = currentItemData->GetItemPath().ToStdString();
					relPath = std::experimental::filesystem::relative(b, a).generic_string();
#else
					std::filesystem::path a = previousItemData->GetItemPath().ToStdString();
					std::filesystem::path b = currentItemData->GetItemPath().ToStdString();
					relPath = std::filesystem::relative(b, a).generic_string();
#endif
					// To process relative path
					wxString token;
					wxStringTokenizer tokenizer(relPath, "/");
					while (tokenizer.HasMoreTokens()) {
						token = tokenizer.GetNextToken();
						if (token == "..") {
							wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_PREVIOUS_FOLDER);
							commandEvent->SetString(std::to_string(currentItemData->GetItemId()));
							wxQueueEvent(eventHandler, commandEvent);
						}
						else {
							wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_NEXT_FOLDER);
							commandEvent->SetString(std::to_string(currentItemData->GetItemId()));
							wxQueueEvent(eventHandler, commandEvent);
						}
					}

					// BUG BUG REMOVE
					wxString debugMessage = wxString::Format("OnSelected Normal Folder: Name: %s, ID: %d, Type: 0x%02X, Path: %s", mediaTreeList->GetItemText(currentItem), currentItemData->GetItemId(),
						currentItemData->GetItemType(), currentItemData->GetItemPath());
					debugSocket->SendTo(addrPeer, debugMessage.data(), debugMessage.Length());

				}
				else {
					// this is the root folder, we have yet to visit
					wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_NEXT_FOLDER);
					commandEvent->SetString(std::to_string(currentItemData->GetItemId()));
					wxQueueEvent(eventHandler, commandEvent);

					// BUG BUG REMOVE
					wxString debugMessage = wxString::Format("OnSelected Root Folder: Name: %s, ID: %d, Type: 0x%02X, Path: %s", mediaTreeList->GetItemText(currentItem), currentItemData->GetItemId(),
						currentItemData->GetItemType(), currentItemData->GetItemPath());
					debugSocket->SendTo(addrPeer, debugMessage.data(), debugMessage.Length());
				}
			}
		}

		
	}
}

// Version Two of Above
void MediaPlayerDialog::OnSelectedV3(wxTreeEvent& event) {

	previousItem = currentItem;
	MediaFileItem *previousItemData = (MediaFileItem *)mediaTreeList->GetItemData(previousItem);

	currentItem = event.GetItem();
	
	if (currentItem.IsOk()) {
		MediaFileItem *currentItemData = (MediaFileItem *)mediaTreeList->GetItemData(currentItem);
		// Only search for child items for folders
		if (currentItemData->GetItemType() != MEDIA_FILE_TYPE::MUSICTRACK) {
			// If we have yet to visit this folder and retrieve the child items
			if (currentItemData->GetVisited() == false) {
				// Mark as having visited this so we don't retrieve the child items again
				currentItemData->SetVisited(true);
				if (currentItem == mediaTreeList->GetRootItem()) {
					// Special Case as this is the root folder, we have yet to visit
					wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_NEXT_FOLDER);
					commandEvent->SetString(std::to_string(currentItemData->GetItemId()));
					wxQueueEvent(eventHandler, commandEvent);

					// BUG BUG REMOVE
					wxString debugMessage = wxString::Format("OnSelected Root Folder: Name: %s, ID: %d, Type: 0x%02X, Path: %s", mediaTreeList->GetItemText(currentItem), currentItemData->GetItemId(),
						currentItemData->GetItemType(), currentItemData->GetItemPath());
					debugSocket->SendTo(addrPeer, debugMessage.data(), debugMessage.Length());
				}
				else { // Not the root folder, need to construct a relative path
					wxString relPath;
					std::filesystem::path a = previousItemData->GetItemPath().ToStdString();
					std::filesystem::path b = currentItemData->GetItemPath().ToStdString();
					relPath = std::filesystem::relative(b, a).generic_string();

					// To process relative path
					wxString token;
					wxStringTokenizer tokenizer(relPath, "/");
					while (tokenizer.HasMoreTokens()) {
						token = tokenizer.GetNextToken();
						if (token == "..") {
							wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_PREVIOUS_FOLDER);
							commandEvent->SetString(std::to_string(currentItemData->GetItemId()));
							wxQueueEvent(eventHandler, commandEvent);
						}
						else {
							wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_NEXT_FOLDER);
							commandEvent->SetString(std::to_string(currentItemData->GetItemId()));
							wxQueueEvent(eventHandler, commandEvent);
						}
					}

					// BUG BUG REMOVE
					wxString debugMessage = wxString::Format("OnSelected Normal Folder: Name: %s, ID: %d, Type: 0x%02X, Path: %s", mediaTreeList->GetItemText(currentItem), currentItemData->GetItemId(),
						currentItemData->GetItemType(), currentItemData->GetItemPath());
					debugSocket->SendTo(addrPeer, debugMessage.data(), debugMessage.Length());
				}
			}
		}
	}
	else {
		// Item is not OK ?
		// BUG BUG REMOVE
		wxString debugMessage = "OnSelected Not OK";
		debugSocket->SendTo(addrPeer, debugMessage.data(), debugMessage.Length());
	}
}

// Version Three of Above
void MediaPlayerDialog::OnSelected(wxTreeEvent& event) {

	wxString debugMessage;

	previousItem = currentItem;
	MediaFileItem *previousItemData = (MediaFileItem *)mediaTreeList->GetItemData(previousItem);

	currentItem = mediaTreeList->GetFocusedItem(); //event.GetItem();
	MediaFileItem *currentItemData = (MediaFileItem *)mediaTreeList->GetItemData(currentItem);

	if (currentItem == mediaTreeList->GetRootItem()) {
		// This is the root and we have yet to retrieve any child items
		if (!mediaTreeList->HasChildren(currentItem)) {
			currentItemData->SetVisited(true);
			wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_NEXT_FOLDER);
			commandEvent->SetString(std::to_string(currentItemData->GetItemId()));
			wxQueueEvent(eventHandler, commandEvent);

			// BUG BUG REMOVE
			debugMessage = wxString::Format("OnSelected Root Folder: Name: %s, ID: %d, Type: 0x%02X, Path: %s", mediaTreeList->GetItemText(currentItem), currentItemData->GetItemId(),
				currentItemData->GetItemType(), currentItemData->GetItemPath());
			debugSocket->SendTo(addrPeer, debugMessage.data(), debugMessage.Length());
		}
	}
	else {
		// Not the root
		// Only search for child items if it is a folder not a music track
		if (currentItemData->GetItemType() != MEDIA_FILE_TYPE::MUSICTRACK) {
			// If we have yet to visit this folder and retrieve the child items
			// Can't use HasChildren in case it is an empty folder that we have already visited
			if (currentItemData->GetVisited() == false) {
				// Mark as having visited this so we don't retrieve the child items again
				currentItemData->SetVisited(true);

				wxString relPath;
				std::filesystem::path a = previousItemData->GetItemPath().ToStdString();
				std::filesystem::path b = currentItemData->GetItemPath().ToStdString();
				relPath = std::filesystem::relative(b, a).generic_string();

				debugMessage = wxString::Format("A: %s, B: %s, Rel: %s", previousItemData->GetItemPath().ToAscii().data(), currentItemData->GetItemPath().ToAscii().data(), relPath.ToAscii().data());
				debugSocket->SendTo(addrPeer, debugMessage.data(), debugMessage.Length());

				// To process relative path
				wxString token;
				wxStringTokenizer tokenizer(relPath, "/");
				while (tokenizer.HasMoreTokens()) {
					token = tokenizer.GetNextToken();
					if (token == "..") {
						wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_PREVIOUS_FOLDER);
						commandEvent->SetString(std::to_string(currentItemData->GetItemId()));
						wxQueueEvent(eventHandler, commandEvent);
					}
					else {
						wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_NEXT_FOLDER);
						commandEvent->SetString(std::to_string(currentItemData->GetItemId()));
						wxQueueEvent(eventHandler, commandEvent);
					}
				}

				// BUG BUG REMOVE
				debugMessage = wxString::Format("OnSelected Normal Folder: Name: %s, ID: %d, Type: 0x%02X, Path: %s", mediaTreeList->GetItemText(currentItem), currentItemData->GetItemId(),
					currentItemData->GetItemType(), currentItemData->GetItemPath());
				debugSocket->SendTo(addrPeer, debugMessage.data(), debugMessage.Length());
			} // not visited
		} // not musc folder
	} // not root
}



void MediaPlayerDialog::OnButtonMute(wxCommandEvent& event) {
	wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_MUTE);
	commandEvent->SetString(buttonMute->GetLabel());
	wxQueueEvent(eventHandler, commandEvent);
	isMuted = !isMuted;
	buttonMute->SetLabel(isMuted ? "Unmute" : "Mute");
}

void MediaPlayerDialog::OnButtonPower(wxCommandEvent& event) {
	wxCommandEvent *commandEvent = new wxCommandEvent(wxEVT_MEDIAPLAYER_DIALOG_EVENT, MEDIAPLAYER_ON);
	commandEvent->SetString(buttonPower->GetLabel());
	wxQueueEvent(eventHandler, commandEvent);
	isOn = !isOn;
	buttonPower->SetLabel(isOn ? "Off" : "On");
}

// Setters invoked to keep dialog in synch with responses received from the media player
// Eg. the media player could be simultaneously controlled by its own remote control, 
// so we monitor the responses and ensure we display the same values.
void MediaPlayerDialog::SetZone0Volume(unsigned int volume) {
	sliderZone0Volume->SetValue(volume);
}

void MediaPlayerDialog::SetZone1Volume(unsigned int volume) {
	sliderZone1Volume->SetValue(volume);
}

// BUG BUG Does not work due to use of wxStaticBoxSizer
void MediaPlayerDialog::SetZone0Name(wxString name) {
	sliderZone0Volume->SetLabel(name);
}

void MediaPlayerDialog::SetZone1Name(wxString name) {
	sliderZone1Volume->SetLabelText(name);
}

void MediaPlayerDialog::SetStatus(wxString statusMessage) {
	labelMediaPlayerStatus->SetLabel(statusMessage);
}

void MediaPlayerDialog::SetGaugeRange(unsigned int seconds) {
	gaugeProgress->SetRange(seconds);
}

void MediaPlayerDialog::SetGaugeValue(unsigned int elapsedTime) {
	gaugeProgress->SetValue(elapsedTime);
	gaugeProgress->Refresh();
}

void MediaPlayerDialog::SetPlayState(bool playingState) {
	isPaused = playingState;
	buttonPause->SetLabelText(isPaused ? "Play" : "Pause");
}

void MediaPlayerDialog::SetPowerState(bool powerState) {
	isOn = powerState;
	buttonPower->SetLabel(isOn ? "Off" : "On");
}

void MediaPlayerDialog::SetMuteState(bool muteState) {
	isMuted = muteState;
	buttonMute->SetLabel(isMuted ? "Unmute" : "Mute");
}

void MediaPlayerDialog::SetSource(wxString sourceName) {
	for (size_t i = 0; i < radioBoxSource->GetCount(); i++) {
		if (radioBoxSource->GetString(i).Lower() == sourceName) {
			if (radioBoxSource->GetSelection() != i) {
				radioBoxSource->SetSelection(i);
			}
		}
	}
	// Pause/Play only applicable to USB/MTP/iPod devices
	if (radioBoxSource->GetString(radioBoxSource->GetSelection()).Lower() == "usb") {
		buttonPause->Enable(true);
	}
	else {
		buttonPause->Enable(false);
	}
}

void MediaPlayerDialog::ResetDirectory(wxString rootFolderName, int fileType, int fileId) {
	mediaTreeList->DeleteAllItems();
	currentItem = mediaTreeList->AddRoot(rootFolderName);
	// Root folder has an id of zero
	mediaTreeList->SetItemData(currentItem, new MediaFileItem(fileType, fileId, std::to_string(0)));
	mediaTreeList->Refresh();
}

void MediaPlayerDialog::AppendFolder(wxString folderName, int fileType, int fileId) {
	MediaFileItem *currentItemData;
	currentItemData = (MediaFileItem *)mediaTreeList->GetItemData(currentItem);
	currentItemData->SetVisited(true);
	wxTreeItemId child = mediaTreeList->AppendItem(currentItem, folderName);
	mediaTreeList->SetItemData(child, new MediaFileItem(fileType, fileId, wxString::Format("%s/%d", currentItemData->GetItemPath(), fileId)));
	mediaTreeList->ExpandAll();
}

// BUG BUG Is this ever used ??
void MediaPlayerDialog::RenameFolder(wxString oldName, wxString newName) {
	wxTreeItemId itemId;
	wxTreeItemIdValue itemValue;
	mediaTreeList->GetFirstChild(itemId, itemValue);
	while (itemId.IsOk()) {
		if (mediaTreeList->GetItemText(itemId) == oldName) {
			mediaTreeList->SetItemText(itemId, newName);
		}
		mediaTreeList->GetNextChild(itemId, itemValue);
	}
}

void MediaPlayerDialog::AppendFile(wxString fileName, int fileId) {
	wxTreeItemId itemId = mediaTreeList->AppendItem(currentItem, fileName);
	// No need to generate a path for file items.
	mediaTreeList->SetItemData(itemId, new MediaFileItem(MEDIA_FILE_TYPE::MUSICTRACK, fileId, wxEmptyString));
	mediaTreeList->ExpandAllChildren(currentItem);
}