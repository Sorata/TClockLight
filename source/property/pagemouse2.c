/*-------------------------------------------------------------
  pagemouse2.c : "Mouse" - "Drop" page of properties
  (C) Kazuto Sato 1997-2003
  For the license, please read readme.txt.
  
  Written by Kazubon, Nanashi-san
---------------------------------------------------------------*/

#include "tcprop.h"

#if TC_ENABLE_MOUSEDROP

/* Statics */

static void SendPSChanged(HWND hDlg);
static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnDropFilesChange(HWND hDlg);
static void OnBrowse(HWND hDlg);

static const char *m_section = "Mouse";
static BOOL m_bInit = FALSE;
static BOOL m_bChanged = FALSE;

/*------------------------------------------------
  Dialog procedure
--------------------------------------------------*/
INT_PTR CALLBACK PageMouse2Proc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			OnInit(hDlg);
			return TRUE;
		case WM_COMMAND:
		{
			WORD id, code;
			id = LOWORD(wParam); code = HIWORD(wParam);
			switch(id)
			{
				case IDC_DROPFILES:
					if(code == CBN_SELCHANGE)
					{
						OnDropFilesChange(hDlg);
						g_bApplyClock = TRUE;
						SendPSChanged(hDlg);
					}
					break;
				case IDC_DROPFILESAPP:
					if(code == EN_CHANGE)
						SendPSChanged(hDlg);
					break;
				case IDC_DROPFILESAPPSANSHO:
					OnBrowse(hDlg);
					break;
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: MyHelp(GetParent(hDlg), "Mouse2"); break;
			}
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------
  notify parent window to enable "Apply" button
--------------------------------------------------*/
void SendPSChanged(HWND hDlg)
{
	if(m_bInit)
	{
		m_bChanged = TRUE;
		SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(hDlg), 0);
	}
}

/*------------------------------------------------
  initialize
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	char s[MAX_PATH];
	
	m_bInit = FALSE;
	
	// common/tclang.c
	SetDialogLanguage(hDlg, "Mouse2", g_hfontDialog);
	
	CBAddString(hDlg, IDC_DROPFILES,
		(LPARAM)MyString(IDS_NONE, "None"));
	CBAddString(hDlg, IDC_DROPFILES,
		(LPARAM)MyString(IDS_RECYCLE, "Recycle"));
	CBAddString(hDlg, IDC_DROPFILES,
		(LPARAM)MyString(IDS_OPENBY, "OpenBy"));
	CBAddString(hDlg, IDC_DROPFILES,
		(LPARAM)MyString(IDS_COPYTO, "CopyTo"));
	CBAddString(hDlg, IDC_DROPFILES,
		(LPARAM)MyString(IDS_MOVETO, "MoveTo"));
	
	CBSetCurSel(hDlg, IDC_DROPFILES,
		GetMyRegLong(m_section, "DropFiles", 0));
	GetMyRegStr(m_section, "DropFilesApp", s, MAX_PATH, "");
	SetDlgItemText(hDlg, IDC_DROPFILESAPP, s);	
	
	OnDropFilesChange(hDlg);
	
	m_bInit = TRUE;
}

/*------------------------------------------------
   apply - save settings
--------------------------------------------------*/
void OnApply(HWND hDlg)
{
	char s[MAX_PATH];
	int n;
	
	if(!m_bChanged) return;
	m_bChanged = FALSE;
	
	n = CBGetCurSel(hDlg, IDC_DROPFILES);
	SetMyRegLong(NULL, "DropFiles", n > 0);
	SetMyRegLong(m_section, "DropFiles", n);
	GetDlgItemText(hDlg, IDC_DROPFILESAPP, s, MAX_PATH);
	SetMyRegStr(m_section, "DropFilesApp", s);
}

/*------------------------------------------------
  "Function" is selected
--------------------------------------------------*/
void OnDropFilesChange(HWND hDlg)
{
	int i, n;
	
	n = CBGetCurSel(hDlg, IDC_DROPFILES);
	
	if(m_bInit)
		SetDlgItemText(hDlg, IDC_DROPFILESAPP, "");
	
	if(n == 2)
		SetDlgItemText(hDlg, IDC_LABDROPFILESAPP,
			MyString(IDS_APPLICATION, "Application"));
	else if(n == 3 || n == 4)
		SetDlgItemText(hDlg, IDC_LABDROPFILESAPP,
			MyString(IDS_FOLDER, "Folder"));
	
	for(i = IDC_LABDROPFILESAPP; i <= IDC_DROPFILESAPPSANSHO; i++)
		ShowDlgItem(hDlg, i, (2 <= n && n <= 4));
}

/*------------------------------------------------
  select file / folder
--------------------------------------------------*/
void OnBrowse(HWND hDlg)
{
	int n;
	char deffile[MAX_PATH], fname[MAX_PATH];
	
	n = CBGetCurSel(hDlg, IDC_DROPFILES);
	
	if(n == 2)
	{
		GetDlgItemText(hDlg, IDC_DROPFILESAPP, deffile, MAX_PATH);
		
		// common/selectfile.c
		if(!SelectMyFile(g_hInst, hDlg,
			"Application (*.exe)\0*.exe\0All (*.*)\0*.*\0",
			0, deffile, fname))
				return;
	}
	else if(n == 3 || n == 4)
	{
		GetDlgItemText(hDlg, IDC_DROPFILESAPP, deffile, MAX_PATH);
		
		// common/selectfile.c
		if(!SelectFolder(hDlg, deffile, fname)) return;
	}
	else return;
	
	SetDlgItemText(hDlg, IDC_DROPFILESAPP, fname);
	PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
	SendPSChanged(hDlg);
}

#endif	/* TC_ENABLE_MOUSEDROP */
