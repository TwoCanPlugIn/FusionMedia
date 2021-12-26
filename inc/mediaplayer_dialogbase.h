///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/gauge.h>
#include <wx/statbox.h>
#include <wx/treectrl.h>
#include <wx/radiobox.h>
#include <wx/slider.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MediaPlayerDialogBase
///////////////////////////////////////////////////////////////////////////////
class MediaPlayerDialogBase : public wxPanel
{
	private:

	protected:
		wxStaticText* labelMediaPlayerStatus;
		wxGauge* gaugeProgress;
		wxTreeCtrl* mediaTreeList;
		wxRadioBox* radioBoxSource;
		wxSlider* sliderZone0Volume;
		wxSlider* sliderZone1Volume;
		wxButton* buttonReverse;
		wxButton* buttonPause;
		wxButton* buttonForward;
		wxButton* buttonMute;
		wxButton* buttonPower;

		// Virtual event handlers, overide them in your derived class
		virtual void OnRightMouse( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnExpanded( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnExpanding( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnItemContextMenu( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnSelected( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnSourceChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnZone0VolumeChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnZone1VolumeChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonReverse( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonPause( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonForward( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonMute( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonPower( wxCommandEvent& event ) { event.Skip(); }


	public:

		MediaPlayerDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 573,669 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );
		~MediaPlayerDialogBase();

};

