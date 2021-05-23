/*
	Command & Conquer 3 Tools
	APT dumper utility
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

#define add(x) *((unsigned char **)&x) += (unsigned int)aptaptdata;
AptConstData *c;
std::vector<const char *> cp;
char *ActionNames[0xC0] = {
	"ActionEnd", //0x00
	"", //0x01
	"", //0x02
	"", //0x03
	"ActionNextFrame", //0x04
	"ActionPrevFrame", //0x05
	"ActionPlay", //0x06
	"ActionStop", //0x07
	"ActionToggleQuality", //0x08
	"ActionStopSounds", //0x09
	"ActionAdd", //0x0a
	"ActionSubtract", //0x0b
	"ActionMultiply", //0x0c
	"ActionDivide", //0x0d
	"ActionEqual", //0x0e
	"ActionLessThan", //0x0f
	"ActionLogicalAnd", //0x10
	"ActionLogicalOr", //0x11
	"ActionLogicalNot", //0x12
	"ActionStringEq", //0x13
	"ActionStringLength", //0x14
	"ActionSubString", //0x15
	"", //0x16
	"ActionPop", //0x17
	"ActionInt", //0x18
	"", //0x19
	"", //0x1a
	"", //0x1b
	"ActionGetVariable", //0x1c
	"ActionSetVariable", //0x1d
	"", //0x1e
	"", //0x1f
	"ActionSetTargetExpression", //0x20
	"ActionStringConcat", //0x21
	"ActionGetProperty", //0x22
	"ActionSetProperty", //0x23
	"ActionDuplicateClip", //0x24
	"ActionRemoveClip", //0x25
	"ActionTrace", //0x26
	"ActionStartDrag", //0x27
	"ActionStopDrag", //0x28
	"ActionStringCompare", //0x29
	"ActionThrow", //0x2a
	"ActionCastOp", //0x2b
	"ActionImplementsOp", //0x2c
	"", //0x2d
	"", //0x2e
	"", //0x2f
	"ActionRandom", //0x30
	"ActionMBLength", //0x31
	"ActionOrd", //0x32
	"AcrionChr", //0x33
	"ActionGetTimer", //0x34
	"ActionMBSubString", //0x35
	"ActionMBOrd", //0x36
	"ActionMBChr", //0x37
	"", //0x38
	"", //0x39
	"ActionDelete", //0x3a
	"ActionDelete2", //0x3b
	"ActionDefineLocal", //0x3c
	"ActionCallFunction", //0x3d
	"ActionReturn", //0x3e
	"ActionModulo", //0x3f
	"ActionNew", //0x40
	"ActionVar", //0x41
	"ActionInitArray", //0x42
	"ActionInitObject", //0x43
	"ActionTypeOf", //0x44
	"ActionTargetPath", //0x45
	"ActionEnumerate", //0x46
	"ActionNewAdd", //0x47
	"ActionNewLessThan", //0x48
	"ActionNewEquals", //0x49
	"ActionToNumber", //0x4a
	"ActionToString", //0x4b
	"ActionDup", //0x4c
	"ActionSwap", //0x4d
	"ActionGetMember", //0x4e
	"ActionSetMember", //0x4f
	"ActionIncrement", //0x50
	"ActionDecrement", //0x51
	"ActionCallMethod", //0x52
	"ActionNewMethod", //0x53
	"ActionInstanceOf", //0x54
	"ActionEnum2", //0x55
	"EAUnknownAction56", //0x56
	"", //0x57
	"EAUnknownAction58", //0x58
	"EAPushZero", //0x59
	"EAPushOne", //0x5a
	"EACallFunctionPop", //0x5b
	"EACallFunction", //0x5c
	"EACAllMethodPop", //0x5d
	"EACallMethod", //0x5e
	"", //0x5f
	"ActionBitwiseAnd", //0x60
	"ActionBitwiseOr", //0x61
	"ActionBitwiseXor", //0x62
	"ActionShiftLeft", //0x63
	"ActionShiftRight", //0x64
	"ActionShiftRight2", //0x65
	"ActionStrictEq", //0x66
	"ActionGreater", //0x67
	"ActionStringGreater", //0x68
	"ActionExtends", //0x69
	"", //0x6a
	"", //0x6b
	"", //0x6c
	"", //0x6d
	"", //0x6e
	"", //0x6f
	"EAPushThis", //0x70
	"EAPushGlobal", //0x71
	"EAZeroVariable", //0x72
	"EAPushTrue", //0x73
	"EAPushFalse", //0x74
	"EAPushNull", //0x75
	"EAPushUndefined", //0x76
	"", //0x77
	"", //0x78
	"", //0x79
	"", //0x7a
	"", //0x7b
	"", //0x7c
	"", //0x7d
	"", //0x7e
	"", //0x7f
	"", //0x80
	"ActionGotoFrame", //0x81
	"", //0x82
	"ActionGetURL", //0x83
	"", //0x84
	"", //0x85
	"", //0x86
	"ActionSetRegister", //0x87
	"ActionConstantPool", //0x88
	"", //0x89
	"ActionWaitForFrame", //0x8a
	"ActionSetTarget", //0x8b
	"ActionGotoLabel", //0x8c
	"ActionWaitForFrameExpression", //0x8d
	"ActionDefineFunction2", //0x8e
	"ActionTry", //0x8f
	"", //0x90
	"", //0x91
	"", //0x92
	"", //0x93
	"ActionWith", //0x94
	"", //0x95
	"ActionPushData", //0x96
	"", //0x97
	"", //0x98
	"ActionBranchAlways", //0x99
	"ActionGetURL2", //0x9a
	"ActionDefineFunction", //0x9b
	"", //0x9c
	"ActionBranchIfTrue", //0x9d
	"ActionCallFrame", //0x9e
	"ActionGotoExpression", //0x9f
	"", //0xa0
	"EAPushString", //0xa1
	"EAPushConstant", //0xa2
	"EAPushWordConstant", //0xa3
	"EAGetStringVar", //0xa4
	"EAGetStringMember", //0xa5
	"EASetStringVar", //0xa6
	"EASetStringMember", //0xa7
	"", //0xa8
	"", //0xa9
	"", //0xaa
	"", //0xab
	"", //0xac
	"", //0xad
	"EAPushValueOfVar", //0xae
	"EAGetNamedMember", //0xaf
	"EACallNamedFunctionPop", //0xb0
	"EACallNamedFunction", //0xb1
	"EACallNamedMethodPop", //0xb2
	"EACallNamedMethod", //0xb3
	"EAPushFloat", //0xb4
	"EAPushByte", //0xb5
	"EAPushShort", //0xb6
	"EAPushLong", //0xb7
	"EABranchIfFalse", //0xb8
	"EAPushRegister" //0xb9
};

void ProcessActions(unsigned char *actions,unsigned char *aptaptdata,char *indent)
{
	unsigned char *a = actions;
	action_type action;
	do
	{
		action = (action_type)(*a);
		a++;
		switch (action)
		{
			case ACTION_BRANCHALWAYS:
			case ACTION_BRANCHIFTRUE:
			case EA_BRANCHIFFALSE:
			case ACTION_GOTOFRAME:
			case ACTION_SETREGISTER:
			case ACTION_WITH:
			case ACTION_GOTOEXPRESSION:
				{
					ALIGN(a);
					printf("%s%s %d\n",indent,ActionNames[action],*(unsigned int *)a);
					a += 4;
				}
				break;
			case ACTION_GETURL:
				{
					ALIGN(a);
					add(*(unsigned int *)a);
					char *str1 = (char *)(*(unsigned int *)a);
					a += 4;
					add(*(unsigned int *)a);
					char *str2 = (char *)(*(unsigned int *)a);
					a += 4;
					printf("%s%s \"%s\" \"%s\"\n",indent,ActionNames[action],str1,str2);
				}
				break;
			case ACTION_CONSTANTPOOL:
				{
					ALIGN(a);
					unsigned int count = *(unsigned int *)a;
					a += 4;
					add(*(unsigned int *)a);
					unsigned int *cpd = *(unsigned int **)a;
					a += 4;
					cp.clear();
					for (unsigned int i = 0;i < count;i++)
					{
						if (c->items[cpd[i]]->type == TYPE_STRING)
						{
							cp.push_back(c->items[cpd[i]]->strvalue);
						}
						else
						{
							cp.push_back("");
						}
					}
					printf("%s%s\n",indent,ActionNames[action]);
				}
				break;
			case ACTION_PUSHDATA:
				{
					ALIGN(a);
					unsigned int count = *(unsigned int *)a;
					a += 4;
					add(*(unsigned int *)a);
					unsigned int *pid = *(unsigned int **)a;
					a += 4;
					printf("%s%s ",indent,ActionNames[action]);
					for (unsigned int i = 0;i < count;i++)
					{
						printf("%d",c->items[pid[i]]->numvalue);
						if (i < count - 1)
						{
							printf(" ");
						}
					}
					printf("\n");
				}
				break;
			case ACTION_DEFINEFUNCTION2:
				{
					ALIGN(a);
					add(*(unsigned int *)a);
					char *name = (char *)(*(unsigned int *)a);
					a += 4;
					unsigned int count = *(unsigned int *)a;
					a += 4;
					unsigned int flags = *(unsigned int *)a;
					a += 4;
					printf("%s%s %s 0x%08x (",indent,ActionNames[action],name,flags);
					add(*(unsigned int *)a);
					FunctionArgument *args = (FunctionArgument *)(*(unsigned int *)a);
					a += 4;
					for (unsigned int i = 0;i < count;i++)
					{
						add(args[i].name);
						printf("%d = %s",args[i].reg,args[i].name);
						if (i < count - 1)
						{
							printf(",");
						}
					}
					printf(")\n");
					a += 4;
					a += 4;
					a += 4;
				}
				break;
			case ACTION_TRY:
				{
					ALIGN(a);
					int i1 = *(unsigned int *)a;
					a += 4;
					int i2 = *(unsigned int *)a;
					a += 4;
					int i3 = *(unsigned int *)a;
					a += 4;
					int i4 = *(unsigned int *)a;
					a += 4;
					int i5 = *(unsigned int *)a;
					a += 4;
					printf("%s%s 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",indent,ActionNames[action],i1,i2,i3,i4,i5);
				}
				break;
			case ACTION_DEFINEFUNCTION:
				{
					ALIGN(a);
					add(*(unsigned int *)a);
					char *name = (char *)(*(unsigned int *)a);
					a += 4;
					unsigned int count = *(unsigned int *)a;
					a += 4;
					printf("%s%s %s (",indent,ActionNames[action],name);
					add(*(unsigned int *)a);
					char **args = (char **)(*(unsigned int *)a);
					a += 4;
					for (unsigned int i = 0;i < count;i++)
					{
						add(args[i]);
						printf("%s",args[i]);
						if (i < count - 1)
						{
							printf(",");
						}
					}
					printf(")\n");
					a += 4;
					a += 4;
					a += 4;
				}
				break;
			case EA_PUSHSTRING:
			case EA_GETSTRINGVAR:
			case EA_GETSTRINGMEMBER:
			case EA_SETSTRINGVAR:
			case EA_SETSTRINGMEMBER:
			case ACTION_SETTARGET:
			case ACTION_GOTOLABEL:
				{
					ALIGN(a);
					add(*(unsigned int *)a);
					char *str = (char *)(*(unsigned int *)a);
					a += 4;
					printf("%s%s \"%s\"\n",indent,ActionNames[action],str);
				}
				break;
			case EA_CALLNAMEDFUNCTIONPOP:
			case EA_CALLNAMEDFUNCTION:
			case EA_CALLNAMEDMETHODPOP:
			case EA_CALLNAMEDMETHOD:
			case EA_PUSHCONSTANT:
			case EA_PUSHVALUEOFVAR:
			case EA_GETNAMEDMEMBER:
				{
					printf("%s%s %s\n",indent,ActionNames[action],cp[*(unsigned char *)a]);
					a += 1;
				}
				break;
			case EA_PUSHBYTE:
			case EA_PUSHREGISTER:
				{
					printf("%s%s %d\n",indent,ActionNames[action],*(unsigned char *)a);
					a += 1;
				}
				break;
			case EA_PUSHWORDCONSTANT:
			case EA_PUSHSHORT:
				{
					printf("%s%s %d\n",indent,ActionNames[action],*(unsigned short *)a);
					a += 2;
				}
				break;
			case EA_PUSHFLOAT:
				{
					printf("%s%s %f\n",indent,ActionNames[action],*(float *)a);
					a += 4;
				}
				break;
			case EA_PUSHLONG:
				{
					printf("%s%s %d\n",indent,ActionNames[action],*(unsigned long *)a);
					a += 4;
				}
				break;
			default:
				{
					printf("%s%s\n",indent,ActionNames[action]);
				}
				break;
		}
	} while (action != ACTION_END);
}
struct DatType1 {
	unsigned int number;
	unsigned int top;
	unsigned int left;
	unsigned int bottom;
	unsigned int right;
};
struct DatType2 {
	unsigned int number;
	unsigned int texture;
};
std::vector<DatType1> dat1;
std::vector<DatType2> dat2;
int main(int argc, char* argv[])
{
	char aptdat[MAX_PATH];
	char aptapt[MAX_PATH];
	char aptconst[MAX_PATH];
	char geom[MAX_PATH];
	sprintf(aptdat,"%s.dat",argv[1]);
	sprintf(aptapt,"%s.apt",argv[1]);
	sprintf(aptconst,"%s.const",argv[1]);
	FILE *aconst = fopen(aptconst,"rb");
	if (!aconst)
	{
		return 1;
	}
	int x = fseek(aconst,0,SEEK_END);
	int fsize = ftell(aconst);
	x = fseek(aconst,0,SEEK_SET);
	unsigned char *aptconstdata = new unsigned char[fsize];
	fread(aptconstdata,fsize,1,aconst);
	fclose(aconst);
	FILE *adat = fopen(aptdat,"rb");
	if (!adat)
	{
		return 1;
	}
	x = fseek(adat,0,SEEK_END);
	fsize = ftell(adat);
	x = fseek(adat,0,SEEK_SET);
	unsigned char *aptdatdata = new unsigned char[fsize+1];
	memset(aptdatdata,0,fsize+1);
	fread(aptdatdata,fsize,1,adat);
	fclose(adat);
	char *datc = strtok((char *)aptdatdata,"\r\n");
	std::vector<char *>lines;
	while (datc)
	{
		lines.push_back(datc);
		datc = strtok(NULL,"\r\n");
	}
	for (unsigned int i = 0;i < lines.size();i++)
	{
		if (lines[i][0] == ';')
		{
			continue;
		}
		DatType1 d;
		DatType2 d2;
		int res = sscanf(lines[i],"%d->%d",&d2.number,&d2.texture);
		if (res == 2)
		{
			dat2.push_back(d2);
		}
		else
		{
			res = sscanf(lines[i],"%d=%d %d %d %d",&d.number,&d.top,&d.left,&d.bottom,&d.right);
			if (res == 5)
			{
				dat1.push_back(d);
			}
		}
	}
	FILE *aapt = fopen(aptapt,"rb");
	if (!aapt)
	{
		return 1;
	}
	x = fseek(aapt,0,SEEK_END);
	fsize = ftell(aapt);
	x = fseek(aapt,0,SEEK_SET);
	unsigned char *aptaptdata = new unsigned char[fsize];
	fread(aptaptdata,fsize,1,aapt);
	fclose(aapt);
	unsigned char *p = aptconstdata;
	p += 0x14;
	c = new AptConstData;
	c->aptdataoffset = *(unsigned int *)p;
	p += 4;
	c->itemcount = *(unsigned int *)p;
	p += 4;
	p += 4;
	AptConstItem *aci = (AptConstItem *)p;
	p += c->itemcount * 8;
	for (unsigned int i = 0;i < c->itemcount;i++)
	{
		AptConstItem *a = new AptConstItem;
		c->items.push_back(a);
		c->items[i]->type = aci->type;
		if (c->items[i]->type == TYPE_STRING)
		{
			c->items[i]->strvalue = (char *)(aptconstdata + (unsigned int)aci->strvalue);
		}
		else
		{
			c->items[i]->numvalue = aci->numvalue;
		}
		aci++;
	}
	OutputMovie *m = (OutputMovie *)(aptaptdata+c->aptdataoffset);
	add(m->characters);
	add(m->exports);
	add(m->imports);
	add(m->frames);
	printf("Movie Clip:\n");
	printf("Screen Size %d %d\n",m->screensizex,m->screensizey);
	printf("Unknown %d\n",m->unknown);
	printf("Imports:\n");
	for (unsigned int i = 0;i < m->importcount;i++)
	{
		add(m->imports[i].movie);
		add(m->imports[i].name);
		printf("\tCharacter %s imported from movie %s at position %d\n",m->imports[i].name,m->imports[i].movie,m->imports[i].character);
	}
	printf("Exports:\n");
	for (unsigned int i = 0;i < m->exportcount;i++)
	{
		add(m->exports[i].name);
		printf("\tCharacter %d exported as %s\n",m->exports[i].character,m->exports[i].name);
	}
	for (unsigned int i = 0;i < m->framecount;i++)
	{
		add(m->frames[i].frameitems);
		printf("Frame %d:\n",i);
		for (unsigned int j = 0;j < m->frames[i].frameitemcount;j++)
		{
			add(m->frames[i].frameitems[j]);
			switch (m->frames[i].frameitems[j]->type)
			{
			case ACTION:
				{
					OutputAction *oa = (OutputAction *)m->frames[i].frameitems[j];
					add(oa->actionbytes);
					printf("\tAction:\n");
					ProcessActions(oa->actionbytes,aptaptdata,"\t\t");
				}
				break;
			case FRAMELABEL:
				{
					FrameLabel *fl = (FrameLabel *)m->frames[i].frameitems[j];
					add(fl->label);
					printf("\tFrame Label:\n");
					printf("\tLabel \"%s\"\n",fl->label);
					printf("\tFlags 0x%08x\n",fl->flags);
					printf("\tFrame %d\n",fl->frame);
				}
				break;
			case PLACEOBJECT:
				{
					OutputPlaceObject *po = (OutputPlaceObject *)m->frames[i].frameitems[j];
					PlaceObjectFlags fl = *(PlaceObjectFlags *)(&po->flags);
					printf("\tPlace Object:\n");
					printf("\tHas Clip Actions %s\n",B(fl.PlaceFlagHasClipActions));
					printf("\tHas Clip Depth %s\n",B(fl.PlaceFlagHasClipDepth));
					printf("\tHas Name %s\n",B(fl.PlaceFlagHasName));
					printf("\tHas Ratio %s\n",B(fl.PlaceFlagHasRatio));
					printf("\tHas Color Transform %s\n",B(fl.PlaceFlagHasColorTransform));
					printf("\tHas Matrix %s\n",B(fl.PlaceFlagHasMatrix));
					printf("\tHas Character %s\n",B(fl.PlaceFlagHasCharacter));
					printf("\tMove %s\n",B(fl.PlaceFlagMove));
					printf("\tDepth %d\n",po->depth);
					printf("\tCharacter %d\n",po->character);
					printf("\tRotate/Scale %f %f %f %f\n",po->rotateandscale.m00,po->rotateandscale.m01,po->rotateandscale.m10,po->rotateandscale.m11);
					printf("\tTranslate %f %f\n",po->translate.X,po->translate.Y);
					printf("\tColor Transform %d\n",po->colortransform);
					printf("\tUnknown %d\n",po->unknown);
					printf("\tRatio %f\n",po->ratio);
					if (po->name)
					{
						add(po->name);
						printf("\tName \"%s\"\n",po->name);
					}
					printf("\tClip Depth %d\n",po->clipdepth);
					if (po->poa)
					{
						add(po->poa);
						add(po->poa->actions);
						printf("\tClip Actions:\n");
						for (unsigned int k = 0;k < po->poa->clipactioncount;k++)
						{
							printf("\tFlags 0x%08x\n",po->poa->actions[k].flags);
							printf("\tFlags2 0x%08x\n",po->poa->actions[k].flags2);
							add(po->poa->actions[k].actiondata);
							ProcessActions(po->poa->actions[k].actiondata,aptaptdata,"\t\t");
						}
					}
				}
				break;
			case REMOVEOBJECT:
				{
					RemoveObject *ro = (RemoveObject *)m->frames[i].frameitems[j];
					printf("\tRemove Object:\n");
					printf("\tDepth %d\n",ro->depth);
				}
				break;
			case BACKGROUNDCOLOR:
				{
					BackgroundColor *bg = (BackgroundColor *)m->frames[i].frameitems[j];
					printf("\tBackground Color:\n");
					printf("\tColor 0x%08x\n",bg->color);
				}
				break;
			case INITACTION:
				{
					OutputInitAction *oa = (OutputInitAction *)m->frames[i].frameitems[j];
					add(oa->actionbytes);
					printf("\tInitAction for sprite %d:\n",oa->sprite);
					ProcessActions(oa->actionbytes,aptaptdata,"\t\t");
				}
				break;
			}
		}
	}
	for (unsigned int ch = 0;ch < m->charactercount;ch++)
	{
		add(m->characters[ch]);
		printf("Character %d:\n",ch);
		switch(m->characters[ch]->type)
		{
		case SHAPE:
			{
				Shape *sh = (Shape *)m->characters[ch];
				printf("Shape:\n");
				printf("Bounds %f %f %f %f\n",sh->bounds.top,sh->bounds.left,sh->bounds.bottom,sh->bounds.right);
				sprintf(geom,"%s_geometry\\%d.ru",argv[1],sh->geometry);
				FILE *ageom = fopen(geom,"rb");
				if (!ageom)
				{
					return 1;
				}
				x = fseek(ageom,0,SEEK_END);
				unsigned int gfsize = ftell(ageom);
				x = fseek(ageom,0,SEEK_SET);
				unsigned char *aptgeomdata = new unsigned char[gfsize+1];
				memset(aptgeomdata,0,gfsize+1);
				fread(aptgeomdata,gfsize,1,ageom);
				fclose(ageom);
				char *geomc = strtok((char *)aptgeomdata,"\r\n");
				std::vector<char *>glines;
				while (geomc)
				{
					glines.push_back(geomc);
					geomc = strtok(NULL,"\r\n");
				}
				printf("Geometry Data\n");
				for (unsigned int i = 0;i < glines.size();i++)
				{
					printf("%s\n",glines[i]);
					//code to parse geometry data goes here
				}
			}
			break;
		case EDITTEXT:
			{
				EditText *et = (EditText *)m->characters[ch];
				printf("EditText:\n");
				printf("Bounds %f %f %f %f\n",et->bounds.top,et->bounds.left,et->bounds.bottom,et->bounds.right);
				printf("Font %d\n",et->font);
				printf("Alignment %d\n",et->alignment);
				printf("Color 0x%08x\n",et->color);
				printf("Font Height %f\n",et->fontheight);
				printf("Read Only %s\n",B(et->readonly));
				printf("Multiline %s\n",B(et->multiline));
				printf("Word Wrap %s\n",B(et->wordwrap));
				if (et->text)
				{
					add(et->text);
					printf("Text %s\n",et->text);
				}
				if (et->variable)
				{
					add(et->variable);
					printf("Variable %s\n",et->variable);
				}
			}
			break;
		case FONT:
			{
				OutputFont *fo = (OutputFont *)m->characters[ch];
				add(fo->name);
				printf("Font:\n");
				printf("Name %s\n",fo->name);
				if (fo->glyphcount)
				{
					add(fo->glyphs);
					printf("Glyphs ");
					for (unsigned int i = 0;i < fo->glyphcount;i++)
					{
						printf("%d",fo->glyphs[i]);
						if (i < fo->glyphcount - 1)
						{
							printf(",");
						}
					}
					printf("\n");
				}
			}
			break;
		case BUTTON:
			{
				OutputButton *ob = (OutputButton *)m->characters[ch];
				printf("Button:\n");
				printf("Unknown %d\n",ob->unknown);
				printf("Unknown2 %d\n",ob->unknown2);
				printf("Bounds %f %f %f %f\n",ob->bounds.top,ob->bounds.left,ob->bounds.bottom,ob->bounds.right);
				if (ob->trianglecount)
				{
					printf("Triangles:\n");
					add(ob->vertexes);
					add(ob->triangles);
					for (unsigned int i = 0;i < ob->trianglecount;i++)
					{
						printf("\tTriangle %d v1 %f %f v2 %f %f v3 %f %f\n",i,ob->vertexes[ob->triangles[i].v1].X,ob->vertexes[ob->triangles[i].v1].Y,ob->vertexes[ob->triangles[i].v2].X,ob->vertexes[ob->triangles[i].v2].Y,ob->vertexes[ob->triangles[i].v3].X,ob->vertexes[ob->triangles[i].v3].Y);
					}
				}
				if (ob->recordcount)
				{
					add(ob->buttonrecords);
					for (unsigned int i = 0;i < ob->recordcount;i++)
					{
						printf("Record %d:\n",i);
						printf("\tFlags 0x%08x\n",ob->buttonrecords[i].flags);
						printf("\tCharacter %d\n",ob->buttonrecords[i].character);
						printf("\tDepth %d\n",ob->buttonrecords[i].depth);
						printf("\tRotate/Scale %f %f %f %f\n",ob->buttonrecords[i].rotateandscale.m00,ob->buttonrecords[i].rotateandscale.m01,ob->buttonrecords[i].rotateandscale.m10,ob->buttonrecords[i].rotateandscale.m11);
						printf("\tTranslate %f %f\n",ob->buttonrecords[i].translate.X,ob->buttonrecords[i].translate.Y);
						printf("\tColor %f %f %f %f\n",ob->buttonrecords[i].color.top,ob->buttonrecords[i].color.left,ob->buttonrecords[i].color.bottom,ob->buttonrecords[i].color.right);
						printf("\tUnknown %f %f %f %f\n",ob->buttonrecords[i].unknown.top,ob->buttonrecords[i].unknown.left,ob->buttonrecords[i].unknown.bottom,ob->buttonrecords[i].unknown.right);
					}
				}
				if (ob->buttonactioncount)
				{
					add(ob->buttonactionrecords);
					for (unsigned int i = 0;i < ob->buttonactioncount;i++)
					{
						printf("Button Action Record %d:\n",i);
						printf("\tFlags 0x%08x\n",ob->buttonactionrecords[i].flags);
						add(ob->buttonactionrecords[i].actiondata);
						ProcessActions(ob->buttonactionrecords[i].actiondata,aptaptdata,"\t");
					}
				}
			}
			break;
		case SPRITE:
			{
				OutputSprite *sp = (OutputSprite *)m->characters[ch];
				printf("Sprite:\n");
				add(sp->frames);
				for (unsigned int i = 0;i < sp->framecount;i++)
				{
					add(sp->frames[i].frameitems);
					printf("Frame %d:\n",i);
					for (unsigned int j = 0;j < sp->frames[i].frameitemcount;j++)
					{
						add(sp->frames[i].frameitems[j]);
						switch (sp->frames[i].frameitems[j]->type)
						{
						case ACTION:
							{
								OutputAction *oa = (OutputAction *)sp->frames[i].frameitems[j];
								add(oa->actionbytes);
								printf("\tAction:\n");
								ProcessActions(oa->actionbytes,aptaptdata,"\t\t");
							}
							break;
						case FRAMELABEL:
							{
								FrameLabel *fl = (FrameLabel *)sp->frames[i].frameitems[j];
								add(fl->label);
								printf("\tFrame Label:\n");
								printf("\tLabel \"%s\"\n",fl->label);
								printf("\tFlags 0x%08x\n",fl->flags);
								printf("\tFrame %d\n",fl->frame);
							}
							break;
						case PLACEOBJECT:
							{
								OutputPlaceObject *po = (OutputPlaceObject *)sp->frames[i].frameitems[j];
								PlaceObjectFlags fl = *(PlaceObjectFlags *)(&po->flags);
								printf("\tPlace Object:\n");
								printf("\tHas Clip Actions %s\n",B(fl.PlaceFlagHasClipActions));
								printf("\tHas Clip Depth %s\n",B(fl.PlaceFlagHasClipDepth));
								printf("\tHas Name %s\n",B(fl.PlaceFlagHasName));
								printf("\tHas Ratio %s\n",B(fl.PlaceFlagHasRatio));
								printf("\tHas Color Transform %s\n",B(fl.PlaceFlagHasColorTransform));
								printf("\tHas Matrix %s\n",B(fl.PlaceFlagHasMatrix));
								printf("\tHas Character %s\n",B(fl.PlaceFlagHasCharacter));
								printf("\tMove %s\n",B(fl.PlaceFlagMove));
								printf("\tDepth %d\n",po->depth);
								printf("\tCharacter %d\n",po->character);
								printf("\tRotate/Scale %f %f %f %f\n",po->rotateandscale.m00,po->rotateandscale.m01,po->rotateandscale.m10,po->rotateandscale.m11);
								printf("\tTranslate %f %f\n",po->translate.X,po->translate.Y);
								printf("\tColor Transform %d\n",po->colortransform);
								printf("\tUnknown %d\n",po->unknown);
								printf("\tRatio %f\n",po->ratio);
								if (po->name)
								{
									add(po->name);
									printf("\tName \"%s\"\n",po->name);
								}
								printf("\tClip Depth %d\n",po->clipdepth);
								if (po->poa)
								{
									add(po->poa);
									add(po->poa->actions);
									printf("\tClip Actions:\n");
									for (unsigned int k = 0;k < po->poa->clipactioncount;k++)
									{
										printf("\tFlags 0x%08x\n",po->poa->actions[k].flags);
										printf("\tFlags2 0x%08x\n",po->poa->actions[k].flags2);
										add(po->poa->actions[k].actiondata);
										ProcessActions(po->poa->actions[k].actiondata,aptaptdata,"\t\t");
									}
								}
							}
							break;
						case REMOVEOBJECT:
							{
								RemoveObject *ro = (RemoveObject *)sp->frames[i].frameitems[j];
								printf("\tRemove Object:\n");
								printf("\tDepth %d\n",ro->depth);
							}
							break;
						}
					}
				}
			}
			break;
		case IMAGE:
			{
				Image *im = (Image *)m->characters[ch];
				printf("Image:\n");
				for (unsigned int i = 0;i < dat1.size();i++)
				{
					if (dat1[i].number == im->texture)
					{
						printf("apt_%s_%d.tga %d %d %d %d\n",argv[1],dat1[i].number,dat1[i].top,dat1[i].left,dat1[i].bottom,dat1[i].right);
					}
				}
				for (unsigned int i = 0;i < dat2.size();i++)
				{
					if (dat2[i].number == im->texture)
					{
						printf("apt_%s_%d.tga\n",argv[1],dat2[i].texture);
					}
				}
			}
			break;
		case MORPH:
			{
				Morph *mo = (Morph *)m->characters[ch];
				printf("Morph:\n");
				printf("Start Shape: %d\n",mo->startshape);
				printf("End Shape: %d\n",mo->endshape);
			}
			break;
		case TEXT:
			{
				OutputText *te = (OutputText *)m->characters[ch];
				printf("Text:\n");
				printf("Bounds %f %f %f %f\n",te->bounds.top,te->bounds.left,te->bounds.bottom,te->bounds.right);
				printf("\tRotate/Scale %f %f %f %f\n",te->rotateandscale.m00,te->rotateandscale.m01,te->rotateandscale.m10,te->rotateandscale.m11);
				printf("\tTranslate %f %f\n",te->translate.X,te->translate.Y);
				if (te->recordcount)
				{
					add(te->records);
					for (unsigned int i = 0;i < te->recordcount;i++)
					{
						printf("Record %d:\n",i);
						printf("\tFont %d\n",te->records[i].font);
						printf("\tColor %f %f %f %f\n",te->records[i].color.top,te->records[i].color.left,te->records[i].color.bottom,te->records[i].color.right);
						printf("\tUnknown %f %f %f %f\n",te->records[i].unknown.top,te->records[i].unknown.left,te->records[i].unknown.bottom,te->records[i].unknown.right);
						printf("\tOffset %f %f\n",te->records[i].offset.X,te->records[i].offset.Y);
						printf("\tHeight %f\n",te->records[i].textheight);
						if (te->records[i].glyphcount)
						{
							printf("\tGlyphs:\n");
							add(te->records[i].glyphs);
							for (unsigned int j = 0;j < te->records[i].glyphcount;j++)
							{
								printf("\tIndex %d Advance %d\n",te->records[i].glyphs[j].index,te->records[i].glyphs[j].advance);
							}
						}
					}
				}
			}
			break;
		}
	}
	return 0;
}
