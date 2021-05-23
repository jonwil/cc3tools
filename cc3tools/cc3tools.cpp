/*
	Command & Conquer 3 Tools
	cc3tools shared dll code
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
#include <shlwapi.h>
#include <shlobj.h>
unsigned long CC3TOOLS_DLL_EXPORT ComputeCC3Hash(const char *input, int length)
{
	unsigned long num_blocks, extra, current_val;
	if(length == 0)
	{
		return 0x1337c0de;
	}
	num_blocks = length / 4;
	extra = length % 4;
	current_val = length;
	while(num_blocks--)
	{
		current_val += input[1]<<8 | input[0];
		input += 4;
		current_val ^= ((input[-1]<<8 | input[-2]) ^ (current_val<<5))<<11;
		current_val += current_val>>11;
	}
	if(extra)
	{
		switch(extra)
		{
			case 1:
				current_val += input[0];
				current_val = (current_val<<10)^current_val;
				current_val += current_val>>1;
				break;
			case 2:
				current_val += input[1]<<8 | input[0];
				current_val ^= (current_val<<11);
				current_val += current_val>>17;
				break;
			case 3:
				current_val += input[1]<<8 | input[0];
				current_val ^= (current_val ^ (input[2]<<2))<<16;
				current_val += current_val>>11;
				break;
		}
	}
	current_val ^= (current_val<<3);
	current_val += (current_val>>5);
	current_val ^= (current_val<<2);
	current_val += (current_val>>15);
	current_val ^= (current_val<<10);
	return current_val;
}

void CC3TOOLS_DLL_EXPORT GetInstallPath(char *installpath,unsigned long size)
{
	HKEY key;
	unsigned long type;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Electronic Arts\\Electronic Arts\\Command and Conquer 3",0,KEY_READ,&key);
	RegQueryValueEx(key,"InstallPath",0,&type,(BYTE *)installpath,&size);
	RegCloseKey(key);
}

void CC3TOOLS_DLL_EXPORT GetLanguage(char *language,unsigned long size)
{
	HKEY key;
	unsigned long type;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Electronic Arts\\Electronic Arts\\Command and Conquer 3",0,KEY_READ,&key);
	RegQueryValueEx(key,"Language",0,&type,(BYTE *)language,&size);
	RegCloseKey(key);
}

void CC3TOOLS_DLL_EXPORT GetModPath(char *modpath,unsigned long size)
{
	char temp[MAX_PATH];
	char temp2[MAX_PATH];
	char temp3[MAX_PATH];
	HKEY key;
	unsigned long type;
	SHGetSpecialFolderPath(NULL,temp,CSIDL_PERSONAL,false);
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Electronic Arts\\Electronic Arts\\Command and Conquer 3",0,KEY_READ,&key);
	RegQueryValueEx(key,"UserDataLeafName",0,&type,(BYTE *)temp2,&size);
	RegCloseKey(key);
	PathCombine(temp3,temp,temp2);
	PathCombine(modpath,temp3,"Mods");
}

void CC3TOOLS_DLL_EXPORT GetMapPath(char *mappath,unsigned long size)
{
	char temp[MAX_PATH];
	char temp2[MAX_PATH];
	char temp3[MAX_PATH];
	HKEY key;
	unsigned long type;
	SHGetSpecialFolderPath(NULL,temp,CSIDL_APPDATA,false);
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Electronic Arts\\Electronic Arts\\Command and Conquer 3",0,KEY_READ,&key);
	RegQueryValueEx(key,"UserDataLeafName",0,&type,(BYTE *)temp2,&size);
	RegCloseKey(key);
	PathCombine(temp3,temp,temp2);
	PathCombine(mappath,temp3,"Maps");
}

void CC3TOOLS_DLL_EXPORT GetSDKPath(char *sdkpath,unsigned long size)
{
	HKEY key;
	unsigned long type;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Electronic Arts\\Electronic Arts\\Command and Conquer 3",0,KEY_ALL_ACCESS,&key);
	if (RegQueryValueEx(key,"ModSDKPath",0,&type,(BYTE *)sdkpath,&size) == ERROR_FILE_NOT_FOUND)
	{
		MessageBox(NULL,"Please Select the location where you installed the MOD SDK, i.e. the folder containing buildmap.bat.\nYou will only need to do this the first time you run this program.","Select SDK Location",MB_OK|MB_ICONQUESTION);
		XBrowseForFolder(NULL,NULL,sdkpath,size);
		type = REG_SZ;
		RegSetValueEx(key,"ModSDKPath",NULL,type,(BYTE *)sdkpath,size);
	}
	RegCloseKey(key);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
