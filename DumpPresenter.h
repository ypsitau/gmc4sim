// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __DUMPPRESENTER_H__
#define __DUMPPRESENTER_H__

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/html/htmlwin.h>
#include "VirtualMachine.h"
#include "Generator.h"
#include "Config.h"

class DumpPresenter : public wxDialog, public VirtualMachine::Observer {
private:
	enum {
		ID_RB_COL4 = 1000,
		ID_RB_COL8,
		ID_RB_COL16,
		ID_CHK_AddrBin,
		ID_Notebook,
	};
	enum {
		PAGE_View,
		PAGE_HTML,
		PAGE_DMPFormat,
	};
	class OutputString : public Generator::Output {
	private:
		wxString &_string;
	public:
		inline OutputString(wxString &string) : _string(string) {}
		virtual void Printf(const char *format, ...);
	};
private:
	const VirtualMachine &_vm;
	wxHtmlWindow *_pHtmlWindow;
	wxTextCtrl *_pTextHtml;
	wxTextCtrl *_pTextDmp;
public:
	DumpPresenter(wxWindow *pParent,
				const wxPoint &pos, const wxSize &size, const VirtualMachine &vm);
	~DumpPresenter();
	void UpdateContents();
	// virtual functions of VirtualMachine::Observer
	virtual void NotifyModeChange(const VirtualMachine &vm);
	virtual void NotifyAddressChange(const VirtualMachine &vm);
	virtual void NotifyOpExecuted(const VirtualMachine &vm);
	virtual void NotifyProgramChange(const VirtualMachine &vm);
	DECLARE_EVENT_TABLE()
	void OnRadioBtn_Column(wxCommandEvent &event);
	void OnCheck_AddrBin(wxCommandEvent &event);
	void OnNotebookPageChanged(wxNotebookEvent &event);
};

#endif
