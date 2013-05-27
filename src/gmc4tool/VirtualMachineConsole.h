// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __VIRTUALMACHINECONSOLE_H__
#define __VIRTUALMACHINECONSOLE_H__

#include "VirtualMachine.h"
#include "MidiController.h"

class VirtualMachineConsole : public VirtualMachine {
private:
	MIDIController _midiCtrl;
public:
	VirtualMachineConsole();
	std::string MakeLEDString() const;
	std::string MakeWatchString() const;
	void RunKeyHandler();
	void ThreadFunc();
	static DWORD WINAPI ThreadFuncStub(LPVOID pvArg);
	// virtual functions of VirtualMachine
	virtual void Beep(unsigned char note, int msec);
	virtual void RefreshBinLED();
	virtual void RefreshNumLED();
	virtual void Delay(int msec);
};

#endif
