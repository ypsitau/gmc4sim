// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include "stdafx.h"
#include "RegisterWatcher.h"

//-----------------------------------------------------------------------------
// RegisterWatcher
//-----------------------------------------------------------------------------
RegisterWatcher::RegisterWatcher(wxWindow *pParent,
							wxWindowID id, long style, VirtualMachine &vm) :
		wxPanel(pParent, id, wxDefaultPosition, wxSize(100, 100), style | wxTAB_TRAVERSAL),
		_vm(vm),
		COLOUR_TopLabelFg(64, 64, 192), COLOUR_TopLabelBg(192, 192, 255),
		COLOUR_Normal(*wxWHITE), COLOUR_Indexed(255, 255, 128)
{
	const int wdMgnRight = 4;
	wxBoxSizer *pOuterBox = new wxBoxSizer(wxVERTICAL);
	SetSizer(pOuterBox);
	wxBoxSizer *pVBox = new wxBoxSizer(wxVERTICAL);
	pOuterBox->Add(pVBox, wxSizerFlags(0).Expand());
	do {
		wxStaticText *pCtrl = new wxStaticText(this, wxID_ANY, wxT(" Register"),
					wxDefaultPosition, wxDefaultSize, 0);
		pVBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxBOTTOM, 2));
		pCtrl->SetForegroundColour(COLOUR_TopLabelFg);
		pCtrl->SetBackgroundColour(COLOUR_TopLabelBg);
	} while (0);
	do {
		wxBoxSizer *pHBox = new wxBoxSizer(wxHORIZONTAL);
		pVBox->Add(pHBox, wxSizerFlags(0).Expand().Border(wxRIGHT, wdMgnRight));
		_textCtrlTbl.push_back(CreateItem(pHBox, wxT("Ar"),
							new TextCtrl_Nibble(this, vm.RegA())));
		_textCtrlTbl.push_back(CreateItem(pHBox, wxT("A'r"),
							new TextCtrl_Nibble(this, vm.RegAx())));
	} while (0);
	do {
		wxBoxSizer *pHBox = new wxBoxSizer(wxHORIZONTAL);
		pVBox->Add(pHBox, wxSizerFlags(0).Expand().Border(wxRIGHT, wdMgnRight));
		_textCtrlTbl.push_back(CreateItem(pHBox, wxT("Br"),
							new TextCtrl_Nibble(this, vm.RegB())));
		_textCtrlTbl.push_back(CreateItem(pHBox, wxT("B'r"),
							new TextCtrl_Nibble(this, vm.RegBx())));
	} while (0);
	do {
		wxBoxSizer *pHBox = new wxBoxSizer(wxHORIZONTAL);
		pVBox->Add(pHBox, wxSizerFlags(0).Expand().Border(wxRIGHT, wdMgnRight));
		_textCtrlTbl.push_back(CreateItem(pHBox, wxT("Yr"),
							new TextCtrl_Nibble(this, vm.RegY())));
		_textCtrlTbl.push_back(CreateItem(pHBox, wxT("Y'r"),
							new TextCtrl_Nibble(this, vm.RegYx())));
	} while (0);
	do {
		wxBoxSizer *pHBox = new wxBoxSizer(wxHORIZONTAL);
		pVBox->Add(pHBox, wxSizerFlags(0).Expand().Border(wxRIGHT, wdMgnRight));
		_textCtrlTbl.push_back(CreateItem(pHBox, wxT("Zr"),
							new TextCtrl_Nibble(this, vm.RegZ())));
		_textCtrlTbl.push_back(CreateItem(pHBox, wxT("Z'r"),
							new TextCtrl_Nibble(this, vm.RegZx())));
	} while (0);
	pVBox->AddSpacer(2);
	do {
		wxBoxSizer *pHBox = new wxBoxSizer(wxHORIZONTAL);
		pVBox->Add(pHBox, wxSizerFlags(0).Expand().Border(wxRIGHT, wdMgnRight));
		_textCtrlTbl.push_back(CreateItem(pHBox, wxT("PC"),
							new TextCtrl_Address(this)));
		_textCtrlTbl.push_back(CreateItem(pHBox, wxT("Exec"),
							new TextCtrl_ExecFlag(this)));
	} while (0);
	pVBox->AddSpacer(2);
	do {
		wxStaticText *pCtrl = new wxStaticText(this, wxID_ANY, wxT(" Memory"),
					wxDefaultPosition, wxDefaultSize, 0);
		pVBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxBOTTOM, 2));
		pCtrl->SetForegroundColour(COLOUR_TopLabelFg);
		pCtrl->SetBackgroundColour(COLOUR_TopLabelBg);
	} while (0);
	for (int iRow = 0; iRow < 8; iRow++) {
		wxBoxSizer *pHBox = new wxBoxSizer(wxHORIZONTAL);
		pVBox->Add(pHBox, wxSizerFlags(0).Expand().Border(wxRIGHT, wdMgnRight));
		for (int iCol = 0; iCol < 2; iCol++) {
			int idxMem = iRow + iCol * 8;
			_textCtrlTbl.push_back(CreateItem(pHBox, wxString::Format(wxT("M [%X]"), idxMem),
						new TextCtrl_Nibble(this, vm.RegM(idxMem))));
			_pTextRegM[idxMem] = _textCtrlTbl.back();
		}
	}
	for (int idxMem = 0; idxMem < 16 - 1; idxMem++) {
		_pTextRegM[idxMem + 1]->MoveAfterInTabOrder(_pTextRegM[idxMem]);
	}
	do {
		wxStaticLine *pCtrl = new wxStaticLine(this, wxID_ANY,
							wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
		pVBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxTOP, 2));
	} while (0);
	do {
		wxCheckBox *pCtrl = new wxCheckBox(this, wxID_ANY, wxT("Update during Run mode"));
		pVBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxALL, 2));
		_pChk_UpdateDuringRunMode = pCtrl;
	} while (0);
}

void RegisterWatcher::InvalidateContent()
{
	foreach (std::vector<TextCtrl *>, ppTextCtrl, _textCtrlTbl) {
		(*ppTextCtrl)->NullToControl();
	}
	for (int idxMem = 0; idxMem < 16; idxMem++) {
		_pTextRegM[idxMem]->SetBackgroundColour(COLOUR_Normal);
		_pTextRegM[idxMem]->Refresh();
	}
}

void RegisterWatcher::UpdateContent()
{
	foreach (std::vector<TextCtrl *>, ppTextCtrl, _textCtrlTbl) {
		(*ppTextCtrl)->ToControl();
	}
	for (int idxMem = 0; idxMem < 16; idxMem++) {
		_pTextRegM[idxMem]->SetBackgroundColour(
			*((_vm.RegY() == idxMem)? &COLOUR_Indexed : &COLOUR_Normal));
		_pTextRegM[idxMem]->Refresh();
	}
}

RegisterWatcher::TextCtrl *RegisterWatcher::CreateItem(wxBoxSizer *pHBox,
								const wxChar *label, TextCtrl *pTextCtrl)
{
	const int wdLabelMgn = 8;
	const long styleLabel = wxALIGN_LEFT;
	do {
		wxStaticText *pCtrl = new wxStaticText(this, wxID_ANY, label,
					wxDefaultPosition, wxSize(40, -1), styleLabel);
		pHBox->Add(pCtrl, wxSizerFlags(0).Border(wxLEFT, wdLabelMgn).Center());
	} while (0);
	do {
		pHBox->Add(pTextCtrl, wxSizerFlags(0).Expand().Border(wxTOP, 1));
	} while (0);
	return pTextCtrl;
}

void RegisterWatcher::NotifyModeChange(const VirtualMachine &vm)
{
	UpdateContent();
}

void RegisterWatcher::NotifyAddressChange(const VirtualMachine &vm)
{
	if (vm.GetMode() != VirtualMachine::MODE_Run ||
								_pChk_UpdateDuringRunMode->GetValue()) {
		UpdateContent();
	}
}

void RegisterWatcher::NotifyOpExecuted(const VirtualMachine &vm)
{
	if (vm.GetMode() != VirtualMachine::MODE_Run ||
								_pChk_UpdateDuringRunMode->GetValue()) {
		UpdateContent();
	}
}

void RegisterWatcher::NotifyProgramChange(const VirtualMachine &vm)
{
	// Content of memory may possibly have been modified
	UpdateContent();
}

//-----------------------------------------------------------------------------
// RegisterWatcher::TextCtrl
//-----------------------------------------------------------------------------
RegisterWatcher::TextCtrl::TextCtrl(RegisterWatcher *pRegisterWatcher, long style) :
		wxTextCtrl(pRegisterWatcher, wxID_ANY, wxT(""), wxDefaultPosition,
						wxSize(40, 16), wxTE_CENTRE | wxNO_BORDER | style),
		_pRegisterWatcher(pRegisterWatcher)
{
}

BEGIN_EVENT_TABLE(RegisterWatcher::TextCtrl, wxTextCtrl)
	EVT_KILL_FOCUS(RegisterWatcher::TextCtrl::OnKillFocus)
END_EVENT_TABLE()

void RegisterWatcher::TextCtrl::OnKillFocus(wxFocusEvent &event)
{
	FromControl();
}

//-----------------------------------------------------------------------------
// RegisterWatcher::TextCtrl_Nibble
//-----------------------------------------------------------------------------
void RegisterWatcher::TextCtrl_Nibble::NullToControl()
{
	SetValue(wxT("-"));
}

void RegisterWatcher::TextCtrl_Nibble::ToControl()
{
	SetValue(wxString::Format(wxT("%X"), _value & 0xf));
}

void RegisterWatcher::TextCtrl_Nibble::FromControl()
{
	unsigned long valueRaw;
	if (GetValue().ToULong(&valueRaw, 16)) {
		_value = static_cast<unsigned char>(valueRaw) & 0xf;
	}
	if (&_value == &GetVM().RegY()) {
		_pRegisterWatcher->UpdateContent();
	} else {
		ToControl();
	}
}

//-----------------------------------------------------------------------------
// RegisterWatcher::TextCtrl_Address
//-----------------------------------------------------------------------------
void RegisterWatcher::TextCtrl_Address::NullToControl()
{
	SetValue(wxT("--"));
}

void RegisterWatcher::TextCtrl_Address::ToControl()
{
	SetValue(wxString::Format(wxT("%02X"), GetVM().GetAddress()));
}

void RegisterWatcher::TextCtrl_Address::FromControl()
{
	unsigned long valueRaw;
	if (GetValue().ToULong(&valueRaw, 16)) {
		GetVM().SetAddress(static_cast<unsigned char>(valueRaw));
	}
	ToControl();
}

//-----------------------------------------------------------------------------
// RegisterWatcher::TextCtrl_ExecFlag
//-----------------------------------------------------------------------------
void RegisterWatcher::TextCtrl_ExecFlag::NullToControl()
{
	SetValue(wxT("----"));
}

void RegisterWatcher::TextCtrl_ExecFlag::ToControl()
{
	SetValue(GetVM().GetExecFlag()? wxT("true") : wxT("false"));
}

void RegisterWatcher::TextCtrl_ExecFlag::FromControl()
{
}
