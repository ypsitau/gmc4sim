// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <stdio.h>
#include "VirtualMachine.h"

class Generator {
public:
	enum FileType {
		FILETYPE_None,
		FILETYPE_ASM,
		FILETYPE_HEX,
		FILETYPE_DMP,
		FILETYPE_FXP,
		FILETYPE_HTMLDUMP,
	};
public:
	class Output {
	public:
		virtual void Printf(const char *format, ...) = 0;
	};
	class OutputFile : public Output {
	private:
		FILE *_fp;
	public:
		inline OutputFile(FILE *fp) : _fp(fp) {}
		virtual void Printf(const char *format, ...);
	};
private:
	const VirtualMachine &_vm;
	Output &_output;
	bool _upperCaseFlag;
public:
	inline Generator(const VirtualMachine &vm, Output &output) :
					_vm(vm), _output(output), _upperCaseFlag(true) {}
	inline Generator &UpperCase(bool upperCaseFlag) {
		_upperCaseFlag = upperCaseFlag;
		return *this;
	}
	void GenerateAsm() const;
	void GenerateHex(bool addrFlag, int nCols) const;
	void GenerateFxp() const;
	void GenerateHtmlDump(bool addrBinFlag, int nCols, bool frameFlag) const;
	static FileType GetFileType(const char *fileName);
};

#endif
