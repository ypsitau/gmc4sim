// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include "stdafx.h"
#include "ProgramMonitor.h"

//-----------------------------------------------------------------------------
// ProgramMonitor
//-----------------------------------------------------------------------------
const ProgramMonitor::ColInfo ProgramMonitor::_colInfoTbl[] = {
	{ wxT(""),			0,	wxLIST_FORMAT_LEFT,		},
	{ wxT("Addr"),		30,	wxLIST_FORMAT_CENTRE,	},
	{ wxT("Symbol"),	30,	wxLIST_FORMAT_CENTRE,	},
	{ wxT("Code"),		30,	wxLIST_FORMAT_CENTRE,	},
};

ProgramMonitor::ProgramMonitor(wxWindow *pParent, wxWindowID id, long style) :
		wxListCtrl(pParent, id, wxDefaultPosition, wxDefaultSize,
										style | wxLC_REPORT | wxLC_HRULES),
		_iRowHighlight(-1), _upperCaseFlag(true),
		COLOUR_Highlight(255, 255, 128)
{
	SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	for (int iCol = 0; iCol < NUMBEROF(_colInfoTbl); iCol++) {
		wxListItem item;
		item.SetText(_colInfoTbl[iCol].label);
		item.SetWidth(_colInfoTbl[iCol].width);
		item.SetAlign(_colInfoTbl[iCol].align);
		InsertColumn(iCol, item);
	}
}

void ProgramMonitor::UpdateContent(const VirtualMachine &vm)
{
	_addrToRowMap.clear();
	_rowToAddrMap.clear();
	DeleteAllItems();
	int iRow = 0;
	for (unsigned char addr = 0x00; vm.IsValidMem(addr) &&
								addr < VirtualMachine::ADDR_REGISTER; ) {
		const Operator *pOperator = vm.GetOperatorMap().Find(vm.GetMem(addr));
		if (pOperator == NULL) break;
		_addrToRowMap[addr] = iRow;
		_rowToAddrMap[iRow] = addr;
		InsertItem(iRow, wxT(""));
		SetItem(iRow, COL_Address, wxString::Format(wxT("%02X"), addr));
		SetItem(iRow, COL_Symbol, pOperator->GetSymbol(_upperCaseFlag));
		SetItem(iRow, COL_Code, wxString::Format(wxT("%X"), pOperator->GetCode()));
		iRow++;
		if (pOperator->GetType() == Operator::TYPE_NoOperand) {
			// nothing to do
		} else if (pOperator->GetType() == Operator::TYPE_NibbleOperand) {
			unsigned char value = pOperator->FetchOperand(vm, addr);
			_addrToRowMap[addr + 1] = iRow;
			_rowToAddrMap[iRow] = addr + 1;
			InsertItem(iRow, wxT(""));
			SetItem(iRow, COL_Address, wxString::Format(wxT("%02X"), addr + 1));
			SetItem(iRow, COL_Symbol, wxString::Format(wxT("<%X>"), value));
			SetItem(iRow, COL_Code, wxString::Format(wxT("%X"), value));
			iRow++;
		} else if (pOperator->GetType() == Operator::TYPE_JUMP) {
			unsigned char addrDest = pOperator->FetchOperand(vm, addr);
			_addrToRowMap[addr + 1] = iRow;
			_rowToAddrMap[iRow] = addr + 1;
			InsertItem(iRow, wxT(""));
			SetItem(iRow, COL_Address, wxString::Format(wxT("%02X"), addr + 1));
			SetItem(iRow, COL_Symbol, wxString::Format(wxT("<%X>"), addrDest >> 4));
			SetItem(iRow, COL_Code, wxString::Format(wxT("%X"), addrDest >> 4));
			iRow++;
			_addrToRowMap[addr + 2] = iRow;
			_rowToAddrMap[iRow] = addr + 2;
			InsertItem(iRow, wxT(""));
			SetItem(iRow, COL_Address, wxString::Format(wxT("%02X"), addr + 2));
			SetItem(iRow, COL_Symbol, wxString::Format(wxT("<%X>"), addrDest & 0xf));
			SetItem(iRow, COL_Code, wxString::Format(wxT("%X"), addrDest & 0xf));
			iRow++;
		} else if (pOperator->GetType() == Operator::TYPE_CAL) {
			unsigned char codeSub = vm.GetMem(addr + 1);
			const Operator *pOperatorSub = vm.GetOperatorSubMap().Find(codeSub);
			_addrToRowMap[addr + 1] = iRow;
			_rowToAddrMap[iRow] = addr + 1;
			InsertItem(iRow, wxT(""));
			SetItem(iRow, COL_Address, wxString::Format(wxT("%02X"), addr + 1));
			wxString symbol;
			if (pOperatorSub != NULL) {
				symbol = wxString::Format(wxT("_%s"),
									pOperatorSub->GetSymbol(_upperCaseFlag));
			}
			SetItem(iRow, COL_Symbol, symbol);
			SetItem(iRow, COL_Code, wxString::Format(wxT("%X"), codeSub));
			iRow++;
		} else { // Operator::TYPE_None
			// nothing to do
		}
		addr += pOperator->GetNibbles();
	}
	HighlightRowByAddress(vm.GetAddress());
}

unsigned char ProgramMonitor::GetAddressByRow(int iRow)
{
	RowToAddrMap::iterator iter = _rowToAddrMap.find(iRow);
	return (iter == _rowToAddrMap.end())? 0 : iter->second;
}

void ProgramMonitor::HighlightRow(int iRow)
{
	if (_iRowHighlight >= 0) {
		SetItemBackgroundColour(_iRowHighlight, GetBackgroundColour());
		//SetItemState(_iRowHighlight, wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED);
	}
	if (iRow >= 0) {
		SetItemBackgroundColour(iRow, COLOUR_Highlight);
		//SetItemState(iRow, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	}
	_iRowHighlight = iRow;
}

void ProgramMonitor::HighlightRowByAddress(unsigned char addr)
{
	AddrToRowMap::iterator iter = _addrToRowMap.find(addr);
	if (iter == _addrToRowMap.end()) {
		HighlightRow(-1);
	} else {
		int iRow = iter->second;
		HighlightRow(iRow);
		EnsureVisible(iRow);
	}
}

void ProgramMonitor::NotifyModeChange(const VirtualMachine &vm)
{
	if (vm.GetMode() == VirtualMachine::MODE_Run) {
		HighlightRow(-1);
		Refresh();
	} else {
		// nothing to do
	}
}

void ProgramMonitor::NotifyAddressChange(const VirtualMachine &vm)
{
	if (vm.GetMode() == VirtualMachine::MODE_Run) {
		// nothing to do
		//HighlightRowByAddress(vm.GetAddress());
	} else {
		HighlightRowByAddress(vm.GetAddress());
		Refresh();
	}
}

void ProgramMonitor::NotifyOpExecuted(const VirtualMachine &vm)
{
	// nothing to do
}

void ProgramMonitor::NotifyProgramChange(const VirtualMachine &vm)
{
	if (vm.GetMode() == VirtualMachine::MODE_Run) {
		// nothing to do
	} else {
		UpdateContent(vm);
	}
}

BEGIN_EVENT_TABLE(ProgramMonitor, wxListCtrl)
	EVT_SIZE(ProgramMonitor::OnSize)
	EVT_KEY_DOWN(ProgramMonitor::OnKeyDown)
END_EVENT_TABLE()

void ProgramMonitor::OnSize(wxSizeEvent &event)
{
	event.Skip();
	wxSize sz = GetClientSize();
	int widthAll = 0;
	for (int iCol = 0; iCol < NUMBEROF(_colInfoTbl); iCol++) {
		widthAll += _colInfoTbl[iCol].width;
	}
	for (int iCol = 0; iCol < NUMBEROF(_colInfoTbl); iCol++) {
		wxListItem item;
		GetColumn(iCol, item);
		item.SetWidth(_colInfoTbl[iCol].width * sz.GetWidth() / widthAll);
		SetColumn(iCol, item);
	}
}

void ProgramMonitor::OnKeyDown(wxKeyEvent &event)
{
	int keyCode = event.GetKeyCode();
	if (keyCode == WXK_RETURN) {
		
	} else if (keyCode == WXK_LEFT) {
		
	} else if (keyCode == WXK_RIGHT) {
		
	} else {
		event.Skip();
	}
}
