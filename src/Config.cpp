// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "Config.h"

Config::Config() : wxFileConfig(wxEmptyString, wxEmptyString,
				MakeIniFileName(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE)
{
}

void Config::Prepare()
{
	Config *pCfg = new Config();
	wxConfig::Set(pCfg);
	pCfg->ReadBurst();
	pCfg->WriteBurst();
}

void Config::ReadBurst()
{
	do {
		Read(CFG_Frame_X, &frame.x, -1);
		Read(CFG_Frame_Y, &frame.y, -1);
		Read(CFG_Frame_Width, &frame.width, 680);
		Read(CFG_Frame_Height, &frame.height, 560);
	} while (0);
	do {
		Read(CFG_BoardDialog_X, &boardDialog.x, -1);
		Read(CFG_BoardDialog_Y, &boardDialog.y, -1);
		Read(CFG_BoardDialog_Shown, &boardDialog.shown, false);
	} while (0);
	do {
		Read(CFG_DumpPresenter_X, &dumpPresenter.x, -1);
		Read(CFG_DumpPresenter_Y, &dumpPresenter.y, -1);
		Read(CFG_DumpPresenter_Width, &dumpPresenter.width, 340);
		Read(CFG_DumpPresenter_Height, &dumpPresenter.height, 500);
		Read(CFG_DumpPresenter_Shown, &dumpPresenter.shown, false);
	} while (0);
	do {
		Read(CFG_Config_DumpPresenterAddrBin, &dumpPresenterAddrBin, true);
		Read(CFG_Config_DumpPresenterColumns, &dumpPresenterColumns, 4);
		if (dumpPresenterColumns <= 0 || dumpPresenterColumns > 16) {
			dumpPresenterColumns = 4;
		}
		Read(CFG_Config_UpperCaseAsm, &upperCaseAsm, true);
		Read(CFG_Config_UpperCaseHex, &upperCaseHex, true);
		Read(CFG_Config_BoardOnDialog, &boardOnDialog, false);
		Read(CFG_Config_AutoHideBoardDialog, &autoHideBoardDialog, true);
	} while (0);
}

void Config::WriteBurst()
{
	do {
		Write(CFG_Frame_X, frame.x);
		Write(CFG_Frame_Y, frame.y);
		Write(CFG_Frame_Width, frame.width);
		Write(CFG_Frame_Height, frame.height);
	} while (0);
	do {
		Write(CFG_BoardDialog_X, boardDialog.x);
		Write(CFG_BoardDialog_Y, boardDialog.y);
		Write(CFG_BoardDialog_Shown, boardDialog.shown);
	} while (0);
	do {
		Write(CFG_DumpPresenter_X, dumpPresenter.x);
		Write(CFG_DumpPresenter_Y, dumpPresenter.y);
		Write(CFG_DumpPresenter_Width, dumpPresenter.width);
		Write(CFG_DumpPresenter_Height, dumpPresenter.height);
		Write(CFG_DumpPresenter_Shown, dumpPresenter.shown);
	} while (0);
	do {
		Write(CFG_Config_DumpPresenterAddrBin, dumpPresenterAddrBin);
		Write(CFG_Config_DumpPresenterColumns, dumpPresenterColumns);
		Write(CFG_Config_UpperCaseAsm, upperCaseAsm);
		Write(CFG_Config_UpperCaseHex, upperCaseHex);
		Write(CFG_Config_BoardOnDialog, boardOnDialog);
		Write(CFG_Config_AutoHideBoardDialog, autoHideBoardDialog);
	} while (0);
}

wxString Config::MakeIniFileName()
{
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	fileName.SetExt(wxT("ini"));
	return fileName.GetFullPath();
}
