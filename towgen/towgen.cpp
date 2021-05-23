/*
	Command & Conquer 3 Tools
	theater of war UI file generator
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
#include <windowsx.h>
struct MissionPos {
	char *textpos;
	float x;
	float y;
	float ratio;
	unsigned int frame;
	unsigned int depth;
};
std::vector<MissionPos *> missions;
void maketowmap(unsigned int townum,char *side,char *folder)
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
	sprintf(gf,"%s\\Tow%s_%d_geometry",folder,side,townum);
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
	char aptdat[MAX_PATH];
	char aptapt[MAX_PATH];
	char aptconst[MAX_PATH];
	char geom[MAX_PATH];
	char xml[MAX_PATH];
	sprintf(aptdat,"Tow%s_%d.dat",side,townum);
	sprintf(aptapt,"Tow%s_%d.apt",side,townum);
	sprintf(aptconst,"Tow%s_%d.const",side,townum);
	sprintf(xml,"Tow%s_%d.xml",side,townum);
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
	fprintf(aptdatfile,"; Created by AptToXML.\n");
	fprintf(aptdatfile,"; Created by AptToXML.\n");
	fprintf(aptdatfile,"8->2\n");
	fprintf(aptdatfile,"12->3\n");
	fprintf(aptdatfile,"5->1\n");
	fclose(aptdatfile);
	AptConstData *c = new AptConstData;
	c->aptdataoffset = 0x74;
	c->itemcount = 8;
	AptConstItem *i1 = new AptConstItem;
	i1->type = TYPE_STRING;
	i1->strvalue = "gotoLocation";
	c->items.push_back(i1);
	AptConstItem *i2 = new AptConstItem;
	i2->type = TYPE_STRING;
	i2->strvalue = "_buttons";
	c->items.push_back(i2);
	AptConstItem *i3 = new AptConstItem;
	i3->type = TYPE_STRING;
	i3->strvalue = "this";
	c->items.push_back(i3);
	AptConstItem *i4 = new AptConstItem;
	i4->type = TYPE_STRING;
	i4->strvalue = "_root";
	c->items.push_back(i4);
	AptConstItem *i5 = new AptConstItem;
	i5->type = TYPE_STRING;
	i5->strvalue = "_parent";
	c->items.push_back(i5);
	AptConstItem *i6 = new AptConstItem;
	i6->type = TYPE_STRING;
	i6->strvalue = "vGotoLocation";
	c->items.push_back(i6);
	AptConstItem *i7 = new AptConstItem;
	i7->type = TYPE_STRING;
	i7->strvalue = "_name";
	c->items.push_back(i7);
	AptConstItem *i8 = new AptConstItem;
	i8->type = TYPE_STRING;
	i8->strvalue = " Go: ";
	c->items.push_back(i8);
	GenerateAptConstFile(c,aptconst);
	fprintf(xmlfile,"<\?xml version=\'1.0\' encoding=\'utf-8\'\?>\n<AssetDeclaration xmlns=\"uri:ea.com:eala:asset\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n");
	fprintf(xmlfile,"	<AptAptData id=\"Tow%s_%d_apt\" File=\"Tow%s_%d.apt\" />\n",side,townum,side,townum);
	fprintf(xmlfile,"	<AptConstData id=\"Tow%s_%d_const\" File=\"Tow%s_%d.const\" />\n",side,townum,side,townum);
	fprintf(xmlfile,"	<AptDatData id=\"Tow%s_%d_dat\" File=\"Tow%s_%d.dat\" />\n",side,townum,side,townum);
	fprintf(xmlfile,"	<AptGeometryData id=\"Tow%s_%d_13\" File=\"Tow%s_%d_geometry\\13.ru\" AptID=\"13\"/>\n",side,townum,side,townum);
	fprintf(xmlfile,"	<AptGeometryData id=\"Tow%s_%d_6\" File=\"Tow%s_%d_geometry\\6.ru\" AptID=\"6\"/>\n",side,townum,side,townum);
	fprintf(xmlfile,"	<AptGeometryData id=\"Tow%s_%d_9\" File=\"Tow%s_%d_geometry\\9.ru\" AptID=\"9\"/>\n",side,townum,side,townum);
	fprintf(xmlfile,"	<Texture id=\"apt_Tow%s_%d_1\" File=\"art\\Textures\\apt_Tow%s_%d_1.tga\" OutputFormat=\"A8R8G8B8\" GenerateMipMaps=\"false\" AllowAutomaticResize=\"false\"/>\n",side,townum,side,townum);
	fprintf(xmlfile,"	<Texture id=\"apt_Tow%s_%d_2\" File=\"art\\Textures\\apt_Tow%s_%d_2.tga\" OutputFormat=\"A8R8G8B8\" GenerateMipMaps=\"false\" AllowAutomaticResize=\"false\"/>\n",side,townum,side,townum);
	fprintf(xmlfile,"	<Texture id=\"apt_Tow%s_%d_3\" File=\"art\\Textures\\apt_Tow%s_%d_3.tga\" OutputFormat=\"A8R8G8B8\" GenerateMipMaps=\"false\" AllowAutomaticResize=\"false\"/>\n",side,townum,side,townum);
	fprintf(xmlfile,"</AssetDeclaration>\n");
	fclose(xmlfile);
	Movie *m = new Movie;
	m->type = MOVIE;
	m->signature = CHAR_SIG;
	m->pointer = 0;
	m->screensizex = 1024;
	m->screensizey = 768;
	m->unknown = 33;
	m->importcount = 1;
	Import *im = new Import;
	im->character = 1;
	im->movie = "TowButtons";
	im->name = "TowButton";
	im->pointer = 0;
	m->imports.push_back(im);
	m->exportcount = 0;
	m->count = 0;
	m->characters.push_back(m);
	m->charactercount = 1;
	m->framecount = 0;
	RemoveObject *ro;
	Frame *f;
	Action *a;
	BackgroundColor *b;
	PlaceObject *pl;
	FrameLabel *l;
	Sprite *sp;
	Image *ima;
	Shape *sh;

	//frame 0
	f = new Frame;
	f->frameitemcount = 0;
	m->frames.push_back(f);
	m->framecount++;
	b = new BackgroundColor;
	b->type = BACKGROUNDCOLOR;
	b->color = 0xFF000000;
	f->frameitems.push_back(b);
	f->frameitemcount++;
	a = new Action;
	a->type = ACTION;
	a->ab.constantcount = 0;
	a->ab.stringcount = 0;
	a->ab.actionbytecount = 0;
	AddStringAction(EA_GETSTRINGVAR,&a->ab,"PreLoadButtons");
	AddStringAction(EA_PUSHSTRING,&a->ab,"_visible");
	AddAction(EA_PUSHFALSE,&a->ab);
	AddAction(ACTION_SETMEMBER,&a->ab);
	AddAction(ACTION_END,&a->ab);
	f->frameitems.push_back(a);
	f->frameitemcount++;
	pl = new PlaceObject;
	pl->type = PLACEOBJECT;
	pl->flags = 166;
	pl->depth = 1;
	pl->character = 1;
	pl->rotateandscale.m00 = 1.0;
	pl->rotateandscale.m10 = 0;
	pl->rotateandscale.m01 = 0;
	pl->rotateandscale.m11 = 1.0;
	pl->translate.X = (float)45.099998;
	pl->translate.Y = (float)47.150002;
	pl->colortransform = 0xFFFFFFFF;
	pl->unknown = 0;
	pl->ratio = 0;
	pl->name = "PreLoadButtons";
	pl->clipdepth = 0xFFFFFFFF;
	pl->poa = new PlaceObjectActions;
	pl->poa->clipactioncount = 1;
	PlaceObjectAction *poa = new PlaceObjectAction;
	pl->poa->actions.push_back(poa);
	pl->poa->actions[0]->flags = 0x00040000;
	pl->poa->actions[0]->flags2 = 0;
	pl->poa->actions[0]->actiondata = new Action;
	pl->poa->actions[0]->actiondata->ab.constantcount = 0;
	pl->poa->actions[0]->actiondata->ab.stringcount = 0;
	pl->poa->actions[0]->actiondata->ab.actionbytecount = 0;
	AddStringAction(EA_PUSHSTRING,&pl->poa->actions[0]->actiondata->ab,"TextPosition");
	AddStringAction(EA_SETSTRINGVAR,&pl->poa->actions[0]->actiondata->ab,"LeftCenter");
	AddAction(ACTION_END,&pl->poa->actions[0]->actiondata->ab);
	f->frameitems.push_back(pl);
	f->frameitemcount++;

	//frame 1
	f = new Frame;
	f->frameitemcount = 0;
	m->frames.push_back(f);
	m->framecount++;
	ro = new RemoveObject;
	ro->type = REMOVEOBJECT;
	ro->depth = 1;
	f->frameitems.push_back(ro);
	f->frameitemcount++;
	a = new Action;
	a->type = ACTION;
	a->ab.constantcount = 0;
	a->ab.stringcount = 0;
	a->ab.actionbytecount = 0;
	a->ab.constantcount = 8;
	a->ab.constants.push_back(0x0);
	a->ab.constants.push_back(0x1);
	a->ab.constants.push_back(0x2);
	a->ab.constants.push_back(0x3);
	a->ab.constants.push_back(0x4);
	a->ab.constants.push_back(0x5);
	a->ab.constants.push_back(0x6);
	a->ab.constants.push_back(0x7);
	AddConstantPoolAction(ACTION_CONSTANTPOOL,&a->ab,8);
	AddByteAction(EA_PUSHCONSTANT,&a->ab,0x0);
	AddByteAction(EA_PUSHCONSTANT,&a->ab,0x1);
	AddAction(ACTION_DEFINELOCAL,&a->ab);
	AddAction(EA_PUSHTHIS,&a->ab);
	AddByteAction(EA_PUSHVALUEOFVAR,&a->ab,0x03);
	AddAction(ACTION_NEWEQUALS,&a->ab);
	AddAction(ACTION_LOGICALNOT,&a->ab);
	AddAction(ACTION_LOGICALNOT,&a->ab);
	AddIntAction(ACTION_BRANCHIFTRUE,&a->ab,7);
	AddByteAction(EA_PUSHCONSTANT,&a->ab,0x0);
	AddByteAction(EA_PUSHVALUEOFVAR,&a->ab,0x4);
	AddByteAction(EA_GETNAMEDMEMBER,&a->ab,0x5);
	AddAction(ACTION_SETVARIABLE,&a->ab);
	AddByteAction(EA_PUSHVALUEOFVAR,&a->ab,0x4);
	AddByteAction(EA_GETNAMEDMEMBER,&a->ab,0x6);
	AddByteAction(EA_PUSHCONSTANT,&a->ab,0x7);
	AddAction(ACTION_NEWADD,&a->ab);
	AddByteAction(EA_PUSHVALUEOFVAR,&a->ab,0x0);
	AddAction(ACTION_NEWADD,&a->ab);
	AddAction(ACTION_TRACE,&a->ab);
	AddByteAction(EA_PUSHVALUEOFVAR,&a->ab,0x0);
	AddIntAction(ACTION_GOTOEXPRESSION,&a->ab,0x1);
	AddAction(ACTION_END,&a->ab);
	f->frameitems.push_back(a);
	f->frameitemcount++;

	//frame 2
	f = new Frame;
	f->frameitemcount = 0;
	m->frames.push_back(f);
	m->framecount++;
	a = new Action;
	a->type = ACTION;
	a->ab.constantcount = 0;
	a->ab.stringcount = 0;
	a->ab.actionbytecount = 0;
	AddStringAction(EA_GETSTRINGVAR,&a->ab,"_parent");
	AddStringAction(EA_GETSTRINGMEMBER,&a->ab,"_name");
	AddStringAction(EA_PUSHSTRING,&a->ab," In: _buttons");
	AddAction(ACTION_NEWADD,&a->ab);
	AddAction(ACTION_TRACE,&a->ab);
	AddAction(ACTION_STOP,&a->ab);
	AddAction(ACTION_END,&a->ab);
	f->frameitems.push_back(a);
	f->frameitemcount++;
	l = new FrameLabel;
	l->type = FRAMELABEL;
	l->label = "_buttons";
	l->frame = 2;
	l->flags = 0x70000;
	f->frameitems.push_back(l);
	f->frameitemcount++;
	pl = new PlaceObject;
	pl->type = PLACEOBJECT;
	pl->flags = 54;
	pl->depth = 1;
	pl->character = 4;
	pl->rotateandscale.m00 = (float)1.000076;
	pl->rotateandscale.m10 = 0;
	pl->rotateandscale.m01 = 0;
	pl->rotateandscale.m11 = (float)1.000290;
	pl->translate.X = 0;
	pl->translate.Y = 0;
	pl->colortransform = 0xFFFFFFFF;
	pl->unknown = 0;
	pl->ratio = (float)0.000031;
	pl->name = "Buttons";
	pl->clipdepth = 0xFFFFFFFF;
	pl->poa = 0;
	f->frameitems.push_back(pl);
	f->frameitemcount++;

	//frame 3
	m->frames.push_back(0);
	m->framecount++;

	//frame 4
	m->frames.push_back(0);
	m->framecount++;

	//frame 5
	m->frames.push_back(0);
	m->framecount++;

	//frame 6
	m->frames.push_back(0);
	m->framecount++;

	//frame 7
	m->frames.push_back(0);
	m->framecount++;

	//frame 8
	m->frames.push_back(0);
	m->framecount++;

	//frame 9
	m->frames.push_back(0);
	m->framecount++;

	//frame 10
	f = new Frame;
	f->frameitemcount = 0;
	m->frames.push_back(f);
	m->framecount++;
	ro = new RemoveObject;
	ro->type = REMOVEOBJECT;
	ro->depth = 1;
	f->frameitems.push_back(ro);
	f->frameitemcount++;
	a = new Action;
	a->type = ACTION;
	a->ab.constantcount = 0;
	a->ab.stringcount = 0;
	a->ab.actionbytecount = 0;
	AddStringAction(EA_GETSTRINGVAR,&a->ab,"_parent");
	AddStringAction(EA_GETSTRINGMEMBER,&a->ab,"_name");
	AddStringAction(EA_PUSHSTRING,&a->ab," In: _startMap");
	AddAction(ACTION_NEWADD,&a->ab);
	AddAction(ACTION_TRACE,&a->ab);
	AddAction(ACTION_STOP,&a->ab);
	AddAction(ACTION_END,&a->ab);
	f->frameitems.push_back(a);
	f->frameitemcount++;
	l = new FrameLabel;
	l->type = FRAMELABEL;
	l->label = "_startMap";
	l->frame = 10;
	l->flags = 0x70000;
	f->frameitems.push_back(l);
	f->frameitemcount++;
	pl = new PlaceObject;
	pl->type = PLACEOBJECT;
	pl->flags = 22;
	pl->depth = 1;
	pl->character = 7;
	pl->rotateandscale.m00 = 1.0;
	pl->rotateandscale.m10 = 0;
	pl->rotateandscale.m01 = 0;
	pl->rotateandscale.m11 = 1.0;
	pl->translate.X = 0;
	pl->translate.Y = 0;
	pl->colortransform = 0xFFFFFFFF;
	pl->unknown = 0;
	pl->ratio = (float)0.000153;
	pl->name = 0;
	pl->clipdepth = 0xFFFFFFFF;
	pl->poa = 0;
	f->frameitems.push_back(pl);
	f->frameitemcount++;

	//frame 11
	m->frames.push_back(0);
	m->framecount++;

	//frame 12
	m->frames.push_back(0);
	m->framecount++;

	//frame 13
	m->frames.push_back(0);
	m->framecount++;

	//frame 14
	m->frames.push_back(0);
	m->framecount++;

	//frame 15
	m->frames.push_back(0);
	m->framecount++;

	//frame 16
	m->frames.push_back(0);
	m->framecount++;

	//frame 17
	m->frames.push_back(0);
	m->framecount++;

	//frame 18
	f = new Frame;
	f->frameitemcount = 0;
	m->frames.push_back(f);
	m->framecount++;
	ro = new RemoveObject;
	ro->type = REMOVEOBJECT;
	ro->depth = 1;
	f->frameitems.push_back(ro);
	f->frameitemcount++;
	a = new Action;
	a->type = ACTION;
	a->ab.constantcount = 0;
	a->ab.stringcount = 0;
	a->ab.actionbytecount = 0;
	AddStringAction(EA_GETSTRINGVAR,&a->ab,"_parent");
	AddStringAction(EA_GETSTRINGMEMBER,&a->ab,"_name");
	AddStringAction(EA_PUSHSTRING,&a->ab," In: _radarMap");
	AddAction(ACTION_NEWADD,&a->ab);
	AddAction(ACTION_TRACE,&a->ab);
	AddAction(ACTION_STOP,&a->ab);
	AddAction(ACTION_END,&a->ab);
	f->frameitems.push_back(a);
	f->frameitemcount++;
	l = new FrameLabel;
	l->type = FRAMELABEL;
	l->label = "_radarMap";
	l->frame = 18;
	l->flags = 0x70000;
	f->frameitems.push_back(l);
	f->frameitemcount++;
	pl = new PlaceObject;
	pl->type = PLACEOBJECT;
	pl->flags = 22;
	pl->depth = 1;
	pl->character = 10;
	pl->rotateandscale.m00 = 1.0;
	pl->rotateandscale.m10 = 0;
	pl->rotateandscale.m01 = 0;
	pl->rotateandscale.m11 = 1.0;
	pl->translate.X = 0;
	pl->translate.Y = 0;
	pl->colortransform = 0xFFFFFFFF;
	pl->unknown = 0;
	pl->ratio = (float)0.000275;
	pl->name = 0;
	pl->clipdepth = 0xFFFFFFFF;
	pl->poa = 0;
	f->frameitems.push_back(pl);
	f->frameitemcount++;

	//frame 19
	m->frames.push_back(0);
	m->framecount++;

	//frame 20
	m->frames.push_back(0);
	m->framecount++;

	//frame 21
	m->frames.push_back(0);
	m->framecount++;

	//frame 22
	m->frames.push_back(0);
	m->framecount++;

	//frame 23
	m->frames.push_back(0);
	m->framecount++;

	//frame 24
	m->frames.push_back(0);
	m->framecount++;

	//frame 25
	m->frames.push_back(0);
	m->framecount++;

	//frame 26
	m->frames.push_back(0);
	m->framecount++;

	//frame 27
	f = new Frame;
	f->frameitemcount = 0;
	m->frames.push_back(f);
	m->framecount++;
	ro = new RemoveObject;
	ro->type = REMOVEOBJECT;
	ro->depth = 1;
	f->frameitems.push_back(ro);
	f->frameitemcount++;
	a = new Action;
	a->type = ACTION;
	a->ab.constantcount = 0;
	a->ab.stringcount = 0;
	a->ab.actionbytecount = 0;
	AddStringAction(EA_GETSTRINGVAR,&a->ab,"_parent");
	AddStringAction(EA_GETSTRINGMEMBER,&a->ab,"_name");
	AddStringAction(EA_PUSHSTRING,&a->ab," In: _endMap");
	AddAction(ACTION_NEWADD,&a->ab);
	AddAction(ACTION_TRACE,&a->ab);
	AddAction(ACTION_STOP,&a->ab);
	AddAction(ACTION_END,&a->ab);
	f->frameitems.push_back(a);
	f->frameitemcount++;
	l = new FrameLabel;
	l->type = FRAMELABEL;
	l->label = "_endMap";
	l->frame = 27;
	l->flags = 0x70000;
	f->frameitems.push_back(l);
	f->frameitemcount++;
	pl = new PlaceObject;
	pl->type = PLACEOBJECT;
	pl->flags = 22;
	pl->depth = 1;
	pl->character = 14;
	pl->rotateandscale.m00 = 1.0;
	pl->rotateandscale.m10 = 0;
	pl->rotateandscale.m01 = 0;
	pl->rotateandscale.m11 = 1.0;
	pl->translate.X = 0;
	pl->translate.Y = 0;
	pl->colortransform = 0xFFFFFFFF;
	pl->unknown = 0;
	pl->ratio = (float)0.000412;
	pl->name = 0;
	pl->clipdepth = 0xFFFFFFFF;
	pl->poa = 0;
	f->frameitems.push_back(pl);
	f->frameitemcount++;

	//frame 28
	m->frames.push_back(0);
	m->framecount++;

	//frame 29
	m->frames.push_back(0);
	m->framecount++;

	//frame 30
	m->frames.push_back(0);
	m->framecount++;

	//frame 31
	m->frames.push_back(0);
	m->framecount++;

	//frame 32
	m->frames.push_back(0);
	m->framecount++;

	//frame 33
	m->frames.push_back(0);
	m->framecount++;

	//frame 34
	m->frames.push_back(0);
	m->framecount++;

	//frame 35
	f = new Frame;
	f->frameitemcount = 0;
	m->frames.push_back(f);
	m->framecount++;
	ro = new RemoveObject;
	ro->type = REMOVEOBJECT;
	ro->depth = 1;
	f->frameitems.push_back(ro);
	f->frameitemcount++;
	a = new Action;
	a->type = ACTION;
	a->ab.constantcount = 0;
	a->ab.stringcount = 0;
	a->ab.actionbytecount = 0;
	AddStringAction(EA_GETSTRINGVAR,&a->ab,"_parent");
	AddStringAction(EA_GETSTRINGMEMBER,&a->ab,"_name");
	AddStringAction(EA_PUSHSTRING,&a->ab," In: _none");
	AddAction(ACTION_NEWADD,&a->ab);
	AddAction(ACTION_TRACE,&a->ab);
	AddAction(ACTION_STOP,&a->ab);
	AddAction(ACTION_END,&a->ab);
	f->frameitems.push_back(a);
	f->frameitemcount++;
	l = new FrameLabel;
	l->type = FRAMELABEL;
	l->label = "_none";
	l->frame = 35;
	l->flags = 0x70000;
	f->frameitems.push_back(l);
	f->frameitemcount++;

	//frame 36
	m->frames.push_back(0);
	m->framecount++;

	//frame 37
	m->frames.push_back(0);
	m->framecount++;

	//frame 38
	m->frames.push_back(0);
	m->framecount++;

	//frame 39
	m->frames.push_back(0);
	m->framecount++;

	//character 1
	m->characters.push_back(0);
	m->charactercount++;

	//character 2
	Font *fo = new Font;
	m->characters.push_back(fo);
	m->charactercount++;
	fo->type = FONT;
	fo->signature = CHAR_SIG;
	fo->name = "RussellSquare";
	fo->glyphcount = 0;

	//character 3
	EditText *et = new EditText;
	m->characters.push_back(et);
	m->charactercount++;
	et->type = EDITTEXT;
	et->signature = CHAR_SIG;
	et->bounds.top = -2.000000;
	et->bounds.left = -2.000000;
	et->bounds.bottom = (float)68.000000;
	et->bounds.right = (float)54.549999;
	et->font = 2;
	et->alignment = 0;
	et->color = 0xff000099;
	et->fontheight = 16;
	et->readonly = 1;
	et->multiline = 1;
	et->wordwrap = 0;
	et->text = "Mission Select Buttons";
	et->variable = "";

	//character 4
	sp = new Sprite;
	m->characters.push_back(sp);
	m->charactercount++;
	sp->type = SPRITE;
	sp->signature = CHAR_SIG;
	sp->framecount = 0;
	sp->pointer = 0;

	//frame 0
	f = new Frame;
	f->frameitemcount = 0;
	sp->frames.push_back(f);
	sp->framecount++;
	a = new Action;
	a->type = ACTION;
	a->ab.constantcount = 0;
	a->ab.stringcount = 0;
	a->ab.actionbytecount = 0;
	AddStringAction(EA_GETSTRINGVAR,&a->ab,"Text");
	AddStringAction(EA_PUSHSTRING,&a->ab,"_visible");
	AddAction(EA_PUSHFALSE,&a->ab);
	AddAction(ACTION_SETMEMBER,&a->ab);
	AddAction(EA_PUSHZERO,&a->ab);
	AddStringAction(EA_GETSTRINGVAR,&a->ab,"_root");
	AddStringAction(EA_GETSTRINGMEMBER,&a->ab,"Campaign");
	AddStringAction(EA_PUSHSTRING,&a->ab,"IsMissionSummary");
	AddAction(ACTION_CALLMETHOD,&a->ab);
	AddAction(ACTION_LOGICALNOT,&a->ab);
	AddIntAction(ACTION_BRANCHIFTRUE,&a->ab,9);
	AddStringAction(ACTION_GOTOLABEL,&a->ab,"_revealAll");
	AddAction(ACTION_PLAY,&a->ab);
	AddAction(ACTION_END,&a->ab);
	f->frameitems.push_back(a);
	f->frameitemcount++;
	pl = new PlaceObject;
	pl->type = PLACEOBJECT;
	pl->flags = 38;
	pl->depth = 1;
	pl->character = 3;
	pl->rotateandscale.m00 = 1.0;
	pl->rotateandscale.m10 = 0;
	pl->rotateandscale.m01 = 0;
	pl->rotateandscale.m11 = 1.0;
	pl->translate.X = (float)-68.000000;
	pl->translate.Y = (float)-54.549999;
	pl->colortransform = 0xFFFFFFFF;
	pl->unknown = 0;
	pl->ratio = 0;
	pl->name = "Text";
	pl->clipdepth = 0xFFFFFFFF;
	pl->poa = 0;
	f->frameitems.push_back(pl);
	f->frameitemcount++;

	//frame 1
	f = new Frame;
	f->frameitemcount = 0;
	sp->frames.push_back(f);
	sp->framecount++;
	ro = new RemoveObject;
	ro->type = REMOVEOBJECT;
	ro->depth = 1;
	f->frameitems.push_back(ro);
	f->frameitemcount++;

	for (unsigned int i = 2;i < 60;i++)
	{
		//frames for mission buttons
		int mission = 0;
		bool mf = false;
		for (unsigned int j = 0;j < missions.size();j++)
		{
			if (missions[j]->frame == i)
			{
				mission = j;
				mf = true;
			}
		}
		if (mf)
		{
			f = new Frame;
			f->frameitemcount = 0;
			sp->frames.push_back(f);
			sp->framecount++;
			pl = new PlaceObject;
			pl->type = PLACEOBJECT;
			pl->flags = 182;
			pl->depth = missions[mission]->depth;
			pl->character = 1;
			pl->rotateandscale.m00 = 1.0;
			pl->rotateandscale.m10 = 0;
			pl->rotateandscale.m01 = 0;
			pl->rotateandscale.m11 = 1.0;
			pl->translate.X = missions[mission]->x;
			pl->translate.Y = missions[mission]->y;
			pl->colortransform = 0xFFFFFFFF;
			pl->unknown = 0;
			pl->ratio = missions[mission]->ratio;
			char *n = new char[2];
			n[0] = (char)(mission + '0');
			n[1] = 0;
			pl->name = n;
			pl->clipdepth = 0xFFFFFFFF;
			pl->poa = new PlaceObjectActions;
			pl->poa->clipactioncount = 1;
			poa = new PlaceObjectAction;
			pl->poa->actions.push_back(poa);
			pl->poa->actions[0]->flags = 0x00040000;
			pl->poa->actions[0]->flags2 = 0;
			pl->poa->actions[0]->actiondata = new Action;
			pl->poa->actions[0]->actiondata->ab.constantcount = 0;
			pl->poa->actions[0]->actiondata->ab.stringcount = 0;
			pl->poa->actions[0]->actiondata->ab.actionbytecount = 0;
			AddStringAction(EA_PUSHSTRING,&pl->poa->actions[0]->actiondata->ab,"TextPosition");
			AddStringAction(EA_SETSTRINGVAR,&pl->poa->actions[0]->actiondata->ab,missions[mission]->textpos);
			AddAction(ACTION_END,&pl->poa->actions[0]->actiondata->ab);
			f->frameitems.push_back(pl);
			f->frameitemcount++;
		}
		else
		{
			sp->frames.push_back(0);
			sp->framecount++;
		}
	}

	//frame 60
	f = new Frame;
	f->frameitemcount = 0;
	sp->frames.push_back(f);
	sp->framecount++;
	a = new Action;
	a->type = ACTION;
	a->ab.constantcount = 0;
	a->ab.stringcount = 0;
	a->ab.actionbytecount = 0;
	AddAction(ACTION_STOP,&a->ab);
	AddAction(ACTION_END,&a->ab);
	f->frameitems.push_back(a);
	f->frameitemcount++;
	l = new FrameLabel;
	l->type = FRAMELABEL;
	l->label = "_revealAll";
	l->frame = sp->framecount;
	l->flags = 0x70000;
	f->frameitems.push_back(l);
	f->frameitemcount++;

	//frame 61
	sp->frames.push_back(0);
	sp->framecount++;

	//frame 62
	sp->frames.push_back(0);
	sp->framecount++;

	//frame 63
	sp->frames.push_back(0);
	sp->framecount++;

	//frame 64
	sp->frames.push_back(0);
	sp->framecount++;

	//frame 65
	sp->frames.push_back(0);
	sp->framecount++;

	//frame 66
	sp->frames.push_back(0);
	sp->framecount++;

	//frame 67
	sp->frames.push_back(0);
	sp->framecount++;

	//frame 68
	sp->frames.push_back(0);
	sp->framecount++;

	//frame 69
	sp->frames.push_back(0);
	sp->framecount++;

	//frame 70
	sp->frames.push_back(0);
	sp->framecount++;

	//character 5
	ima = new Image;
	ima->type = IMAGE;
	ima->signature = CHAR_SIG;
	ima->texture = 5;
	m->characters.push_back(ima);
	m->charactercount++;

	//character 6
	sh = new Shape;
	sh->type = SHAPE;
	sh->signature = CHAR_SIG;
	sh->bounds.top = 0;
	sh->bounds.left = 0;
	sh->bounds.bottom = 1024;
	sh->bounds.right = 768;
	sh->geometry = 6;
	m->characters.push_back(sh);
	m->charactercount++;

	sprintf(geom,"Tow%s_%d_geometry\\6.ru",side,townum);
	FILE *geomfile = fopen(geom,"wt");
	if (!geomfile)
	{
		return;
	}
	fprintf(geomfile,"c\n");
	fprintf(geomfile,"s tc:255:255:255:255:5:1:0:0:1:0:1\n");
	fprintf(geomfile,"t 0:768:0:0:1024:0\n");
	fprintf(geomfile,"t 1024:0:1024:768:0:768\n");
	fclose(geomfile);

	//character 7
	sp = new Sprite;
	m->characters.push_back(sp);
	m->charactercount++;
	sp->type = SPRITE;
	sp->signature = CHAR_SIG;
	sp->framecount = 0;
	sp->pointer = 0;

	//frame 0
	f = new Frame;
	f->frameitemcount = 0;
	sp->frames.push_back(f);
	sp->framecount++;
	pl = new PlaceObject;
	pl->type = PLACEOBJECT;
	pl->flags = 6;
	pl->depth = 1;
	pl->character = 6;
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
	f->frameitems.push_back(pl);
	f->frameitemcount++;

	//character 8
	ima = new Image;
	ima->type = IMAGE;
	ima->signature = CHAR_SIG;
	ima->texture = 8;
	m->characters.push_back(ima);
	m->charactercount++;

	//character 9
	sh = new Shape;
	sh->type = SHAPE;
	sh->signature = CHAR_SIG;
	sh->bounds.top = 0;
	sh->bounds.left = 0;
	sh->bounds.bottom = 1024;
	sh->bounds.right = 768;
	sh->geometry = 9;
	m->characters.push_back(sh);
	m->charactercount++;

	sprintf(geom,"Tow%s_%d_geometry\\9.ru",side,townum);
	geomfile = fopen(geom,"wt");
	if (!geomfile)
	{
		return;
	}
	fprintf(geomfile,"c\n");
	fprintf(geomfile,"s tc:255:255:255:255:8:1:0:0:1:0:1\n");
	fprintf(geomfile,"t 0:768:0:0:1024:0\n");
	fprintf(geomfile,"t 1024:0:1024:768:0:768\n");
	fclose(geomfile);

	//character 10
	sp = new Sprite;
	m->characters.push_back(sp);
	m->charactercount++;
	sp->type = SPRITE;
	sp->signature = CHAR_SIG;
	sp->framecount = 0;
	sp->pointer = 0;

	//frame 0
	f = new Frame;
	f->frameitemcount = 0;
	sp->frames.push_back(f);
	sp->framecount++;
	pl = new PlaceObject;
	pl->type = PLACEOBJECT;
	pl->flags = 6;
	pl->depth = 1;
	pl->character = 9;
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
	f->frameitems.push_back(pl);
	f->frameitemcount++;

	//character 11
	sp = new Sprite;
	m->characters.push_back(sp);
	m->charactercount++;
	sp->type = SPRITE;
	sp->signature = CHAR_SIG;
	sp->framecount = 0;
	sp->pointer = 0;
	f = new Frame;
	f->frameitemcount = 0;
	sp->frames.push_back(f);
	sp->framecount++;

	//character 12
	ima = new Image;
	ima->type = IMAGE;
	ima->signature = CHAR_SIG;
	ima->texture = 12;
	m->characters.push_back(ima);
	m->charactercount++;

	//character 13
	sh = new Shape;
	sh->type = SHAPE;
	sh->signature = CHAR_SIG;
	sh->bounds.top = 0;
	sh->bounds.left = 0;
	sh->bounds.bottom = 1024;
	sh->bounds.right = 768;
	sh->geometry = 13;
	m->characters.push_back(sh);
	m->charactercount++;

	sprintf(geom,"Tow%s_%d_geometry\\13.ru",side,townum);
	geomfile = fopen(geom,"wt");
	if (!geomfile)
	{
		return;
	}
	fprintf(geomfile,"c\n");
	fprintf(geomfile,"s tc:255:255:255:255:12:1:0:0:1:0:1\n");
	fprintf(geomfile,"t 0:768:0:0:1024:0\n");
	fprintf(geomfile,"t 1024:0:1024:768:0:768\n");
	fclose(geomfile);

	//character 14
	sp = new Sprite;
	m->characters.push_back(sp);
	m->charactercount++;
	sp->type = SPRITE;
	sp->signature = CHAR_SIG;
	sp->framecount = 0;
	sp->pointer = 0;

	//frame 0
	f = new Frame;
	f->frameitemcount = 0;
	sp->frames.push_back(f);
	sp->framecount++;
	pl = new PlaceObject;
	pl->type = PLACEOBJECT;
	pl->flags = 6;
	pl->depth = 1;
	pl->character = 11;
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
	f->frameitems.push_back(pl);
	f->frameitemcount++;
	pl = new PlaceObject;
	pl->type = PLACEOBJECT;
	pl->flags = 6;
	pl->depth = 3;
	pl->character = 13;
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
	f->frameitems.push_back(pl);
	f->frameitemcount++;

	GenerateAptAptFile(m,aptapt);
}

float GetDlgItemFloat(HWND hDlg, int id)
{
	char *pEnd;
	char szItemText[20];
	GetDlgItemText(hDlg, id, szItemText, 20);
	return (float)strtod(szItemText, &pEnd);
}

BOOL CALLBACK TheaterOfWarGeneratorDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
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
				int townum = GetDlgItemInt(hwnd,IDC_TOWNUM,NULL,false);
				if (strlen(c) && side && townum)
				{
					maketowmap(townum,side,c);
				}
			}
			break;
		case IDC_ADD:
			{
				char *tp = 0;
				float x = 0;
				float y = 0;
				float ratio = 0;
				unsigned int frame = 0;
				unsigned int depth = 0;
				if (IsDlgButtonChecked(hwnd,IDC_LEFT))
				{
					tp = "LeftCenter";
				}
				if (IsDlgButtonChecked(hwnd,IDC_RIGHT))
				{
					tp = "RightCenter";
				}
				x = GetDlgItemFloat(hwnd,IDC_X);
				y = GetDlgItemFloat(hwnd,IDC_Y);
				BOOL b;
				frame = GetDlgItemInt(hwnd,IDC_FRAME,&b,false);
				depth = GetDlgItemInt(hwnd,IDC_DEPTH,&b,false);
				ratio = GetDlgItemFloat(hwnd,IDC_RATIO);
				if (tp && x && y && frame && depth && ratio)
				{
					char str[100];
					sprintf(str,"%s %f %f",tp,x,y);
					MissionPos *m = new MissionPos;
					m->textpos = _strdup(tp);
					m->x = x;
					m->y = y;
					m->ratio = ratio;
					m->frame = frame;
					m->depth = depth;
					missions.push_back(m);
					ListBox_AddString(GetDlgItem(hwnd,IDC_MISSIONLIST),str);
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
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_TOWGEN), NULL,(DLGPROC) TheaterOfWarGeneratorDlgProc);
	return 0;
}
