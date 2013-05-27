// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __GMC4SIM_H__
#define __GMC4SIM_H__

#include "Utils.h"
#include "Config.h"
#include "Parser.h"
#include "Generator.h"
#include "Board.h"
#include "ProgramMonitor.h"
#include "AssemblerEditor.h"
#include "RegisterWatcher.h"
#include "DumpPresenter.h"
#include "ConfigDialog.h"

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------
class MyApp : public wxApp {
public:
	virtual bool OnInit();
};

//-----------------------------------------------------------------------------
// FrameMain
//-----------------------------------------------------------------------------
class FrameMain : public wxFrame {
public:
	enum {
		ID_ReOpen = 1000,
		ID_ExportHex,
		ID_ExportDmp,
		ID_ExportFxp,
		ID_ExportHtmlDump,
		ID_Build,
		ID_DisasmHex,
		ID_RunMode,
		ID_StepMode,
		ID_BeeperMode,
		ID_Reset,
		ID_HardReset,
		ID_ShowBoard,
		ID_ShowDumpPresenter,
		ID_Config,
		ID_ProgramMonitor,
		ID_AssemblerEditor,
	};
public:
	class BoardDialog : public wxDialog {
	private:
		FrameMain *_pFrameMain;
	public:
		BoardDialog(FrameMain *pFrameMain, const wxPoint &pos);
		~BoardDialog();
		DECLARE_EVENT_TABLE()
		void OnClose(wxCloseEvent &event);
		void OnMenu_Cancel(wxCommandEvent &event);
		void OnMenu_RunMode(wxCommandEvent &event);
		void OnMenu_StepMode(wxCommandEvent &event);
		void OnMenu_BeeperMode(wxCommandEvent &event);
		void OnMenu_Reset(wxCommandEvent &event);
	};
	class Panel : public wxPanel, public VirtualMachine::Observer {
	private:
		VirtualMachineGUI &_vm;
		ProgramMonitor *_pProgramMonitor;
		AssemblerEditor *_pAssemblerEditor;
		wxStaticText *_pIndicator;
	public:
		Panel(wxWindow *pParent, wxWindowID id, BoardDialog *pBoardDialog, VirtualMachineGUI &vm);
		inline AssemblerEditor *GetAssemblerEditor() { return _pAssemblerEditor; }
		inline ProgramMonitor *GetProgramMonitor() { return _pProgramMonitor; }
		void UpdateIndicator();
	private:
		// virtual functions of VirtualMachine::Observer
		virtual void NotifyModeChange(const VirtualMachine &vm);
		virtual void NotifyAddressChange(const VirtualMachine &vm);
		virtual void NotifyOpExecuted(const VirtualMachine &vm);
		virtual void NotifyProgramChange(const VirtualMachine &vm);
		DECLARE_EVENT_TABLE()
		void OnListItemSelected_ProgramMonitor(wxListEvent &event);
	};
	class OutputString : public Generator::Output {
	private:
		wxString &_string;
	public:
		inline OutputString(wxString &string) : _string(string) {}
		virtual void Printf(const char *format, ...);
	};
private:
	BoardDialog *_pBoardDialog;
	Panel *_pPanel;
	DumpPresenter *_pDumpPresenter;
	VirtualMachineGUI _vm;
	wxFileName _fileName;
	wxCHMHelpController _helpController;
public:
	FrameMain(wxWindow *pParent, const wxPoint &pos, const wxSize &size);
	~FrameMain();
	void UpdateCaption();
	void DoExportHex();
	void DoExportDmp();
	void DoExportFxp();
	void DoExportHtmlDump();
	void DoRunMode(bool beeperModeFlag);
	void DoStepMode();
	void DoReset();
private:
	bool ParseFile(const wxFileName &fileName);
	bool ParseEditor(Parser *pParser);
	void ReadHeaderComment(wxString &string, wxInputStream &stream);
private:
	DECLARE_EVENT_TABLE()
	void OnMenuOpen(wxMenuEvent &event);
	void OnMenu_Open(wxCommandEvent &event);
	void OnMenu_ReOpen(wxCommandEvent &event);
	void OnMenu_Save(wxCommandEvent &event);
	void OnMenu_SaveAs(wxCommandEvent &event);
	void OnMenu_ExportHex(wxCommandEvent &event);
	void OnMenu_ExportDmp(wxCommandEvent &event);
	void OnMenu_ExportFxp(wxCommandEvent &event);
	void OnMenu_ExportHtmlDump(wxCommandEvent &event);
	void OnMenu_Close(wxCommandEvent &event);
	void OnMenu_Build(wxCommandEvent &event);
	void OnMenu_DisasmHex(wxCommandEvent &event);
	void OnMenu_RunMode(wxCommandEvent &event);
	void OnMenu_StepMode(wxCommandEvent &event);
	void OnMenu_BeeperMode(wxCommandEvent &event);
	void OnMenu_Reset(wxCommandEvent &event);
	void OnMenu_HardReset(wxCommandEvent &event);
	void OnMenu_ShowBoard(wxCommandEvent &event);
	void OnMenu_ShowDumpPresenter(wxCommandEvent &event);
	void OnMenu_Config(wxCommandEvent &event);
	void OnMenu_Help(wxCommandEvent &event);
	void OnMenu_About(wxCommandEvent &event);
	void OnText_AssemblerEditor(wxCommandEvent &event);
};

#endif
