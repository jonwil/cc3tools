/*
	Command & Conquer 3 Tools
	theater of war minimap UI file generator
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
#include "apt_file.h"
#include "resource.h"
#include <shlwapi.h>
#include <shlobj.h>
void makeminimap(unsigned int mapcount,char *side,char *folder)
{
	if (!PathFileExists(folder))
	{
		SHCreateDirectoryEx(NULL,folder,NULL);
	}
	char gf[MAX_PATH];
	if (folder[strlen(folder) - 1] == '\\')
	{
		folder[strlen(folder) - 1] = '\0';
	}
	sprintf(gf,"%s\\TowMiniMap%s_geometry",folder,side);
	if (!PathFileExists(gf))
	{
		SHCreateDirectoryEx(NULL,gf,NULL);
	}
	sprintf(gf,"%s\\art\\textures",folder);
	if (!PathFileExists(gf))
	{
		SHCreateDirectoryEx(NULL,gf,NULL);
	}
	SetCurrentDirectory(folder);
	Movie *m = new Movie;
	m->type = MOVIE;
	m->signature = CHAR_SIG;
	m->pointer = 0;
	m->screensizex = 1024;
	m->screensizey = 768;
	m->unknown = 33;
	m->importcount = 0;
	m->exportcount = 0;
	m->count = 0;
	m->characters.push_back(m);
	m->charactercount = 1;
	m->framecount = 0;
	char aptdat[MAX_PATH];
	char aptapt[MAX_PATH];
	char aptconst[MAX_PATH];
	char geom[MAX_PATH];
	char xml[MAX_PATH];
	sprintf(aptdat,"TowMiniMap%s.dat",side);
	sprintf(aptapt,"TowMiniMap%s.apt",side);
	sprintf(aptconst,"TowMiniMap%s.const",side);
	sprintf(xml,"TowMiniMap%s.xml",side);
	FILE *aptdatfile = fopen(aptdat,"wt");
	if (!aptdatfile)
	{
		return;
	}
	FILE *xmlfile = fopen(xml,"wt");
	if (!xmlfile)
	{
		return;
	}
	AptConstData *c = new AptConstData;
	c->aptdataoffset = 20 + (mapcount * 8);
	c->itemcount = 6;
	AptConstItem *i1 = new AptConstItem;
	i1->type = TYPE_STRING;
	i1->strvalue = "vInitialized";
	c->items.push_back(i1);
	AptConstItem *i2 = new AptConstItem;
	i2->type = TYPE_STRING;
	i2->strvalue = "_root";
	c->items.push_back(i2);
	AptConstItem *i3 = new AptConstItem;
	i3->type = TYPE_STRING;
	i3->strvalue = "Campaign";
	c->items.push_back(i3);
	AptConstItem *i4 = new AptConstItem;
	i4->type = TYPE_STRING;
	i4->strvalue = "UpdateMiniMap";
	c->items.push_back(i4);
	AptConstItem *i5 = new AptConstItem;
	i5->type = TYPE_STRING;
	i5->strvalue = "MiniMap set to";
	c->items.push_back(i5);
	AptConstItem *i6 = new AptConstItem;
	i6->type = TYPE_STRING;
	i6->strvalue = 0;
	c->items.push_back(i6);
	GenerateAptConstFile(c,aptconst);
	fprintf(aptdatfile,"; Created by AptToXML.\n");
	fprintf(xmlfile,"<\?xml version=\'1.0\' encoding=\'utf-8\'\?>\n<AssetDeclaration xmlns=\"uri:ea.com:eala:asset\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n");
	fprintf(xmlfile,"	<AptAptData id=\"TowMiniMap%s_apt\" File=\"TowMiniMap%s.apt\" />\n",side,side);
	fprintf(xmlfile,"	<AptConstData id=\"TowMiniMap%s_const\" File=\"TowMiniMap%s.const\" />\n",side,side);
	fprintf(xmlfile,"	<AptDatData id=\"TowMiniMap%s_dat\" File=\"TowMiniMap%s.dat\" />\n",side,side);
	for (unsigned int i = 0;i < mapcount;i++)
	{
		Frame *f = new Frame;
		f->frameitemcount = 0;
		m->frames.push_back(f);
		m->framecount++;
		Action *a = new Action;
		a->type = ACTION;
		a->ab.constantcount = 0;
		a->ab.stringcount = 0;
		a->ab.actionbytecount = 0;
		if (!i)
		{
			BackgroundColor *b = new BackgroundColor;
			b->type = BACKGROUNDCOLOR;
			b->color = 0xFF000000;
			m->frames[i]->frameitems.push_back(b);
			m->frames[i]->frameitemcount++;
			a->ab.constantcount = 6;
			a->ab.constants.push_back(0x0);
			a->ab.constants.push_back(0x1);
			a->ab.constants.push_back(0x2);
			a->ab.constants.push_back(0x3);
			a->ab.constants.push_back(0x4);
			a->ab.constants.push_back(0x5);
			AddConstantPoolAction(ACTION_CONSTANTPOOL,&a->ab,6);
			AddAction(ACTION_STOP,&a->ab);
			AddByteAction(EA_PUSHVALUEOFVAR,&a->ab,0x00);
			AddAction(EA_PUSHUNDEFINED,&a->ab);
			AddAction(ACTION_NEWEQUALS,&a->ab);
			AddAction(ACTION_LOGICALNOT,&a->ab);
			AddIntAction(ACTION_BRANCHIFTRUE,&a->ab,0x10);
			AddByteAction(EA_PUSHCONSTANT,&a->ab,0x00);
			AddAction(EA_PUSHTRUE,&a->ab);
			AddAction(ACTION_SETVARIABLE,&a->ab);
			AddAction(EA_PUSHZERO,&a->ab);
			AddByteAction(EA_PUSHVALUEOFVAR,&a->ab,0x01);
			AddByteAction(EA_GETNAMEDMEMBER,&a->ab,0x02);
			AddByteAction(EA_CALLNAMEDMETHODPOP,&a->ab,0x03);
			AddIntAction(ACTION_BRANCHALWAYS,&a->ab,0x01);
		}
		AddAction(ACTION_STOP,&a->ab);
		AddAction(ACTION_END,&a->ab);
		m->frames[i]->frameitems.push_back(a);
		m->frames[i]->frameitemcount++;
		FrameLabel *l = new FrameLabel;
		l->type = FRAMELABEL;
		char *x = new char[3];
		sprintf(x,"%d",i);
		l->label = x;
		l->flags = 0x00070000;
		l->frame = i;
		m->frames[i]->frameitems.push_back(l);
		m->frames[i]->frameitemcount++;
		PlaceObject *pl = new PlaceObject;
		pl->type = PLACEOBJECT;
		if (!i)
		{
			pl->flags = 6;
		}
		else
		{
			pl->flags = 3;
		}
		pl->depth = 1;
		pl->character = (i * 2) + 2;
		pl->rotateandscale.m00 = 1.0;
		pl->rotateandscale.m10 = 0;
		pl->rotateandscale.m01 = 0;
		pl->rotateandscale.m11 = 1.0;
		pl->translate.X = 0;
		pl->translate.Y = 0;
		pl->colortransform = 0xFFFFFFFF;
		pl->unknown = 0;
		pl->ratio = 0;
		pl->name = 0;
		pl->clipdepth = 0xFFFFFFFF;
		pl->poa = 0;
		m->frames[i]->frameitems.push_back(pl);
		m->frames[i]->frameitemcount++;
		Image *im = new Image;
		im->type = IMAGE;
		im->signature = CHAR_SIG;
		im->texture = (i * 2) + 1;
		m->characters.push_back(im);
		m->charactercount++;
		Shape *sh = new Shape;
		sh->type = SHAPE;
		sh->signature = CHAR_SIG;
		sh->bounds.top = 0;
		sh->bounds.left = 0;
		sh->bounds.bottom = 165;
		sh->bounds.right = 165;
		sh->geometry = (i * 2) + 2;
		m->characters.push_back(sh);
		m->charactercount++;
		fprintf(aptdatfile,"%d=0 0 165 165\n",(i * 2) + 1);
		sprintf(geom,"TowMiniMap%s_geometry\\%d.ru",side,(i * 2) + 2);
		FILE *geomfile = fopen(geom,"wt");
		if (!geomfile)
		{
			return;
		}
		fprintf(geomfile,"c\n");
		fprintf(geomfile,"s tc:255:255:255:255:%d:1.000015:0.000015:-0.000015:1.000015:0.000000:0.000000\n",(i * 2) + 1);
		fprintf(geomfile,"t 0:165:0:0:165:0\n");
		fprintf(geomfile,"t 165:0:165:165:0:165\n");
		fclose(geomfile);
		fprintf(xmlfile,"	<AptGeometryData id=\"TowMiniMap%s_%d\" File=\"TowMiniMap%s_geometry\\%d.ru\" AptID=\"%d\" />\n",side,(i * 2) + 2,side,(i * 2) + 2,(i * 2) + 2);
		fprintf(xmlfile,"	<Texture id=\"apt_TowMiniMap%s_%d\" File=\"art\\textures\\apt_TowMiniMap%s_%d.tga\" OutputFormat=\"A8R8G8B8\" GenerateMipMaps=\"false\" AllowAutomaticResize=\"false\"/>\n",side,(i * 2) + 1,side,(i * 2) + 1);
	}
	fprintf(aptdatfile,"; Created by AptToXML.\n");
	fprintf(xmlfile,"</AssetDeclaration>\n");
	fclose(aptdatfile);
	fclose(xmlfile);
	GenerateAptAptFile(m,aptapt);
}

BOOL CALLBACK MinimapGeneratorDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_GENERATE:
			{
				char *side = 0;
				if (IsDlgButtonChecked(hwnd,IDC_GDI))
				{
					side = "Gdi";
				}
				if (IsDlgButtonChecked(hwnd,IDC_NOD))
				{
					side = "Nod";
				}
				if (IsDlgButtonChecked(hwnd,IDC_SCRIN))
				{
					side = "Alien";
				}
				char c[MAX_PATH];
				GetDlgItemText(hwnd,IDC_DESTINATION,c,MAX_PATH);
				int towcount = GetDlgItemInt(hwnd,IDC_TOWCOUNT,NULL,false);
				if (strlen(c) && side && towcount)
				{
					makeminimap(towcount,side,c);
				}
			}
			break;
		case IDC_BROWSE:
			{
				char outputfolder[MAX_PATH];
				XBrowseForFolder(hwnd,NULL,outputfolder,MAX_PATH);
				SetDlgItemText(hwnd,IDC_DESTINATION,outputfolder);
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
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MMGEN), NULL,(DLGPROC) MinimapGeneratorDlgProc);
	return 0;
}
