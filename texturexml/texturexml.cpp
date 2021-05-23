/*
	Command & Conquer 3 Tools
	texture extractor
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
#include "resource.h"
#include "cc3tools.h"
char xmlfolder[MAX_PATH];
void GenerateXML(char *folder)
{
	WIN32_FIND_DATA fd;
	char tmp[MAX_PATH];
	sprintf(tmp,"%s\\*.tga",folder);
	HANDLE findHandle = FindFirstFile(tmp, &fd);
	if(findHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		char *tmpx = _strdup(fd.cFileName);
		if (!tmpx)
		{
			return;
		}
		strrchr(tmpx,'.')[1] = 'x';
		strrchr(tmpx,'.')[2] = 'm';
		strrchr(tmpx,'.')[3] = 'l';
		sprintf(tmp,"%s\\%s",folder,tmpx);
		FILE *xml = fopen(tmp,"wt");
		if (!xml)
		{
			return;
		}
		strrchr(tmpx,'.')[0] = 0;
		fprintf(xml,"<\?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
		fprintf(xml,"<AssetDeclaration xmlns=\"uri:ea.com:eala:asset\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n");
		fprintf(xml,"  <Texture id=\"%s\" File=\"%s\" />\n",tmpx,fd.cFileName);
		fprintf(xml,"</AssetDeclaration>\n");
		fclose(xml);
	} while(FindNextFile(findHandle, &fd));
}

BOOL CALLBACK ExtractDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_EXTRACT:
			{
				GetDlgItemText(hwnd,IDC_DESTINATION,xmlfolder,MAX_PATH);
				if (strlen(xmlfolder))
				{
					SetDlgItemText(hwnd,IDC_STATUS,"Generating Texture XMLs...");
					GenerateXML(xmlfolder);
					SetDlgItemText(hwnd,IDC_STATUS,"Generation Complete");
				}
			}
			break;
		case IDC_BROWSE:
			XBrowseForFolder(hwnd,NULL,xmlfolder,MAX_PATH);
			SetDlgItemText(hwnd,IDC_DESTINATION,xmlfolder);
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_EXTRACT), NULL,(DLGPROC) ExtractDlgProc);
	return 0;
}
