// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#include "GMC4Sim.h"

#define CAPTION	wxT("GMC-4 Simulator")

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------
IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	SetVendorName(wxT("DansanSoft"));
	Config::Prepare();
	FrameMain *pFrame = new FrameMain(NULL,
		wxPoint(Config::Get()->frame.x, Config::Get()->frame.y),
		wxSize(Config::Get()->frame.width, Config::Get()->frame.height));
	pFrame->UpdateCaption();
	pFrame->Show();
	SetTopWindow(pFrame);
	return true;
}

//-----------------------------------------------------------------------------
// FrameMain
//-----------------------------------------------------------------------------
FrameMain::FrameMain(wxWindow *pParent, const wxPoint &pos, const wxSize &size) :
		wxFrame(pParent, wxID_ANY, wxT(""), pos, size),
		_pPanel(NULL), _pBoardDialog(NULL)
{
	SetIcon(wxICON(MainIcon));
	wxMenuBar *pMenuBar = new wxMenuBar();
	SetMenuBar(pMenuBar);
	do {
		wxMenu *pMenu = new wxMenu();
		pMenuBar->Append(pMenu, wxT("&File"));
		pMenu->Append(wxID_OPEN, wxT("&Open...\tCtrl+O"));
		pMenu->Append(ID_ReOpen, wxT("&Reopen\tF5"));
		pMenu->AppendSeparator();
		pMenu->Append(wxID_SAVE, wxT("&Save\tCtrl+S"));
		pMenu->Append(wxID_SAVEAS, wxT("Save &As..."));
		pMenu->AppendSeparator();
		pMenu->Append(ID_ExportHex, wxT("Export &Hex..."));
		pMenu->Append(ID_ExportDmp, wxT("Export &Dmp..."));
		pMenu->Append(ID_ExportFxp, wxT("Export &Fxp..."));
		pMenu->Append(ID_ExportHtmlDump, wxT("Export H&TML Dump..."));
		pMenu->AppendSeparator();
		pMenu->Append(wxID_CLOSE, wxT("E&xit"));
	} while (0);
	do {
		wxMenu *pMenu = new wxMenu();
		pMenuBar->Append(pMenu, wxT("&Simulator"));
		pMenu->Append(ID_Build, wxT("&Build\tCtrl+Return"));
		pMenu->Append(ID_DisasmHex, wxT("&Disasm Hex"));
		pMenu->AppendSeparator();
		pMenu->Append(ID_RunMode, wxT("&Run Mode\tCtrl+R"));
		pMenu->Append(ID_StepMode, wxT("S&tep Mode\tCtrl+T"));
		pMenu->Append(ID_BeeperMode, wxT("Bee&per Mode"));
		pMenu->AppendSeparator();
		pMenu->Append(ID_Reset, wxT("R&eset\tCtrl+E"));
		pMenu->Append(ID_HardReset, wxT("&Hard Reset"));
	} while (0);
	do {
		wxMenu *pMenu = new wxMenu();
		pMenuBar->Append(pMenu, wxT("&Window"));
		pMenu->AppendCheckItem(ID_ShowBoard, wxT("Show &Board\tCtrl+B"));
		pMenu->AppendCheckItem(ID_ShowDumpPresenter,
							wxT("Show D&ump\tCtrl+U"));
		pMenu->Append(ID_Config, wxT("&Config..."));
	} while (0);
	do {
		wxMenu *pMenu = new wxMenu();
		pMenuBar->Append(pMenu, wxT("&Help"));
		pMenu->Append(wxID_HELP, wxT("&Help"));
		pMenu->AppendSeparator();
		pMenu->Append(wxID_ABOUT, wxT("&About"));
	} while (0);
	do {
		wxHelpControllerHelpProvider *pProvider = new wxHelpControllerHelpProvider();
		pProvider->SetHelpController(&_helpController);
		_helpController.Initialize(wxT("GMC4Sim"));
	} while (0);
	do {
		_pDumpPresenter = new DumpPresenter(this,
			wxPoint(Config::Get()->dumpPresenter.x, Config::Get()->dumpPresenter.y),
			wxSize(Config::Get()->dumpPresenter.width, Config::Get()->dumpPresenter.height), _vm);
		_vm.AddObserver(_pDumpPresenter);
	} while (0);
	if (Config::Get()->boardOnDialog) {
		_pBoardDialog = new BoardDialog(this,
			wxPoint(Config::Get()->boardDialog.x, Config::Get()->boardDialog.y));
	}
	do {
		_pPanel = new Panel(this, wxID_ANY, _pBoardDialog, _vm);
		_vm.AddObserver(_pPanel);
	} while (0);
	if (_pBoardDialog != NULL) {
		_pBoardDialog->Show(Config::Get()->boardDialog.shown);
	}
	do {
		_pDumpPresenter->Show(Config::Get()->dumpPresenter.shown);
	} while (0);
	_vm.Reset();
}

FrameMain::~FrameMain()
{
	if (IsFullScreen() || IsIconized() || IsMaximized()) {
		// nothing to do
	} else {
		wxRect rc = GetRect();
		Config::Get()->frame.x = rc.GetX();
		Config::Get()->frame.y = rc.GetY();
		Config::Get()->frame.width = rc.GetWidth();
		Config::Get()->frame.height = rc.GetHeight();
	}
	if (_pBoardDialog != NULL) {
		wxRect rc = _pBoardDialog->GetRect();
		Config::Get()->boardDialog.x = rc.GetX();
		Config::Get()->boardDialog.y = rc.GetY();
		Config::Get()->boardDialog.shown = _pBoardDialog->IsShown();
	}
	do {
		wxRect rc = _pDumpPresenter->GetRect();
		Config::Get()->dumpPresenter.x = rc.GetX();
		Config::Get()->dumpPresenter.y = rc.GetY();
		Config::Get()->dumpPresenter.width = rc.GetWidth();
		Config::Get()->dumpPresenter.height = rc.GetHeight();
		Config::Get()->dumpPresenter.shown = _pDumpPresenter->IsShown();
	} while (0);
	Config::Get()->WriteBurst();
}

void FrameMain::UpdateCaption()
{
	wxString fileName = _fileName.GetFullName();
	wxString title(CAPTION wxT(" - "));
	if (fileName.IsEmpty() || Parser::GetFileType(fileName) != Parser::FILETYPE_ASM) {
		title += wxT("(no source)");
	} else {
		title += fileName;
	}
	if (_pPanel->GetAssemblerEditor()->IsModified()) {
		title += wxT(" *");
	}
	SetLabel(title);
}

void FrameMain::DoExportHex()
{
	FILE *fp = NULL;
	wxString fileName = ::wxFileSelector(
			wxT("Save GMC-4 Hex-format File"),
			_fileName.GetPath(), _fileName.GetName(), wxT("hex"),
			wxT("GMC-4 hex file (*.hex)|*.hex")
			wxT("|All files (*.*)|*.*"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (fileName.IsEmpty()) {
		// nothing to do
	} else if ((fp = ::fopen(fileName, "wt")) == NULL) {
		wxString errMsg = wxString::Format(wxT("Can't open file %s"), fileName);
		::wxMessageBox(errMsg, wxT("Write File Error"), wxOK | wxICON_ERROR);
	} else {
		Generator::OutputFile output(fp);
		Generator(_vm, output).UpperCase(Config::Get()->upperCaseHex).GenerateHex(
									false, Config::Get()->dumpPresenterColumns);
		::fclose(fp);
	}
}

void FrameMain::DoExportDmp()
{
	FILE *fp = NULL;
	wxString fileName = ::wxFileSelector(
			wxT("Save GMC-4 Dmp-format File"),
			_fileName.GetPath(), _fileName.GetName(), wxT("dmp"),
			wxT("GMC-4 dmp file (*.dmp)|*.dmp")
			wxT("|All files (*.*)|*.*"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (fileName.IsEmpty()) {
		// nothing to do
	} else if ((fp = ::fopen(fileName, "wt")) == NULL) {
		wxString errMsg = wxString::Format(wxT("Can't open file %s"), fileName);
		::wxMessageBox(errMsg, wxT("Write File Error"), wxOK | wxICON_ERROR);
	} else {
		Generator::OutputFile output(fp);
		Generator(_vm, output).UpperCase(Config::Get()->upperCaseHex).GenerateHex(
									true, Config::Get()->dumpPresenterColumns);
		::fclose(fp);
	}
}

void FrameMain::DoExportFxp()
{
	FILE *fp = NULL;
	wxString fileName = ::wxFileSelector(
			wxT("Save GMC-4 Hex-format File"),
			_fileName.GetPath(), _fileName.GetName(), wxT("fxp"),
			wxT("GMC-4 hex file (*.fxp)|*.fxp")
			wxT("|All files (*.*)|*.*"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (fileName.IsEmpty()) {
		// nothing to do
	} else if ((fp = ::fopen(fileName, "wt")) == NULL) {
		wxString errMsg = wxString::Format(wxT("Can't open file %s"), fileName);
		::wxMessageBox(errMsg, wxT("Write File Error"), wxOK | wxICON_ERROR);
	} else {
		Generator::OutputFile output(fp);
		Generator(_vm, output).GenerateFxp();
		::fclose(fp);
	}
}

void FrameMain::DoExportHtmlDump()
{
	FILE *fp = NULL;
	wxString fileName = ::wxFileSelector(
			wxT("Save GMC-4 Program Dump in HTML"),
			_fileName.GetPath(), _fileName.GetName(), wxT("html"),
			wxT("HTML file (*.html)|*.html")
			wxT("|All files (*.*)|*.*"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (fileName.IsEmpty()) {
		// nothing to do
	} else if ((fp = ::fopen(fileName, "wt")) == NULL) {
		wxString errMsg = wxString::Format(wxT("Can't open file %s"), fileName);
		::wxMessageBox(errMsg, wxT("Write File Error"), wxOK | wxICON_ERROR);
	} else {
		Generator::OutputFile output(fp);
		Generator(_vm, output).GenerateHtmlDump(
					Config::Get()->dumpPresenterAddrBin,
					Config::Get()->dumpPresenterColumns, true);
		::fclose(fp);
	}
}

void FrameMain::DoRunMode(bool beeperModeFlag)
{
	if (_pBoardDialog != NULL && !_pBoardDialog->IsShown()) {
		_pBoardDialog->Show(true);
	}
	_vm.GetBoard()->SetFocus();
	_vm.GetBoard()->ClearKeyBuff();
	_vm.Reset();
	_vm.EnterRunMode(beeperModeFlag);
}

void FrameMain::DoStepMode()
{
	if (_pBoardDialog != NULL && !_pBoardDialog->IsShown()) {
		_pBoardDialog->Show(true);
	}
	_vm.GetBoard()->SetFocus();
	_vm.GetBoard()->ClearKeyBuff();
	_vm.Reset();
	_vm.EnterStepMode();
}

void FrameMain::DoReset()
{
	if (_vm.GetMode() == VirtualMachine::MODE_Run) {
		_vm.LeaveRunMode();
	} else if (_vm.GetMode() == VirtualMachine::MODE_Step) {
		_vm.LeaveStepMode();
	} else {
		// nothing to do
	}
	if (_pBoardDialog != NULL && _pBoardDialog->IsShown() &&
										Config::Get()->autoHideBoardDialog) {
		_pBoardDialog->Show(false);
	}
	_pPanel->GetAssemblerEditor()->SetFocus();
	_vm.GetBoard()->ClearKeyBuff();
}

bool FrameMain::ParseFile(const wxFileName &fileName)
{
	std::auto_ptr<Parser> pParser;
	Parser::FileType fileTypeIn = Parser::GetFileType(fileName.GetFullName());
	if (fileTypeIn == Parser::FILETYPE_ASM) {
		pParser.reset(new ParserAsm(_vm));
	} else { // FILETYPE_HEX, FILETYPE_FXP, FILETYPE_None
		pParser.reset(new ParserHex(_vm));
	}
	if (!pParser->ParseFile(fileName.GetFullPath())) {
		wxString errMsg = wxString::Format(wxT("%s: %s"),
							fileName.GetFullName(), pParser->GetError());
		::wxMessageBox(errMsg, wxT("Parse File Error"), wxOK | wxICON_ERROR);
		return false;
	}
	_vm.PutMemBuff(pParser->GetMemBuff());
	_vm.GetBoard()->ClearKeyBuff();
	_vm.GetBoard()->Refresh();
	if (fileTypeIn == Parser::FILETYPE_ASM) {
		_pPanel->GetAssemblerEditor()->LoadFile(fileName.GetFullPath());
	} else {
		wxString str;
		OutputString output(str);
		ReadHeaderComment(str, wxFileInputStream(fileName.GetFullPath()));
		Generator(_vm, output).UpperCase(Config::Get()->upperCaseAsm).GenerateAsm();
		_pPanel->GetAssemblerEditor()->ChangeValue(str);
		_pPanel->GetAssemblerEditor()->MarkDirty();
	}
	_vm.Reset();
	return true;
}

bool FrameMain::ParseEditor(Parser *pParser)
{
	bool rtn = true;
	wxString text = _pPanel->GetAssemblerEditor()->GetValue();
	for (size_t i = 0; i < text.Length(); i++) {
		int ch = static_cast<unsigned char>(text.GetChar(i));
		if (!pParser->FeedChar(ch)) {
			rtn = false;
			break;
		}
	}
	if (rtn && pParser->FeedChar(-1)) {
		_pPanel->GetAssemblerEditor()->SetAddressByLineTbl(pParser->GetAddressByLineTbl());
		_vm.PutMemBuff(pParser->GetMemBuff());
		_vm.GetBoard()->ClearKeyBuff();
		_vm.GetBoard()->Refresh();
		do {
			ProgramMonitor *pProgramMonitor = _pPanel->GetProgramMonitor();
			pProgramMonitor->UpdateContent(_vm);
			//pProgramMonitor->HighlightRowByAddress(_vm.GetAddress());
		} while (0);
		_vm.SetAddress(_pPanel->GetAssemblerEditor()->GetPointedAddress(), true);
	} else {
		::wxMessageBox(wxString::Format(wxT("%s"), pParser->GetError()),
								wxT("Parse Error"), wxOK | wxICON_ERROR);
	}
	return rtn;
}

void FrameMain::ReadHeaderComment(wxString &string, wxInputStream &stream)
{
	enum {
		STAT_LineTop, STAT_Comment,
	} stat = STAT_LineTop;
	//FILE *fp = ::fopen(fileName.c_str(), "rt");
	//if (fp == NULL) return;
	for (;;) {
		int ch = stream.Eof()? -1 : stream.GetC();
		if (stat == STAT_LineTop) {
			if (ch == ';') {
				stat = STAT_Comment;
				string.Append(ch);
			} else {
				break;
			}
		} else if (stat == STAT_Comment) {
			if (ch < 0) {
				string.Append('\n');
				break;
			} else if (ch == '\n') {
				string.Append(ch);
				stat = STAT_LineTop;
			} else {
				string.Append(ch);
			}
		}
	}
	//::fclose(fp);
}

BEGIN_EVENT_TABLE(FrameMain, wxFrame)
	EVT_MENU_OPEN(FrameMain::OnMenuOpen)
	EVT_MENU(wxID_OPEN,			FrameMain::OnMenu_Open)
	EVT_MENU(ID_ReOpen,			FrameMain::OnMenu_ReOpen)
	EVT_MENU(wxID_SAVE,			FrameMain::OnMenu_Save)
	EVT_MENU(wxID_SAVEAS,		FrameMain::OnMenu_SaveAs)
	EVT_MENU(ID_ExportHex,		FrameMain::OnMenu_ExportHex)
	EVT_MENU(ID_ExportDmp,		FrameMain::OnMenu_ExportDmp)
	EVT_MENU(ID_ExportFxp,		FrameMain::OnMenu_ExportFxp)
	EVT_MENU(ID_ExportHtmlDump,	FrameMain::OnMenu_ExportHtmlDump)
	EVT_MENU(wxID_CLOSE,		FrameMain::OnMenu_Close)
	EVT_MENU(ID_Build,			FrameMain::OnMenu_Build)
	EVT_MENU(ID_DisasmHex,		FrameMain::OnMenu_DisasmHex)
	EVT_MENU(ID_RunMode,		FrameMain::OnMenu_RunMode)
	EVT_MENU(ID_StepMode,		FrameMain::OnMenu_StepMode)
	EVT_MENU(ID_BeeperMode,		FrameMain::OnMenu_BeeperMode)
	EVT_MENU(ID_Reset,			FrameMain::OnMenu_Reset)
	EVT_MENU(ID_HardReset,		FrameMain::OnMenu_HardReset)
	EVT_MENU(ID_ShowBoard,		FrameMain::OnMenu_ShowBoard)
	EVT_MENU(ID_ShowDumpPresenter, FrameMain::OnMenu_ShowDumpPresenter)
	EVT_MENU(ID_Config,			FrameMain::OnMenu_Config)
	EVT_MENU(wxID_HELP,			FrameMain::OnMenu_Help)
	EVT_MENU(wxID_ABOUT,		FrameMain::OnMenu_About)
	EVT_TEXT(ID_AssemblerEditor, FrameMain::OnText_AssemblerEditor)
END_EVENT_TABLE()

void FrameMain::OnMenuOpen(wxMenuEvent &event)
{
	const wxMenuItemList &list = event.GetMenu()->GetMenuItems();
	bool programExistFlag = (_vm.GetInvalidRegionAddress() > 0x00);
	for (wxMenuItemList::Node *pNode = list.GetFirst();
							pNode != NULL; pNode = pNode->GetNext()) {
		wxMenuItem *pItem = pNode->GetData();
		int id = pItem->GetId();
		if (id == wxID_OPEN) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run);
		} else if (id == ID_ReOpen) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run &&
									!_fileName.GetFullPath().IsEmpty());
		} else if (id == wxID_SAVE) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run);
		} else if (id == wxID_SAVEAS) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run);
		} else if (id == ID_ExportHex) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run && programExistFlag);
		} else if (id == ID_ExportDmp) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run && programExistFlag);
		} else if (id == ID_ExportFxp) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run && programExistFlag);
		} else if (id == ID_ExportHtmlDump) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run && programExistFlag);
		} else if (id == wxID_CLOSE) {
			// nothing to do
		} else if (id == ID_Build) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run);
		} else if (id == ID_RunMode) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run);
		} else if (id == ID_StepMode) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run);
		} else if (id == ID_BeeperMode) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run);
		} else if (id == ID_Reset) {
			// nothing to do
		} else if (id == ID_HardReset) {
			// nothing to do
		} else if (id == ID_ShowBoard) {
			if (_pBoardDialog == NULL) {
				pItem->Enable(false);
				pItem->Check(true);
			} else {
				pItem->Enable(true);
				pItem->Check(_pBoardDialog->IsShown());
			}
		} else if (id == ID_ShowDumpPresenter) {
			pItem->Check(_pDumpPresenter->IsShown());
		} else if (id == ID_Config) {
			pItem->Enable(_vm.GetMode() != VirtualMachine::MODE_Run);
		}
	}
}

void FrameMain::OnMenu_Open(wxCommandEvent &event)
{
	wxString fileName = ::wxFileSelector(
		wxT("Open GMC-4 File"), wxT(""), wxT(""), wxT(""),
		wxT("GMC-4 files (*.asm; *.gmc; *.hex; *.dmp; *.fxp)|*.asm;*.gmc;*.hex;*.dmp;*.fxp")
		wxT("|All files (*.*)|*.*"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (fileName.empty()) {
		// nothing to do
	} else if (ParseFile(fileName)) {
		_fileName.Assign(fileName);
		UpdateCaption();
	} else {
		// nothing to do
	}
}

void FrameMain::OnMenu_ReOpen(wxCommandEvent &event)
{
	if (_fileName.GetFullPath().IsEmpty()) {
		// nothing to do
	} else if (ParseFile(_fileName.GetFullPath())) {
		UpdateCaption();
	} else {
		// nothing to do
	}
}

void FrameMain::OnMenu_Save(wxCommandEvent &event)
{
	wxString fileName = _fileName.GetFullPath();
	if (fileName.IsEmpty()) {
		fileName = ::wxFileSelector(
				wxT("Save GMC-4 Assembler File"),
				wxT(""), wxT(""), wxT("asm"),
				wxT("GMC-4 assembler file (*.asm)|*.asm")
				wxT("|All files (*.*)|*.*"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	} else if (Parser::GetFileType(fileName) != Parser::FILETYPE_ASM) {
		fileName = ::wxFileSelector(
				wxT("Save GMC-4 Assembler File"),
				_fileName.GetPath(), _fileName.GetName(), wxT("asm"),
				wxT("GMC-4 assembler file (*.asm)|*.asm")
				wxT("|All files (*.*)|*.*"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	}
	if (fileName.IsEmpty()) {
		// nothing to do
	} else if (!_pPanel->GetAssemblerEditor()->SaveFile(fileName)) {
		::wxMessageBox(wxString::Format(wxT("Can't open file %s"), fileName),
								wxT("Write File Error"), wxOK | wxICON_ERROR);
	} else {
		_fileName.Assign(fileName);
		UpdateCaption();
	}
}

void FrameMain::OnMenu_SaveAs(wxCommandEvent &event)
{
	wxString fileName = _fileName.GetFullPath();
	if (fileName.IsEmpty()) {
		fileName = ::wxFileSelector(
				wxT("Save GMC-4 Assembler File"),
				wxT(""), wxT(""), wxT("asm"),
				wxT("GMC-4 assembler file (*.asm)|*.asm")
				wxT("|All files (*.*)|*.*"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	} else {
		fileName = ::wxFileSelector(
				wxT("Save GMC-4 Assembler File"),
				_fileName.GetPath(), _fileName.GetName(), _fileName.GetExt(),
				wxT("GMC-4 assembler file (*.asm)|*.asm")
				wxT("|All files (*.*)|*.*"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	}
	if (fileName.IsEmpty()) {
		// nothing to do
	} else if (!_pPanel->GetAssemblerEditor()->SaveFile(fileName)) {
		::wxMessageBox(wxString::Format(wxT("Can't open file %s"), fileName),
								wxT("Write File Error"), wxOK | wxICON_ERROR);
	} else {
		_fileName.Assign(fileName);
		UpdateCaption();
	}
}

void FrameMain::OnMenu_ExportHex(wxCommandEvent &event)
{
	DoExportHex();
}

void FrameMain::OnMenu_ExportDmp(wxCommandEvent &event)
{
	DoExportDmp();
}

void FrameMain::OnMenu_ExportFxp(wxCommandEvent &event)
{
	DoExportFxp();
}

void FrameMain::OnMenu_ExportHtmlDump(wxCommandEvent &event)
{
	DoExportHtmlDump();
}

void FrameMain::OnMenu_Close(wxCommandEvent &event)
{
	Close();
}

void FrameMain::OnMenu_Build(wxCommandEvent &event)
{
	std::auto_ptr<Parser> pParser(new ParserAsm(_vm));
	ParseEditor(pParser.get());
}

void FrameMain::OnMenu_DisasmHex(wxCommandEvent &event)
{
	std::auto_ptr<Parser> pParser(new ParserHex(_vm));
	ParseEditor(pParser.get());
	do {
		wxString str;
		OutputString output(str);
		ReadHeaderComment(str, wxStringInputStream(
							_pPanel->GetAssemblerEditor()->GetValue()));
		Generator(_vm, output).UpperCase(Config::Get()->upperCaseAsm).GenerateAsm();
		_pPanel->GetAssemblerEditor()->ChangeValue(str);
		_pPanel->GetAssemblerEditor()->MarkDirty();
	} while (0);
}

void FrameMain::OnMenu_RunMode(wxCommandEvent &event)
{
	DoRunMode(false);
}

void FrameMain::OnMenu_StepMode(wxCommandEvent &event)
{
	DoStepMode();
}

void FrameMain::OnMenu_BeeperMode(wxCommandEvent &event)
{
	DoRunMode(true);
}

void FrameMain::OnMenu_Reset(wxCommandEvent &event)
{
	DoReset();
}

void FrameMain::OnMenu_HardReset(wxCommandEvent &event)
{
	if (_vm.GetMode() == VirtualMachine::MODE_Run) {
		_vm.LeaveRunMode();
		_pPanel->GetAssemblerEditor()->SetFocus();
	} else if (_vm.GetMode() == VirtualMachine::MODE_Step) {
		_vm.LeaveStepMode();
		_pPanel->GetAssemblerEditor()->SetFocus();
	} else {
		// nothing to do
	}
	_vm.InitializeRegs();
	_vm.InvalidateMemBuff();
	_vm.GetBoard()->ClearKeyBuff();
}

void FrameMain::OnMenu_ShowBoard(wxCommandEvent &event)
{
	if (_pBoardDialog != NULL) {
		_pBoardDialog->Show(!_pBoardDialog->IsShown());
	}
}

void FrameMain::OnMenu_ShowDumpPresenter(wxCommandEvent &event)
{
	_pDumpPresenter->Show(!_pDumpPresenter->IsShown());
}

void FrameMain::OnMenu_Config(wxCommandEvent &event)
{
	ConfigDialog dlg(this, ConfigDialog::PAGEID_None);
	dlg.ShowModal();
	if (dlg.GetRebootFlag()) {
		::wxMessageBox(wxT("Changes will take effect after reboot"),
						wxT("Configuration"), wxOK | wxICON_INFORMATION);
	}
}

void FrameMain::OnMenu_Help(wxCommandEvent &event)
{
	//_helpController.DisplayContents();
	do {
		wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
		fileName.SetFullName(wxT("GMC4Sim.pdf"));
		// Win32 API
		::ShellExecute(NULL, _T("open"), fileName.GetFullPath(), NULL, NULL, SW_SHOW);
	} while (0);
}

void FrameMain::OnMenu_About(wxCommandEvent &event)
{
	wxAboutDialogInfo info;
	info.SetIcon(wxICON(MainIcon));
	info.SetName(CAPTION);
	info.SetVersion(wxT("version ") VERSION);
	info.SetDescription(wxT("Development Tool for GMC-4, 4-bit Microcomputer of Gakken Otona-no-Kagaku"));
	info.SetCopyright(wxT("Copyright (C) 2009 Dan-san"));
	info.SetWebSite(wxT("http://dansan.air-nifty.com/blog/gmc4-simulator.html"));
	::wxAboutBox(info);
}

void FrameMain::OnText_AssemblerEditor(wxCommandEvent &event)
{
	UpdateCaption();
}

//-----------------------------------------------------------------------------
// FrameMain::BoardDialog
//-----------------------------------------------------------------------------
FrameMain::BoardDialog::BoardDialog(FrameMain *pFrameMain, const wxPoint &pos) :
	wxDialog(pFrameMain, wxID_ANY, CAPTION, pos), _pFrameMain(pFrameMain)
{
	SetIcon(wxICON(MainIcon));
	wxAcceleratorEntry entries[3];
	entries[0].Set(wxACCEL_CTRL, 'R', ID_RunMode);
	entries[1].Set(wxACCEL_CTRL, 'T', ID_StepMode);
	entries[2].Set(wxACCEL_CTRL, 'E', ID_Reset);
	wxAcceleratorTable accelTbl(NUMBEROF(entries), entries);
	SetAcceleratorTable(accelTbl);
	// dummy button to enable window close by escape key
	//new wxButton(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(1, 1));
}

FrameMain::BoardDialog::~BoardDialog()
{
}

BEGIN_EVENT_TABLE(FrameMain::BoardDialog, wxDialog)
	EVT_CLOSE(FrameMain::BoardDialog::OnClose)
	EVT_MENU(wxID_CANCEL,	FrameMain::BoardDialog::OnMenu_Cancel)
	EVT_MENU(ID_RunMode,	FrameMain::BoardDialog::OnMenu_RunMode)
	EVT_MENU(ID_StepMode,	FrameMain::BoardDialog::OnMenu_StepMode)
	EVT_MENU(ID_BeeperMode,	FrameMain::BoardDialog::OnMenu_BeeperMode)
	EVT_MENU(ID_Reset,		FrameMain::BoardDialog::OnMenu_Reset)
END_EVENT_TABLE()

void FrameMain::BoardDialog::OnClose(wxCloseEvent &event)
{
	_pFrameMain->DoReset();
	Show(false);
}

void FrameMain::BoardDialog::OnMenu_Cancel(wxCommandEvent &event)
{
}

void FrameMain::BoardDialog::OnMenu_RunMode(wxCommandEvent &event)
{
	_pFrameMain->DoRunMode(false);
}

void FrameMain::BoardDialog::OnMenu_StepMode(wxCommandEvent &event)
{
	_pFrameMain->DoStepMode();
}

void FrameMain::BoardDialog::OnMenu_BeeperMode(wxCommandEvent &event)
{
	_pFrameMain->DoRunMode(true);
}

void FrameMain::BoardDialog::OnMenu_Reset(wxCommandEvent &event)
{
	_pFrameMain->DoReset();
}

//-----------------------------------------------------------------------------
// FrameMain::Panel
//-----------------------------------------------------------------------------
FrameMain::Panel::Panel(wxWindow *pParent, wxWindowID id,
						BoardDialog *pBoardDialog, VirtualMachineGUI &vm) :
		wxPanel(pParent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL),
		_vm(vm)
{
	wxBoxSizer *pOuterBox = new wxBoxSizer(wxVERTICAL);
	SetSizer(pOuterBox);
	wxBoxSizer *pHBox = new wxBoxSizer(wxHORIZONTAL);
	pOuterBox->Add(pHBox, wxSizerFlags(1).Expand());
	do {
		wxBoxSizer *pVBox = new wxBoxSizer(wxVERTICAL);
		pHBox->Add(pVBox, wxSizerFlags(0).Expand());
		do {
			RegisterWatcher *pCtrl =
					new RegisterWatcher(this, wxID_ANY, wxSUNKEN_BORDER, vm);
			pVBox->Add(pCtrl, wxSizerFlags(0).Expand());
			vm.AddObserver(pCtrl);
		} while (0);
		do {
			ProgramMonitor *pCtrl =
					new ProgramMonitor(this, ID_ProgramMonitor, wxSUNKEN_BORDER);
			pVBox->Add(pCtrl, wxSizerFlags(1).Expand().Border(wxTOP, 2));
			vm.AddObserver(pCtrl);
			_pProgramMonitor = pCtrl;
		} while (0);
	} while (0);
	do {
		wxBoxSizer *pVBox = new wxBoxSizer(wxVERTICAL);
		pHBox->Add(pVBox, wxSizerFlags(1).Expand().Border(wxLEFT, 2));
		if (pBoardDialog == NULL) {
			Board *pCtrl = new Board(this, wxID_ANY, wxSUNKEN_BORDER, vm);
			pVBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxBOTTOM, 2));
			vm.SetBoard(pCtrl);
			vm.AddObserver(pCtrl);
		} else {
			Board *pCtrl = new Board(pBoardDialog, wxID_ANY, 0, vm);
			pBoardDialog->Fit();
			vm.SetBoard(pCtrl);
			vm.AddObserver(pCtrl);
		}
		do {
			AssemblerEditor *pCtrl = new AssemblerEditor(this,
									ID_AssemblerEditor, wxSUNKEN_BORDER, vm);
			pVBox->Add(pCtrl, wxSizerFlags(1).Expand());
			vm.AddObserver(pCtrl);
			_pAssemblerEditor = pCtrl;
			pCtrl->SetFocus();
		} while (0);
		do {
			wxStaticText *pCtrl = new wxStaticText(this, wxID_ANY, wxT(""),
					wxDefaultPosition, wxSize(-1, 20),
					wxSTATIC_BORDER | wxST_NO_AUTORESIZE);
			pVBox->Add(pCtrl, wxSizerFlags(0).Expand().Border(wxTOP, 2));
			_pIndicator = pCtrl;
		} while (0);
	} while (0);
	UpdateIndicator();
	vm.SetMode(VirtualMachine::MODE_Program);
}

void FrameMain::Panel::UpdateIndicator()
{
	int nNibbles = 0;
	int nNibblesAll = VirtualMachine::ADDR_REGISTER;
	for (unsigned addr = 0x00; addr < VirtualMachine::ADDR_REGISTER; addr++) {
		if (_vm.IsValidMem(addr)) nNibbles++;
	}
	_pIndicator->SetLabel(wxString::Format(
			wxT("  Occupied nibbles: %d / %d (0x%02X / 0x%02X)"),
			nNibbles, nNibblesAll, nNibbles, nNibblesAll));
}

void FrameMain::Panel::NotifyModeChange(const VirtualMachine &vm)
{
	// nothing to do
}

void FrameMain::Panel::NotifyAddressChange(const VirtualMachine &vm)
{
	// nothing to do
}

void FrameMain::Panel::NotifyOpExecuted(const VirtualMachine &vm)
{
	// nothing to do
}

void FrameMain::Panel::NotifyProgramChange(const VirtualMachine &vm)
{
	UpdateIndicator();
}

BEGIN_EVENT_TABLE(FrameMain::Panel, wxPanel)
	EVT_LIST_ITEM_SELECTED(ID_ProgramMonitor,
				FrameMain::Panel::OnListItemSelected_ProgramMonitor)
END_EVENT_TABLE()

void FrameMain::Panel::OnListItemSelected_ProgramMonitor(wxListEvent &event)
{
	if (_vm.GetMode() != VirtualMachine::MODE_Run) {
		_vm.SetAddress(_pProgramMonitor->GetAddressByRow(event.GetIndex()));
	}
}

//-----------------------------------------------------------------------------
// FrameMain::OutputString
//-----------------------------------------------------------------------------
void FrameMain::OutputString::Printf(const char *format, ...)
{
	va_list list;
	va_start(list, format);
	_string.Append(wxString::FormatV(format, list));
	va_end(list);
}
