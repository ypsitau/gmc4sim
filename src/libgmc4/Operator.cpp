// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include "Operator.h"
#include "VirtualMachine.h"

//-----------------------------------------------------------------------------
OperatorOwner::~OperatorOwner()
{
	foreach (OperatorOwner, ppOp, *this) {
		Operator *pOp = *ppOp;
		delete pOp;
	}
}

void OperatorOwner::Add(Operator *pOp)
{
	push_back(pOp);
}

void OperatorOwner::Clear()
{
	foreach (OperatorOwner, ppOp, *this) {
		Operator *pOp = *ppOp;
		delete pOp;
	}
	clear();
}

const Operator *OperatorMapByCode::Find(unsigned char code) const
{
	const_iterator iter = find(code);
	return (iter == end())? NULL : iter->second;
}

const Operator *OperatorMapBySymbol::Find(const std::string &symbol) const
{
	const_iterator iter = find(symbol);
	return (iter == end())? NULL : iter->second;
}

//-----------------------------------------------------------------------------
Operator::Operator(const char *symbol, unsigned char code, Type type) :
							_symbolUpperCase(symbol), _code(code), _type(type)
{
	const char *srcp = _symbolUpperCase;
	char *dstp = _symbolLowerCase;
	for ( ; *srcp != '\0'; srcp++, dstp++) *dstp = tolower(*srcp);
	*dstp = '\0';
}

bool Operator::Execute(VirtualMachine &vm) const
{
	return true;
}

unsigned char Operator::FetchOperand(
						const VirtualMachine &vm, unsigned char addr) const
{
	return 0x00;
}

unsigned char Operator_NibbleOperand::FetchOperand(
						const VirtualMachine &vm, unsigned char addr) const
{
	return vm.GetMem(addr + 1) & 0xf;
}

//-----------------------------------------------------------------------------
bool Operator_KA::Execute(VirtualMachine &vm) const
{
	// K->Ar, 0->exec when key pressed, 1->exec otherwise
	vm.IncrAddress(GetNibbles());
	bool rtn = !vm.GetKey(&vm.RegA());
	return rtn;
}

bool Operator_AO::Execute(VirtualMachine &vm) const
{
	// Ar->Op, 1->exec
	vm.SetNumLED(vm.RegA());
	vm.RefreshNumLED();
	vm.IncrAddress(GetNibbles());
	return true;
}

bool Operator_CH::Execute(VirtualMachine &vm) const
{
	// Ar<->Br, Yr<->Zr, 1->exec
	Swap(vm.RegA(), vm.RegB());
	Swap(vm.RegY(), vm.RegZ());
	vm.IncrAddress(GetNibbles());
	return true;
}

bool Operator_CY::Execute(VirtualMachine &vm) const
{
	// Ar<->Yr, 1->exec
	Swap(vm.RegA(), vm.RegY());
	vm.IncrAddress(GetNibbles());
	return true;
}

bool Operator_AM::Execute(VirtualMachine &vm) const
{
	// Ar->M, 1->exec
	vm.RegM(vm.RegY()) = vm.RegA();
	vm.IncrAddress(GetNibbles());
	return true;
}

bool Operator_MA::Execute(VirtualMachine &vm) const
{
	// M->Ar, 1->exec
	vm.RegA() = vm.RegM(vm.RegY()) & 0xf;
	vm.IncrAddress(GetNibbles());
	return true;
}

bool Operator_MPlus::Execute(VirtualMachine &vm) const
{
	// M+Ar->Ar, carry->exec
	unsigned char value = (vm.RegM(vm.RegY()) & 0xf) + vm.RegA();
	vm.RegA() = value & 0xf;
	vm.IncrAddress(GetNibbles());
	bool rtn = (value >> 4)? true : false;
	return rtn;
}

bool Operator_MMinus::Execute(VirtualMachine &vm) const
{
	// M-Ar->Ar, borrow->exec
	bool rtn = (vm.RegM(vm.RegY() & 0xf) < vm.RegA());
	vm.RegA() = ((vm.RegM(vm.RegY()) & 0xf) - vm.RegA()) & 0xf;
	vm.IncrAddress(GetNibbles());
	return rtn;
}

bool Operator_TIA::Execute(VirtualMachine &vm) const
{
	// imm->Ar, 1->exec
	vm.RegA() = vm.GetMem(vm.GetAddress() + 1);
	vm.IncrAddress(GetNibbles());
	return true;
}

bool Operator_AIA::Execute(VirtualMachine &vm) const
{
	// Ar+imm->Ar, carry->exec
	unsigned char value = vm.RegA() + vm.GetMem(vm.GetAddress() + 1);
	vm.RegA() = value & 0xf;
	vm.IncrAddress(GetNibbles());
	bool rtn = (value >> 4)? true : false;
	return rtn;
}

bool Operator_TIY::Execute(VirtualMachine &vm) const
{
	// imm->Yr, 1->exec
	vm.RegY() = vm.GetMem(vm.GetAddress() + 1);
	vm.IncrAddress(GetNibbles());
	return true;
}

bool Operator_AIY::Execute(VirtualMachine &vm) const
{
	// Yr+Imm->Yr, carry->exec
	unsigned char value = vm.RegY() + vm.GetMem(vm.GetAddress() + 1);
	vm.RegY() = value & 0xf;
	vm.IncrAddress(GetNibbles());
	bool rtn = (value >> 4)? true : false;
	return rtn;
}

bool Operator_CIA::Execute(VirtualMachine &vm) const
{
	// exec->1 when Ar!=Imm, exec->0 otherwise
	bool rtn = vm.RegA() != vm.GetMem(vm.GetAddress() + 1);
	vm.IncrAddress(GetNibbles());
	return rtn;
}

bool Operator_CIY::Execute(VirtualMachine &vm) const
{
	// exec->1 when Yr!=Imm, exec->0 otherwise
	bool rtn = vm.RegY() != vm.GetMem(vm.GetAddress() + 1);
	vm.IncrAddress(GetNibbles());
	return rtn;
}

bool Operator_CAL::Execute(VirtualMachine &vm) const
{
	bool rtn = true;
	if (vm.GetExecFlag()) {
		unsigned char codeSub = vm.GetMem(vm.GetAddress() + 1);
		const Operator *pOperatorSub = vm.GetOperatorSubMap().Find(codeSub);
		if (pOperatorSub == NULL) {
			vm.SetContinueFlag(false);
			return true;
		}
		rtn = pOperatorSub->Execute(vm);
	}
	vm.IncrAddress(GetNibbles());
	return rtn;
}

bool Operator_JUMP::Execute(VirtualMachine &vm) const
{
	// Imm->addr, 1->exec
	if (vm.GetExecFlag()) {
		vm.SetAddress((vm.GetMem(vm.GetAddress() + 1) << 4) |
					   vm.GetMem(vm.GetAddress() + 2), true);
	} else {
		vm.IncrAddress(GetNibbles());
	}
	return true;
}

unsigned char Operator_JUMP::FetchOperand(
					const VirtualMachine &vm, unsigned char addr) const
{
	return (vm.GetMem(addr + 1) << 4) | vm.GetMem(addr + 2);
}

bool Operator_Sub_RSTO::Execute(VirtualMachine &vm) const
{
	// Turn off number LED, 1->exec
	vm.SetNumLED(0xff);
	vm.RefreshNumLED();
	return true;
}

bool Operator_Sub_SETR::Execute(VirtualMachine &vm) const
{
	// Turn on binary LED indexed by Yr, 1->exec
	vm.SetBinLED(vm.GetBinLED() | (1 << vm.RegY()));
	vm.RefreshBinLED();
	return true;
}

bool Operator_Sub_RSTR::Execute(VirtualMachine &vm) const
{
	// Turn off binary LED indexed by Yr, 1->exec
	vm.SetBinLED(vm.GetBinLED() & ~(1 << vm.RegY()));
	vm.RefreshBinLED();
	return true;
}

bool Operator_Sub_INPT::Execute(VirtualMachine &vm) const
{
	// (not available for GMC-4)
	return true;
}

bool Operator_Sub_CMPL::Execute(VirtualMachine &vm) const
{
	// ~Ar->Ar, 1->exec
	vm.RegA() = ~vm.RegA() & 0xf;
	return true;
}

bool Operator_Sub_CHNG::Execute(VirtualMachine &vm) const
{
	// Ar<->Ar', Br<->Br', Yr<->Yr', Zr<->Zr', 1->exec
	Swap(vm.RegA(), vm.RegAx());
	Swap(vm.RegB(), vm.RegBx());
	Swap(vm.RegY(), vm.RegYx());
	Swap(vm.RegZ(), vm.RegZx());
	return true;
}

bool Operator_Sub_SIFT::Execute(VirtualMachine &vm) const
{
	// Ar>>1->Ar, !carry->exec
	bool rtn = (vm.RegA() & 1)? false : true;
	vm.RegA() = vm.RegA() >> 1;
	return rtn;
}

bool Operator_Sub_ENDS::Execute(VirtualMachine &vm) const
{
	// Make a sound indicating something's end, 1->exec
	for (unsigned char note = 80; note < 95; note += 2) {
		vm.Beep(note, 60);
	}
	return true;
}

bool Operator_Sub_ERRS::Execute(VirtualMachine &vm) const
{
	// Make a sound indicating an error, 1->exec
	for (int i = 0; i < 8; i++) {
		vm.Beep(80, 30);
		vm.Beep(85, 30);
		vm.Beep(90, 30);
	}
	return true;
}

bool Operator_Sub_SHTS::Execute(VirtualMachine &vm) const
{
	// Make a short sound of morse signal, 1->exec
	vm.Beep(66, 150);
	vm.Delay(150);
	return true;
}

bool Operator_Sub_LONS::Execute(VirtualMachine &vm) const
{
	// Make a long sound of morse signal, 1->exec
	vm.Beep(66, 450);
	vm.Delay(150);
	return true;
}

bool Operator_Sub_SUND::Execute(VirtualMachine &vm) const
{
	// Make a sound selected by Ar value as follows, 1->exec
	// 0:none, 1:<A, 2:<B, 3:C,  4:D,  5:E,  6:F,  7:G,
	// 8:A,    9:B,  A:>C, B:>D, C:>E, D:>F, E:>G, F:none
	static const unsigned char noteTbl[] = {
		0, 45, 47, 48, 50, 52, 53, 55,
		57, 59, 60, 62, 64, 65, 67, 0,
	};
	unsigned char note = noteTbl[vm.RegA() & 0xf];
	int msec = 200;
	if (note == 0) {
		vm.Delay(msec);
	} else {
		vm.Beep(note, msec);
	}
	return true;
}

bool Operator_Sub_TIMR::Execute(VirtualMachine &vm) const
{
	// Wait for (Ar+1)x0.1sec, 1->exec
	vm.Delay((vm.RegA() + 1) * 100);
	return true;
}

bool Operator_Sub_DSPR::Execute(VirtualMachine &vm) const
{
	// Put the value of M(0xf) to binary LED 6 to 4 and
	//     the value of M(0xe) to binary LED 3 to 0, 1->exec
	vm.SetBinLED(((vm.RegM(0xf) & 0x7) << 4) | (vm.RegM(0xe) & 0xf));
	vm.RefreshBinLED();
	return true;
}

// CAL DEM+ and CAL DEM - have been implemented by referring the following site
// http://xl.cocolog-nifty.com/blog/2009/07/gmc-4-no-9829.html
bool Operator_Sub_DEMMinus::Execute(VirtualMachine &vm) const
{
	// M-Ar->M (decimal adjustment), 1->exec
	unsigned char &valM = vm.RegM(vm.RegY());
	if ((valM & 0xf) < vm.RegA()) {
		vm.RegM((vm.RegY() - 1) & 0xf) = 1;
		valM = 10 + (valM & 0xf) - vm.RegA();
	} else {
		valM = ((valM & 0xf) - vm.RegA()) & 0xf;
	}
	vm.RegY() = (vm.RegY() - 1) & 0xf;
	return true;
}

bool Operator_Sub_DEMPlus::Execute(VirtualMachine &vm) const
{
	// M+Ar->M (decimal adjustment), 1->exec
	unsigned char val;
	unsigned char valAdd = vm.RegA();
	for (unsigned char idx = vm.RegY(); ; idx = (idx - 1) & 0xf) {
		val = (vm.RegM(idx) & 0xf) + valAdd;
		if (val < 10) {
			vm.RegM(idx) = val;
			break;
		}
		vm.RegM(idx) = (val - 10) & 0xf;
		valAdd = 1;
	}
	vm.RegY() = (vm.RegY() - 1) & 0xf;
	return true;
}
