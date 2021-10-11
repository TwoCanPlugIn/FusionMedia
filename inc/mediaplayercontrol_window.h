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

#ifndef MEDIAPLAYERCONTROL_WINDOW_H
#define MEDIAPLAYERCONTROL_WINDOW_H

// The wxFrame base class from which we are derived
// Note wxFormBuilder used to generate UI
#include "mediaplayercontrol_windowbase.h"

// For the Stopwatch/Countdown timer
#include <wx/timer.h>

// BUG BUG DEBUG - Logging
#include <wx/log.h>

#include <algorithm>

// image for dialog icon
extern wxBitmap *mediaPlayerControlImage;

// Events posted to plugin
extern const wxEventType wxEVT_MEDIAPLAYERCONTROL_DIALOG_EVENT;
extern const int DIALOG_CLOSED;
extern const int DIALOG_ON;
extern const int DIALOG_OFF;


extern bool mediaPlayerControlWindowVisible;

class MediaPlayerControlWindow : public MediaPlayerControlWindowBase {
	
public:
	MediaPlayerControlWindow(wxWindow* parent, wxEvtHandler *handler);
	~MediaPlayerControlWindow();

	// wxTimer used as countdown timer 
	wxTimer *stopWatch;
	void OnTimer(wxTimerEvent& event);
	
	// The plugin event handler to which we post events
	wxEvtHandler *eventHandlerAddress;
	
protected:
	//overridden methods from the base class
	void OnClose(wxCloseEvent& event);
	void OnReset(wxCommandEvent &event);
	void OnStart(wxCommandEvent &event);
	void OnPort(wxCommandEvent &event);
	void OnStarboard(wxCommandEvent &event);
	void OnCancel(wxCommandEvent &event);
		
private:
	void Initialize(void);
	void ResetTimer(void);
	// Race start countdown timer
	int totalSeconds;
	int seconds;
	int minutes;
	// Port and Starboard ends of the start line
	double starboardLatitude;
	double starboardLongitude;
	double portLatitude;
	double portLongitude;
	// Whether the ends of the start line have been "pinged".
	bool portMark;
	bool starboardMark;
	// Bearing of the start Line (from starboard to port end of the line)
	double startLineBearing;
	// Where we are projected to cross the start line.
	double intersectLatitude;
	double intersectLongitude;
	// Distance & Bearing functions
	bool CalculateIntersection(double latitude1, double longitude1, double  bearing1, double latitude2, double longitude2, double bearing2, double *lat3, double *lon3);
	double BearingBetweenPoints(double latitude1, double longitude1, double latitude2, double longitude2);
	double HaversineFormula(double latutude1, double longitude1, double latitude2, double longitude2);
	double SphericalCosines(double latitude1, double longitude1, double latitude2, double longitude2);
	double Bearing(double latitude1, double longitude1, double latitude2, double longitude2);
	bool IsWithinBoundingBox(double latitude, double longitude);
		
	// Parent Window size
	int parentWidth;
	int parentHeight;
};

#endif
