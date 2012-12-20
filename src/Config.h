// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CFG_Frame_X					wxT("/Frame/X")
#define CFG_Frame_Y					wxT("/Frame/Y")
#define CFG_Frame_Width				wxT("/Frame/Width")
#define CFG_Frame_Height			wxT("/Frame/Height")

#define CFG_BoardDialog_X			wxT("/BoardDialog/X")
#define CFG_BoardDialog_Y			wxT("/BoardDialog/Y")
#define CFG_BoardDialog_Shown		wxT("/BoardDialog/Shown")

#define CFG_DumpPresenter_X			wxT("/DumpPresenter/X")
#define CFG_DumpPresenter_Y			wxT("/DumpPresenter/Y")
#define CFG_DumpPresenter_Width		wxT("/DumpPresenter/Width")
#define CFG_DumpPresenter_Height	wxT("/DumpPresenter/Height")
#define CFG_DumpPresenter_Shown		wxT("/DumpPresenter/Shown")

#define CFG_Config_DumpPresenterAddrBin	wxT("/Config/DumpPresenterAddrBin")
#define CFG_Config_DumpPresenterColumns	wxT("/Config/DumpPresenterColumns")
#define CFG_Config_UpperCaseAsm			wxT("/Config/UpperCaseAsm")
#define CFG_Config_UpperCaseHex			wxT("/Config/UpperCaseHex")
#define CFG_Config_BoardOnDialog		wxT("/Config/BoardOnDialog")
#define CFG_Config_AutoHideBoardDialog	wxT("/Config/AutoHideBoardDialog")

class Config : public wxFileConfig {
public:
	struct {
		int x, y;
		int width, height;
	} frame;
	struct {
		int x, y;
		bool shown;
	} boardDialog;
	struct {
		int x, y;
		int width, height;
		bool shown;
	} dumpPresenter;
	bool dumpPresenterAddrBin;
	int dumpPresenterColumns;
	bool upperCaseAsm;
	bool upperCaseHex;
	bool boardOnDialog;
	bool autoHideBoardDialog;
public:
	Config();
	static void Prepare();
	static inline Config *Get() { return dynamic_cast<Config *>(wxConfig::Get()); }
	void ReadBurst();
	void WriteBurst();
private:
	static wxString MakeIniFileName();
};

#endif
