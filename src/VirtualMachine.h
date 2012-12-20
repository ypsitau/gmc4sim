// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __VIRTUALMACHINE_H__
#define __VIRTUALMACHINE_H__

#include <stdarg.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <memory>
#include <algorithm>
#include "Operator.h"

class VirtualMachine {
public:
	enum Mode {
		MODE_Program, MODE_Run, MODE_Step,
	};
	enum {
		ADDR_PROGRAM	= 0x00,
		ADDR_MEMORY		= 0x50,
		ADDR_REGISTER	= 0x60,
		ADDR_MAX		= 0x80,
	};
	enum {
		TEMPO_MIN = 30,
		TEMPO_MAX = (440 - 3 * TEMPO_MIN) / 2, // set 9 when tempo is 88
	};
	class Observer {
	public:
		virtual void NotifyModeChange(const VirtualMachine &vm) = 0;
		virtual void NotifyAddressChange(const VirtualMachine &vm) = 0;
		virtual void NotifyOpExecuted(const VirtualMachine &vm) = 0;
		virtual void NotifyProgramChange(const VirtualMachine &vm) = 0;
	};
	typedef std::vector<Observer *> ObserverTbl;
protected:
	Mode _mode;
	unsigned char _addr;
	bool _execFlag;
	bool _continueFlag;
	unsigned char _numLED;
	unsigned char _binLED;
	int _beepLengthUnit;	// set by SetBeepTempo()
	struct {
		bool pressedFlag;
		unsigned char value;
	} _keyScan;
	unsigned char _memBuff[ADDR_MAX];
	OperatorOwner _operatorOwner;
	OperatorOwner _operatorSubOwner;
	OperatorMapByCode _operatorMap;
	OperatorMapByCode _operatorSubMap;
	ObserverTbl _observerTbl;
public:
	VirtualMachine();
	void Reset();
	inline const OperatorOwner &GetOperatorOwner() const { return _operatorOwner; }
	inline const OperatorOwner &GetOperatorSubOwner() const { return _operatorSubOwner; }
	inline const OperatorMapByCode &GetOperatorMap() const { return _operatorMap; }
	inline const OperatorMapByCode &GetOperatorSubMap() const { return _operatorSubMap; }
	void SetMode(Mode mode);
	inline Mode GetMode() const { return _mode; }
	inline unsigned char GetAddress() const { return _addr; }
	void SetAddress(unsigned char addr, bool forceNotifyFlag = false);
	void IncrAddress(int n = 1);
	inline bool GetExecFlag() const { return _execFlag; }
	inline void SetExecFlag(bool flag) { _execFlag = flag; }
	inline bool GetContinueFlag() const { return _continueFlag; }
	inline void SetContinueFlag(bool flag) { _continueFlag = flag; }
	inline void SetNumLED(unsigned char value) { _numLED = value; }
	inline void SetBinLED(unsigned char value) { _binLED = value; }
	inline unsigned char GetNumLED() const { return _numLED; }
	inline unsigned char GetBinLED() const { return _binLED; }
	inline unsigned char &RegA() { return _memBuff[0x6f]; }
	inline unsigned char &RegB() { return _memBuff[0x6c]; }
	inline unsigned char &RegY() { return _memBuff[0x6e]; }
	inline unsigned char &RegZ() { return _memBuff[0x6d]; }
	inline unsigned char &RegAx() { return _memBuff[0x69]; }
	inline unsigned char &RegBx() { return _memBuff[0x67]; }
	inline unsigned char &RegYx() { return _memBuff[0x68]; }
	inline unsigned char &RegZx() { return _memBuff[0x66]; }
	inline unsigned char &RegM(unsigned char idx) { return _memBuff[0x50 + idx]; }
	inline const unsigned char &RegA() const { return _memBuff[0x6f]; }
	inline const unsigned char &RegB() const { return _memBuff[0x6c]; }
	inline const unsigned char &RegY() const { return _memBuff[0x6e]; }
	inline const unsigned char &RegZ() const { return _memBuff[0x6d]; }
	inline const unsigned char &RegAx() const { return _memBuff[0x69]; }
	inline const unsigned char &RegBx() const { return _memBuff[0x67]; }
	inline const unsigned char &RegYx() const { return _memBuff[0x68]; }
	inline const unsigned char &RegZx() const { return _memBuff[0x66]; }
	inline const unsigned char &RegM(unsigned char idx) const { return _memBuff[0x50 + idx]; }
	inline bool IsValidMem(unsigned char addr) const { return _memBuff[addr] < 0x10; }
	inline void PutMem(unsigned char addr, unsigned char value) { _memBuff[addr] = value; }
	inline unsigned char GetMem(unsigned char addr) const { return _memBuff[addr] & 0xf; }
	unsigned char GetInvalidRegionAddress() const;
	void AddObserver(Observer *pObserver);
	void RemoveObserver(Observer *pObserver);
	void DoNotifyModeChange() const;
	void DoNotifyAddressChange() const;
	void DoNotifyOpExecuted() const;
	void DoNotifyProgramChange() const;
	void InitializeRegs();
	void InvalidateMemBuff();
	void PutMemBuff(const unsigned char *memBuff);
	bool RunSingle();
	bool BeepSingle();
	void SetBeepTempo(unsigned char tempoGMC);
	void SetKeyScan(bool pressedFlag);
	void SetKeyScan(bool pressedFlag, unsigned char value);
	bool GetKey(unsigned char *pValue);
	virtual void Beep(unsigned char note, int msec) = 0;
	virtual void RefreshBinLED() = 0;
	virtual void RefreshNumLED() = 0;
	virtual void Delay(int msec) = 0;
};

typedef std::vector<unsigned char> AddressByLineTbl;

#endif
