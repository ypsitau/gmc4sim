// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include <stdarg.h>
#include "VirtualMachineGUI.h"
#include "Board.h"

//-----------------------------------------------------------------------------
// VirtualMachineGUI
//-----------------------------------------------------------------------------
VirtualMachineGUI::VirtualMachineGUI() : _pThread(NULL), _pBoard(NULL)
{
	_midiCtrl.Open();
	_midiCtrl.ProgramChange(0, 23); // Harmonica
}

void VirtualMachineGUI::EnterRunMode(bool beeperModeFlag)
{
	if (_mode != MODE_Program) return;
	Reset();
	SetNumLED(0xff);
	RefreshNumLED();
	SetMode(MODE_Run);
	if (beeperModeFlag) {
		_pThread = new Thread_Beeper(*this);
	} else {
		_pThread = new Thread_Simulator(*this);
	}
	_pThread->Create();
	_pThread->Run();
}

void VirtualMachineGUI::LeaveRunMode()
{
	SetContinueFlag(false);
	_pThread->Wait();
	delete _pThread;
	_pThread = NULL;
	SetMode(MODE_Program);
	SetAddress(0x00);
	//DoNotifyRegisterChange();
}

void VirtualMachineGUI::EnterStepMode()
{
	if (_mode != MODE_Program) return;
	Reset();
	SetMode(MODE_Step);
}

void VirtualMachineGUI::LeaveStepMode()
{
	SetMode(MODE_Program);
}

void VirtualMachineGUI::Beep(unsigned char note, int msec)
{
	//_pBoard->SetBinLED(_binLED | (1 << 3));
	//_pBoard->Refresh();
	//_pBoard->Update();
	_midiCtrl.NoteOn(0, note, 127);
	Delay(msec);
	_midiCtrl.NoteOn(0, note, 0);
	//_pBoard->SetBinLED(_binLED);
	//_pBoard->Refresh();
	//_pBoard->Update();
}

void VirtualMachineGUI::RefreshBinLED()
{
	if (_pBoard == NULL) return;
	_pBoard->SetBinLED(_binLED);
	_pBoard->Refresh();
}

void VirtualMachineGUI::RefreshNumLED()
{
	if (_pBoard == NULL) return;
	_pBoard->SetNumLED(_numLED);
	_pBoard->Refresh();
}

void VirtualMachineGUI::Delay(int msec)
{
	::Sleep(msec);
}

//-----------------------------------------------------------------------------
// VirtualMachineGUI::Thread_Simulator
//-----------------------------------------------------------------------------
VirtualMachineGUI::Thread_Simulator::Thread_Simulator(VirtualMachineGUI &vm) :
								wxThread(wxTHREAD_JOINABLE), _vm(vm)
{
}

wxThread::ExitCode VirtualMachineGUI::Thread_Simulator::Entry()
{
	while (_vm.RunSingle()) ;
	return 0;
}

//-----------------------------------------------------------------------------
// VirtualMachineGUI::Thread_Beeper
//-----------------------------------------------------------------------------
VirtualMachineGUI::Thread_Beeper::Thread_Beeper(VirtualMachineGUI &vm) :
								wxThread(wxTHREAD_JOINABLE), _vm(vm)
{
}

wxThread::ExitCode VirtualMachineGUI::Thread_Beeper::Entry()
{
	_vm.SetBeepTempo(_vm.GetMem(0x00));
	_vm.SetAddress(0x01);
	while (_vm.BeepSingle()) ;
	return 0;
}
