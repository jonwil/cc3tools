/*
	Command & Conquer 3 Tools
	Art dumper tool
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
#include <d3d9.h>
#include <d3dx9.h>
#include "w3x_file.h"
#include <algorithm>

char *cmdline;
HINSTANCE hInst;
HACCEL accel;
HHOOK hook;
HWND hdlg;
Manifest *mf;
char outputfolder[MAX_PATH];
unsigned int nv = 0;
unsigned int pc = 0;

#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#define SAFE_DELETE(p) { if (p) { delete (p); (p)=NULL; } }
struct AudioFileRuntime {
	unsigned int zeros;
	unsigned int subtitlelen;
	char *subtitle;
	unsigned int numberofsamples;
	unsigned int samplerate;
	void *headerdata;
	unsigned int headerdatasize;
	unsigned char numberofchannels;
};

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
		}
		break;
	case WM_CLOSE:
		if (mf)
		{
			StreamClose(mf);
		}
		EndDialog(hwnd, 0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_FILE_EXIT:
			if (mf)
			{
				StreamClose(mf);
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
				ofn.lpstrFilter = "Streams (*.manifest)\0*.manifest\0";
				ofn.lpstrFile = sfile;
				ofn.lpstrTitle = "Open Stream";
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrDefExt = ".manifest";
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT | OFN_NOCHANGEDIR;
				if (GetOpenFileName(&ofn))
				{
					if (mf)
					{
						StreamClose(mf);
					}
					ListBox_ResetContent(GetDlgItem(hwnd,IDC_FILELIST));
					strrchr(sfile,'.')[0] = 0;
					mf = StreamOpen(sfile,0,0);
					for (unsigned int i = 0;i < mf->Header[mf->Assets.size()-1]->AssetCount;i++)
					{
						if (mf->Assets[mf->Assets.size()-1][i].Header.TypeId == 0x942FFF2D)
						{
							if (mf->Assets[mf->Assets.size()-1][i].Header.InstanceDataSize)
							{
								const char *name = strchr(mf->Assets[mf->Assets.size()-1][i].Name,':')+1;
								int index = ListBox_AddString(GetDlgItem(hwnd,IDC_FILELIST),name);
								ListBox_SetItemData(GetDlgItem(hwnd,IDC_FILELIST),index,i);
							}
						}
					}
				}
			}
			break;
		case ID_FILE_SAVE:
			{
				int index = ListBox_GetCurSel(GetDlgItem(hwnd,IDC_FILELIST));
				if (index != -1)
				{
					MessageBox(hwnd,"Please Select the location to extract the art to.","Select Location",MB_OK|MB_ICONQUESTION);
					int i = ListBox_GetItemData(GetDlgItem(hwnd,IDC_FILELIST),index);
					if (XBrowseForFolder(hwnd,NULL,outputfolder,MAX_PATH))
					{
						Asset *as = &(mf->Assets[mf->Assets.size()-1][i]);
						std::vector<Asset *> Textures;
						std::vector<Asset *> W3DContainers;
						std::vector<Asset *> W3DAnimations;
						std::vector<Asset *> W3DHierarchies;
						std::vector<Asset *> W3DMeshes;
						std::vector<Asset *> W3DCollisionBoxes;
						std::vector<Asset *> FXLists;
						std::vector<Asset *> FXParticleSystemTemplates;
						for (unsigned int a1 = 0;a1 < as->Header.AssetReferenceCount;a1++)
						{
							if (as->AssetReferences[a1].TypeId == 0x28e7fd7f)
							{
								Asset *as1 = StreamFindAsset(mf,as->AssetReferences[a1].TypeId,as->AssetReferences[a1].InstanceId);
								if (as1)
								{
									std::vector<Asset *>::iterator result = std::find(FXParticleSystemTemplates.begin(),FXParticleSystemTemplates.end(),as1);
									if (result == FXParticleSystemTemplates.end())
									{
										FXParticleSystemTemplates.push_back(as1);
									}
								}
							}
							if (as->AssetReferences[a1].TypeId == 0x86682E78)
							{
								Asset *as1 = StreamFindAsset(mf,as->AssetReferences[a1].TypeId,as->AssetReferences[a1].InstanceId);
								if (as1)
								{
									std::vector<Asset *>::iterator result = std::find(FXLists.begin(),FXLists.end(),as1);
									if (result == FXLists.end())
									{
										FXLists.push_back(as1);
									}
								}
							}
							if (as->AssetReferences[a1].TypeId == 0x21E727DA)
							{
								Asset *as1 = StreamFindAsset(mf,as->AssetReferences[a1].TypeId,as->AssetReferences[a1].InstanceId);
								if (as1)
								{
									std::vector<Asset *>::iterator result = std::find(Textures.begin(),Textures.end(),as1);
									if (result == Textures.end())
									{
										Textures.push_back(as1);
									}
								}
							}
							if (as->AssetReferences[a1].TypeId == 0xf0f08712)
							{
								Asset *as1 = StreamFindAsset(mf,as->AssetReferences[a1].TypeId,as->AssetReferences[a1].InstanceId);
								if (as1)
								{
									std::vector<Asset *>::iterator result = std::find(W3DContainers.begin(),W3DContainers.end(),as1);
									if (result == W3DContainers.end())
									{
										W3DContainers.push_back(as1);
									}
								}
							}
							if (as->AssetReferences[a1].TypeId == 0x2448AE30)
							{
								Asset *as1 = StreamFindAsset(mf,as->AssetReferences[a1].TypeId,as->AssetReferences[a1].InstanceId);
								if (as1)
								{
									std::vector<Asset *>::iterator result = std::find(W3DAnimations.begin(),W3DAnimations.end(),as1);
									if (result == W3DAnimations.end())
									{
										W3DAnimations.push_back(as1);
									}
								}
							}
							if (as->AssetReferences[a1].TypeId == 0x61D7EA40)
							{
								Asset *as1 = StreamFindAsset(mf,as->AssetReferences[a1].TypeId,as->AssetReferences[a1].InstanceId);
								if (as1)
								{
									std::vector<Asset *>::iterator result = std::find(W3DHierarchies.begin(),W3DHierarchies.end(),as1);
									if (result == W3DHierarchies.end())
									{
										W3DHierarchies.push_back(as1);
									}
								}
							}
							if (as->AssetReferences[a1].TypeId == 0xC2B1A262)
							{
								Asset *as1 = StreamFindAsset(mf,as->AssetReferences[a1].TypeId,as->AssetReferences[a1].InstanceId);
								if (as1)
								{
									std::vector<Asset *>::iterator result = std::find(W3DMeshes.begin(),W3DMeshes.end(),as1);
									if (result == W3DMeshes.end())
									{
										W3DMeshes.push_back(as1);
									}
								}
							}
							if (as->AssetReferences[a1].TypeId == 0xE3181C04)
							{
								Asset *as1 = StreamFindAsset(mf,as->AssetReferences[a1].TypeId,as->AssetReferences[a1].InstanceId);
								if (as1)
								{
									std::vector<Asset *>::iterator result = std::find(W3DCollisionBoxes.begin(),W3DCollisionBoxes.end(),as1);
									if (result == W3DCollisionBoxes.end())
									{
										W3DCollisionBoxes.push_back(as1);
									}
								}
							}
						}
						for (unsigned int a1 = 0;a1 < FXLists.size();a1++)
						{
							Asset *as2 = FXLists[a1];
							for (unsigned int a2 = 0;a2 < as2->Header.AssetReferenceCount;a2++)
							{
								if (as2->AssetReferences[a2].TypeId == 0x28e7fd7f)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(FXParticleSystemTemplates.begin(),FXParticleSystemTemplates.end(),as1);
										if (result == FXParticleSystemTemplates.end())
										{
											FXParticleSystemTemplates.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0x21E727DA)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(Textures.begin(),Textures.end(),as1);
										if (result == Textures.end())
										{
											Textures.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xf0f08712)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DContainers.begin(),W3DContainers.end(),as1);
										if (result == W3DContainers.end())
										{
											W3DContainers.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0x2448AE30)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DAnimations.begin(),W3DAnimations.end(),as1);
										if (result == W3DAnimations.end())
										{
											W3DAnimations.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0x61D7EA40)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DHierarchies.begin(),W3DHierarchies.end(),as1);
										if (result == W3DHierarchies.end())
										{
											W3DHierarchies.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xC2B1A262)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DMeshes.begin(),W3DMeshes.end(),as1);
										if (result == W3DMeshes.end())
										{
											W3DMeshes.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xE3181C04)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DCollisionBoxes.begin(),W3DCollisionBoxes.end(),as1);
										if (result == W3DCollisionBoxes.end())
										{
											W3DCollisionBoxes.push_back(as1);
										}
									}
								}
							}
						}
						for (unsigned int a1 = 0;a1 < FXParticleSystemTemplates.size();a1++)
						{
							Asset *as2 = FXParticleSystemTemplates[a1];
							for (unsigned int a2 = 0;a2 < as2->Header.AssetReferenceCount;a2++)
							{
								if (as2->AssetReferences[a2].TypeId == 0x21E727DA)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(Textures.begin(),Textures.end(),as1);
										if (result == Textures.end())
										{
											Textures.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xf0f08712)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DContainers.begin(),W3DContainers.end(),as1);
										if (result == W3DContainers.end())
										{
											W3DContainers.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0x2448AE30)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DAnimations.begin(),W3DAnimations.end(),as1);
										if (result == W3DAnimations.end())
										{
											W3DAnimations.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0x61D7EA40)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DHierarchies.begin(),W3DHierarchies.end(),as1);
										if (result == W3DHierarchies.end())
										{
											W3DHierarchies.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xC2B1A262)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DMeshes.begin(),W3DMeshes.end(),as1);
										if (result == W3DMeshes.end())
										{
											W3DMeshes.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xE3181C04)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DCollisionBoxes.begin(),W3DCollisionBoxes.end(),as1);
										if (result == W3DCollisionBoxes.end())
										{
											W3DCollisionBoxes.push_back(as1);
										}
									}
								}
							}
						}
						for (unsigned int a1 = 0;a1 < W3DContainers.size();a1++)
						{
							Asset *as2 = W3DContainers[a1];
							for (unsigned int a2 = 0;a2 < as2->Header.AssetReferenceCount;a2++)
							{
								if (as2->AssetReferences[a2].TypeId == 0x21E727DA)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(Textures.begin(),Textures.end(),as1);
										if (result == Textures.end())
										{
											Textures.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xf0f08712)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DContainers.begin(),W3DContainers.end(),as1);
										if (result == W3DContainers.end())
										{
											W3DContainers.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0x2448AE30)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DAnimations.begin(),W3DAnimations.end(),as1);
										if (result == W3DAnimations.end())
										{
											W3DAnimations.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0x61D7EA40)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DHierarchies.begin(),W3DHierarchies.end(),as1);
										if (result == W3DHierarchies.end())
										{
											W3DHierarchies.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xC2B1A262)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DMeshes.begin(),W3DMeshes.end(),as1);
										if (result == W3DMeshes.end())
										{
											W3DMeshes.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xE3181C04)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DCollisionBoxes.begin(),W3DCollisionBoxes.end(),as1);
										if (result == W3DCollisionBoxes.end())
										{
											W3DCollisionBoxes.push_back(as1);
										}
									}
								}
							}
						}
						for (unsigned int a1 = 0;a1 < W3DMeshes.size();a1++)
						{
							Asset *as2 = W3DMeshes[a1];
							for (unsigned int a2 = 0;a2 < as2->Header.AssetReferenceCount;a2++)
							{
								if (as2->AssetReferences[a2].TypeId == 0x21E727DA)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(Textures.begin(),Textures.end(),as1);
										if (result == Textures.end())
										{
											Textures.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xf0f08712)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DContainers.begin(),W3DContainers.end(),as1);
										if (result == W3DContainers.end())
										{
											W3DContainers.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0x2448AE30)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DAnimations.begin(),W3DAnimations.end(),as1);
										if (result == W3DAnimations.end())
										{
											W3DAnimations.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0x61D7EA40)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DHierarchies.begin(),W3DHierarchies.end(),as1);
										if (result == W3DHierarchies.end())
										{
											W3DHierarchies.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xC2B1A262)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DMeshes.begin(),W3DMeshes.end(),as1);
										if (result == W3DMeshes.end())
										{
											W3DMeshes.push_back(as1);
										}
									}
								}
								if (as2->AssetReferences[a2].TypeId == 0xE3181C04)
								{
									Asset *as1 = StreamFindAsset(mf,as2->AssetReferences[a2].TypeId,as2->AssetReferences[a2].InstanceId);
									if (as1)
									{
										std::vector<Asset *>::iterator result = std::find(W3DCollisionBoxes.begin(),W3DCollisionBoxes.end(),as1);
										if (result == W3DCollisionBoxes.end())
										{
											W3DCollisionBoxes.push_back(as1);
										}
									}
								}
							}
						}
						SetCurrentDirectory(outputfolder);
						char oin[50];
						sprintf(oin,"%s.txt",strchr(as->Name,':')+1);
						FILE *objectinfo = fopen(oin,"wt");
						for (unsigned int a1 = 0;a1 < Textures.size();a1++)
						{
							Asset *as1 = Textures[a1];
							if (StreamLoad(as1))
							{
								char assetname[50];
								sprintf(assetname,"%x.%x.asset",as1->Header.TypeId,as1->Header.InstanceId);
								FILE *f = fopen(assetname,"wb");
								fwrite(&(as1->Header.TypeId),4,1,f);
								fwrite(&(as1->Header.InstanceId),4,1,f);
								fwrite(&(as1->Header.TypeHash),4,1,f);
								fwrite(&(as1->Header.InstanceHash),4,1,f);
								fwrite(&(as1->Header.AssetReferenceCount),4,1,f);
								fwrite(&(as1->Header.InstanceDataSize),4,1,f);
								fwrite(&(as1->Header.RelocationDataSize),4,1,f);
								fwrite(&(as1->Header.ImportsDataSize),4,1,f);
								if (as1->Header.AssetReferenceCount)
								{
									fwrite(as1->AssetReferences,as1->Header.AssetReferenceCount*sizeof(AssetId),1,f);
								}
								int nl = strlen(as1->Name);
								fwrite(&nl,4,1,f);
								fwrite(as1->Name,nl,1,f);
								int snl = strlen(as1->SourceFileName);
								fwrite(&snl,4,1,f);
								fwrite(as1->SourceFileName,snl,1,f);
								fwrite(as1->InstanceData,as1->Header.InstanceDataSize,1,f);
								if (as1->Header.RelocationDataSize)
								{
									fwrite(as1->RelocationData,as1->Header.RelocationDataSize,1,f);
								}
								if (as1->Header.ImportsDataSize)
								{
									fwrite(as1->ImportsData,as1->Header.ImportsDataSize,1,f);
								}
								fclose(f);
								fprintf(objectinfo,"%s\n",as1->Name);
								fprintf(objectinfo,"%s\n",assetname);
							}
						}

						for (unsigned int a1 = 0;a1 < W3DContainers.size();a1++)
						{
							Asset *as1 = W3DContainers[a1];
							if (StreamLoad(as1))
							{
								char assetname[50];
								sprintf(assetname,"%x.%x.asset",as1->Header.TypeId,as1->Header.InstanceId);
								FILE *f = fopen(assetname,"wb");
								fwrite(&(as1->Header.TypeId),4,1,f);
								fwrite(&(as1->Header.InstanceId),4,1,f);
								fwrite(&(as1->Header.TypeHash),4,1,f);
								fwrite(&(as1->Header.InstanceHash),4,1,f);
								fwrite(&(as1->Header.AssetReferenceCount),4,1,f);
								fwrite(&(as1->Header.InstanceDataSize),4,1,f);
								fwrite(&(as1->Header.RelocationDataSize),4,1,f);
								fwrite(&(as1->Header.ImportsDataSize),4,1,f);
								if (as1->Header.AssetReferenceCount)
								{
									fwrite(as1->AssetReferences,as1->Header.AssetReferenceCount*sizeof(AssetId),1,f);
								}
								int nl = strlen(as1->Name);
								fwrite(&nl,4,1,f);
								fwrite(as1->Name,nl,1,f);
								int snl = strlen(as1->SourceFileName);
								fwrite(&snl,4,1,f);
								fwrite(as1->SourceFileName,snl,1,f);
								fwrite(as1->InstanceData,as1->Header.InstanceDataSize,1,f);
								if (as1->Header.RelocationDataSize)
								{
									fwrite(as1->RelocationData,as1->Header.RelocationDataSize,1,f);
								}
								if (as1->Header.ImportsDataSize)
								{
									fwrite(as1->ImportsData,as1->Header.ImportsDataSize,1,f);
								}
								StreamParseRelocations(as1);
								fclose(f);
								fprintf(objectinfo,"%s\n",as1->Name);
								fprintf(objectinfo,"%s\n",assetname);
								W3DContainer *w3dc = (W3DContainer *)as1->InstanceData;
								for (unsigned int so = 0;so < w3dc->SubObjectCount;so++)
								{
									char *bn = new char[w3dc->SubObjects[so].SubObjectNameLength+1];
									memset(bn,0,w3dc->SubObjects[so].SubObjectNameLength+1);
									memcpy(bn,w3dc->SubObjects[so].SubObjectName,w3dc->SubObjects[so].SubObjectNameLength);
									fprintf(objectinfo,"subobject %s bone %d\n",bn,w3dc->SubObjects[so].BoneIndex);
								}
							}
						}

						for (unsigned int a1 = 0;a1 < W3DAnimations.size();a1++)
						{
							Asset *as1 = W3DAnimations[a1];
							if (StreamLoad(as1))
							{
								char assetname[50];
								sprintf(assetname,"%x.%x.asset",as1->Header.TypeId,as1->Header.InstanceId);
								FILE *f = fopen(assetname,"wb");
								fwrite(&(as1->Header.TypeId),4,1,f);
								fwrite(&(as1->Header.InstanceId),4,1,f);
								fwrite(&(as1->Header.TypeHash),4,1,f);
								fwrite(&(as1->Header.InstanceHash),4,1,f);
								fwrite(&(as1->Header.AssetReferenceCount),4,1,f);
								fwrite(&(as1->Header.InstanceDataSize),4,1,f);
								fwrite(&(as1->Header.RelocationDataSize),4,1,f);
								fwrite(&(as1->Header.ImportsDataSize),4,1,f);
								if (as1->Header.AssetReferenceCount)
								{
									fwrite(as1->AssetReferences,as1->Header.AssetReferenceCount*sizeof(AssetId),1,f);
								}
								int nl = strlen(as1->Name);
								fwrite(&nl,4,1,f);
								fwrite(as1->Name,nl,1,f);
								int snl = strlen(as1->SourceFileName);
								fwrite(&snl,4,1,f);
								fwrite(as1->SourceFileName,snl,1,f);
								fwrite(as1->InstanceData,as1->Header.InstanceDataSize,1,f);
								if (as1->Header.RelocationDataSize)
								{
									fwrite(as1->RelocationData,as1->Header.RelocationDataSize,1,f);
								}
								if (as1->Header.ImportsDataSize)
								{
									fwrite(as1->ImportsData,as1->Header.ImportsDataSize,1,f);
								}
								StreamParseRelocations(as1);
								fclose(f);
								fprintf(objectinfo,"%s\n",as1->Name);
								fprintf(objectinfo,"%s\n",assetname);
							}
						}

						for (unsigned int a1 = 0;a1 < W3DHierarchies.size();a1++)
						{
							Asset *as1 = W3DHierarchies[a1];
							if (StreamLoad(as1))
							{
								char assetname[50];
								sprintf(assetname,"%x.%x.asset",as1->Header.TypeId,as1->Header.InstanceId);
								FILE *f = fopen(assetname,"wb");
								fwrite(&(as1->Header.TypeId),4,1,f);
								fwrite(&(as1->Header.InstanceId),4,1,f);
								fwrite(&(as1->Header.TypeHash),4,1,f);
								fwrite(&(as1->Header.InstanceHash),4,1,f);
								fwrite(&(as1->Header.AssetReferenceCount),4,1,f);
								fwrite(&(as1->Header.InstanceDataSize),4,1,f);
								fwrite(&(as1->Header.RelocationDataSize),4,1,f);
								fwrite(&(as1->Header.ImportsDataSize),4,1,f);
								if (as1->Header.AssetReferenceCount)
								{
									fwrite(as1->AssetReferences,as1->Header.AssetReferenceCount*sizeof(AssetId),1,f);
								}
								int nl = strlen(as1->Name);
								fwrite(&nl,4,1,f);
								fwrite(as1->Name,nl,1,f);
								int snl = strlen(as1->SourceFileName);
								fwrite(&snl,4,1,f);
								fwrite(as1->SourceFileName,snl,1,f);
								fwrite(as1->InstanceData,as1->Header.InstanceDataSize,1,f);
								if (as1->Header.RelocationDataSize)
								{
									fwrite(as1->RelocationData,as1->Header.RelocationDataSize,1,f);
								}
								if (as1->Header.ImportsDataSize)
								{
									fwrite(as1->ImportsData,as1->Header.ImportsDataSize,1,f);
								}
								StreamParseRelocations(as1);
								fclose(f);
								fprintf(objectinfo,"%s\n",as1->Name);
								fprintf(objectinfo,"%s\n",assetname);
							}
						}

						for (unsigned int a1 = 0;a1 < W3DMeshes.size();a1++)
						{
							Asset *as1 = W3DMeshes[a1];
							if (StreamLoad(as1))
							{
								char assetname[50];
								sprintf(assetname,"%x.%x.asset",as1->Header.TypeId,as1->Header.InstanceId);
								FILE *f = fopen(assetname,"wb");
								fwrite(&(as1->Header.TypeId),4,1,f);
								fwrite(&(as1->Header.InstanceId),4,1,f);
								fwrite(&(as1->Header.TypeHash),4,1,f);
								fwrite(&(as1->Header.InstanceHash),4,1,f);
								fwrite(&(as1->Header.AssetReferenceCount),4,1,f);
								fwrite(&(as1->Header.InstanceDataSize),4,1,f);
								fwrite(&(as1->Header.RelocationDataSize),4,1,f);
								fwrite(&(as1->Header.ImportsDataSize),4,1,f);
								if (as1->Header.AssetReferenceCount)
								{
									fwrite(as1->AssetReferences,as1->Header.AssetReferenceCount*sizeof(AssetId),1,f);
								}
								int nl = strlen(as1->Name);
								fwrite(&nl,4,1,f);
								fwrite(as1->Name,nl,1,f);
								int snl = strlen(as1->SourceFileName);
								fwrite(&snl,4,1,f);
								fwrite(as1->SourceFileName,snl,1,f);
								fwrite(as1->InstanceData,as1->Header.InstanceDataSize,1,f);
								if (as1->Header.RelocationDataSize)
								{
									fwrite(as1->RelocationData,as1->Header.RelocationDataSize,1,f);
								}
								if (as1->Header.ImportsDataSize)
								{
									fwrite(as1->ImportsData,as1->Header.ImportsDataSize,1,f);
								}
								StreamParseRelocations(as1);
								fclose(f);
								fprintf(objectinfo,"%s\n",as1->Name);
								fprintf(objectinfo,"%s\n",assetname);
							}
						}

						for (unsigned int a1 = 0;a1 < W3DCollisionBoxes.size();a1++)
						{
							Asset *as1 = W3DCollisionBoxes[a1];
							if (StreamLoad(as1))
							{
								char assetname[50];
								sprintf(assetname,"%x.%x.asset",as1->Header.TypeId,as1->Header.InstanceId);
								FILE *f = fopen(assetname,"wb");
								fwrite(&(as1->Header.TypeId),4,1,f);
								fwrite(&(as1->Header.InstanceId),4,1,f);
								fwrite(&(as1->Header.TypeHash),4,1,f);
								fwrite(&(as1->Header.InstanceHash),4,1,f);
								fwrite(&(as1->Header.AssetReferenceCount),4,1,f);
								fwrite(&(as1->Header.InstanceDataSize),4,1,f);
								fwrite(&(as1->Header.RelocationDataSize),4,1,f);
								fwrite(&(as1->Header.ImportsDataSize),4,1,f);
								if (as1->Header.AssetReferenceCount)
								{
									fwrite(as1->AssetReferences,as1->Header.AssetReferenceCount*sizeof(AssetId),1,f);
								}
								int nl = strlen(as1->Name);
								fwrite(&nl,4,1,f);
								fwrite(as1->Name,nl,1,f);
								int snl = strlen(as1->SourceFileName);
								fwrite(&snl,4,1,f);
								fwrite(as1->SourceFileName,snl,1,f);
								fwrite(as1->InstanceData,as1->Header.InstanceDataSize,1,f);
								if (as1->Header.RelocationDataSize)
								{
									fwrite(as1->RelocationData,as1->Header.RelocationDataSize,1,f);
								}
								if (as1->Header.ImportsDataSize)
								{
									fwrite(as1->ImportsData,as1->Header.ImportsDataSize,1,f);
								}
								StreamParseRelocations(as1);
								fclose(f);
								fprintf(objectinfo,"%s\n",as1->Name);
								fprintf(objectinfo,"%s\n",assetname);
							}
						}
						fclose(objectinfo);
					}
				}
			}
			break;
		case IDC_FILELIST:
			{
				if (HIWORD(wParam) == LBN_SELCHANGE)
				{
					EnableMenuItem(GetMenu(hwnd),ID_FILE_SAVE,MF_GRAYED);
					int index = ListBox_GetCurSel(GetDlgItem(hwnd,IDC_FILELIST));
					if (index != -1)
					{
						EnableMenuItem(GetMenu(hwnd),ID_FILE_SAVE,MF_ENABLED);
					}
				}
			}
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
	hInst = hInstance;
	if (lpCmdLine)
	{
		cmdline = _strdup(lpCmdLine);
	}
	else
	{
		cmdline = 0;
	}
	mf = 0;
	accel = LoadAccelerators(hInst,MAKEINTRESOURCE(IDR_BIGEXTRACT));
	hook = SetWindowsHookEx(WH_MSGFILTER, AccelHook, NULL, GetCurrentThreadId());
	DialogBox(hInst, MAKEINTRESOURCE(IDD_EXTRACT), NULL,(DLGPROC) ExtractDlgProc);
	UnhookWindowsHookEx(hook);
	return 0;
}
