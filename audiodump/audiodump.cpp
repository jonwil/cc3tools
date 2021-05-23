/*
	Command & Conquer 3 Tools
	Audio dumper tool
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

struct AudioFileRefWithWeight {
	int ImportId;
	int Weight;
	float Volume; //percentage
};
struct AudioVolumeSliderMultiplier {
	int Slider; //AudioVolumeSlider enum SOUNDFX, VOICE, MUSIC, AMBIENT, MOVIE, NONE
	float Multiplier;
};
struct AudioEvent {
	void *empty;
	float Volume; //percent
	float VolumeShift; //percent
	float PerFileVolumeShift; //percent
	float MinVolume; //percent
	float ShrunkenPitchModifier;
	float ShrunkenVolumeModifier; //percent
	float PlayPercent; //percent
	int Limit;
	int Priority; //AudioPriority enum LOWEST, LOW, NORMAL, HIGH, HIGHEST, CRITICAL
	int Type; //AudioTypeFlag bitfield UI, WORLD, SHROUDED, VOICE, PLAYER, ALLIES, ENEMIES, EVERYONE
	int Control; //AudioControlFlag bitfield LOOP, SEQUENTIAL, RANDOMSTART, INTERRUPT, SMART_LIMITING, FADE_ON_KILL, FADE_ON_START, ALLOW_KILL_MID_FILE, IMMEDIATE_DECAY_ON_KILL
	float MinRange;
	float MaxRange;
	float LowPassCutoff; //percent
	float ZoomedInOffscreenVolumePercent; //percent
	float ZoomedInOffscreenMinVolumePercent; //percent
	float ZoomedInOffscreenOcclusionPercent; //percent
	float ReverbEffectLevel; //percent
	float DryLevel; //percent
	int *SubmixSlider; //AudioVolumeSlider enum SOUNDFX, VOICE, MUSIC, AMBIENT, MOVIE, NONE
	float *PitchShift; //range
	float *PerFilePitchShift; //range
	int *Delay; //range
	int *InitialDelay; //range
	int VolumeSliderMultiplierCount;
	AudioVolumeSliderMultiplier *VolumeSliderMultiplier;
	float *MinRangeShift; //range
	float *MaxRangeShift; //range
	int LimitGroupCount;
	void *LimitGroup;
	float *NonInterruptibleTime; //time range
	int AttackCount;
	AudioFileRefWithWeight *Attack;
	int SoundCount;
	AudioFileRefWithWeight *Sound;
	int DecayCount;
	AudioFileRefWithWeight *Decay;
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
						if (mf->Assets[mf->Assets.size()-1][i].Header.TypeId == 0x166B084D)
						//if (mf->Assets[mf->Assets.size()-1][i].Header.TypeId == 0x86682E78)
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
						Asset *as1 = &(mf->Assets[mf->Assets.size()-1][i]);
						SetCurrentDirectory(outputfolder);
						StreamLoad(as1);
						char oin[50];
						sprintf(oin,"%s.txt",strchr(as1->Name,':')+1);
						FILE *objectinfo = fopen(oin,"wt");
						char assetname[50];
						sprintf(assetname,"%x.%x.asset",as1->Header.TypeId,as1->Header.InstanceId);
						FILE *f = fopen(assetname,"wb");
						//FILE *f = fopen(assetname,"wt");
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
						/*StreamParseRelocations(as1);
						char *name = strrchr(as1->Name,':') + 1;
						AudioEvent *e = (AudioEvent *)(as1->InstanceData);
						fprintf(f,"<AudioEvent id=\"%s\"\n",name);
						fprintf(f,"Volume=\"%f\"\n",e->Volume * 100);
						fprintf(f,"VolumeShift=\"%f\"\n",e->VolumeShift * 100);
						fprintf(f,"PerFileVolumeShift=\"%f\"\n",e->PerFileVolumeShift * 100);
						fprintf(f,"MinVolume=\"%f\"\n",e->MinVolume * 100);
						fprintf(f,"ShrunkenPitchModifier=\"%f\"\n",e->ShrunkenPitchModifier);
						fprintf(f,"ShrunkenVolumeModifier=\"%f\"\n",e->ShrunkenVolumeModifier * 100);
						fprintf(f,"PlayPercent=\"%f\"\n",e->PlayPercent * 100);
						fprintf(f,"Limit=\"%d\"\n",e->Limit);
						fprintf(f,"Priority=\"%d\"\n",e->Priority);
						fprintf(f,"Type=\"%d\"\n",e->Type);
						fprintf(f,"Control=\"%d\"\n",e->Control);
						fprintf(f,"MinRange=\"%f\"\n",e->MinRange);
						fprintf(f,"MaxRange=\"%f\"\n",e->MaxRange);
						fprintf(f,"LowPassCutoff=\"%f\"\n",e->LowPassCutoff * 100);
						fprintf(f,"ZoomedInOffscreenVolumePercent=\"%f\"\n",e->ZoomedInOffscreenVolumePercent * 100);
						fprintf(f,"ZoomedInOffscreenMinVolumePercent=\"%f\"\n",e->ZoomedInOffscreenMinVolumePercent * 100);
						fprintf(f,"ZoomedInOffscreenOcclusionPercent=\"%f\"\n",e->ZoomedInOffscreenOcclusionPercent * 100);
						fprintf(f,"ReverbEffectLevel=\"%f\"\n",e->ReverbEffectLevel * 100);
						fprintf(f,"DryLevel=\"%f\"\n",e->DryLevel * 100);
						if (e->SubmixSlider)
						{
							fprintf(f,"SubmixSlider=\"%d\"\n",*e->SubmixSlider);
						}
						fprintf(f,">\n");
						if (e->PitchShift)
						{
							fprintf(f,"<PitchShift Low=\"%f\" High=\"%f\" />\n",e->PitchShift[0],e->PitchShift[1]);
						}
						if (e->PerFilePitchShift)
						{
							fprintf(f,"<PerFilePitchShift Low=\"%f\" High=\"%f\" />\n",e->PerFilePitchShift[0],e->PerFilePitchShift[1]);
						}
						if (e->Delay)
						{
							fprintf(f,"<Delay Low=\"%d\" High=\"%d\" />\n",e->Delay[0],e->Delay[1]);
						}
						if (e->InitialDelay)
						{
							fprintf(f,"<InitialDelay Low=\"%d\" High=\"%d\" />\n",e->InitialDelay[0],e->InitialDelay[1]);
						}
						if (e->InitialDelay)
						{
							fprintf(f,"<InitialDelay Low=\"%d\" High=\"%d\" />\n",e->InitialDelay[0],e->InitialDelay[1]);
						}
						if (e->VolumeSliderMultiplierCount)
						{
							fprintf(f,"VolumeSliderMultiplier\n");
						}
						if (e->MinRangeShift)
						{
							fprintf(f,"<MinRangeShift Low=\"%f\" High=\"%f\" />\n",e->MinRangeShift[0],e->MinRangeShift[1]);
						}
						if (e->MaxRangeShift)
						{
							fprintf(f,"<MaxRangeShift Low=\"%f\" High=\"%f\" />\n",e->MaxRangeShift[0],e->MaxRangeShift[1]);
						}
						if (e->NonInterruptibleTime)
						{
							fprintf(f,"<NonInterruptibleTime Low=\"%f\" High=\"%f\" />\n",e->NonInterruptibleTime[0],e->NonInterruptibleTime[1]);
						}
						for (int ix = 0;ix < e->AttackCount;ix++)
						{
							fprintf(f,"<Attack");
							if (e->Attack[ix].Volume * 100 != 100)
							{
								fprintf(f," Volume=\"%f\"",e->Attack[ix].Volume * 100);
							}
							if (e->Attack[ix].Weight != 1000)
							{
								fprintf(f," Weight=\"%d\"",e->Attack[ix].Weight);
							}
							fprintf(f,">%d</Attack>\n",e->Attack[i].ImportId);
						}

						for (int ix = 0;ix < e->SoundCount;ix++)
						{
							fprintf(f,"<Sound");
							if (e->Sound[ix].Volume * 100 != 100)
							{
								fprintf(f," Volume=\"%f\"",e->Sound[ix].Volume * 100);
							}
							if (e->Sound[ix].Weight != 1000)
							{
								fprintf(f," Weight=\"%d\"",e->Sound[ix].Weight);
							}
							fprintf(f,">%d</Sound>\n",e->Sound[ix].ImportId);
						}

						for (int ix = 0;ix < e->DecayCount;ix++)
						{
							fprintf(f,"<Decay");
							if (e->Decay[ix].Volume * 100 != 100)
							{
								fprintf(f," Volume=\"%f\"",e->Decay[ix].Volume * 100);
							}
							if (e->Decay[ix].Weight != 1000)
							{
								fprintf(f," Weight=\"%d\"",e->Decay[ix].Weight);
							}
							fprintf(f,">%d</Decay>\n",e->Decay[ix].ImportId);
						}
						fprintf(f,"</AudioEvent>\n");*/
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
						fclose(objectinfo);
						char cdata[MAX_PATH];
						sprintf(cdata,"%s\\cdata\\%08x.%08x.%08x.%08x.cdata",as1->ManifestName,as1->Header.TypeId,as1->Header.TypeHash,as1->Header.InstanceId,as1->Header.InstanceHash);
						char cdata2[MAX_PATH];
						sprintf(cdata2,"%s\\%08x.%08x.%08x.%08x.cdata",outputfolder,as1->Header.TypeId,as1->Header.TypeHash,as1->Header.InstanceId,as1->Header.InstanceHash);
						CopyFile(cdata,cdata2,false);
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
