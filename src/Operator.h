// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __OPERATOR_H__
#define __OPERATOR_H__

#include "Utils.h"

class VirtualMachine;
class Operator;

//-----------------------------------------------------------------------------
typedef std::vector<Operator *> OperatorTbl;

class OperatorOwner : public OperatorTbl {
public:
	~OperatorOwner();
	void Add(Operator *pOp);
	void Clear();
};

class OperatorMapByCode : public std::map<unsigned char, const Operator *> {
public:
	const Operator *Find(unsigned char code) const;
};

class OperatorMapBySymbol : public std::map<std::string, const Operator *> {
public:
	const Operator *Find(const std::string &symbol) const;
};

//-----------------------------------------------------------------------------
class Operator {
public:
	enum Type {
		TYPE_None,
		TYPE_NoOperand,
		TYPE_NibbleOperand,
		TYPE_JUMP,
		TYPE_CAL,
		TYPE_Subroutine,
		TYPE_Pseudo_ORG,
		TYPE_Pseudo_DN,
		TYPE_Pseudo_DS,
		TYPE_Pseudo_EQU,
	};
private:
	const char *_symbolUpperCase;
	char _symbolLowerCase[8];
	unsigned char _code;
	Type _type;
public:
	Operator(const char *symbol, unsigned char code, Type type);
	inline const char *GetSymbol(bool upperCaseFlag) const {
		return upperCaseFlag? _symbolUpperCase : _symbolLowerCase;
	}
	inline unsigned char GetCode() const { return _code; }
	inline Type GetType() const { return _type; }
	inline int GetNibbles() const {
		return (_type == TYPE_CAL)? 2 :
			(_type == TYPE_NoOperand)? 1 :
			(_type == TYPE_NibbleOperand)? 2 :
			(_type == TYPE_JUMP)? 3 : 0;
	}
	virtual unsigned char FetchOperand(
					const VirtualMachine &vm, unsigned char addr) const;
	virtual bool Execute(VirtualMachine &vm) const;
	static inline void Swap(unsigned char &x, unsigned char &y) {
		unsigned char tmp = x;
		x = y; y = tmp;
	}
};

class Operator_NoOperand : public Operator {
public:
	inline Operator_NoOperand(const char *symbol, unsigned char code) :
								Operator(symbol, code, TYPE_NoOperand) {}
};

class Operator_Sub : public Operator {
public:
	inline Operator_Sub(const char *symbol, unsigned char code) :
								Operator(symbol, code, TYPE_Subroutine) {}
};

class Operator_NibbleOperand : public Operator {
public:
	inline Operator_NibbleOperand(const char *symbol, unsigned char code) :
								Operator(symbol, code, TYPE_NibbleOperand) {}
	virtual unsigned char FetchOperand(
					const VirtualMachine &vm, unsigned char addr) const;
};

//-----------------------------------------------------------------------------
class Operator_KA : public Operator_NoOperand {
public:
	enum { OP_CODE = 0x0 };
public:
	inline Operator_KA() : Operator_NoOperand("KA", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_AO : public Operator_NoOperand {
public:
	enum { OP_CODE = 0x1 };
public:
	inline Operator_AO() : Operator_NoOperand("AO", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_CH : public Operator_NoOperand {
public:
	enum { OP_CODE = 0x2 };
public:
	inline Operator_CH() : Operator_NoOperand("CH", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_CY : public Operator_NoOperand {
public:
	enum { OP_CODE = 0x3 };
public:
	inline Operator_CY() : Operator_NoOperand("CY", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_AM : public Operator_NoOperand {
public:
	enum { OP_CODE = 0x4 };
public:
	inline Operator_AM() : Operator_NoOperand("AM", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_MA : public Operator_NoOperand {
public:
	enum { OP_CODE = 0x5 };
public:
	inline Operator_MA() : Operator_NoOperand("MA", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_MPlus : public Operator_NoOperand {
public:
	enum { OP_CODE = 0x6 };
public:
	inline Operator_MPlus() : Operator_NoOperand("M+", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_MMinus : public Operator_NoOperand {
public:
	enum { OP_CODE = 0x7 };
public:
	inline Operator_MMinus() : Operator_NoOperand("M-", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_TIA : public Operator_NibbleOperand {
public:
	enum { OP_CODE = 0x8 };
public:
	inline Operator_TIA() : Operator_NibbleOperand("TIA", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_AIA : public Operator_NibbleOperand {
public:
	enum { OP_CODE = 0x9 };
public:
	inline Operator_AIA() : Operator_NibbleOperand("AIA", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_TIY : public Operator_NibbleOperand {
public:
	enum { OP_CODE = 0xa };
public:
	inline Operator_TIY() : Operator_NibbleOperand("TIY", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_AIY : public Operator_NibbleOperand {
public:
	enum { OP_CODE = 0xb };
public:
	inline Operator_AIY() : Operator_NibbleOperand("AIY", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_CIA : public Operator_NibbleOperand {
public:
	enum { OP_CODE = 0xc };
public:
	inline Operator_CIA() : Operator_NibbleOperand("CIA", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_CIY : public Operator_NibbleOperand {
public:
	enum { OP_CODE = 0xd };
public:
	inline Operator_CIY() : Operator_NibbleOperand("CIY", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_CAL : public Operator {
public:
	enum { OP_CODE = 0xe };
public:
	inline Operator_CAL() : Operator("CAL", OP_CODE, TYPE_CAL) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_JUMP : public Operator {
public:
	enum { OP_CODE = 0xf };
public:
	inline Operator_JUMP() : Operator("JUMP", OP_CODE, TYPE_JUMP) {}
	virtual unsigned char FetchOperand(
					const VirtualMachine &vm, unsigned char addr) const;
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_RSTO : public Operator_Sub {
public:
	enum { OP_CODE = 0x0 };
public:
	inline Operator_Sub_RSTO() : Operator_Sub("RSTO", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_SETR : public Operator_Sub {
public:
	enum { OP_CODE = 0x1 };
public:
	inline Operator_Sub_SETR() : Operator_Sub("SETR", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_RSTR : public Operator_Sub {
public:
	enum { OP_CODE = 0x2 };
public:
	inline Operator_Sub_RSTR() : Operator_Sub("RSTR", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_INPT : public Operator_Sub {
public:
	enum { OP_CODE = 0x3 };
public:
	inline Operator_Sub_INPT() : Operator_Sub("INPT", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_CMPL : public Operator_Sub {
public:
	enum { OP_CODE = 0x4 };
public:
	inline Operator_Sub_CMPL() : Operator_Sub("CMPL", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_CHNG : public Operator_Sub {
public:
	enum { OP_CODE = 0x5 };
public:
	inline Operator_Sub_CHNG() : Operator_Sub("CHNG", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_SIFT : public Operator_Sub {
public:
	enum { OP_CODE = 0x6 };
public:
	inline Operator_Sub_SIFT() : Operator_Sub("SIFT", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_ENDS : public Operator_Sub {
public:
	enum { OP_CODE = 0x7 };
public:
	inline Operator_Sub_ENDS() : Operator_Sub("ENDS", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_ERRS : public Operator_Sub {
public:
	enum { OP_CODE = 0x8 };
public:
	inline Operator_Sub_ERRS() : Operator_Sub("ERRS", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_SHTS : public Operator_Sub {
public:
	enum { OP_CODE = 0x9 };
public:
	inline Operator_Sub_SHTS() : Operator_Sub("SHTS", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_LONS : public Operator_Sub {
public:
	enum { OP_CODE = 0xa };
public:
	inline Operator_Sub_LONS() : Operator_Sub("LONS", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_SUND : public Operator_Sub {
public:
	enum { OP_CODE = 0xb };
public:
	inline Operator_Sub_SUND() : Operator_Sub("SUND", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_TIMR : public Operator_Sub {
public:
	enum { OP_CODE = 0xc };
public:
	inline Operator_Sub_TIMR() : Operator_Sub("TIMR", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_DSPR : public Operator_Sub {
public:
	enum { OP_CODE = 0xd };
public:
	inline Operator_Sub_DSPR() : Operator_Sub("DSPR", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_DEMMinus : public Operator_Sub {
public:
	enum { OP_CODE = 0xe };
public:
	inline Operator_Sub_DEMMinus() : Operator_Sub("DEM-", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Sub_DEMPlus : public Operator_Sub {
public:
	enum { OP_CODE = 0xf };
public:
	inline Operator_Sub_DEMPlus() : Operator_Sub("DEM+", OP_CODE) {}
	virtual bool Execute(VirtualMachine &vm) const;
};

class Operator_Pseudo_ORG : public Operator {
public:
	enum { OP_CODE = 0x0 };
public:
	inline Operator_Pseudo_ORG() : Operator("ORG", OP_CODE, TYPE_Pseudo_ORG) {}
};

class Operator_Pseudo_DN : public Operator {
public:
	enum { OP_CODE = 0x0 };
public:
	inline Operator_Pseudo_DN() : Operator("DN", OP_CODE, TYPE_Pseudo_DN) {}
};

class Operator_Pseudo_DS : public Operator {
public:
	enum { OP_CODE = 0x0 };
public:
	inline Operator_Pseudo_DS() : Operator("DS", OP_CODE, TYPE_Pseudo_DS) {}
};

class Operator_Pseudo_EQU : public Operator {
public:
	enum { OP_CODE = 0x0 };
public:
	inline Operator_Pseudo_EQU() : Operator("EQU", OP_CODE, TYPE_Pseudo_EQU) {}
};

#endif
