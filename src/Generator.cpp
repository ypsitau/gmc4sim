// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include "stdafx.h"
#include "Generator.h"

//-----------------------------------------------------------------------------
// Generator
//-----------------------------------------------------------------------------
void Generator::GenerateAsm() const
{
	const char *formatNibbleOperand =
				_upperCaseFlag? "\t%s\t%X\n" : "\t%s\t%x\n";
	const char *formatJumpOperand =
				_upperCaseFlag? "\t%s\t%02XH\n" : "\t%s\t%02xh\n";
	const char *formatDN =
				_upperCaseFlag? "\tDN\t" : "\tdn\t";
	const char *formatDNItem =
				_upperCaseFlag? "%X" : "%x";
	const char *formatDS =
				_upperCaseFlag? "\tDS\t%02XH\n" : "\tds\t%02xh\n";
	typedef std::map<unsigned char, std::string> AddrMap;
	AddrMap addrMap;
	for (unsigned char addr = 0x00;
				_vm.IsValidMem(addr) && addr < VirtualMachine::ADDR_REGISTER; ) {
		const Operator *pOperator = _vm.GetOperatorMap().Find(_vm.GetMem(addr));
		if (pOperator == NULL) break;
		if (pOperator->GetType() == Operator::TYPE_JUMP) {
			unsigned char addrDst = pOperator->FetchOperand(_vm, addr);
			char str[32];
			::sprintf(str, _upperCaseFlag? "LBL_%02X" : "lbl_%02x", addrDst);
			addrMap[addrDst] = str;
		}
		addr += pOperator->GetNibbles();
	}
	int nibblesToSkip = 0;
	for (unsigned char addr = 0x00; addr < _vm.GetInvalidRegionAddress(); ) {
		if (!_vm.IsValidMem(addr)) {
			nibblesToSkip++;
			addr++;
			continue;
		}
		if (nibblesToSkip > 0) {
			_output.Printf(formatDS, nibblesToSkip);
			nibblesToSkip = 0;
		}
		const Operator *pOperator = _vm.GetOperatorMap().Find(_vm.GetMem(addr));
		if (pOperator == NULL) {
			_output.Printf(formatDN);
			_output.Printf(formatDNItem, _vm.GetMem(addr));
			_output.Printf("\n");
			addr++;
			continue;
		}
		if (addr + pOperator->GetNibbles() >= VirtualMachine::ADDR_REGISTER) {
			_output.Printf(formatDN);
			for (int i = 0; i < VirtualMachine::ADDR_REGISTER - addr; i++) {
				if (i > 0) _output.Printf(", ");
				_output.Printf(formatDNItem, _vm.GetMem(addr + i));
			}
			_output.Printf("\n");
			break;
		}
		AddrMap::iterator iterAddrMap = addrMap.find(addr);
		if (iterAddrMap != addrMap.end()) {
			_output.Printf("%s:\n", iterAddrMap->second.c_str());
		}
		if (pOperator->GetType() == Operator::TYPE_NoOperand) {
			_output.Printf("\t%s\n", pOperator->GetSymbol(_upperCaseFlag));
		} else if (pOperator->GetType() == Operator::TYPE_NibbleOperand) {
			_output.Printf(formatNibbleOperand, pOperator->GetSymbol(_upperCaseFlag),
								pOperator->FetchOperand(_vm, addr));
		} else if (pOperator->GetType() == Operator::TYPE_JUMP) {
			unsigned char addrDst = pOperator->FetchOperand(_vm, addr);
			if (_vm.IsValidMem(addrDst) && addrDst < VirtualMachine::ADDR_REGISTER) {
				_output.Printf("\t%s\t%s\n",
						pOperator->GetSymbol(_upperCaseFlag), addrMap[addrDst].c_str());
			} else {
				_output.Printf(formatJumpOperand,
						pOperator->GetSymbol(_upperCaseFlag), addrDst);
			}
		} else if (pOperator->GetType() == Operator::TYPE_CAL) {
			unsigned char codeSub = _vm.GetMem(addr + 1);
			const Operator *pOperatorSub = _vm.GetOperatorSubMap().Find(codeSub);
			if (pOperatorSub == NULL) break;
			_output.Printf("\t%s\t%s\n",
						pOperator->GetSymbol(_upperCaseFlag),
						pOperatorSub->GetSymbol(_upperCaseFlag));
		} else { // Operator::TYPE_None
			// nothing to do
		}
		addr += pOperator->GetNibbles();
	}
}

void Generator::GenerateHex(bool addrFlag, int nCols) const
{
	int col = 0;
	bool skipInvalidFlag = addrFlag;
	const char *formatAddr = _upperCaseFlag? "%02X:" : "%02x:";
	const char *formatData = _upperCaseFlag? "%X" : "%x";
	const char *formatBlank = _upperCaseFlag? "f" : "F";
	for (unsigned char addr = 0x00; addr < _vm.GetInvalidRegionAddress(); addr++) {
		if (skipInvalidFlag && !_vm.IsValidMem(addr)) {
			if (col > 0) _output.Printf("\n");
			col = 0;
			continue;
		}
		if (col > 0) {
			_output.Printf(" ");
		} else if (addrFlag) {
			_output.Printf(formatAddr, addr);
		} else {
			// nothing to do
		}
		if (_vm.IsValidMem(addr)) {
			_output.Printf(formatData, _vm.GetMem(addr));
		} else {
			_output.Printf(formatBlank);
		}
		col++;
		if (col == nCols) {
			col = 0;
			_output.Printf("\n");
		}
	}
}

void Generator::GenerateFxp() const
{
	_output.Printf("<?xml version=\"1.0\"?>\n");
	_output.Printf("<ArrayOfString xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
			" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\n");
	for (unsigned char addr = 0x00; addr < VirtualMachine::ADDR_REGISTER; addr++) {
		if (_vm.IsValidMem(addr)) {
			_output.Printf("  <string>%x</string>\n", _vm.GetMem(addr));
		} else {
			_output.Printf("  <string>F</string>\n");
		}
	}
	_output.Printf("</ArrayOfString>\n");
}

void Generator::GenerateHtmlDump(bool addrBinFlag, int nCols, bool frameFlag) const
{
	int iCol = 0, iRow = 0;
	int widthAddr = addrBinFlag? 30 : 50;
	const int widthAddrBin = 120, widthData = 30;
	unsigned char addrEnd = _vm.GetInvalidRegionAddress();
	if (addrEnd == 0x00) return;
	if (frameFlag) {
		_output.Printf(
			"<html>\n"
			"<body>\n"
			"\n");
	}
	_output.Printf(
		"<table cellspacing=\"0\" style=\"border-collapse: collapse\">\n"
		"<tr bgcolor=\"#ccddcc\">\n");
	if (addrBinFlag) {
		_output.Printf(
			"<td colspan=\"2\" align=\"center\">"
			"<font color=\"#008800\" size=\"-1\">"
			"<strong>Address</strong>"
			"</font>"
			"</td>\n");
	} else {
		_output.Printf(
			"<td align=\"center\">"
			"<font color=\"#008800\" size=\"-1\">"
			"<strong>Addr</strong>"
			"</font>"
			"</td>\n");
	}
	for (int iCol = 0; iCol < nCols; iCol++) {
		_output.Printf(
			"<td align=\"center\">"
			"<font color=\"#008800\" size=\"-1\">"
			"<strong>+%X</strong>"
			"</font>"
			"</td>\n", iCol);
	}
	_output.Printf(
		"</font>\n"
		"</tr>\n"
		"<font color=\"#000000\">\n");
	for (unsigned char addr = 0x00; addr < addrEnd; ) {
		if (iCol == 0) {
			unsigned char addrFwd = addr;
			for ( ; addrFwd < addrEnd && !_vm.IsValidMem(addrFwd); addrFwd++) ;
			int nRowsBlank = static_cast<int>(addrFwd - addr) / nCols;
			if (nRowsBlank == 0) {
				_output.Printf(
					"<tr bgcolor=\"%s\">\n"
					"<td width=\"%d\" align=\"center\">%02X</td>\n",
					(iRow % 2 == 0)? "#fcfcfc" : "#f4f4f4", widthAddr, addr);
				if (addrBinFlag) {
					std::string addrBin;
					for (int i = 0; i < 7; i++) {
						if (i > 0) addrBin += " ";
						addrBin += (addr & (1 << (6 - i)))? "*" : "_";
					}
					_output.Printf(
						"<td width=\"%d\" align=\"center\">\n"
						"<font color=\"#cc0000\" size=\"-1\"><strong><tt>%s</tt></strong></font>\n"
						"</td>\n",
						widthAddrBin, addrBin.c_str());
				}
			} else {
				_output.Printf(
					"<tr><td><br /></td></tr>\n");
				addr += nCols * nRowsBlank;
				continue;
			}
		}
		if (_vm.IsValidMem(addr)) {
			_output.Printf(
				"<td width=\"%d\"align=\"center\">%X</td>\n",
				widthData, _vm.GetMem(addr));
		} else {
			_output.Printf(
				"<td width=\"%d\"align=\"center\"><br /></td>\n", widthData);
		}
		iCol++;
		if (iCol == nCols) {
			_output.Printf(
				"</tr>\n");
			iCol = 0, iRow++;
		}
		addr++;
	}
	if (iCol > 0) {
		for ( ; iCol < nCols; iCol++) {
			_output.Printf(
				"<td width=\"%d\"align=\"center\"><br /></td>\n", widthData);
		}
		_output.Printf(
			"</tr>\n");
	}
	_output.Printf(
		"</font>\n"
		"</table>\n");
	if (frameFlag) {
		_output.Printf(
			"\n"
			"</body>\n"
			"</html>\n");
	}
}

Generator::FileType Generator::GetFileType(const char *fileName)
{
	const char *ext = ::strrchr(fileName, '.');
	ext = (ext == NULL)? "" : ext + 1;
	return
		(::stricmp(ext, "asm") == 0)? FILETYPE_ASM :
		(::stricmp(ext, "gmc") == 0)? FILETYPE_ASM :
		(::stricmp(ext, "hex") == 0)? FILETYPE_HEX :
		(::stricmp(ext, "fxp") == 0)? FILETYPE_FXP :
		(::stricmp(ext, "html") == 0)? FILETYPE_HTMLDUMP :
		FILETYPE_None;
}

//-----------------------------------------------------------------------------
// Generator::OutputFile
//-----------------------------------------------------------------------------
void Generator::OutputFile::Printf(const char *format, ...)
{
	va_list list;
	va_start(list, format);
	::vfprintf(_fp, format, list);
	va_end(list);
}
