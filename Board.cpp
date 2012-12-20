// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include <wx/grid.h>
#include <wx/dcbuffer.h>
#include "Board.h"

//-----------------------------------------------------------------------------
// Board
//-----------------------------------------------------------------------------
Board::Board(wxWindow *pParent, wxWindowID id, long style, VirtualMachineGUI &vm) :
		wxPanel(pParent, id, wxDefaultPosition, wxDefaultSize, style | wxWANTS_CHARS),
		_vm(vm),
		_pElementPressed(NULL),
		FONT_Key(18, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD),
		FONT_KeySmall(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD),
		FONT_ModeIndicator(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD),
		PEN_ModeIndicator(wxColour(96, 192, 96)),
		COLOUR_ModeIndicatorOn(96, 214, 96),
		COLOUR_ModeIndicatorOff(196, 196, 196),
		COLOUR_InfoText(255, 255, 255),
		BRUSH_BoardFocusOn(wxColour(40, 128, 80)),
		BRUSH_BoardFocusOff(wxColour(50, 144, 96)),
		BRUSH_BinLED_On(wxColour(255, 64, 64)),
		BRUSH_BinLED_Off(wxColour(230, 230, 230)),
		BRUSH_BinLEDBackground(wxColour(255, 255, 255)),
		PEN_BinLED_On(wxColour(255, 128, 128)),
		PEN_BinLED_Off(wxColour(255, 255, 255)),
		PEN_BinLEDBorder(wxColour(196, 220, 196)),
		BRUSH_NumLED_On(wxColour(255, 64, 64)),
		BRUSH_NumLED_Off(wxColour(64, 64, 64)),
		BRUSH_NumLEDBackground(wxColour(32, 32, 32)),
		PEN_NumLED_On(wxColour(128, 32, 32)),
		PEN_NumLED_Off(wxColour(32, 32, 32)),
		PEN_NumLEDBorder(wxColour(96, 128, 96), 2),
		BRUSH_KeyFocusOn(wxColour(16, 16, 16)),
		BRUSH_KeyFocusOff(wxColour(92, 92, 92)),
		COLOUR_KeyText(214, 214, 214),
		COLOUR_KeyBlightSide(128, 128, 128)
{
	ClearKeyBuff();
	SetInitialBestSize(wxSize(480, 240));
	const int CX_Key = 50, CY_Key = 40;
	do {
		const int CX_NumLED = 55, CY_NumLED = 80;
		int x = 120, y = 50;
		NumLED *pNumLED = new NumLED(this, wxRect(x, y, CX_NumLED, CY_NumLED),
				wxString::Format(wxT("7-seg LED")));
		_elementTbl.push_back(pNumLED);
		_pNumLED = pNumLED;
	} while (0);
	do {
		const int CX_BinLED = 20, CY_BinLED= 16;
		for (int i = 0; i < CNT_BinLED; i++) {
			int x = (CNT_BinLED - 1 - i) * 70 + 10, y = 10;
			BinLED *pBinLED = new BinLED(this, wxRect(x, y, CX_BinLED, CY_BinLED),
				wxString::Format(wxT("LED%d"), i));
			_elementTbl.push_back(pBinLED);
			_pBinLEDTbl[i] = pBinLED;
		}
	} while (0);
	do {
		const int CX_ModeIndicator = 80, CY_ModeIndicator = 20;
		wxRect rc(10, 150, CX_ModeIndicator, CY_ModeIndicator);
		_pModeIndicatorProgram = new ModeIndicator(this, rc, wxT("Program"));
		_elementTbl.push_back(_pModeIndicatorProgram);
		rc.y += 20;
		_pModeIndicatorRun = new ModeIndicator(this, rc, wxT("Run"));
		_elementTbl.push_back(_pModeIndicatorRun);
		rc.y += 20;
		_pModeIndicatorStep = new ModeIndicator(this, rc, wxT("Step"));
		_elementTbl.push_back(_pModeIndicatorStep);
	} while (0);
	do {
		static const struct {
			const wxChar *text;
			int keyCode;
			unsigned char value;
			int type;
		} tbl[] = {
			{ wxT("C"),		'C',		0xc,	Key_Hex::TYPE	},
			{ wxT("D"),		'D',		0xd,	Key_Hex::TYPE	},
			{ wxT("E"),		'E',		0xe,	Key_Hex::TYPE	},
			{ wxT("F"),		'F',		0xf,	Key_Hex::TYPE	},
			{ wxT("A SET"),	WXK_BACK,	0x0,	Key_ASET::TYPE	},
			{ wxT("8"),		'8',		0x8,	Key_Hex::TYPE	},
			{ wxT("9"),		'9',		0x9,	Key_Hex::TYPE	},
			{ wxT("A"),		'A',		0xa,	Key_Hex::TYPE	},
			{ wxT("B"),		'B',		0xb,	Key_Hex::TYPE	},
			{ wxT("INCR"),	WXK_RETURN,	0x0,	Key_INCR::TYPE	},
			{ wxT("4"),		'4',		0x4,	Key_Hex::TYPE	},
			{ wxT("5"),		'5',		0x5,	Key_Hex::TYPE	},
			{ wxT("6"),		'6',		0x6,	Key_Hex::TYPE	},
			{ wxT("7"),		'7',		0x7,	Key_Hex::TYPE	},
			{ wxT("RUN"),	0,			0x0,	Key_RUN::TYPE	},
			{ wxT("0"),		'0',		0x0,	Key_Hex::TYPE	},
			{ wxT("1"),		'1',		0x1,	Key_Hex::TYPE	},
			{ wxT("2"),		'2',		0x2,	Key_Hex::TYPE	},
			{ wxT("3"),		'3',		0x3,	Key_Hex::TYPE	},
			{ wxT("RESET"),	WXK_HOME,	0x0,	Key_RESET::TYPE	},
		};
		for (int i = 0; i < NUMBEROF(tbl); i++) {
			int x = (i % 5) * (CX_Key + 4) + 200;
			int y = (i / 5) * (CY_Key + 4) + 50;
			wxFont *pFont = (::wxStrlen(tbl[i].text) == 1)?
											&FONT_Key : &FONT_KeySmall;
			Key *pKey;
			if (tbl[i].type == Key_Hex::TYPE) {
				pKey = new Key_Hex(this, wxRect(x, y, CX_Key, CY_Key),
							tbl[i].text, tbl[i].keyCode, *pFont, tbl[i].value);
			} else if (tbl[i].type == Key_ASET::TYPE) {
				pKey = new Key_ASET(this, wxRect(x, y, CX_Key, CY_Key),
							tbl[i].text, tbl[i].keyCode, *pFont, tbl[i].value);
			} else if (tbl[i].type == Key_INCR::TYPE) {
				pKey = new Key_INCR(this, wxRect(x, y, CX_Key, CY_Key),
							tbl[i].text, tbl[i].keyCode, *pFont, tbl[i].value);
			} else if (tbl[i].type == Key_RUN::TYPE) {
				pKey = new Key_RUN(this, wxRect(x, y, CX_Key, CY_Key),
							tbl[i].text, tbl[i].keyCode, *pFont, tbl[i].value);
			} else if (tbl[i].type == Key_RESET::TYPE) {
				pKey = new Key_RESET(this, wxRect(x, y, CX_Key, CY_Key),
							tbl[i].text, tbl[i].keyCode, *pFont, tbl[i].value);
			}
			_elementTbl.push_back(pKey);
		}
	} while (0);
	SetModeIndicator(vm.GetMode());
}

Board::~Board()
{
	foreach (ElementTbl, ppElement, _elementTbl) {
		delete (*ppElement);
	}
}

void Board::SetBinLED(unsigned char value)
{
	for (int iBit = 0; iBit < CNT_BinLED; iBit++) {
		_pBinLEDTbl[iBit]->SetValue((value & (1 << iBit))? true : false);
	}
}

void Board::SetNumLED(unsigned char value)
{
	_pNumLED->SetValue(value);
}

void Board::SetModeIndicator(VirtualMachine::Mode mode)
{
	_pModeIndicatorProgram->SetHighlight(mode == VirtualMachine::MODE_Program);
	_pModeIndicatorRun->SetHighlight(mode == VirtualMachine::MODE_Run);
	_pModeIndicatorStep->SetHighlight(mode == VirtualMachine::MODE_Step);
}

Board::Element *Board::GetElementByKeyCode(int keyCode)
{
	foreach (ElementTbl, ppElement, _elementTbl) {
		Element *pElement = *ppElement;
		if (pElement->GetKeyCode() == keyCode) return pElement;
	}
	return NULL;
}

Board::Element *Board::GetElementByPoint(const wxPoint &pt)
{
	foreach (ElementTbl, ppElement, _elementTbl) {
		Element *pElement = *ppElement;
		if (pElement->GetRect().Contains(pt)) return pElement;
	}
	return NULL;
}

void Board::NotifyModeChange(const VirtualMachine &vm)
{
	SetModeIndicator(vm.GetMode());
	Refresh();
}

void Board::NotifyAddressChange(const VirtualMachine &vm)
{
	if (vm.GetMode() == VirtualMachine::MODE_Program) {
		SetBinLED(vm.GetAddress());
		SetNumLED(vm.GetMem(vm.GetAddress()) & 0xf);
		Refresh();
	} else if (vm.GetMode() != VirtualMachine::MODE_Run) {
		// nothing to do
	} else if (vm.GetMode() == VirtualMachine::MODE_Step) {
		SetBinLED(vm.GetAddress());
		Refresh();
	}
}

void Board::NotifyOpExecuted(const VirtualMachine &vm)
{
	// nothing to do
}

void Board::NotifyProgramChange(const VirtualMachine &vm)
{
	SetBinLED(vm.GetAddress());
	SetNumLED(vm.GetMem(vm.GetAddress()) & 0xf);
}

BEGIN_EVENT_TABLE(Board, wxPanel)
	EVT_ERASE_BACKGROUND(Board::OnEraseBackground)
	EVT_PAINT(Board::OnPaint)
	EVT_SET_FOCUS(Board::OnSetFocus)
	EVT_KILL_FOCUS(Board::OnKillFocus)
	EVT_MOTION(Board::OnMotion)
	EVT_LEFT_DOWN(Board::OnLeftDown)
	EVT_LEFT_UP(Board::OnLeftUp)
	EVT_LEFT_DCLICK(Board::OnLeftDClick)
	EVT_RIGHT_DOWN(Board::OnRightDown)
	EVT_RIGHT_UP(Board::OnRightUp)
	EVT_LEAVE_WINDOW(Board::OnLeaveWindow)
	EVT_KEY_DOWN(Board::OnKeyDown)
	EVT_KEY_UP(Board::OnKeyUp)
	EVT_CHAR(Board::OnChar)
END_EVENT_TABLE()

void Board::OnEraseBackground(wxEraseEvent &event)
{
	// nothing to do
}

void Board::OnPaint(wxPaintEvent &event)
{
	wxBufferedPaintDC dc(this);
	if (wxWindow::FindFocus() == this) {
		dc.SetBackground(BRUSH_BoardFocusOn);
	} else {
		dc.SetBackground(BRUSH_BoardFocusOff);
	}
	dc.Clear();
	do {
		dc.SetPen(PEN_ModeIndicator);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.DrawRectangle(5, 150, 90, 60);
	} while (0);
	foreach (ElementTbl, ppElement, _elementTbl) {
		(*ppElement)->OnDraw(dc);
	}
}

void Board::OnSetFocus(wxFocusEvent &event)
{
	Refresh();
}

void Board::OnKillFocus(wxFocusEvent &event)
{
	Refresh();
}

void Board::OnMotion(wxMouseEvent &event)
{
	Element *pElement = GetElementByPoint(event.GetPosition());
	if (_pElementPressed != NULL && _pElementPressed != pElement) {
		_pElementPressed->OnRelease(false);
		_pElementPressed = NULL;
	}
}

void Board::OnLeftDown(wxMouseEvent &event)
{
	SetFocus();
	Element *pElement = GetElementByPoint(event.GetPosition());
	if (_pElementPressed != NULL && _pElementPressed != pElement) {
		_pElementPressed->OnRelease(false);
	}
	if (pElement != NULL) {
		pElement->OnPress();
	}
	Refresh();
	_pElementPressed = pElement;
}

void Board::OnLeftUp(wxMouseEvent &event)
{
	Element *pElement = GetElementByPoint(event.GetPosition());
	if (_pElementPressed != NULL) {
		pElement->OnRelease(_pElementPressed == pElement);
	}
	Refresh();
	_pElementPressed = NULL;
}

void Board::OnLeftDClick(wxMouseEvent &event)
{
}

void Board::OnRightDown(wxMouseEvent &event)
{
}

void Board::OnRightUp(wxMouseEvent &event)
{
}

void Board::OnLeaveWindow(wxMouseEvent &event)
{
	if (_pElementPressed != NULL) {
		_pElementPressed->OnRelease(false);
		_pElementPressed = NULL;
	}
}

void Board::OnKeyDown(wxKeyEvent &event)
{
	if (event.GetKeyCode() == WXK_ESCAPE) {
		event.Skip();
		return;
	} else if (event.GetKeyCode() == WXK_TAB) {
		wxNavigationKeyEvent eventNew;
		eventNew.SetDirection(!event.ShiftDown());
		GetEventHandler()->ProcessEvent(eventNew);
		return;
	} else if (event.AltDown()) {
		event.Skip();
		return;
	}
	Element *pElement = GetElementByKeyCode(event.GetKeyCode());
	if (_pElementPressed != NULL) {
		if (_pElementPressed != pElement) {
			_pElementPressed->OnRelease(false);
		} else {
			return;
		}
	}
	if (pElement != NULL) {
		pElement->OnPress();
	}
	Refresh();
	_pElementPressed = pElement;
}

void Board::OnKeyUp(wxKeyEvent &event)
{
	Element *pElement = GetElementByKeyCode(event.GetKeyCode());
	if (_pElementPressed != NULL) {
		_pElementPressed->OnRelease(_pElementPressed == pElement);
	}
	Refresh();
	_pElementPressed = NULL;
}

void Board::OnChar(wxKeyEvent &event)
{
	event.Skip();
}

//-----------------------------------------------------------------------------
// Board::Element
//-----------------------------------------------------------------------------
void Board::Element::OnPress()
{
}

void Board::Element::OnRelease(bool activeFlag)
{
}

//-----------------------------------------------------------------------------
// Board::NumLED
//-----------------------------------------------------------------------------
const char Board::NumLED::_patternTbl[16][NUM_SEGS] = {
	{ 1, 1, 1, 1, 1, 1, 0 },	// 0
	{ 0, 1, 1, 0, 0, 0, 0 },	// 1
	{ 1, 1, 0, 1, 1, 0, 1 },	// 2
	{ 1, 1, 1, 1, 0, 0, 1 },	// 3
	{ 0, 1, 1, 0, 0, 1, 1 },	// 4
	{ 1, 0, 1, 1, 0, 1, 1 },	// 5
	{ 1, 0, 1, 1, 1, 1, 1 },	// 6
	{ 1, 1, 1, 0, 0, 0, 0 },	// 7
	{ 1, 1, 1, 1, 1, 1, 1 },	// 8
	{ 1, 1, 1, 1, 0, 1, 1 },	// 9
	{ 1, 1, 1, 0, 1, 1, 1 },	// A
	{ 0, 0, 1, 1, 1, 1, 1 },	// B
	{ 1, 0, 0, 1, 1, 1, 0 },	// C
	{ 0, 1, 1, 1, 1, 0, 1 },	// D
	{ 1, 0, 0, 1, 1, 1, 1 },	// E
	{ 1, 0, 0, 0, 1, 1, 1 },	// F
};

Board::NumLED::NumLED(Board *pBoard, const wxRect &rc, const wxString &text) :
			Element(pBoard, rc, text, 0), _value(0)
{
	const int wdLine = 8;
	wxRect rcBox(_rc);
	rcBox.Deflate(_rc.width * 15 / 100, _rc.height * 10 / 100);
	int htCenter = rcBox.height / 2;
	int xMiddle = rcBox.x + wdLine;
	int xRight = rcBox.GetRight() - wdLine;
	int wdMiddle = xRight - xMiddle;
	_rcSegTbl[0] = wxRect(xMiddle, rcBox.y, wdMiddle, wdLine);
	_rcSegTbl[1] = wxRect(xRight, rcBox.y, wdLine, htCenter);
	_rcSegTbl[2] = wxRect(xRight, rcBox.y + htCenter, wdLine, htCenter);
	_rcSegTbl[3] = wxRect(xMiddle, rcBox.GetBottom() - wdLine + 1, wdMiddle, wdLine);
	_rcSegTbl[4] = wxRect(rcBox.x, rcBox.y + htCenter, wdLine, htCenter);
	_rcSegTbl[5] = wxRect(rcBox.x, rcBox.y, wdLine, htCenter);
	_rcSegTbl[6] = wxRect(xMiddle, rcBox.y + htCenter - wdLine / 2, wdMiddle, wdLine);
}

void Board::NumLED::SetValue(unsigned char value)
{
	_value = value;
}

void Board::NumLED::OnDraw(wxDC &dc)
{
	dc.SetPen(_pBoard->PEN_NumLEDBorder);
	dc.SetBrush(_pBoard->BRUSH_NumLEDBackground);
	dc.DrawRectangle(_rc);
	for (int i = 0; i < NUM_SEGS; i++) {
		if (_value < 0x10 && _patternTbl[_value][i]) {
			dc.SetPen(_pBoard->PEN_NumLED_On);
			dc.SetBrush(_pBoard->BRUSH_NumLED_On);
		} else {
			dc.SetPen(_pBoard->PEN_NumLED_Off);
			dc.SetBrush(_pBoard->BRUSH_NumLED_Off);
		}
		dc.DrawRectangle(_rcSegTbl[i]);
	}
}

//-----------------------------------------------------------------------------
// Board::BinLED
//-----------------------------------------------------------------------------
Board::BinLED::BinLED(Board *pBoard, const wxRect &rc, const wxString &text) :
			Element(pBoard, rc, text, 0), _value(false)
{
}

void Board::BinLED::SetValue(bool value)
{
	_value = value;
}

void Board::BinLED::OnDraw(wxDC &dc)
{
	dc.SetPen(_pBoard->PEN_BinLEDBorder);
	dc.SetBrush(_pBoard->BRUSH_BinLEDBackground);
	dc.DrawRectangle(_rc);
	if (_value) {
		dc.SetPen(_pBoard->PEN_BinLED_On);
		dc.SetBrush(_pBoard->BRUSH_BinLED_On);
	} else {
		dc.SetPen(_pBoard->PEN_BinLED_Off);
		dc.SetBrush(_pBoard->BRUSH_BinLED_Off);
	}
	int diameter = _rc.height * 9 / 10;
	int x = _rc.x + (_rc.width - diameter) / 2;
	int y = _rc.y + (_rc.height - diameter) / 2;
	dc.DrawEllipse(x, y, diameter, diameter);
}

//-----------------------------------------------------------------------------
// Board::ModeIndicator
//-----------------------------------------------------------------------------
Board::ModeIndicator::ModeIndicator(Board *pBoard, const wxRect &rc, const wxString &text) :
								Element(pBoard, rc, text, 0)
{
}

void Board::ModeIndicator::SetHighlight(bool flag)
{
	_highlightFlag = flag;
}

void Board::ModeIndicator::OnDraw(wxDC &dc)
{
	wxCoord wdText, htText;
	dc.SetFont(_pBoard->FONT_ModeIndicator);
	if (_highlightFlag) {
		dc.SetTextForeground(_pBoard->COLOUR_ModeIndicatorOn);
	} else {
		dc.SetTextForeground(_pBoard->COLOUR_ModeIndicatorOff);
	}
	dc.GetTextExtent(_text, &wdText, &htText);
	int x = _rc.x;
	int y = _rc.y + (_rc.height - htText) / 2;
	dc.DrawText(_text, x, y);
}

//-----------------------------------------------------------------------------
// Board::Key
//-----------------------------------------------------------------------------
Board::Key::Key(Board *pBoard, const wxRect &rc,
			const wxChar *text, long keyCode, wxFont &font, unsigned char value) :
	Element(pBoard, rc, text ,keyCode), _font(font), _value(value), _pressedFlag(false)
{
}

void Board::Key::OnPress()
{
	_pressedFlag = true;
	//_pBoard->Refresh();
	Action();
}

void Board::Key::OnRelease(bool activeFlag)
{
	_pressedFlag = false;
	//_pBoard->Refresh();
}

void Board::Key::OnDraw(wxDC &dc)
{
	do {
		dc.SetPen(*wxTRANSPARENT_PEN);
		if (wxWindow::FindFocus() == _pBoard) {
			dc.SetBrush(_pBoard->BRUSH_KeyFocusOn);
		} else {
			dc.SetBrush(_pBoard->BRUSH_KeyFocusOff);
		}
		dc.DrawRectangle(_rc);
	} while (0);
	do {
		wxCoord wdText, htText;
		dc.SetFont(_font);
		dc.SetTextForeground(_pBoard->COLOUR_KeyText);
		dc.GetTextExtent(_text, &wdText, &htText);
		int x = _rc.x + (_rc.width - wdText) / 2;
		int y = _rc.y + (_rc.height - htText) / 2;
		if (_pressedFlag) {
			x += 1, y += 1;
		}
		dc.DrawText(_text, x, y);
	} while (0);
	if (!_pressedFlag) {
		dc.SetPen(_pBoard->COLOUR_KeyBlightSide);
		dc.DrawLine(_rc.GetTopLeft(), _rc.GetTopRight());
		dc.DrawLine(_rc.GetTopLeft(), _rc.GetBottomLeft());
	}
}

void Board::Key_Hex::OnPress()
{
	Key::OnPress();
	_pBoard->GetVM().SetKeyScan(true, _value);
}

void Board::Key_Hex::OnRelease(bool activeFlag)
{
	Key::OnRelease(activeFlag);
	_pBoard->GetVM().SetKeyScan(false);
}

void Board::Key_Hex::Action()
{
	VirtualMachineGUI &vm = _pBoard->GetVM();
	if (vm.GetMode() == VirtualMachine::MODE_Program ||
		vm.GetMode() == VirtualMachine::MODE_Step) {
		_pBoard->SetNumLED(_value);
		_pBoard->PutKeyBuff(_value);
	} else if (vm.GetMode() == VirtualMachine::MODE_Run) {
		// nothing to do
	} else {
		// nothing to do
	}
}

void Board::Key_ASET::Action()
{
	VirtualMachineGUI &vm = _pBoard->GetVM();
	if (vm.GetMode() == VirtualMachine::MODE_Program ||
		vm.GetMode() == VirtualMachine::MODE_Step) {
		vm.SetAddress(_pBoard->GetKeyBuff(), true);
		_pBoard->ClearKeyBuff();
	} else if (vm.GetMode() == VirtualMachine::MODE_Run) {
		// nothing to do
	} else {
		// nothing to do
	}
}

void Board::Key_INCR::Action()
{
	VirtualMachineGUI &vm = _pBoard->GetVM();
	if (vm.GetMode() == VirtualMachine::MODE_Program) {
		if (_pBoard->CountKeyBuff() > 0) {
			unsigned char value = _pBoard->GetKeyBuff() & 0xf;
			vm.PutMem(vm.GetAddress(), value);
			if (vm.GetAddress() < VirtualMachine::ADDR_MEMORY) {
				vm.DoNotifyProgramChange();
			}
		}
		vm.IncrAddress();
		_pBoard->ClearKeyBuff();
	} else if (vm.GetMode() == VirtualMachine::MODE_Run) {
		// nothing to do
	} else if (vm.GetMode() == VirtualMachine::MODE_Step) {
		vm.RunSingle();
	} else {
		// nothing to do
	}
}

void Board::Key_RUN::Action()
{
	VirtualMachineGUI &vm = _pBoard->GetVM();
	if (vm.GetMode() == VirtualMachine::MODE_Program) {
		unsigned char runMode = _pBoard->GetKeyBuff() & 0xf;
		_pBoard->ClearKeyBuff();
		if (runMode == 1 || runMode == 2) {
			vm.EnterRunMode(false);
		} else if (runMode == 5 || runMode == 6) {
			vm.Reset();
			vm.EnterStepMode();
		} else if (runMode == 0xa) {
			vm.EnterRunMode(true);
		} else {
			// nothing to do
		}
	} else if (vm.GetMode() == VirtualMachine::MODE_Run) {
		// nothing to do
	} else if (vm.GetMode() == VirtualMachine::MODE_Step) {
		// nothing to do
	} else {
		// nothing to do
	}
}

void Board::Key_RESET::Action()
{
	VirtualMachineGUI &vm = _pBoard->GetVM();
	if (vm.GetMode() == VirtualMachine::MODE_Program) {
		_pBoard->ClearKeyBuff();
		vm.SetAddress(0x00, true);
	} else if (vm.GetMode() == VirtualMachine::MODE_Run) {
		_pBoard->ClearKeyBuff();
		vm.LeaveRunMode();
	} else if (vm.GetMode() == VirtualMachine::MODE_Step) {
		_pBoard->ClearKeyBuff();
		vm.LeaveStepMode();
	} else {
		// nothing to do
	}
}
