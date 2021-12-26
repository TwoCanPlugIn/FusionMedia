///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "mediaplayer_dialogbase.h"

///////////////////////////////////////////////////////////////////////////

MediaPlayerDialogBase::MediaPlayerDialogBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* sizerDialog;
	sizerDialog = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sizerStatus;
	sizerStatus = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Media Player Status") ), wxVERTICAL );

	wxBoxSizer* sizerLabel;
	sizerLabel = new wxBoxSizer( wxHORIZONTAL );

	labelMediaPlayerStatus = new wxStaticText( sizerStatus->GetStaticBox(), wxID_ANY, wxT("Now playing..."), wxDefaultPosition, wxDefaultSize, 0 );
	labelMediaPlayerStatus->Wrap( -1 );
	sizerLabel->Add( labelMediaPlayerStatus, 1, wxALL, 5 );


	sizerStatus->Add( sizerLabel, 1, wxEXPAND, 5 );

	wxBoxSizer* sizerGauge;
	sizerGauge = new wxBoxSizer( wxHORIZONTAL );

	gaugeProgress = new wxGauge( sizerStatus->GetStaticBox(), wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	gaugeProgress->SetValue( 0 );
	sizerGauge->Add( gaugeProgress, 1, wxALL, 5 );


	sizerStatus->Add( sizerGauge, 1, wxEXPAND, 5 );


	sizerDialog->Add( sizerStatus, 1, wxEXPAND, 5 );

	wxStaticBoxSizer* sizerMedia;
	sizerMedia = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Media") ), wxHORIZONTAL );

	mediaTreeList = new wxTreeCtrl( sizerMedia->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_SINGLE );
	sizerMedia->Add( mediaTreeList, 2, wxALL|wxEXPAND, 5 );

	wxString radioBoxSourceChoices[] = { wxT("AM"), wxT("FM"), wxT("Aux"), wxT("Aux2"), wxT("USB"), wxT("BT") };
	int radioBoxSourceNChoices = sizeof( radioBoxSourceChoices ) / sizeof( wxString );
	radioBoxSource = new wxRadioBox( sizerMedia->GetStaticBox(), wxID_ANY, wxT("Audio Source"), wxDefaultPosition, wxDefaultSize, radioBoxSourceNChoices, radioBoxSourceChoices, 1, wxRA_SPECIFY_COLS );
	radioBoxSource->SetSelection( 0 );
	sizerMedia->Add( radioBoxSource, 1, wxALL, 5 );


	sizerDialog->Add( sizerMedia, 1, wxEXPAND, 5 );

	wxBoxSizer* sizerControls;
	sizerControls = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sizerZone0Volume;
	sizerZone0Volume = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Zone 0") ), wxHORIZONTAL );

	sliderZone0Volume = new wxSlider( sizerZone0Volume->GetStaticBox(), wxID_ANY, 1, 0, 10, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	sizerZone0Volume->Add( sliderZone0Volume, 1, wxALL, 5 );


	sizerControls->Add( sizerZone0Volume, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sizerZone1Volume;
	sizerZone1Volume = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Zone 1") ), wxHORIZONTAL );

	sliderZone1Volume = new wxSlider( sizerZone1Volume->GetStaticBox(), wxID_ANY, 1, 0, 10, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	sizerZone1Volume->Add( sliderZone1Volume, 1, wxALL, 5 );


	sizerControls->Add( sizerZone1Volume, 0, wxEXPAND, 5 );


	sizerDialog->Add( sizerControls, 0, wxEXPAND, 5 );

	wxBoxSizer* sizerButons;
	sizerButons = new wxBoxSizer( wxHORIZONTAL );

	buttonReverse = new wxButton( this, wxID_ANY, wxT("Previous"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	sizerButons->Add( buttonReverse, 1, wxALL, 5 );

	buttonPause = new wxButton( this, wxID_ANY, wxT(" Play"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	sizerButons->Add( buttonPause, 1, wxALL, 5 );

	buttonForward = new wxButton( this, wxID_ANY, wxT("Next"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	sizerButons->Add( buttonForward, 1, wxALL, 5 );

	buttonMute = new wxButton( this, wxID_ANY, wxT("  Mute   "), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	sizerButons->Add( buttonMute, 1, wxALL, 5 );

	buttonPower = new wxButton( this, wxID_ANY, wxT("   On   "), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	sizerButons->Add( buttonPower, 1, wxALL, 5 );


	sizerDialog->Add( sizerButons, 0, wxEXPAND, 5 );


	this->SetSizer( sizerDialog );
	this->Layout();

	// Connect Events
	mediaTreeList->Connect( wxEVT_RIGHT_UP, wxMouseEventHandler( MediaPlayerDialogBase::OnRightMouse ), NULL, this );
	mediaTreeList->Connect( wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( MediaPlayerDialogBase::OnExpanded ), NULL, this );
	mediaTreeList->Connect( wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler( MediaPlayerDialogBase::OnExpanding ), NULL, this );
	mediaTreeList->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( MediaPlayerDialogBase::OnItemContextMenu ), NULL, this );
	mediaTreeList->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( MediaPlayerDialogBase::OnSelected ), NULL, this );
	radioBoxSource->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( MediaPlayerDialogBase::OnSourceChanged ), NULL, this );
	sliderZone0Volume->Connect( wxEVT_SLIDER, wxCommandEventHandler( MediaPlayerDialogBase::OnZone0VolumeChanged ), NULL, this );
	sliderZone1Volume->Connect( wxEVT_SLIDER, wxCommandEventHandler( MediaPlayerDialogBase::OnZone1VolumeChanged ), NULL, this );
	buttonReverse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MediaPlayerDialogBase::OnButtonReverse ), NULL, this );
	buttonPause->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MediaPlayerDialogBase::OnButtonPause ), NULL, this );
	buttonForward->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MediaPlayerDialogBase::OnButtonForward ), NULL, this );
	buttonMute->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MediaPlayerDialogBase::OnButtonMute ), NULL, this );
	buttonPower->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MediaPlayerDialogBase::OnButtonPower ), NULL, this );
}

MediaPlayerDialogBase::~MediaPlayerDialogBase()
{
	// Disconnect Events
	mediaTreeList->Disconnect( wxEVT_RIGHT_UP, wxMouseEventHandler( MediaPlayerDialogBase::OnRightMouse ), NULL, this );
	mediaTreeList->Disconnect( wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( MediaPlayerDialogBase::OnExpanded ), NULL, this );
	mediaTreeList->Disconnect( wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler( MediaPlayerDialogBase::OnExpanding ), NULL, this );
	mediaTreeList->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( MediaPlayerDialogBase::OnItemContextMenu ), NULL, this );
	mediaTreeList->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( MediaPlayerDialogBase::OnSelected ), NULL, this );
	radioBoxSource->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( MediaPlayerDialogBase::OnSourceChanged ), NULL, this );
	sliderZone0Volume->Disconnect( wxEVT_SLIDER, wxCommandEventHandler( MediaPlayerDialogBase::OnZone0VolumeChanged ), NULL, this );
	sliderZone1Volume->Disconnect( wxEVT_SLIDER, wxCommandEventHandler( MediaPlayerDialogBase::OnZone1VolumeChanged ), NULL, this );
	buttonReverse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MediaPlayerDialogBase::OnButtonReverse ), NULL, this );
	buttonPause->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MediaPlayerDialogBase::OnButtonPause ), NULL, this );
	buttonForward->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MediaPlayerDialogBase::OnButtonForward ), NULL, this );
	buttonMute->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MediaPlayerDialogBase::OnButtonMute ), NULL, this );
	buttonPower->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MediaPlayerDialogBase::OnButtonPower ), NULL, this );

}
