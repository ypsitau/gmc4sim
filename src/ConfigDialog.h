// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __CONFIGDIALOG_H__
#define __CONFIGDIALOG_H__

#include "Config.h"

class ConfigDialog : public wxDialog {
public:
	enum PageId {
		PAGEID_None,
		PAGEID_General,
	};
private:
	class Page : public wxPanel {
	protected:
		bool &_rebootFlag;
	public:
		inline Page(wxWindow *pParent, bool &rebootFlag) :
			wxPanel(pParent, wxID_ANY), _rebootFlag(rebootFlag) {}
	};
	class Page_General : public Page {
	private:
		wxCheckBox *_pChk_UpperCaseAsm;
		wxCheckBox *_pChk_UpperCaseHex;
		wxCheckBox *_pChk_BoardOnDialog;
		wxCheckBox *_pChk_AutoHideBoardDialog;
	public:
		Page_General(wxWindow *pParent, bool &rebootFlag);
		virtual bool TransferDataToWindow();
		virtual bool TransferDataFromWindow();
		DECLARE_EVENT_TABLE()
	};
private:
	bool _rebootFlag;
public:
	ConfigDialog(wxWindow *pParent, PageId pageId = PAGEID_None);
	inline bool GetRebootFlag() const { return _rebootFlag; }
};

#endif
