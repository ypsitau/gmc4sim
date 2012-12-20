// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __BOARD_H__
#define __BOARD_H__

#include "Utils.h"
#include "VirtualMachineGUI.h"

class Board : public wxPanel, public VirtualMachine::Observer {
private:
	enum {
		CNT_BinLED = 7,
	};
	struct KeyInfo {
		const wxChar *text;
		int keyCode;
	};
	class Element {
	protected:
		Board *_pBoard;
		wxRect _rc;
		wxString _text;
		int _keyCode;
	public:
		inline Element(Board *pBoard, const wxRect &rc, const wxString &text, int keyCode) :
				_pBoard(pBoard), _rc(rc), _text(text), _keyCode(keyCode) {}
		virtual void OnDraw(wxDC &dc) = 0;
		virtual void OnPress();
		virtual void OnRelease(bool activeFlag);
		inline const wxRect &GetRect() const { return _rc; }
		inline const wxString &GetText() const { return _text; }
		inline int GetKeyCode() const { return _keyCode; }
	};
	class BinLED : public Element {
	private:
		bool _value;
	public:
		BinLED(Board *pBoard, const wxRect &rc, const wxString &text);
		void SetValue(bool value);
		virtual void OnDraw(wxDC &dc);
	};
	class NumLED : public Element {
	private:
		enum { NUM_SEGS = 7 };
	private:
		unsigned char _value;
		wxRect _rcSegTbl[NUM_SEGS];
		static const char _patternTbl[16][NUM_SEGS];
	public:
		NumLED(Board *pBoard, const wxRect &rc, const wxString &text);
		void SetValue(unsigned char value);
		virtual void OnDraw(wxDC &dc);
	};
	class ModeIndicator : public Element {
	private:
		bool _highlightFlag;
	public:
		ModeIndicator(Board *pBoard, const wxRect &rc, const wxString &text);
		void SetHighlight(bool flag);
		virtual void OnDraw(wxDC &dc);
	};
	class Key : public Element {
	protected:
		wxFont &_font;
		const unsigned char _value;
		bool _pressedFlag;
	public:
		Key(Board *pBoard, const wxRect &rc, const wxChar *text,
							long keyCode, wxFont &font, unsigned char value);
		virtual void OnDraw(wxDC &dc);
		virtual void OnPress();
		virtual void OnRelease(bool activeFlag);
		virtual void Action() = 0;
	};
	class Key_Hex : public Key {
	public:
		enum { TYPE = 0x0 };
	public:
		inline Key_Hex(Board *pBoard, const wxRect &rc, const wxChar *text,
							long keyCode, wxFont &font, unsigned char value) :
				Key(pBoard, rc, text, keyCode, font, value) {}
		virtual void OnPress();
		virtual void OnRelease(bool activeFlag);
		virtual void Action();
	};
	class Key_ASET : public Key {
	public:
		enum { TYPE = 0x1 };
	public:
		inline Key_ASET(Board *pBoard, const wxRect &rc, const wxChar *text,
							long keyCode, wxFont &font, unsigned char value) :
				Key(pBoard, rc, text, keyCode, font, value) {}
		virtual void Action();
	};
	class Key_INCR : public Key {
	public:
		enum { TYPE = 0x2 };
	public:
		inline Key_INCR(Board *pBoard, const wxRect &rc, const wxChar *text,
							long keyCode, wxFont &font, unsigned char value) :
				Key(pBoard, rc, text, keyCode, font, value) {}
		virtual void Action();
	};
	class Key_RUN : public Key {
	public:
		enum { TYPE = 0x3 };
	public:
		inline Key_RUN(Board *pBoard, const wxRect &rc, const wxChar *text,
							long keyCode, wxFont &font, unsigned char value) :
				Key(pBoard, rc, text, keyCode, font, value) {}
		virtual void Action();
	};
	class Key_RESET : public Key {
	public:
		enum { TYPE = 0x4 };
	public:
		inline Key_RESET(Board *pBoard, const wxRect &rc, const wxChar *text,
							long keyCode, wxFont &font, unsigned char value) :
				Key(pBoard, rc, text, keyCode, font, value) {}
		virtual void Action();
	};
	typedef std::vector<Element *> ElementTbl;
private:
	VirtualMachineGUI &_vm;
	ElementTbl _elementTbl;
	Element *_pElementPressed;
	NumLED *_pNumLED;
	BinLED *_pBinLEDTbl[7];
	ModeIndicator *_pModeIndicatorProgram;
	ModeIndicator *_pModeIndicatorRun;
	ModeIndicator *_pModeIndicatorStep;
	struct {
		int cntNibbles;
		unsigned char value;
	} _keyBuff;
public:
	wxFont FONT_Key;
	wxFont FONT_KeySmall;
	wxFont FONT_ModeIndicator;
	wxPen PEN_ModeIndicator;
	wxColour COLOUR_ModeIndicatorOn;
	wxColour COLOUR_ModeIndicatorOff;
	wxColour COLOUR_InfoText;
	wxBrush BRUSH_BoardFocusOn;
	wxBrush BRUSH_BoardFocusOff;
	wxBrush BRUSH_BinLED_On;
	wxBrush BRUSH_BinLED_Off;
	wxBrush BRUSH_BinLEDBackground;
	wxPen PEN_BinLED_On;
	wxPen PEN_BinLED_Off;
	wxPen PEN_BinLEDBorder;
	wxBrush BRUSH_NumLED_On;
	wxBrush BRUSH_NumLED_Off;
	wxBrush BRUSH_NumLEDBackground;
	wxPen PEN_NumLED_On;
	wxPen PEN_NumLED_Off;
	wxPen PEN_NumLEDBorder;
	wxBrush BRUSH_KeyFocusOn;
	wxBrush BRUSH_KeyFocusOff;
	wxColour COLOUR_KeyText;
	wxColour COLOUR_KeyBlightSide;
public:
	Board(wxWindow *pParent, wxWindowID id, long style, VirtualMachineGUI &vm);
	~Board();
	void SetBinLED(unsigned char value);
	void SetNumLED(unsigned char value);
	void SetModeIndicator(VirtualMachine::Mode mode);
	inline VirtualMachineGUI &GetVM() { return _vm; }
	inline void PutKeyBuff(unsigned char value) {
		_keyBuff.value = (_keyBuff.value << 4) | (value & 0xf);
		if (_keyBuff.cntNibbles < 2) _keyBuff.cntNibbles++;
	}
	inline bool CountKeyBuff() const { return _keyBuff.cntNibbles; }
	inline void ClearKeyBuff() {
		_keyBuff.value = 0x00, _keyBuff.cntNibbles = 0;
	}
	inline unsigned char GetKeyBuff() const { return _keyBuff.value; }
private:
	Element *GetElementByKeyCode(int keyCode);
	Element *GetElementByPoint(const wxPoint &pt);
private:
	// virtual functions of VirtualMachine::Observer
	virtual void NotifyModeChange(const VirtualMachine &vm);
	virtual void NotifyAddressChange(const VirtualMachine &vm);
	virtual void NotifyOpExecuted(const VirtualMachine &vm);
	virtual void NotifyProgramChange(const VirtualMachine &vm);
	DECLARE_EVENT_TABLE()
	void OnEraseBackground(wxEraseEvent &event);
	void OnPaint(wxPaintEvent &event);
	void OnSetFocus(wxFocusEvent &event);
	void OnKillFocus(wxFocusEvent &event);
	void OnMotion(wxMouseEvent &event);
	void OnLeftDown(wxMouseEvent &event);
	void OnLeftUp(wxMouseEvent &event);
	void OnLeftDClick(wxMouseEvent &event);
	void OnRightDown(wxMouseEvent &event);
	void OnRightUp(wxMouseEvent &event);
	void OnLeaveWindow(wxMouseEvent &event);
	void OnKeyDown(wxKeyEvent &event);
	void OnKeyUp(wxKeyEvent &event);
	void OnChar(wxKeyEvent &event);
};

#endif
