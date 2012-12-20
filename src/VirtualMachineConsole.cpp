// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include "stdafx.h"
#include "VirtualMachineConsole.h"

//-----------------------------------------------------------------------------
// VirtualMachineConsole
//-----------------------------------------------------------------------------
VirtualMachineConsole::VirtualMachineConsole()
{
	_midiCtrl.Open();
	_midiCtrl.ProgramChange(0, 23); // Harmonica
}

std::string VirtualMachineConsole::MakeLEDString() const
{
	std::string rtn;
	const char hexTbl[] = "0123456789ABCDEF";
	rtn.push_back('[');
	rtn.push_back((GetNumLED() > 0xf)? ' ' : hexTbl[GetNumLED() & 0xf]);
	rtn.push_back(']');
	for (unsigned char mask = 0x40; mask != 0; mask >>= 1) {
		rtn.push_back(' ');
		rtn.push_back((GetBinLED() & mask)? '*' : '_');
	}
	return rtn;
}

std::string VirtualMachineConsole::MakeWatchString() const
{
	char rtn[80];
	const char hexTbl[] = "0123456789ABCDEF";
	::sprintf(rtn, "A=%X,%X Y=%X,%X B=%X,%X Z=%X,%X exec=%d M=",
		RegA(), RegAx(), RegY(), RegYx(), RegB(), RegBx(), RegZ(), RegZx(), GetExecFlag());
	int pos = ::strlen(rtn);
	for (int i = 0; i < 16; i++, pos++) rtn[pos] = hexTbl[RegM(i)];
	rtn[pos] = '\0';
	return std::string(rtn);
}

void VirtualMachineConsole::Beep(unsigned char note, int msec)
{
	_midiCtrl.NoteOn(0, note, 127);
	Delay(msec);
	_midiCtrl.NoteOn(0, note, 0);
}

void VirtualMachineConsole::RefreshBinLED()
{
	::fprintf(stderr, "%s\r", MakeLEDString().c_str());
}

void VirtualMachineConsole::RefreshNumLED()
{
	::fprintf(stderr, "%s\r", MakeLEDString().c_str());
}

void VirtualMachineConsole::Delay(int msec)
{
	::Sleep(msec);
}

void VirtualMachineConsole::RunKeyHandler()
{
	DWORD dwThreadId;
	::CreateThread(NULL, 0, ThreadFuncStub, this, 0, &dwThreadId);
}

void VirtualMachineConsole::ThreadFunc()
{
	for (;;) {
		DWORD cntEvents;
		INPUT_RECORD inputRec;
		::ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &inputRec, 1, &cntEvents);
		if (!(inputRec.EventType & KEY_EVENT)) continue;
		KEY_EVENT_RECORD &event = inputRec.Event.KeyEvent;
		if (!event.bKeyDown) {
			SetKeyScan(false);
		} else if ('0' <= event.wVirtualKeyCode && event.wVirtualKeyCode <= '9') {
			SetKeyScan(true, event.wVirtualKeyCode - '0');
		} else if ('A' <= event.wVirtualKeyCode && event.wVirtualKeyCode <= 'F') {
			SetKeyScan(true, event.wVirtualKeyCode - 'A' + 10);
		} else {
			// nothing to do
		}
	}
}

DWORD WINAPI VirtualMachineConsole::ThreadFuncStub(LPVOID pvArg)
{
	VirtualMachineConsole *self = reinterpret_cast<VirtualMachineConsole *>(pvArg);
	self->ThreadFunc();
	return 0;
}
