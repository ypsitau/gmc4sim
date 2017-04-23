// GMC-4 Simulator
// Copyright (C) 2009-2017 ypsitau
// https://github.com/ypsitau/gmc4sim

#include "stdafx.h"
#include "ConfigDialog.h"

//-----------------------------------------------------------------------------
// ConfigDialog
//-----------------------------------------------------------------------------
ConfigDialog::ConfigDialog(wxWindow *pParent, PageId pageId) :
		wxDialog(pParent, wxID_ANY, wxT("Configuration"), wxDefaultPosition,
							wxSize(400, 250)), _rebootFlag(false)
{
	SetIcon(wxICON(MainIcon));
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
	wxSizer *pOuterBox = new wxBoxSizer(wxVERTICAL);
	SetSizer(pOuterBox);
	wxSizer *pVBox = new wxBoxSizer(wxVERTICAL);
	pOuterBox->Add(pVBox, wxSizerFlags(1).Expand().Border(wxALL, 4));
	wxNotebook *pNotebook = new wxNotebook(this, wxID_ANY);
	pVBox->Add(pNotebook, wxSizerFlags(1).Expand());
	pNotebook->SetPadding(wxSize(16, 4));
	pNotebook->AddPage(new Page_General(pNotebook, _rebootFlag), wxT("General"));
	pNotebook->SetSelection((pageId == PAGEID_General)? 0 : 0);
	do {
		wxSizer *pHBox = new wxBoxSizer(wxHORIZONTAL);
		pVBox->Add(pHBox, wxSizerFlags(0).Expand().Border(wxTOP, 4));
		pHBox->AddStretchSpacer(1);
		wxWindow *pCtrl;
		pCtrl = new wxButton(this, wxID_OK, wxT("OK"),
							wxDefaultPosition, wxSize(80, 20));
		pHBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxLEFT, 0));
		pCtrl->SetFocus();
		pCtrl = new wxButton(this, wxID_CANCEL, wxT("Cancel"),
							wxDefaultPosition, wxSize(80, 20));
		pHBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxLEFT, 4));
	} while (0);
	CentreOnParent();
}

//-----------------------------------------------------------------------------
// ConfigDialog::Page_General
//-----------------------------------------------------------------------------
ConfigDialog::Page_General::Page_General(wxWindow *pParent, bool &rebootFlag) :
												Page(pParent, rebootFlag)
{
	wxSizer *pOuterBox = new wxBoxSizer(wxVERTICAL);
	SetSizer(pOuterBox);
	wxSizer *pVBoxOut = new wxBoxSizer(wxVERTICAL);
	pOuterBox->Add(pVBoxOut, wxSizerFlags(1).Expand().Border(wxALL, 8));
	do {
		wxStaticBoxSizer *pVBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Generator"));
		pVBoxOut->Add(pVBox, wxSizerFlags(0).Expand());
		do {
			wxCheckBox *pCtrl = new wxCheckBox(this, wxID_ANY,
				wxT("&Upper case characters to generate assembler"));
			pVBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxTOP, 4));
			_pChk_UpperCaseAsm = pCtrl;
		} while (0);
		do {
			wxCheckBox *pCtrl = new wxCheckBox(this, wxID_ANY,
				wxT("Upper case characters to generate &HEX file"));
			pVBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxTOP, 8));
			_pChk_UpperCaseHex = pCtrl;
		} while (0);
		pVBox->AddSpacer(8);
	} while (0);
	do {
		wxStaticBoxSizer *pVBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Board Simulator"));
		pVBoxOut->Add(pVBox, wxSizerFlags(0).Expand().Border(wxTOP, 8));
		do {
			wxCheckBox *pCtrl = new wxCheckBox(this, wxID_ANY,
				wxT("&Show board simulator on dialog"));
			pVBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxTOP, 4));
			_pChk_BoardOnDialog = pCtrl;
		} while (0);
		do {
			wxCheckBox *pCtrl = new wxCheckBox(this, wxID_ANY,
				wxT("&Automatically hide board simulator dialog when resetting"));
			pVBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxTOP, 8));
			_pChk_AutoHideBoardDialog = pCtrl;
		} while (0);
		pVBox->AddSpacer(8);
	} while (0);
}

bool ConfigDialog::Page_General::TransferDataToWindow()
{
	_pChk_UpperCaseAsm->SetValue(Config::Get()->upperCaseAsm);
	_pChk_UpperCaseHex->SetValue(Config::Get()->upperCaseHex);
	_pChk_BoardOnDialog->SetValue(Config::Get()->boardOnDialog);
	_pChk_AutoHideBoardDialog->SetValue(Config::Get()->autoHideBoardDialog);
	return true;
}

bool ConfigDialog::Page_General::TransferDataFromWindow()
{
	if (Config::Get()->boardOnDialog != _pChk_BoardOnDialog->GetValue()) {
		_rebootFlag = true;
	}
	Config::Get()->upperCaseAsm = _pChk_UpperCaseAsm->GetValue();
	Config::Get()->upperCaseHex = _pChk_UpperCaseHex->GetValue();
	Config::Get()->boardOnDialog = _pChk_BoardOnDialog->GetValue();
	Config::Get()->autoHideBoardDialog = _pChk_AutoHideBoardDialog->GetValue();
	Config::Get()->WriteBurst();
	return true;
}

BEGIN_EVENT_TABLE(ConfigDialog::Page_General, wxPanel)
END_EVENT_TABLE()
