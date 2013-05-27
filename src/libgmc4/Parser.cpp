// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include "Parser.h"
#include <stdarg.h>

//-----------------------------------------------------------------------------
// Parser
//-----------------------------------------------------------------------------
Parser::Parser() : _addr(0x00)
{
	_error[0] = '\0';
	InvalidateMemBuff();
}

bool Parser::ParseFile(FILE *fp)
{
	int ch;
	bool rtn = true;
	do {
		ch = ::fgetc(fp);
		if (!FeedChar(ch)) {
			rtn = false;
			break;
		}
	} while (ch >= 0);
	return rtn;
}

bool Parser::ParseFile(const char *fileName)
{
	FILE *fp = ::fopen(fileName, "rt");
	if (fp == NULL) {
		SetError("can't open %s", fileName);
		return false;
	}
	bool rtn = ParseFile(fp);
	::fclose(fp);
	return rtn;
}

void Parser::SetError(const char *format, ...)
{
	va_list list;
	va_start(list, format);
	::vsprintf(_error, format, list);
	va_end(list);
}

void Parser::InvalidateMemBuff()
{
	::memset(_memBuff, 0xff, VirtualMachine::ADDR_REGISTER);
}

Parser::FileType Parser::GetFileType(const char *fileName)
{
	const char *ext = ::strrchr(fileName, '.');
	ext = (ext == NULL)? "" : ext + 1;
	return
		(::stricmp(ext, "asm") == 0)? FILETYPE_ASM :
		(::stricmp(ext, "gmc") == 0)? FILETYPE_ASM :
		(::stricmp(ext, "hex") == 0)? FILETYPE_HEX :
		(::stricmp(ext, "fxp") == 0)? FILETYPE_FXP :
		FILETYPE_None;
}

//-----------------------------------------------------------------------------
// ParserAsm
//-----------------------------------------------------------------------------
ParserAsm::ParserAsm(const VirtualMachine &vm) :
		_stat(STAT_Begin), _mmlParser(*this), _pOperator(NULL), _pSymbolLastDefined(NULL)
{
	_point.row = _point.col = 1;
	_addrByLineTbl.clear();
	_addrByLineTbl.push_back(GetAddress());
	foreach_const (OperatorOwner, ppOp, vm.GetOperatorOwner()) {
		Operator *pOp = *ppOp;
		_operatorMap[pOp->GetSymbol(true)] = pOp;
	}
	foreach_const (OperatorOwner, ppOp, vm.GetOperatorSubOwner()) {
		Operator *pOp = *ppOp;
		_operatorSubMap[pOp->GetSymbol(true)] = pOp;
	}
}

bool ParserAsm::FeedChar(int ch)
{
	bool rtn = true;
	bool continueFlag = (GetAddress() < VirtualMachine::ADDR_REGISTER);
	while (continueFlag) {
		continueFlag = false;
		if (_stat == STAT_Error) {
			// nothing to do
		} else if (_stat == STAT_Begin) {
			if (ch < 0) {
				// nothing to do
			} else if (isspace(ch)) {
				// nothing to do
			} else if (IsCommentChar(ch)) {
				_stat = STAT_SkipToEndOfLine;
			} else {
				_field.clear();
				continueFlag = true;
				_stat = STAT_Token;
			}
		} else if (_stat == STAT_Idle) {
			// nothing to do
		} else if (_stat == STAT_SeekEndOfLine) {
			if (ch < 0 || ch == '\n') {
				_stat = STAT_Begin;
			} else if (IsWhite(ch)) {
				// nothing to do
			} else if (IsCommentChar(ch)) {
				_stat = STAT_SkipToEndOfLine;
			} else {
				SetError("%d: unexpected characters appear", _point.row);
				rtn = false;
			}
		} else if (_stat == STAT_SkipToEndOfLine) {
			if (ch < 0 || ch == '\n') {
				_stat = STAT_Begin;
			}
		} else if (_stat == STAT_Token) {
			if (ch == ':') {
				_pSymbolLastDefined =
							_resolver.Provide(_field, GetAddress(), *this);
				_stat = STAT_Begin;
			} else if (IsTokenChar(ch)) {
				_field.push_back(ch);
			} else if (ch < 0 || isspace(ch) || IsCommentChar(ch)) {
				std::string str;
				foreach (std::string, p, _field) {
					str.push_back(toupper(*p));
				}
				if (str == "ORG") {
					_pOperator = &operator_Pseudo_ORG;
					continueFlag = true;
					_stat = STAT_Operand_Pre;
				} else if (str == "DN") {
					_pOperator = &operator_Pseudo_DN;
					continueFlag = true;
					_stat = STAT_OperandList_Pre;
				} else if (str == "DS") {
					_pOperator = &operator_Pseudo_DS;
					continueFlag = true;
					_stat = STAT_Operand_Pre;
				} else if (str == "EQU") {
					_pOperator = &operator_Pseudo_EQU;
					continueFlag = true;
					_stat = STAT_Operand_Pre;
				} else if (str == "MML") {
					_pOperator = NULL;
					_stat = STAT_MML;
				} else if (str == "END") {
					_stat = STAT_Idle;
				} else {
					_pOperator = _operatorMap.Find(str);
					if (_pOperator == NULL) {
						SetError("%d: unknown operator %s", _point.row, str.c_str());
						rtn = false;
					} else if (_pOperator->GetType() == Operator::TYPE_NoOperand) {
						PutMem(GetAddress(), _pOperator->GetCode());
						IncrAddress(_pOperator->GetNibbles());
						continueFlag = true;
						_stat = STAT_SeekEndOfLine;
					} else if (_pOperator->GetType() == Operator::TYPE_NibbleOperand) {
						continueFlag = true;
						_stat = STAT_Operand_Pre;
					} else if (_pOperator->GetType() == Operator::TYPE_JUMP) {
						continueFlag = true;
						_stat = STAT_Operand_Pre;
					} else if (_pOperator->GetType() == Operator::TYPE_CAL) {
						continueFlag = true;
						_stat = STAT_CAL_Pre;
					} else { // Operator::TYPE_CAL or Operator::TYPE_None
						SetError("%d: invalid operator", _point.row, str.c_str());
						_stat = STAT_Error;
						rtn = false;
					}
				}
			} else {
				SetError("%d: invalid character", _point.row);
				rtn = false;
			}
		} else if (_stat == STAT_CAL_Pre) {
			if (ch < 0 || ch == '\n' || IsCommentChar(ch)) {
				SetError("%d: operator CAL requires a function name", _point.row);
				rtn = false;
			} else if (IsWhite(ch)) {
				// nothing to do
			} else if (isalpha(ch)) {
				_field.clear();
				continueFlag = true;
				_stat = STAT_CAL;
			} else {
				SetError("%d: invalid character for a function name", _point.row);
				rtn = false;
			}
		} else if (_stat == STAT_CAL) {
			if (ch < 0 || isspace(ch) || IsCommentChar(ch)) {
				const Operator *pOperatorSub = _operatorSubMap.Find(_field);
				if (pOperatorSub == NULL) {
					SetError("%d: unknown CAL function %s", _point.row, _field.c_str());
					rtn = false;
				} else {
					PutMem(GetAddress(), _pOperator->GetCode());
					PutMem(GetAddress() + 1, pOperatorSub->GetCode());
					IncrAddress(_pOperator->GetNibbles());
				}
				continueFlag = true;
				_stat = (IsCommentChar(ch))? STAT_SkipToEndOfLine : STAT_SeekEndOfLine;
			} else if (IsTokenChar(ch)) {
				if (islower(ch)) ch = toupper(ch);
				_field.push_back(ch);
			} else {
				SetError("%d: invalid character for CAL sub-function name", _point.row);
				rtn = false;
			}
		} else if (_stat == STAT_Operand_Pre) {
			if (ch < 0 || ch == '\n' || IsCommentChar(ch)) {
				SetError("%d: operand missing", _point.row);
				rtn = false;
			} else if (IsWhite(ch)) {
				// nothing to do
			} else if (IsTokenChar(ch)) {
				_field.clear();
				continueFlag = true;
				_stat = STAT_Operand;
			} else {
				SetError("%d: invalid character for an operand", _point.row);
				rtn = false;
			}
		} else if (_stat == STAT_Operand) {
			if (ch < 0 || isspace(ch) || IsCommentChar(ch)) {
				unsigned char value = ToValue(_pOperator, _field);
				if (_pOperator->GetType() == Operator::TYPE_Pseudo_ORG) {
					SetAddress(value);
					*_addrByLineTbl.rbegin() = GetAddress();
				} else if (_pOperator->GetType() == Operator::TYPE_Pseudo_DS) {
					IncrAddress(value);
				} else if (_pOperator->GetType() == Operator::TYPE_Pseudo_EQU) {
					if (_pSymbolLastDefined == NULL) {
						SetError("%d: no symbol to define equ value", _point.row);
						rtn = false;
						break;
					} else {
						_pSymbolLastDefined->validFlag = true;
						_pSymbolLastDefined->value = value;
					}
				} else if (_pOperator->GetType() == Operator::TYPE_NibbleOperand) {
					PutMem(GetAddress(), _pOperator->GetCode());
					PutMem(GetAddress() + 1, value & 0xf);
					IncrAddress(_pOperator->GetNibbles());
				} else if (_pOperator->GetType() == Operator::TYPE_JUMP) {
					PutMem(GetAddress(), _pOperator->GetCode());
					PutMem(GetAddress() + 1, value >> 4);
					PutMem(GetAddress() + 2, value & 0xf);
					IncrAddress(_pOperator->GetNibbles());
				} else {
					// nothing to do
				}
				continueFlag = true;
				_stat = STAT_SeekEndOfLine;
			} else {
				_field.push_back(ch);
			}
		} else if (_stat == STAT_OperandList_Pre) {
			if (ch < 0) {
				SetError("%d: operand missing", _point.row);
				rtn = false;
			} else if (IsWhite(ch)) {
				// nothing to do
			} else if (IsTokenChar(ch)) {
				_field.clear();
				continueFlag = true;
				_stat = STAT_OperandList;
			} else {
				SetError("%d: invalid character for an operand", _point.row);
				rtn = false;
			}
		} else if (_stat == STAT_OperandList) {
			if (ch < 0 || isspace(ch) || IsCommentChar(ch) || ch == ',') {
				unsigned char value = ToValue(_pOperator, _field);
				if (_pOperator->GetType() == Operator::TYPE_Pseudo_DN) {
					PutMem(GetAddress(), value & 0xf);
					IncrAddress();
				} else {
					// nothing to do
				}
				continueFlag = true;
				_stat = STAT_OperandList_Next;
			} else {
				_field.push_back(ch);
			}
		} else if (_stat == STAT_OperandList_Next) {
			if (ch < 0 || ch == '\n' || IsCommentChar(ch)) {
				continueFlag = true;
				_stat = STAT_SeekEndOfLine;
			} else if (IsWhite(ch)) {
				// nothing to do
			} else if (IsTokenChar(ch)) {
				SetError("%d: comma is necessary between list values", _point.row);
				rtn = false;
			} else if (ch == ',') {
				_stat = STAT_OperandList_Pre;
			} else {
				SetError("%d: invalid character for an operand", _point.row);
				rtn = false;
			}
		} else if (_stat == STAT_MML) {
			if (ch < 0 || ch == '\n' || IsCommentChar(ch)) {
				rtn = _mmlParser.FeedChar('\0');
				continueFlag = true;
				_stat = STAT_SeekEndOfLine;
			} else {
				rtn = _mmlParser.FeedChar(ch);
			}
			if (!rtn) {
				SetError("%d: error in MML data", _point.row);
			}
		}
	}
	if (GetAddress() > VirtualMachine::ADDR_REGISTER) {
		SetError("address exceeds limit");
		rtn = false;
	} else if (ch < 0) {
		if (rtn) rtn = _resolver.Resolve(*this);
	} else if (ch == '\n') {
		_addrByLineTbl.push_back(GetAddress());
		_point.col = 1, _point.row++;
	} else {
		_point.col++;
	}
	if (!rtn) _stat = STAT_Error;
	return rtn;
}

unsigned char ParserAsm::ToValue(const Operator *pOperator, const std::string &field)
{
	unsigned char value = 0x00;
	if (field.size() == 1 && IsHexChar(field[0])) {
		value = static_cast<unsigned char>(
					::strtoul(field.c_str(), NULL, 16));
	} else if (field.size() == 2 && IsHexChar(field[0]) &&
												toupper(field[1]) == 'H') {
		value = static_cast<unsigned char>(::strtoul(field.c_str(), NULL, 16));
	} else if (field.size() == 3 && IsHexChar(field[0]) && IsHexChar(field[1]) &&
												toupper(field[2]) == 'H') {
		value = static_cast<unsigned char>(::strtoul(field.c_str(), NULL, 16));
	} else if (field == "$") {
		value = GetAddress();
	} else if (isdigit(field[0])) {
		value = static_cast<unsigned char>(::strtoul(field.c_str(), NULL, 0));
	} else {
		Symbol *pSymbol = _resolver.Require(field);
		if (pSymbol->validFlag) {
			value = pSymbol->value;
		} else {
			SymbolTarget symbolTarget;
			symbolTarget.pOperator = pOperator;
			symbolTarget.address = GetAddress();
			pSymbol->symbolTargetTbl.push_back(symbolTarget);
		}
	}
	return value;
}

void ParserAsm::MmlNote(MmlParser &parser, unsigned char note, int length)
{
	static const unsigned char noteTbl[] = {
		0, 45, 47, 48, 50, 52, 53, 55,
		57, 59, 60, 62, 64, 65, 67, 0,
	};
	unsigned char noteGMC = 0x0;
	for (unsigned char idx = 0; idx < NUMBEROF(noteTbl); idx++) {
		if (noteTbl[idx] == note) {
			noteGMC = idx;
			break;
		}
	}
	PutBeepData(parser, noteGMC, length);
}

void ParserAsm::MmlRest(MmlParser &parser, int length)
{
	PutBeepData(parser, 0x0, length);
}

void ParserAsm::PutBeepData(MmlParser &parser, unsigned char noteGMC, int length)
{
	if (GetAddress() == 0x00) {
		int n = (VirtualMachine::TEMPO_MAX - parser.GetTempo()) * 15 /
					(VirtualMachine::TEMPO_MAX - VirtualMachine::TEMPO_MIN);
		n = (n < 0)? 0 : (n > 15)? 15 : n;
		PutMem(GetAddress(), static_cast<unsigned char>(n));
		IncrAddress();
	}
	PutMem(GetAddress(), noteGMC);
	length = length * 16 / MmlParser::LENGTH_MAX;
	if (length > 0) length--;
	PutMem(GetAddress() + 1, static_cast<unsigned char>(length));
	IncrAddress(2);
}

//-----------------------------------------------------------------------------
// ParserAsm::Resolver
//-----------------------------------------------------------------------------
ParserAsm::Symbol *ParserAsm::Resolver::Provide(const std::string &label,
										unsigned char value, Parser &parser)
{
	Symbol *pSymbol;
	iterator iter = find(label);
	if (iter == end()) {
		pSymbol = new Symbol();
		insert(std::pair<std::string, Symbol *>(label, pSymbol));
	} else {
		pSymbol = iter->second;
	}
	pSymbol->validFlag = true;
	pSymbol->value = value;
	return pSymbol;
}

ParserAsm::Symbol *ParserAsm::Resolver::Require(const std::string &label)
{
	Symbol *pSymbol;
	iterator iter = find(label);
	if (iter == end()) {
		pSymbol = new Symbol();
		pSymbol->validFlag = false;
		pSymbol->value = 0;
		insert(std::pair<std::string, Symbol *>(label, pSymbol));
	} else {
		pSymbol = iter->second;
	}
	return pSymbol;
}

bool ParserAsm::Resolver::Resolve(Parser &parser)
{
	bool rtn = true;
	std::string labelsErr;
	foreach (Resolver, iter, *this) {
		Symbol *pSymbol = iter->second;
		if (pSymbol->validFlag) {
			foreach (std::vector<SymbolTarget>, pSymbolTarget,
											pSymbol->symbolTargetTbl) {
				unsigned char address = pSymbolTarget->address;
				Operator::Type type = pSymbolTarget->pOperator->GetType();
				if (type == Operator::TYPE_NibbleOperand) {
					parser.PutMem(address + 1, pSymbol->value & 0xf);
				} else if (type == Operator::TYPE_JUMP) {
					parser.PutMem(address + 1, pSymbol->value >> 4);
					parser.PutMem(address + 2, pSymbol->value & 0xf);
				} else if (type == Operator::TYPE_Pseudo_DN) {
					parser.PutMem(address, pSymbol->value & 0xf);
				} else {
					// nothing to do
				}
			}
		} else {
			if (!labelsErr.empty()) labelsErr.append(", ");
			labelsErr.append(iter->first.c_str());
			rtn = false;
		}
	}
	if (!rtn) {
		parser.SetError("undefined label %s", labelsErr.c_str());
	}
	return rtn;
}

void ParserAsm::Resolver::Clear()
{
	foreach (Resolver, iter, *this) {
		Symbol *pSymbol = iter->second;
		delete pSymbol;
	}
	clear();
}

//-----------------------------------------------------------------------------
// ParserHex
//-----------------------------------------------------------------------------
ParserHex::ParserHex(const VirtualMachine &vm) : _stat(STAT_LineTop)
{
	_point.row = _point.col = 1;
}

bool ParserHex::FeedChar(int ch)
{
	bool rtn = true;
	bool eatNextFlag;
	do {
		eatNextFlag = true;
		if (_stat == STAT_Error) {
			// nothing to do
		} else if (_stat == STAT_LineTop) {
			if (IsHexChar(ch)) {
				_addrCandidate = ToHex(ch);
				_stat = STAT_Address;
			} else {
				eatNextFlag = false;
				_stat = STAT_Normal;
			}
		} else if (_stat == STAT_Address) {
			if (IsHexChar(ch)) {
				_addrCandidate = (_addrCandidate << 4) | ToHex(ch);
				_stat = STAT_AddressColon;
			} else {
				PutMem(GetAddress(), _addrCandidate & 0x0f);
				IncrAddress();
				eatNextFlag = false;
				_stat = STAT_Normal;
			}
		} else if (_stat == STAT_AddressColon) {
			if (ch == ':') {
				SetAddress(_addrCandidate);
				_stat = STAT_Normal;
			} else {
				PutMem(GetAddress(), _addrCandidate >> 4);
				PutMem(GetAddress() + 1, _addrCandidate & 0x0f);
				IncrAddress(2);
				eatNextFlag = false;
				_stat = STAT_Normal;
			}
		} else if (_stat == STAT_Normal) {
			if (ch < 0 || ch == '\n') {
				_stat = STAT_LineTop;
			} else if (isspace(ch)) {
				// nothing to do
			} else if (IsCommentChar(ch)) {
				_stat = STAT_SkipToEndOfLine;
			} else if (ch == '<') {
				_stat = STAT_SkipToGT;
			} else if (IsHexChar(ch)) {
				unsigned char value = ToHex(ch);
				PutMem(GetAddress(), value);
				IncrAddress();
			} else {
				SetError("%d:%d invalid character %c",
						_point.row, _point.col, isprint(ch)? ch : '.');
				rtn = false;
			}
		} else if (_stat == STAT_SkipToEndOfLine) {
			if (ch < 0 || ch == '\n') {
				_stat = STAT_LineTop;
			}
		} else if (_stat == STAT_SkipToGT) {
			if (ch < 0 || ch == '\n') {
				_stat = STAT_LineTop;
			} else if (ch == '>') {
				_stat = STAT_Normal;
			} else {
				// nothing to do
			}
		}
	} while (!eatNextFlag);
	if (ch == '\n') {
		_point.col = 1, _point.row++;
	} else {
		_point.col++;
	}
	if (!rtn) _stat = STAT_Error;
	return rtn;
}
