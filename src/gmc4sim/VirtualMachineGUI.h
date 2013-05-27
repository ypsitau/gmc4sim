// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __VIRTUALMACHINEGUI_H__
#define __VIRTUALMACHINEGUI_H__

#include "VirtualMachine.h"
#include "MidiController.h"

class Board;

class VirtualMachineGUI : public VirtualMachine {
private:
	class Thread_Simulator : public wxThread {
	private:
		VirtualMachineGUI &_vm;
	public:
		Thread_Simulator(VirtualMachineGUI &vm);
		virtual ExitCode Entry();
	};
	class Thread_Beeper : public wxThread {
	private:
		VirtualMachineGUI &_vm;
	public:
		Thread_Beeper(VirtualMachineGUI &vm);
		virtual ExitCode Entry();
	};
private:
	wxThread *_pThread;
	Board *_pBoard;
	MIDIController _midiCtrl;
public:
	VirtualMachineGUI();
	void EnterRunMode(bool beeperModeFlag);
	void LeaveRunMode();
	void EnterStepMode();
	void LeaveStepMode();
	inline void SetBoard(Board *pBoard) { _pBoard = pBoard; }
	inline Board *GetBoard() { return _pBoard; }
	// virtual functions of VirtualMachine
	virtual void Beep(unsigned char note, int msec);
	virtual void RefreshBinLED();
	virtual void RefreshNumLED();
	virtual void Delay(int msec);
};

#endif
