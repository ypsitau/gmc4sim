// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include "AssemblerEditor.h"

//-----------------------------------------------------------------------------
// AssemblerEditor
//-----------------------------------------------------------------------------
AssemblerEditor::AssemblerEditor(wxWindow *pParent,
							wxWindowID id, long style, VirtualMachine &vm) :
	wxTextCtrl(pParent, id, wxT(""), wxDefaultPosition, wxDefaultSize,
							style | wxWANTS_CHARS | wxTE_MULTILINE),
	_vm(vm), COLOUR_Normal(*wxWHITE), COLOUR_EditProhibited(*wxLIGHT_GREY)
{
	SetBackgroundColour(COLOUR_Normal);
	SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL,
			false, wxT("ÇlÇr ÉSÉVÉbÉN")));
}

void AssemblerEditor::SetAddressByLineTbl(const AddressByLineTbl &addrByLineTbl)
{
	_addrByLineTbl = addrByLineTbl;
}

unsigned char AssemblerEditor::LineToAddress(int iLine) const
{
	return (iLine < static_cast<long>(_addrByLineTbl.size()))?
											_addrByLineTbl[iLine] : 0x00;
}

unsigned char AssemblerEditor::GetPointedAddress() const
{
	long iCol, iLine;
	PositionToXY(GetInsertionPoint(), &iCol, &iLine);
	return LineToAddress(iLine);
}

void AssemblerEditor::DoAutoIndent()
{
	long iCol, iLine;
	PositionToXY(GetInsertionPoint(), &iCol, &iLine);
	wxString text = GetLineText(iLine);
	wxString textHead = wxT("\n");
	if (iCol == 0) {
		// nothing to do
	} else {
		for (size_t i = 0; i < text.Length(); i++) {
			wxChar ch = text[i];
			if (ch != ' ' && ch != '\t') break;
			textHead += ch;
		}
	}
	WriteText(textHead);
}

void AssemblerEditor::DoIndentDown()
{
	long posStart, posEnd;
	GetSelection(&posStart, &posEnd);
	long posEndMod = posEnd;
	wxString text = GetStringSelection();
	wxString textMod;
	enum {
		STAT_LineTop,
		STAT_SeekEOL,
	} stat = STAT_LineTop;
	for (size_t i = 0; i < text.Length(); i++) {
		wxChar ch = text[i];
		if (stat == STAT_LineTop) {
			if (ch == '\t') {
				posEndMod--;
				stat = STAT_SeekEOL;
			} else if (ch == '\n') {
				textMod += ch;
			} else {
				textMod += ch;
				stat = STAT_SeekEOL;
			}
		} else if (stat == STAT_SeekEOL) {
			if (ch == '\n') {
				textMod += ch;
				stat = STAT_LineTop;
			} else {
				textMod += ch;
			}
		}
	}
	Replace(posStart, posEnd, textMod);
	SetSelection(posStart, posEndMod);
}

void AssemblerEditor::DoIndentUp()
{
	long posStart, posEnd;
	GetSelection(&posStart, &posEnd);
	long posEndMod = posEnd;
	wxString text = GetStringSelection();
	wxString textMod;
	wxChar chPrev = wxT('\n');
	for (size_t i = 0; i < text.Length(); i++) {
		wxChar ch = text[i];
		if (chPrev == wxT('\n')) {
			textMod += wxT('\t');
			posEndMod++;
		}
		textMod += ch;
		chPrev = ch;
	}
	Replace(posStart, posEnd, textMod);
	SetSelection(posStart, posEndMod);
}

void AssemblerEditor::NotifyModeChange(const VirtualMachine &vm)
{
	if (vm.GetMode() == VirtualMachine::MODE_Program) {
		SetEditable(true);
		SetBackgroundColour(COLOUR_Normal);
		Update();
	} else {
		SetEditable(false);
		SetBackgroundColour(COLOUR_EditProhibited);
		Update();
	}
}

void AssemblerEditor::NotifyAddressChange(const VirtualMachine &vm)
{
	// nothing to do
}

void AssemblerEditor::NotifyOpExecuted(const VirtualMachine &vm)
{
	// nothing to do
}

void AssemblerEditor::NotifyProgramChange(const VirtualMachine &vm)
{
	// nothing to do
}

BEGIN_EVENT_TABLE(AssemblerEditor, wxTextCtrl)
	EVT_LEFT_UP(AssemblerEditor::OnLeftUp)
	EVT_KEY_DOWN(AssemblerEditor::OnKeyDown)
	EVT_KEY_UP(AssemblerEditor::OnKeyUp)
END_EVENT_TABLE()

void AssemblerEditor::OnLeftUp(wxMouseEvent &event)
{
	event.Skip();
}

void AssemblerEditor::OnKeyDown(wxKeyEvent &event)
{
	if (event.GetKeyCode() == WXK_RETURN) {
		DoAutoIndent();
	} else if (event.GetKeyCode() == WXK_TAB) {
		long pos1, pos2;
		GetSelection(&pos1, &pos2);
		if (pos1 == pos2) {
			if (pos1 == 0 && event.ShiftDown()) {
				event.Skip();
			} else {
				Replace(pos1, pos2, wxT("\t"));
			}
		} else if (event.ShiftDown()) {
			DoIndentDown();
		} else {
			DoIndentUp();
		}
	} else {
		event.Skip();
	}
}

void AssemblerEditor::OnKeyUp(wxKeyEvent &event)
{
	event.Skip();
	//_vm.SetAddress(GetPointedAddress());
}
