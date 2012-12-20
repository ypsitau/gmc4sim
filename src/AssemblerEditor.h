// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __ASSEMBLEREDITOR_H__
#define __ASSEMBLEREDITOR_H__

#include <wx/wx.h>
#include "VirtualMachine.h"
#include "Parser.h"

class AssemblerEditor : public wxTextCtrl, public VirtualMachine::Observer {
private:
	VirtualMachine &_vm;
	wxColour COLOUR_Normal;
	wxColour COLOUR_EditProhibited;
	AddressByLineTbl _addrByLineTbl;
public:
	AssemblerEditor(wxWindow *pParent, wxWindowID id, long style, VirtualMachine &vm);
	void SetAddressByLineTbl(const AddressByLineTbl &addrByLineTbl);
	unsigned char LineToAddress(int iLine) const;
	unsigned char GetPointedAddress() const;
private:
	void DoAutoIndent();
	void DoIndentDown();
	void DoIndentUp();
private:
	// virtual functions of VirtualMachine::Observer
	virtual void NotifyModeChange(const VirtualMachine &vm);
	virtual void NotifyAddressChange(const VirtualMachine &vm);
	virtual void NotifyOpExecuted(const VirtualMachine &vm);
	virtual void NotifyProgramChange(const VirtualMachine &vm);
	DECLARE_EVENT_TABLE()
	void OnLeftUp(wxMouseEvent &event);
	void OnKeyDown(wxKeyEvent &event);
	void OnKeyUp(wxKeyEvent &event);
};

#endif
