// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include "Version.h"
#include "VirtualMachineConsole.h"
#include "Parser.h"
#include "Generator.h"

static void PrintHelp(bool detailFlag);

int main(int argc, char *argv[])
{
	enum {
		MODE_Generator,
		MODE_Simulator,
		MODE_Beeper,
	} mode = MODE_Generator;
	int nCols = -1;
	bool upperCaseFlag = true;
	Generator::FileType fileTypeOut = Generator::FILETYPE_None;
	char *fileNameOut = "";
	for (int i = 1; i < argc; ) {
		char *arg = argv[i];
		if (arg[0] == '-' && arg[1] != '\0') {
			if (::strcmp(&arg[1], "help") == 0) {
				::PrintHelp(true);
			} else if (::strcmp(&arg[1], "lower") == 0) {
				upperCaseFlag = false;
			} else if (::strcmp(&arg[1], "asm") == 0) {
				fileTypeOut = Generator::FILETYPE_ASM;
			} else if (::strcmp(&arg[1], "hex") == 0) {
				if (nCols < 0) nCols = 16;
				fileTypeOut = Generator::FILETYPE_HEX;
			} else if (::strcmp(&arg[1], "dmp") == 0) {
				if (nCols < 0) nCols = 16;
				fileTypeOut = Generator::FILETYPE_DMP;
			} else if (::strcmp(&arg[1], "fxp") == 0) {
				fileTypeOut = Generator::FILETYPE_FXP;
			} else if (::strcmp(&arg[1], "htmldump") == 0) {
				if (nCols < 0) nCols = 4;
				fileTypeOut = Generator::FILETYPE_HTMLDUMP;
			} else if (arg[1] == 'o') {
				fileNameOut = &arg[2];
				if (::strlen(fileNameOut) == 0) {
					::fprintf(stderr, "specify output filename with -o option\n");
					::exit(1);
				}
			} else if (::strcmp(&arg[1], "sim") == 0) {
				mode = MODE_Simulator;
			} else if (::strcmp(&arg[1], "beep") == 0) {
				mode = MODE_Beeper;
			} else {
				::fprintf(stderr, "unknown option %s\n", arg);
				::exit(1);
			}
			argc--;
			for (int j = i; j < argc; j++) {
				argv[j] = argv[j + 1];
			}
		} else {
			i++;
		}
	}
	if (argc < 2) ::PrintHelp(true);
	if (nCols < 0) nCols = 16;
	VirtualMachineConsole vm;
	Parser::FileType fileTypeIn = Parser::GetFileType(argv[1]);
	std::auto_ptr<Parser> pParser;
	if (fileTypeIn == Parser::FILETYPE_ASM) {
		pParser.reset(new ParserAsm(vm));
	} else { // FILETYPE_HEX, FILETYPE_FXP, FILETYPE_None
		pParser.reset(new ParserHex(vm));
	}
	if (!((::strcmp(argv[1], "-") == 0)?
				pParser->ParseFile(stdin) : pParser->ParseFile(argv[1]))) {
		::fprintf(stderr, "%s\n", pParser->GetError());
		::exit(1);
	}
	vm.PutMemBuff(pParser->GetMemBuff());
	if (fileTypeOut == Generator::FILETYPE_None && ::strlen(fileNameOut) > 0) {
		fileTypeOut = Generator::GetFileType(fileNameOut);
	}
	if (fileTypeOut != Parser::FILETYPE_None || mode == MODE_Generator) {
		FILE *fp = stdout;
		if (::strlen(fileNameOut) > 0) {
			fp = ::fopen(fileNameOut, "wt");
			if (fp == NULL) {
				::fprintf(stderr, "can't open file %s\n", fileNameOut);
				::exit(1);
			}
		}
		Generator::OutputFile output(fp);
		Generator generator(vm, output);
		generator.UpperCase(upperCaseFlag);
		if (fileTypeOut == Generator::FILETYPE_ASM) {
			generator.GenerateAsm();
		} else if (fileTypeOut == Generator::FILETYPE_HEX) {
			generator.GenerateHex(false, nCols);
		} else if (fileTypeOut == Generator::FILETYPE_DMP) {
			generator.GenerateHex(true, nCols);
		} else if (fileTypeOut == Generator::FILETYPE_FXP) {
			generator.GenerateFxp();
		} else if (fileTypeOut == Generator::FILETYPE_HTMLDUMP) {
			bool addrBinFlag = true;
			generator.GenerateHtmlDump(addrBinFlag, nCols, true);
		} else if (mode == MODE_Generator) {
			generator.GenerateHex(false, nCols);
		} else {
			// nothing to do
		}
		if (fp != stdout) ::fclose(fp);
	}
	if (mode == MODE_Simulator) {
		vm.RunKeyHandler();
		vm.Reset();
		vm.SetNumLED(0xff);
		while (vm.RunSingle()) ;
	} else if (mode == MODE_Beeper) {
		vm.SetBeepTempo(vm.GetMem(0x00));
		vm.SetAddress(0x01);
		while (vm.BeepSingle());
	} else {
		// nothing to do
	}
	return 0;
}

void PrintHelp(bool detailFlag)
{
	::fprintf(stderr, "GMC-4 Assembler/Disassembler/Simulator v%s\n", VERSION);
	::fprintf(stderr, "usage: gmc4tool [options] <file>\n");
	if (detailFlag) {
		::fprintf(stderr,
			"a file in the following formats can be specified as input\n"
			" assembler, hex-format, dmp-format, fxp-format\n"
			"following options are available\n"
			" -lower    use lower characters for assembler and hexadecimal generation\n"
			" -asm      generate assembler code\n"
			" -hex      generate hex-format object\n"
			" -dmp      generate dmp-format object\n"
			" -fxp      generate fxp-format object\n"
			" -htmldump generate dump in HTML\n"
			" -o<file>  specify output filename\n"
			" -sim      start simulator on console\n"
			" -beep     start beeper\n");
	}
	::exit(1);
}
