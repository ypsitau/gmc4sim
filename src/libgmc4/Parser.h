// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __PARSER_H__
#define __PARSER_H__

#include "VirtualMachine.h"
#include "MmlParser.h"

class Parser {
public:
	enum FileType {
		FILETYPE_None,
		FILETYPE_ASM,
		FILETYPE_HEX,
		FILETYPE_FXP,
	};
protected:
	char _error[128];
	unsigned char _addr;
	unsigned char _memBuff[VirtualMachine::ADDR_MAX];
	AddressByLineTbl _addrByLineTbl;
public:
	Parser();
	bool ParseFile(FILE *fp);
	bool ParseFile(const char *fileName);
	virtual bool FeedChar(int ch) = 0;
	inline static bool IsWhite(int ch) { return ch == ' ' || ch == '\t'; }
	inline static bool IsHexChar(int ch) {
		return isdigit(ch) ||
				('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F');
	}
	inline static bool IsTokenChar(int ch) {
		return isalnum(ch) || ch == '$' || ch == '_' || ch == '+' || ch == '-';
	}
	inline static bool IsCommentChar(int ch) { return ch == ';' || ch == '#'; }
	inline void PutMem(unsigned char addr, unsigned char value) {
		_memBuff[addr] = value;
	}
	inline void SetAddress(unsigned char addr) { _addr = addr; }
	inline unsigned char GetAddress() const { return _addr; }
	inline void IncrAddress(int n = 1) { _addr += n; }
	inline const AddressByLineTbl &GetAddressByLineTbl() { return _addrByLineTbl; }
	void SetError(const char *format, ...);
	const char *GetError() const { return _error; }
	void InvalidateMemBuff();
	const unsigned char *GetMemBuff() const { return _memBuff; }
	static FileType GetFileType(const char *fileName);
};

class ParserAsm : public Parser, public MmlParser::Handler {
private:
	enum Stat {
		STAT_Error,
		STAT_Begin,
		STAT_Idle,
		STAT_SkipToEndOfLine,
		STAT_SeekEndOfLine,
		STAT_Token,
		STAT_CAL_Pre, STAT_CAL,
		STAT_Operand_Pre, STAT_Operand,
		STAT_OperandList_Pre, STAT_OperandList, STAT_OperandList_Next,
		STAT_MML,
	};
	struct SymbolTarget {
		const Operator *pOperator;
		unsigned char address;
	};
	struct Symbol {
		bool validFlag;
		unsigned char value;
		std::vector<SymbolTarget> symbolTargetTbl;
	};
	class Resolver : public std::map<std::string, Symbol *> {
	public:
		Symbol *Provide(const std::string &label, unsigned char value, Parser &parser);
		Symbol *Require(const std::string &label);
		bool Resolve(Parser &parser);
		void Clear();
	};
private:
	struct {
		int row, col;
	} _point;
	Stat _stat;
	std::string _field;
	Resolver _resolver;
	std::auto_ptr<MmlParser> _pMmlParser;
	const Operator *_pOperator;
	Symbol *_pSymbolLastDefined;
	OperatorMapBySymbol _operatorMap;
	OperatorMapBySymbol _operatorSubMap;
	Operator_Pseudo_ORG operator_Pseudo_ORG;
	Operator_Pseudo_DN operator_Pseudo_DN;
	Operator_Pseudo_DS operator_Pseudo_DS;
	Operator_Pseudo_EQU operator_Pseudo_EQU;
public:
	ParserAsm(const VirtualMachine &vm);
	virtual bool FeedChar(int ch);
public:
	// virtual functions of MmlParser::Handler
	virtual void MmlNote(MmlParser &parser, unsigned char note, int length);
	virtual void MmlRest(MmlParser &parser, int length);
private:
	unsigned char ToValue(const Operator *pOperator, const std::string &field);
	void PutBeepData(MmlParser &parser, unsigned char noteGMC, int length);
};

class ParserHex : public Parser {
private:
	enum Stat {
		STAT_Error,
		STAT_LineTop,
		STAT_Address,
		STAT_AddressColon,
		STAT_Normal,
		STAT_SkipToEndOfLine,
		STAT_SkipToGT,
	};
private:
	inline static int ToHex(int ch) {
		return static_cast<unsigned char>
				(('0' <= ch && ch <= '9')? ch - '0' :
				 ('a' <= ch && ch <= 'f')? ch - 'a' + 10 :
				 ('A' <= ch && ch <= 'F')? ch - 'A' + 10 : -1);
	}
private:
	struct {
		int row, col;
	} _point;
	unsigned char _addrCandidate;
	Stat _stat;
public:
	ParserHex(const VirtualMachine &vm);
	virtual bool FeedChar(int ch);
};

#endif
