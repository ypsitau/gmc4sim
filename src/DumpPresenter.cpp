// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include "stdafx.h"
#include "DumpPresenter.h"

//-----------------------------------------------------------------------------
// DumpPresenter
//-----------------------------------------------------------------------------
DumpPresenter::DumpPresenter(wxWindow *pParent,
				const wxPoint &pos, const wxSize &size, const VirtualMachine &vm) :
		wxDialog(pParent, wxID_ANY, wxT("GMC-4 Dump"), pos, size,
					wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER), _vm(vm)
{
	wxWindow *pWindowFirst = NULL;
	SetIcon(wxICON(MainIcon));
	wxSizer *pOuterBox = new wxBoxSizer(wxVERTICAL);
	SetSizer(pOuterBox);
	wxSizer *pVBox = new wxBoxSizer(wxVERTICAL);
	pOuterBox->Add(pVBox, wxSizerFlags(1).Expand());
	do {
		wxSizer *pHBox = new wxBoxSizer(wxHORIZONTAL);
		pVBox->Add(pHBox, wxSizerFlags(0).Expand().Border(wxALL, 4));
		do {
			wxCheckBox *pCtrl = new wxCheckBox(this, ID_CHK_AddrBin, wxT("&Binary Address"));
			pHBox->Add(pCtrl, wxSizerFlags(0).Expand());
			pCtrl->SetValue(Config::Get()->dumpPresenterAddrBin);
			pWindowFirst = pCtrl;
		} while (0);
		do {
			wxStaticText *pCtrl = new wxStaticText(this, wxID_ANY, wxT("Columns"));
			pHBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxLEFT, 12));
		} while (0);
		do {
			wxRadioButton *pCtrl = new wxRadioButton(this, ID_RB_COL4, wxT("&4"),
								wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
			pHBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxLEFT, 8));
		} while (0);
		do {
			wxRadioButton *pCtrl = new wxRadioButton(this, ID_RB_COL8, wxT("&8"));
			pHBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxLEFT, 8));
		} while (0);
		do {
			wxRadioButton *pCtrl = new wxRadioButton(this, ID_RB_COL16, wxT("1&6"));
			pHBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxLEFT, 8));
		} while (0);
	} while (0);
	do {
		wxNotebook *pNotebook = new wxNotebook(this, ID_Notebook);
		pVBox->Add(pNotebook, wxSizerFlags(1).Expand());
		pNotebook->SetPadding(wxSize(16, 4));
		do {
			wxHtmlWindow *pCtrl = new wxHtmlWindow(pNotebook, wxID_ANY,
					wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
			_pHtmlWindow = pCtrl;
			pNotebook->AddPage(pCtrl, wxT("View"));
			pCtrl->SetBorders(2);
		} while (0);
		do {
			wxTextCtrl *pCtrl = new wxTextCtrl(pNotebook, wxID_ANY, wxT(""),
					wxDefaultPosition, wxDefaultSize,
					wxSUNKEN_BORDER | wxTE_MULTILINE | wxTE_READONLY);
			_pTextHtml = pCtrl;
			pNotebook->AddPage(pCtrl, wxT("HTML"));
		} while (0);
		do {
			wxTextCtrl *pCtrl = new wxTextCtrl(pNotebook, wxID_ANY, wxT(""),
					wxDefaultPosition, wxDefaultSize,
					wxSUNKEN_BORDER | wxTE_MULTILINE | wxTE_READONLY);
			_pTextDmp = pCtrl;
			pNotebook->AddPage(pCtrl, wxT("DMP Format"));
			pCtrl->SetFont(wxFont(10, wxFONTFAMILY_TELETYPE,
									wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
		} while (0);
	} while (0);
	do {
		wxSizer *pHBox = new wxBoxSizer(wxHORIZONTAL);
		pVBox->Add(pHBox, wxSizerFlags(0).Expand().Border(wxTOP, 2));
		pHBox->AddStretchSpacer(1);
		do {
			wxButton *pCtrl = new wxButton(this, wxID_CANCEL, wxT("&Close"));
			pHBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxLEFT, 16));
			pCtrl->MoveBeforeInTabOrder(pWindowFirst);
		} while (0);
		pHBox->AddSpacer(8);
	} while (0);
	if (Config::Get()->dumpPresenterColumns == 4) {
		wxDynamicCast(FindWindow(ID_RB_COL4), wxRadioButton)->SetValue(true);
	} else if (Config::Get()->dumpPresenterColumns == 8) {
		wxDynamicCast(FindWindow(ID_RB_COL8), wxRadioButton)->SetValue(true);
	} else if (Config::Get()->dumpPresenterColumns == 16) {
		wxDynamicCast(FindWindow(ID_RB_COL16), wxRadioButton)->SetValue(true);
	} else {
		// nothing to do
	}
}

DumpPresenter::~DumpPresenter()
{
}

void DumpPresenter::UpdateContents()
{
	do {
		wxString text;
		OutputString output(text);
		Generator(_vm, output).GenerateHtmlDump(
					Config::Get()->dumpPresenterAddrBin,
					Config::Get()->dumpPresenterColumns, false);
		do {
			wxString textFramed;
			textFramed += wxT("<html><body>\n");
			textFramed += text;
			textFramed += wxT("</body></html>\n");
			_pHtmlWindow->SetPage(textFramed);
		} while (0);
		do {
			_pTextHtml->SetValue(text);
		} while (0);
	} while (0);
	do {
		wxString text;
		OutputString output(text);
		Generator(_vm, output).GenerateHex(true, Config::Get()->dumpPresenterColumns);
		do {
			_pTextDmp->SetValue(text);
		} while (0);
	} while (0);
}

void DumpPresenter::NotifyModeChange(const VirtualMachine &vm)
{
	// nothing to do
}

void DumpPresenter::NotifyAddressChange(const VirtualMachine &vm)
{
	// nothing to do
}

void DumpPresenter::NotifyOpExecuted(const VirtualMachine &vm)
{
	// nothing to do
}

void DumpPresenter::NotifyProgramChange(const VirtualMachine &vm)
{
	UpdateContents();
}

BEGIN_EVENT_TABLE(DumpPresenter, wxDialog)
	EVT_RADIOBUTTON(ID_RB_COL4,		DumpPresenter::OnRadioBtn_Column)
	EVT_RADIOBUTTON(ID_RB_COL8,		DumpPresenter::OnRadioBtn_Column)
	EVT_RADIOBUTTON(ID_RB_COL16,	DumpPresenter::OnRadioBtn_Column)
	EVT_CHECKBOX(ID_CHK_AddrBin,	DumpPresenter::OnCheck_AddrBin)
	EVT_NOTEBOOK_PAGE_CHANGED(ID_Notebook, DumpPresenter::OnNotebookPageChanged)
END_EVENT_TABLE();

void DumpPresenter::OnRadioBtn_Column(wxCommandEvent &event)
{
	Config::Get()->dumpPresenterColumns =
		wxDynamicCast(FindWindow(ID_RB_COL4), wxRadioButton)->GetValue()? 4 :
		wxDynamicCast(FindWindow(ID_RB_COL8), wxRadioButton)->GetValue()? 8 :
		wxDynamicCast(FindWindow(ID_RB_COL16), wxRadioButton)->GetValue()? 16 :
		4;
	Config::Get()->WriteBurst();
	UpdateContents();
}

void DumpPresenter::OnCheck_AddrBin(wxCommandEvent &event)
{
	Config::Get()->dumpPresenterAddrBin =
		wxDynamicCast(FindWindow(ID_CHK_AddrBin), wxCheckBox)->GetValue();
	Config::Get()->WriteBurst();
	UpdateContents();
}

void DumpPresenter::OnNotebookPageChanged(wxNotebookEvent &event)
{
	int iPage = event.GetSelection();
	if (iPage == PAGE_View) {
		FindWindow(ID_CHK_AddrBin)->Enable(true);
		_pHtmlWindow->SetFocus();
	} else if (iPage == PAGE_HTML) {
		FindWindow(ID_CHK_AddrBin)->Enable(true);
		_pTextHtml->SetFocus();
		_pTextHtml->SetSelection(-1, -1);
	} else if (iPage == PAGE_DMPFormat) {
		FindWindow(ID_CHK_AddrBin)->Enable(false);
		_pTextDmp->SetFocus();
		_pTextDmp->SetSelection(-1, -1);
	} else {
		// nothing to do
	}
}

//-----------------------------------------------------------------------------
// DumpPresenter::OutputString
//-----------------------------------------------------------------------------
void DumpPresenter::OutputString::Printf(const char *format, ...)
{
	va_list list;
	va_start(list, format);
	_string.Append(wxString::FormatV(format, list));
	va_end(list);
}
