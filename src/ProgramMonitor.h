// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __PROGRAMMONITOR_H__
#define __PROGRAMMONITOR_H__

#include "VirtualMachine.h"

//-----------------------------------------------------------------------------
// ProgramMonitor
//-----------------------------------------------------------------------------
class ProgramMonitor : public wxListCtrl, public VirtualMachine::Observer {
private:
	enum {
		COL_Address	= 1,
		COL_Symbol	= 2,
		COL_Code	= 3,
	};
protected:
	struct ColInfo {
		const wxChar *label;
		int width;
		wxListColumnFormat align;
	};
	typedef std::map<unsigned char, int> AddrToRowMap;
	typedef std::map<int, unsigned char> RowToAddrMap;
protected:
	static const ColInfo _colInfoTbl[];
	int _iRowHighlight;
	bool _upperCaseFlag;
	AddrToRowMap _addrToRowMap;
	RowToAddrMap _rowToAddrMap;
	wxColour COLOUR_Highlight;
public:
	ProgramMonitor(wxWindow *pParent, wxWindowID id, long style);
	void UpdateContent(const VirtualMachine &vm);
	unsigned char GetAddressByRow(int iRow);
	void HighlightRow(int iRow);
	void HighlightRowByAddress(unsigned char addr);
private:
	// virtual functions of VirtualMachine::Observer
	virtual void NotifyModeChange(const VirtualMachine &vm);
	virtual void NotifyAddressChange(const VirtualMachine &vm);
	virtual void NotifyOpExecuted(const VirtualMachine &vm);
	virtual void NotifyProgramChange(const VirtualMachine &vm);
	DECLARE_EVENT_TABLE()
	void OnSize(wxSizeEvent &event);
	void OnKeyDown(wxKeyEvent &event);
};

#endif
