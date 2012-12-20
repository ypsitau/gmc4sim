// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __REGISTERWATCHER_H__
#define __REGISTERWATCHER_H__

#include "VirtualMachine.h"

//-----------------------------------------------------------------------------
// RegisterWatcher
//-----------------------------------------------------------------------------
class RegisterWatcher : public wxPanel, public VirtualMachine::Observer {
public:
	class TextCtrl : public wxTextCtrl {
	protected:
		RegisterWatcher *_pRegisterWatcher;
	public:
		TextCtrl(RegisterWatcher *pRegisterWatcher, long style);
		inline VirtualMachine &GetVM() { return _pRegisterWatcher->GetVM(); }
		virtual void NullToControl() = 0;
		virtual void ToControl() = 0;
		virtual void FromControl() = 0;
	private:
		DECLARE_EVENT_TABLE()
		void OnKillFocus(wxFocusEvent &event);
	};
	class TextCtrl_Nibble : public TextCtrl {
	private:
		unsigned char &_value;
	public:
		inline TextCtrl_Nibble(RegisterWatcher *pRegisterWatcher, unsigned char &value) :
				TextCtrl(pRegisterWatcher, 0), _value(value) {}
		virtual void NullToControl();
		virtual void ToControl();
		virtual void FromControl();
	};
	class TextCtrl_Address : public TextCtrl {
	public:
		inline TextCtrl_Address(RegisterWatcher *pRegisterWatcher) :
				TextCtrl(pRegisterWatcher, 0) {}
		virtual void NullToControl();
		virtual void ToControl();
		virtual void FromControl();
	};
	class TextCtrl_ExecFlag : public TextCtrl {
	public:
		inline TextCtrl_ExecFlag(RegisterWatcher *pRegisterWatcher) :
				TextCtrl(pRegisterWatcher, wxTE_READONLY) {}
		virtual void NullToControl();
		virtual void ToControl();
		virtual void FromControl();
	};
private:
	VirtualMachine &_vm;
	std::vector<TextCtrl *> _textCtrlTbl;
	TextCtrl *_pTextRegM[16];
	wxCheckBox *_pChk_UpdateDuringRunMode;
	wxColour COLOUR_TopLabelFg, COLOUR_TopLabelBg;
	wxColour COLOUR_Normal, COLOUR_Indexed;
public:
	RegisterWatcher(wxWindow *pParent, wxWindowID id, long style, VirtualMachine &vm);
	void InvalidateContent();
	void UpdateContent();
	inline VirtualMachine &GetVM() { return _vm; }
private:
	void UpdateNibble(wxTextCtrl *pTextCtrl, unsigned char value);
	void UpdateByte(wxTextCtrl *pTextCtrl, unsigned char value);
	void UpdateText(wxTextCtrl *pTextCtrl, const wxString &text);
	TextCtrl *CreateItem(wxBoxSizer *pHBox, const wxChar *label, TextCtrl *pTextCtrl);
private:
	// virtual functions of VirtualMachine::Observer
	virtual void NotifyModeChange(const VirtualMachine &vm);
	virtual void NotifyAddressChange(const VirtualMachine &vm);
	virtual void NotifyOpExecuted(const VirtualMachine &vm);
	virtual void NotifyProgramChange(const VirtualMachine &vm);
};

#endif
