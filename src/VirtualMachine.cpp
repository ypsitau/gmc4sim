// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include "VirtualMachine.h"

//-----------------------------------------------------------------------------
// VirtualMachine
//-----------------------------------------------------------------------------
VirtualMachine::VirtualMachine() :
	_mode(MODE_Program), _addr(0x00), _execFlag(true), _continueFlag(true),
	_numLED(0x0), _binLED(0x0), _beepLengthUnit(200)
{
	SetKeyScan(false, 0xf);
	InvalidateMemBuff();
	InitializeRegs();
	_operatorOwner.Add(new Operator_KA());
	_operatorOwner.Add(new Operator_AO());
	_operatorOwner.Add(new Operator_CH());
	_operatorOwner.Add(new Operator_CY());
	_operatorOwner.Add(new Operator_AM());
	_operatorOwner.Add(new Operator_MA());
	_operatorOwner.Add(new Operator_MPlus());
	_operatorOwner.Add(new Operator_MMinus());
	_operatorOwner.Add(new Operator_TIA());
	_operatorOwner.Add(new Operator_AIA());
	_operatorOwner.Add(new Operator_TIY());
	_operatorOwner.Add(new Operator_AIY());
	_operatorOwner.Add(new Operator_CIA());
	_operatorOwner.Add(new Operator_CIY());
	_operatorOwner.Add(new Operator_CAL());
	_operatorOwner.Add(new Operator_JUMP());
	_operatorSubOwner.Add(new Operator_Sub_RSTO());
	_operatorSubOwner.Add(new Operator_Sub_SETR());
	_operatorSubOwner.Add(new Operator_Sub_RSTR());
	_operatorSubOwner.Add(new Operator_Sub_INPT());
	_operatorSubOwner.Add(new Operator_Sub_CMPL());
	_operatorSubOwner.Add(new Operator_Sub_CHNG());
	_operatorSubOwner.Add(new Operator_Sub_SIFT());
	_operatorSubOwner.Add(new Operator_Sub_ENDS());
	_operatorSubOwner.Add(new Operator_Sub_ERRS());
	_operatorSubOwner.Add(new Operator_Sub_SHTS());
	_operatorSubOwner.Add(new Operator_Sub_LONS());
	_operatorSubOwner.Add(new Operator_Sub_SUND());
	_operatorSubOwner.Add(new Operator_Sub_TIMR());
	_operatorSubOwner.Add(new Operator_Sub_DSPR());
	_operatorSubOwner.Add(new Operator_Sub_DEMMinus());
	_operatorSubOwner.Add(new Operator_Sub_DEMPlus());
	foreach (OperatorOwner, ppOperator, _operatorOwner) {
		Operator *pOperator = *ppOperator;
		_operatorMap[pOperator->GetCode()] = pOperator;
	}
	foreach (OperatorOwner, ppOperator, _operatorSubOwner) {
		Operator *pOperator = *ppOperator;
		_operatorSubMap[pOperator->GetCode()] = pOperator;
	}
}

void VirtualMachine::Reset()
{
	SetAddress(0x00, true);
	SetNumLED(0);
	SetBinLED(0x00);
	SetKeyScan(false, 0xf);
	//ClearReg();
	SetExecFlag(true);
	SetContinueFlag(true);
}

void VirtualMachine::SetMode(Mode mode)
{
	_mode = mode;
	DoNotifyModeChange();
}

void VirtualMachine::SetAddress(unsigned char addr, bool forceNotifyFlag)
{
	if (forceNotifyFlag || _addr != addr) {
		_addr = addr;
		DoNotifyAddressChange();
	}
}

void VirtualMachine::IncrAddress(int n)
{
	_addr += n;
	DoNotifyAddressChange();
}

unsigned char VirtualMachine::GetInvalidRegionAddress() const
{
	unsigned addr = ADDR_REGISTER - 1;
	for (int i = 0; i < ADDR_REGISTER; i++, addr--) {
		if (IsValidMem(addr)) return addr + 1;
	}
	return ADDR_PROGRAM;
}

void VirtualMachine::AddObserver(Observer *pObserver)
{
	_observerTbl.push_back(pObserver);
}

void VirtualMachine::RemoveObserver(Observer *pObserver)
{
	ObserverTbl::iterator ppObserver =
		std::find(_observerTbl.begin(), _observerTbl.end(), pObserver);
	_observerTbl.erase(ppObserver);
}

void VirtualMachine::DoNotifyModeChange() const
{
	foreach_const (ObserverTbl, ppObserver, _observerTbl) {
		(*ppObserver)->NotifyModeChange(*this);
	}
}

void VirtualMachine::DoNotifyAddressChange() const
{
	foreach_const (ObserverTbl, ppObserver, _observerTbl) {
		(*ppObserver)->NotifyAddressChange(*this);
	}
}

void VirtualMachine::DoNotifyOpExecuted() const
{
	foreach_const (ObserverTbl, ppObserver, _observerTbl) {
		(*ppObserver)->NotifyOpExecuted(*this);
	}
}

void VirtualMachine::DoNotifyProgramChange() const
{
	foreach_const (ObserverTbl, ppObserver, _observerTbl) {
		(*ppObserver)->NotifyProgramChange(*this);
	}
}

void VirtualMachine::InitializeRegs()
{
	for (int addr = ADDR_REGISTER; addr < ADDR_MAX; addr++) {
		_memBuff[addr] = 0xf;
	}
}

void VirtualMachine::InvalidateMemBuff()
{
	::memset(_memBuff, 0xff, ADDR_REGISTER);
	DoNotifyProgramChange();
}

void VirtualMachine::PutMemBuff(const unsigned char *memBuff)
{
	::memcpy(_memBuff, memBuff, ADDR_REGISTER);
	DoNotifyProgramChange();
}

bool VirtualMachine::RunSingle()
{
	if (!(GetAddress() < ADDR_REGISTER &&
					IsValidMem(GetAddress()) && GetContinueFlag())) {
		return false;
	}
	const Operator *pOperator = _operatorMap.Find(GetMem(GetAddress()));
	if (pOperator == NULL) {
		return false;
	}
	SetExecFlag(pOperator->Execute(*this));
	DoNotifyOpExecuted();
	return true;
}

bool VirtualMachine::BeepSingle()
{
	if (!(GetAddress() < ADDR_REGISTER &&
					IsValidMem(GetAddress()) && GetContinueFlag())) {
		return false;
	}
	unsigned char val1 = GetMem(GetAddress());
	unsigned char val2 = GetMem(GetAddress() + 1);
	static const unsigned char noteTbl[] = {
		0, 45, 47, 48, 50, 52, 53, 55,
		57, 59, 60, 62, 64, 65, 67, 0,
	};
	if (val1 == 0xf) {
		if (val2 == 0x0) {
			SetAddress(0x01);
		} else {
			return false;
		}
	} else {
		int length = _beepLengthUnit * (val2 + 1);
		if (val1 == 0x0) {
			Delay(length);
		} else {
			unsigned char note = noteTbl[val1 & 0xf];
			Beep(note, length);
		}
		IncrAddress(2);
	}
	return true;
}

void VirtualMachine::SetBeepTempo(unsigned char tempoGMC)
{
	int tempo = TEMPO_MAX -
			static_cast<int>(TEMPO_MAX - TEMPO_MIN) * tempoGMC / 15;
	_beepLengthUnit = 60000 / tempo * 4 / 16;
}

void VirtualMachine::SetKeyScan(bool pressedFlag)
{
	_keyScan.pressedFlag = pressedFlag;
}

void VirtualMachine::SetKeyScan(bool pressedFlag, unsigned char value)
{
	_keyScan.pressedFlag = pressedFlag, _keyScan.value = value;
}

bool VirtualMachine::GetKey(unsigned char *pValue)
{
	*pValue = _keyScan.value;
	return _keyScan.pressedFlag;
}
