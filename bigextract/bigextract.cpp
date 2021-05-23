/*
	Command & Conquer 3 Tools
	.BIG file extraction tool
	Copyright 2007 Jonathan Wilson
	Portions lifted from BinOpener and TerrainTextureCompiler copyright 2007 booto (temptemp91 at hotmail dot com)

	This file is part of cc3tools.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	The Command & Conquer 3 Tools are distributed in the hope that they will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/
#include "cc3tools.h"
#include "resource.h"
#include <windowsx.h>
#include <shlobj.h>

char *cmdline;
HINSTANCE hInst;
HACCEL accel;
HHOOK hook;
HWND hdlg;
BIGFile *big;
char outputfolder[MAX_PATH];
LRESULT CALLBACK AccelHook(int code, WPARAM wParam, LPARAM lParam)
{
	if (code == MSGF_DIALOGBOX)
	{
		MSG* msg = (MSG*)lParam;
		if (msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST)
		{
			if (TranslateAccelerator(hdlg, accel, msg))
			{
				return true;
			}
		}
		return CallNextHookEx(hook, code, wParam, lParam);
	}
	return false;
}

BOOL CALLBACK ExtractDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
	case WM_INITDIALOG:
		{
			hdlg = hwnd;
			if ((cmdline) && (cmdline[0]))
			{
				if (cmdline[strlen(cmdline) - 1] == '\"')
				{
					cmdline[strlen(cmdline) - 1] = 0;
				}
				if (cmdline[0] == '\"')
				{
					cmdline++;
				}
				ListBox_ResetContent(GetDlgItem(hwnd,IDC_FILELIST));
				big = BigOpen(cmdline);
				for (unsigned int i = 0;i < BigGetNumElements(big);i++)
				{
					const char *name = BigGetFilename(big,i);
					ListBox_AddString(GetDlgItem(hwnd,IDC_FILELIST),name);
				}
			}
		}
		break;
	case WM_CLOSE:
		if (big)
		{
			BigClose(big);
		}
		EndDialog(hwnd, 0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_FILE_EXIT:
			if (big)
			{
				BigClose(big);
			}
			EndDialog(hwnd, 0);
			break;
		case ID_FILE_OPEN:
			{
				OPENFILENAME ofn;
				char sfile[MAX_PATH] = "";
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFilter = ".BIG Files (*.big)\0*.big\0";
				ofn.lpstrFile = sfile;
				ofn.lpstrTitle = "Open .BIG File";
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrDefExt = ".big";
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT | OFN_NOCHANGEDIR;
				if (GetOpenFileName(&ofn))
				{
					if (big)
					{
						BigClose(big);
					}
					ListBox_ResetContent(GetDlgItem(hwnd,IDC_FILELIST));
					big = BigOpen(sfile);
					for (unsigned int i = 0;i < BigGetNumElements(big);i++)
					{
						const char *name = BigGetFilename(big,i);
						ListBox_AddString(GetDlgItem(hwnd,IDC_FILELIST),name);
					}
				}
			}
			break;
		case ID_EDIT_EXTRACT:
			{
				int sel = ListBox_GetCurSel(GetDlgItem(hwnd,IDC_FILELIST));
				if (sel != LB_ERR)
				{
					const char *path = BigGetFilename(big,sel);
					const char *name = strrchr(path,'\\') + 1;
					OPENFILENAME ofn;
					char sfile[MAX_PATH] = "";
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hwnd;
					strcpy(sfile,name);
					ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
					ofn.lpstrFile = sfile;
					ofn.lpstrTitle = "Extract file";
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
					if (GetSaveFileName(&ofn))
					{
						BIGFileEntry *bfe = BigLoadEntry(big,sel);
						HANDLE h = CreateFile(sfile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
						DWORD x;
						unsigned char *buf;
						for (unsigned int i = 0;i < bfe->un_size;i += 100000000)
						{
							int sz = 100000000;
							if (bfe->un_size - i < 100000000)
							{
								sz = bfe->un_size - i;
							}
							buf = new unsigned char[sz];
							bfe->fc->Read(buf,sz);
							if (!WriteFile(h,buf,sz,&x,NULL))
							{
								CloseHandle(h);
								break;
							}
							delete[] buf;
						}
						CloseHandle(h);
					}
				}
			}
			break;
		case ID_EDIT_EXTRACTALL:
			{
				MessageBox(hwnd,"Please Select the location to extract the contents of the .big file to.","Select Location",MB_OK|MB_ICONQUESTION);
				if (XBrowseForFolder(hwnd,NULL,outputfolder,MAX_PATH))
				{
					for (unsigned int i = 0;i < BigGetNumElements(big);i++)
					{
						BIGFileEntry *bfe = BigLoadEntry(big,i);
						char path[MAX_PATH];
						sprintf(path,"%s\\%s",outputfolder,bfe->name);
						strrchr(path,'\\')[0] = 0;
						SHCreateDirectoryEx(NULL,path,NULL);
						sprintf(path,"%s\\%s",outputfolder,bfe->name);
						HANDLE h = CreateFile(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
						DWORD x;
						unsigned char *buf;
						for (unsigned int ix = 0;ix < bfe->un_size;ix += 100000000)
						{
							int sz = 100000000;
							if (bfe->un_size - ix < 100000000)
							{
								sz = bfe->un_size - ix;
							}
							buf = new unsigned char[sz];
							bfe->fc->Read(buf,sz);
							if (!WriteFile(h,buf,sz,&x,NULL))
							{
								CloseHandle(h);
								break;
							}
							delete[] buf;
						}
						CloseHandle(h);
					}
					MessageBox(hwnd,"Extraction Complete","Extraction Complete",MB_OK);
				}
			}
			break;
		}
		break;
	case WM_SIZE:
		{
			RECT r;
			GetClientRect(hwnd,&r);
			SetWindowPos(GetDlgItem(hwnd,IDC_FILELIST),HWND_TOP,0,0,r.right,r.bottom,0);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	hInst = hInstance;
	if (lpCmdLine)
	{
		cmdline = _strdup(lpCmdLine);
	}
	else
	{
		cmdline = 0;
	}
	big = 0;
	HKEY key;
	RegCreateKeyEx(HKEY_CLASSES_ROOT,".big",0,NULL,0,KEY_ALL_ACCESS,NULL,&key,NULL);
	RegSetValueEx(key,NULL,0,REG_SZ,(BYTE *)"bigfile",(DWORD)strlen("bigfile"));
	RegCloseKey(key);
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"bigfile",0,NULL,0,KEY_ALL_ACCESS,NULL,&key,NULL);
	RegSetValueEx(key,NULL,0,REG_SZ,(BYTE *)".BIG File",(DWORD)strlen("C&C3 Mod Project"));
	RegCloseKey(key);
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"bigfile\\DefaultIcon",0,NULL,0,KEY_ALL_ACCESS,NULL,&key,NULL);
	char exepath[MAX_PATH];
	char regpath[MAX_PATH];
	GetModuleFileName(0,exepath,MAX_PATH);
	sprintf(regpath,"%s,1",exepath);
	RegSetValueEx(key,NULL,0,REG_SZ,(BYTE *)regpath,(DWORD)strlen(regpath));
	RegCloseKey(key);
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"bigfile\\shell",0,NULL,0,KEY_ALL_ACCESS,NULL,&key,NULL);
	RegCloseKey(key);
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"bigfile\\shell\\open",0,NULL,0,KEY_ALL_ACCESS,NULL,&key,NULL);
	RegCloseKey(key);
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"bigfile\\shell\\open\\command",0,NULL,0,KEY_ALL_ACCESS,NULL,&key,NULL);
	sprintf(regpath,"\"%s\" \"%%1\"",exepath);
	RegSetValueEx(key,NULL,0,REG_SZ,(BYTE *)regpath,(DWORD)strlen(regpath));
	RegCloseKey(key);
	accel = LoadAccelerators(hInst,MAKEINTRESOURCE(IDR_BIGEXTRACT));
	hook = SetWindowsHookEx(WH_MSGFILTER, AccelHook, NULL, GetCurrentThreadId());
	DialogBox(hInst, MAKEINTRESOURCE(IDD_EXTRACT), NULL,(DLGPROC) ExtractDlgProc);
	UnhookWindowsHookEx(hook);
	return 0;
}
