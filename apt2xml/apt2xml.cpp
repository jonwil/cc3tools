/*
	Command & Conquer 3 Tools
	APT to XML converter
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
#include <string>
#include <sstream>

#include <string>
#include <sstream>

std::string encodeForXml(const std::string& sSrc)
{
	std::ostringstream sRet;

	for (std::string::const_iterator iter = sSrc.begin(); iter != sSrc.end(); iter++)
	{
		unsigned char c = (unsigned char)*iter;

		switch (c)
		{
		case '&': sRet << "&amp;"; break;
		case '<': sRet << "&lt;"; break;
		case '>': sRet << "&gt;"; break;
		case '"': sRet << "&quot;"; break;
		case '\'': sRet << "&apos;"; break;

		default:
			if (c < 32 || c>127)
			{
				sRet << "&#" << (unsigned int)c << ";";
			}
			else
			{
				sRet << c;
			}
		}
	}

	return sRet.str();
}

#define add(x) *((unsigned char **)&x) += (unsigned int)aptaptdata;
AptConstData* c;
void ProcessActions(FILE* xml, unsigned char* actions, unsigned char* aptaptdata, char* indent)
{
	unsigned char* a = actions;
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
			fprintf(xml, "%s<goto action=\"%d\" pos=\"%d\" />\n", indent, action, *(unsigned int*)a);
			a += 4;
		}
		break;
		case ACTION_GETURL:
		{
			ALIGN(a);
			add(*(unsigned int*)a);
			char* str1 = (char*)(*(unsigned int*)a);
			a += 4;
			add(*(unsigned int*)a);
			char* str2 = (char*)(*(unsigned int*)a);
			a += 4;
			fprintf(xml, "%s<geturl str1=\"%s\" str2=\"%s\" />\n", indent, encodeForXml(str1).c_str(), encodeForXml(str2).c_str());
		}
		break;
		case ACTION_CONSTANTPOOL:
		{
			ALIGN(a);
			unsigned int count = *(unsigned int*)a;
			a += 4;
			add(*(unsigned int*)a);
			unsigned int* cpd = *(unsigned int**)a;
			a += 4;
			fprintf(xml, "%s<constantpool>\n", indent);
			for (unsigned int i = 0; i < count; i++)
			{
				fprintf(xml, "%s\t<constant val=\"%d\" />\n", indent, cpd[i]);
			}
			fprintf(xml, "%s</constantpool>\n", indent);
		}
		break;
		case ACTION_PUSHDATA:
		{
			ALIGN(a);
			unsigned int count = *(unsigned int*)a;
			a += 4;
			add(*(unsigned int*)a);
			unsigned int* pid = *(unsigned int**)a;
			a += 4;
			fprintf(xml, "%s<pushdata>\n", indent);
			for (unsigned int i = 0; i < count; i++)
			{
				fprintf(xml, "%s\t<data value=\"%d\" />\n", indent, pid[i]);
			}
			fprintf(xml, "%s</pushdata>\n", indent);
		}
		break;
		case ACTION_DEFINEFUNCTION2:
		{
			ALIGN(a);
			add(*(unsigned int*)a);
			char* name = (char*)(*(unsigned int*)a);
			a += 4;
			unsigned int count = *(unsigned int*)a;
			a += 4;
			unsigned int flags = *(unsigned int*)a;
			a += 4;
			add(*(unsigned int*)a);
			FunctionArgument* args = (FunctionArgument*)(*(unsigned int*)a);
			a += 4;
			unsigned int size = *(unsigned int*)a;
			fprintf(xml, "%s<definefunction2 name=\"%s\" flags=\"%d\" size=\"%d\">\n", indent, encodeForXml(name).c_str(), flags, size);
			for (unsigned int i = 0; i < count; i++)
			{
				add(args[i].name);
				fprintf(xml, "%s\t<argument reg=\"%d\" name=\"%s\" />\n", indent, args[i].reg, encodeForXml(args[i].name).c_str());
			}
			fprintf(xml, "%s</definefunction2>\n", indent);
			a += 4;
			a += 4;
			a += 4;
		}
		break;
		case ACTION_DEFINEFUNCTION:
		{
			ALIGN(a);
			add(*(unsigned int*)a);
			char* name = (char*)(*(unsigned int*)a);
			a += 4;
			unsigned int count = *(unsigned int*)a;
			a += 4;
			add(*(unsigned int*)a);
			char** args = (char**)(*(unsigned int*)a);
			a += 4;
			unsigned int size = *(unsigned int*)a;
			fprintf(xml, "%s<definefunction name=\"%s\" size=\"%d\">\n", indent, encodeForXml(name).c_str(), size);
			for (unsigned int i = 0; i < count; i++)
			{
				add(args[i]);
				fprintf(xml, "%s\t<argument name=\"%s\" />\n", indent, encodeForXml(args[i]).c_str());
			}
			a += 4;
			a += 4;
			a += 4;
			fprintf(xml, "%s</definefunction>\n", indent);
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
			add(*(unsigned int*)a);
			char* str = (char*)(*(unsigned int*)a);
			a += 4;
			fprintf(xml, "%s<string action=\"%d\" str=\"%s\" />\n", indent, action, encodeForXml(str).c_str());
		}
		break;
		case EA_CALLNAMEDFUNCTIONPOP:
		case EA_CALLNAMEDFUNCTION:
		case EA_CALLNAMEDMETHODPOP:
		case EA_CALLNAMEDMETHOD:
		case EA_PUSHCONSTANT:
		case EA_PUSHVALUEOFVAR:
		case EA_GETNAMEDMEMBER:
		case EA_PUSHBYTE:
		case EA_PUSHREGISTER:
		{
			fprintf(xml, "%s<byte action=\"%d\" val=\"%d\" />\n", indent, action, *(unsigned char*)a);
			a += 1;
		}
		break;
		case EA_PUSHWORDCONSTANT:
		case EA_PUSHSHORT:
		{
			fprintf(xml, "%s<short action=\"%d\" val=\"%d\" />\n", indent, action, *(unsigned short*)a);
			a += 2;
		}
		break;
		case EA_PUSHFLOAT:
		{
			fprintf(xml, "%s<float action=\"%d\" val=\"%f\" />\n", indent, action, *(float*)a);
			a += 4;
		}
		break;
		case EA_PUSHLONG:
		{
			fprintf(xml, "%s<long action=\"%d\" val=\"%d\" />\n", indent, action, *(unsigned long*)a);
			a += 4;
		}
		break;
		default:
		{
			fprintf(xml, "%s<noarg action=\"%d\" />\n", indent, action);
		}
		break;
		}
	} while (action != ACTION_END);
}
int main(int argc, char* argv[])
{
	char aptapt[MAX_PATH];
	char aptconst[MAX_PATH];
	char xmln[MAX_PATH];
	sprintf(xmln, "%s.xml", argv[1]);
	sprintf(aptconst, "%s.const", argv[1]);
	sprintf(aptapt, "%s.apt", argv[1]);
	FILE* aconst = fopen(aptconst, "rb");
	if (!aconst)
	{
		return 1;
	}
	int x = fseek(aconst, 0, SEEK_END);
	int fsize = ftell(aconst);
	x = fseek(aconst, 0, SEEK_SET);
	unsigned char* aptconstdata = new unsigned char[fsize];
	fread(aptconstdata, fsize, 1, aconst);
	fclose(aconst);
	FILE* aapt = fopen(aptapt, "rb");
	if (!aapt)
	{
		return 1;
	}
	x = fseek(aapt, 0, SEEK_END);
	fsize = ftell(aapt);
	x = fseek(aapt, 0, SEEK_SET);
	unsigned char* aptaptdata = new unsigned char[fsize];
	fread(aptaptdata, fsize, 1, aapt);
	fclose(aapt);
	unsigned char* p = aptconstdata;
	p += 0x14;
	c = new AptConstData;
	c->aptdataoffset = *(unsigned int*)p;
	p += 4;
	c->itemcount = *(unsigned int*)p;
	p += 4;
	p += 4;
	AptConstItem* aci = (AptConstItem*)p;
	p += c->itemcount * 8;
	for (unsigned int i = 0; i < c->itemcount; i++)
	{
		AptConstItem* a = new AptConstItem;
		c->items.push_back(a);
		c->items[i]->type = aci->type;
		if (c->items[i]->type == TYPE_STRING)
		{
			c->items[i]->strvalue = (char*)(aptconstdata + (unsigned int)aci->strvalue);
		}
		else
		{
			c->items[i]->numvalue = aci->numvalue;
		}
		aci++;
	}
	OutputMovie* m = (OutputMovie*)(aptaptdata + c->aptdataoffset);
	add(m->characters);
	add(m->exports);
	add(m->imports);
	add(m->frames);
	FILE* xml = fopen(xmln, "wt");
	if (!xml)
	{
		return 0;
	}
	fprintf(xml, "<\?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
	fprintf(xml, "<aptdata>\n");
	fprintf(xml, "\t<movieclip screensizex=\"%d\" screensizey=\"%d\" unknown=\"%d\">\n", m->screensizex, m->screensizey, m->unknown);
	fprintf(xml, "\t\t<imports>\n");
	for (unsigned int i = 0; i < m->importcount; i++)
	{
		add(m->imports[i].movie);
		add(m->imports[i].name);
		fprintf(xml, "\t\t\t<import name=\"%s\" movie=\"%s\" character=\"%d\" />\n", encodeForXml(m->imports[i].name).c_str(), encodeForXml(m->imports[i].movie).c_str(), m->imports[i].character);
	}
	fprintf(xml, "\t\t</imports>\n");
	fprintf(xml, "\t\t<exports>\n");
	for (unsigned int i = 0; i < m->exportcount; i++)
	{
		add(m->exports[i].name);
		fprintf(xml, "\t\t\t<export character=\"%d\" name=\"%s\" />\n", m->exports[i].character, encodeForXml(m->exports[i].name).c_str());
	}
	fprintf(xml, "\t\t</exports>\n");
	if (m->framecount)
	{
		fprintf(xml, "\t\t<frames>\n");
		for (unsigned int i = 0; i < m->framecount; i++)
		{
			add(m->frames[i].frameitems);
			fprintf(xml, "\t\t\t<frame id=\"%d\">\n", i);
			for (unsigned int j = 0; j < m->frames[i].frameitemcount; j++)
			{
				add(m->frames[i].frameitems[j]);
				switch (m->frames[i].frameitems[j]->type)
				{
				case ACTION:
				{
					OutputAction* oa = (OutputAction*)m->frames[i].frameitems[j];
					add(oa->actionbytes);
					fprintf(xml, "\t\t\t\t<action>\n");
					ProcessActions(xml, oa->actionbytes, aptaptdata, "\t\t\t\t\t");
					fprintf(xml, "\t\t\t\t</action>\n");
				}
				break;
				case FRAMELABEL:
				{
					FrameLabel* fl = (FrameLabel*)m->frames[i].frameitems[j];
					add(fl->label);
					fprintf(xml, "\t\t\t\t<framelabel label=\"%s\" frame=\"%d\" />\n", encodeForXml(fl->label).c_str(), fl->frame);
				}
				break;
				case PLACEOBJECT:
				{
					OutputPlaceObject* po = (OutputPlaceObject*)m->frames[i].frameitems[j];
					int red = LOBYTE(LOWORD(po->colortransform));
					int green = HIBYTE(LOWORD(po->colortransform));
					int blue = LOBYTE(HIWORD(po->colortransform));
					int alpha = HIBYTE(HIWORD(po->colortransform));
					fprintf(xml, "\t\t\t\t<placeobject flags=\"%d\" depth=\"%d\" character=\"%d\" rotm00=\"%f\" rotm01=\"%f\" rotm10=\"%f\" rotm11=\"%f\" tx=\"%f\" ty=\"%f\" red=\"%d\" green=\"%d\" blue=\"%d\" alpha=\"%d\" ratio=\"%f\" clipdepth=\"%d\" unknown=\"%d\">\n", po->flags, po->depth, po->character, po->rotateandscale.m00, po->rotateandscale.m01, po->rotateandscale.m10, po->rotateandscale.m11, po->translate.X, po->translate.Y, red, green, blue, alpha, po->ratio, po->clipdepth, po->unknown);
					if (po->name)
					{
						add(po->name);
						fprintf(xml, "\t\t\t\t\t<poname name=\"%s\" />\n", encodeForXml(po->name).c_str());
					}
					if (po->poa)
					{
						add(po->poa);
						add(po->poa->actions);
						fprintf(xml, "\t\t\t\t\t<clipactions>\n");
						for (unsigned int k = 0; k < po->poa->clipactioncount; k++)
						{
							fprintf(xml, "\t\t\t\t\t<clipaction flags=\"%d\" flags2=\"%d\">\n", po->poa->actions[k].flags, po->poa->actions[k].flags2);
							add(po->poa->actions[k].actiondata);
							ProcessActions(xml, po->poa->actions[k].actiondata, aptaptdata, "\t\t\t\t\t\t");
							fprintf(xml, "\t\t\t\t</clipaction>\n");
						}
						fprintf(xml, "\t\t\t\t\t</clipactions>\n");
					}
					fprintf(xml, "\t\t\t\t</placeobject>\n");
				}
				break;
				case REMOVEOBJECT:
				{
					RemoveObject* ro = (RemoveObject*)m->frames[i].frameitems[j];
					fprintf(xml, "\t\t\t\t<removeobject depth=\"%d\" />\n", ro->depth);
				}
				break;
				case BACKGROUNDCOLOR:
				{
					BackgroundColor* bg = (BackgroundColor*)m->frames[i].frameitems[j];
					fprintf(xml, "\t\t\t\t<background color=\"%d\" />\n", bg->color);
				}
				break;
				case INITACTION:
				{
					OutputInitAction* oa = (OutputInitAction*)m->frames[i].frameitems[j];
					add(oa->actionbytes);
					fprintf(xml, "\t\t\t\t<initaction sprite=\"%d\">\n", oa->sprite);
					ProcessActions(xml, oa->actionbytes, aptaptdata, "\t\t\t\t\t");
					fprintf(xml, "\t\t\t\t</initaction>\n");
				}
				break;
				}
			}
			fprintf(xml, "\t\t\t</frame>\n");
		}
		fprintf(xml, "\t\t</frames>\n");
	}
	fprintf(xml, "\t</movieclip>\n");
	for (unsigned int ch = 0; ch < m->charactercount; ch++)
	{
		if (m->characters[ch])
		{
			add(m->characters[ch]);
			switch (m->characters[ch]->type)
			{
			case SHAPE:
			{
				Shape* sh = (Shape*)m->characters[ch];
				fprintf(xml, "\t<shape id=\"%d\" top=\"%f\" left=\"%f\" bottom=\"%f\" right=\"%f\" geometry=\"%d\" />\n", ch, sh->bounds.top, sh->bounds.left, sh->bounds.bottom, sh->bounds.right, sh->geometry);
			}
			break;
			case EDITTEXT:
			{
				EditText* et = (EditText*)m->characters[ch];
				int red = LOBYTE(LOWORD(et->color));
				int green = HIBYTE(LOWORD(et->color));
				int blue = LOBYTE(HIWORD(et->color));
				int alpha = HIBYTE(HIWORD(et->color));
				fprintf(xml, "\t<edittext id=\"%d\" top=\"%f\" left=\"%f\" bottom=\"%f\" right=\"%f\" font=\"%d\" alignment=\"%d\" red=\"%d\" green=\"%d\" blue=\"%d\" alpha=\"%d\" height=\"%f\" readonly=\"%d\" multiline=\"%d\" wordwrap=\"%d\">\n", ch, et->bounds.top, et->bounds.left, et->bounds.bottom, et->bounds.right, et->font, et->alignment, red, green, blue, alpha, et->fontheight, et->readonly, et->multiline, et->wordwrap);
				if (et->text)
				{
					add(et->text);
					fprintf(xml, "\t\t<ettext text=\"%s\" />\n", encodeForXml(et->text).c_str());
				}
				if (et->variable)
				{
					add(et->variable);
					fprintf(xml, "\t\t<etvar variable=\"%s\" />\n", encodeForXml(et->variable).c_str());
				}
				fprintf(xml, "\t</edittext>\n");
			}
			break;
			case FONT:
			{
				OutputFont* fo = (OutputFont*)m->characters[ch];
				add(fo->name);
				fprintf(xml, "\t<font id=\"%d\" name=\"%s\">\n", ch, encodeForXml(fo->name).c_str());
				if (fo->glyphcount)
				{
					add(fo->glyphs);
					fprintf(xml, "\t\t<glyphs>\n");
					for (unsigned int i = 0; i < fo->glyphcount; i++)
					{
						fprintf(xml, "\t\t\t<glyph id=\"%d\" />\n", fo->glyphs[i]);
					}
					fprintf(xml, "\t\t</glyphs>\n");
				}
				fprintf(xml, "\t</font>\n");
			}
			break;
			case BUTTON:
			{
				OutputButton* ob = (OutputButton*)m->characters[ch];
				fprintf(xml, "\t<button id=\"%d\" top=\"%f\" left=\"%f\" bottom=\"%f\" right=\"%f\">\n", ch, ob->bounds.top, ob->bounds.left, ob->bounds.bottom, ob->bounds.right);
				if (ob->trianglecount)
				{
					add(ob->vertexes);
					add(ob->triangles);
					fprintf(xml, "\t\t<vertexes>\n");
					for (unsigned int i = 0; i < ob->vertexcount; i++)
					{
						fprintf(xml, "\t\t\t<vertex x=\"%f\" y=\"%f\" />\n", ob->vertexes[i].X, ob->vertexes[i].Y);
					}
					fprintf(xml, "\t\t</vertexes>\n");
					fprintf(xml, "\t\t<triangles>\n");
					for (unsigned int i = 0; i < ob->trianglecount; i++)
					{
						fprintf(xml, "\t\t\t<triangle v1=\"%d\" v2=\"%d\" v3=\"%d\" />\n", ob->triangles[i].v1, ob->triangles[i].v2, ob->triangles[i].v3);
					}
					fprintf(xml, "\t\t</triangles>\n");
				}
				if (ob->recordcount)
				{
					add(ob->buttonrecords);
					fprintf(xml, "\t\t<buttonrecords>\n");
					for (unsigned int i = 0; i < ob->recordcount; i++)
					{
						fprintf(xml, "\t\t\t<buttonrecord flags=\"%d\" character=\"%d\" depth=\"%d\" rotm00=\"%f\" rotm01=\"%f\" rotm10=\"%f\" rotm11=\"%f\" tx=\"%f\" ty=\"%f\" />\n", ob->buttonrecords[i].flags, ob->buttonrecords[i].character, ob->buttonrecords[i].depth, ob->buttonrecords[i].rotateandscale.m00, ob->buttonrecords[i].rotateandscale.m01, ob->buttonrecords[i].rotateandscale.m10, ob->buttonrecords[i].rotateandscale.m11, ob->buttonrecords[i].translate.X, ob->buttonrecords[i].translate.Y);
					}
					fprintf(xml, "\t\t</buttonrecords>\n");
				}
				if (ob->buttonactioncount)
				{
					add(ob->buttonactionrecords);
					fprintf(xml, "\t\t<buttonactions>\n");
					for (unsigned int i = 0; i < ob->buttonactioncount; i++)
					{
						fprintf(xml, "\t\t\t<buttonaction flags=\"%d\">\n", ob->buttonactionrecords[i].flags);
						add(ob->buttonactionrecords[i].actiondata);
						ProcessActions(xml, ob->buttonactionrecords[i].actiondata, aptaptdata, "\t\t\t\t");
						fprintf(xml, "\t\t\t</buttonaction>\n");
					}
					fprintf(xml, "\t\t</buttonactions>\n");
				}
				fprintf(xml, "\t</button>\n");
			}
			break;
			case SPRITE:
			{
				OutputSprite* sp = (OutputSprite*)m->characters[ch];
				fprintf(xml, "\t<sprite id=\"%d\" >\n", ch);
				add(sp->frames);
				if (sp->framecount)
				{
					fprintf(xml, "\t<frames>\n");
					for (unsigned int i = 0; i < sp->framecount; i++)
					{
						add(sp->frames[i].frameitems);
						fprintf(xml, "\t\t<frame id=\"%d\">\n", i);
						for (unsigned int j = 0; j < sp->frames[i].frameitemcount; j++)
						{
							add(sp->frames[i].frameitems[j]);
							switch (sp->frames[i].frameitems[j]->type)
							{
							case ACTION:
							{
								OutputAction* oa = (OutputAction*)sp->frames[i].frameitems[j];
								add(oa->actionbytes);
								fprintf(xml, "\t\t\t<action>\n");
								ProcessActions(xml, oa->actionbytes, aptaptdata, "\t\t\t\t");
								fprintf(xml, "\t\t\t</action>\n");
							}
							break;
							case FRAMELABEL:
							{
								FrameLabel* fl = (FrameLabel*)sp->frames[i].frameitems[j];
								add(fl->label);
								fprintf(xml, "\t\t\t<framelabel label=\"%s\" frame=\"%d\" />\n", encodeForXml(fl->label).c_str(), fl->frame);
							}
							break;
							case PLACEOBJECT:
							{
								OutputPlaceObject* po = (OutputPlaceObject*)sp->frames[i].frameitems[j];
								int red = LOBYTE(LOWORD(po->colortransform));
								int green = HIBYTE(LOWORD(po->colortransform));
								int blue = LOBYTE(HIWORD(po->colortransform));
								int alpha = HIBYTE(HIWORD(po->colortransform));
								fprintf(xml, "\t\t\t<placeobject flags=\"%d\" depth=\"%d\" character=\"%d\" rotm00=\"%f\" rotm01=\"%f\" rotm10=\"%f\" rotm11=\"%f\" tx=\"%f\" ty=\"%f\" red=\"%d\" green=\"%d\" blue=\"%d\" alpha=\"%d\" ratio=\"%f\" clipdepth=\"%d\" unknown=\"%d\">\n", po->flags, po->depth, po->character, po->rotateandscale.m00, po->rotateandscale.m01, po->rotateandscale.m10, po->rotateandscale.m11, po->translate.X, po->translate.Y, red, green, blue, alpha, po->ratio, po->clipdepth, po->unknown);
								if (po->name)
								{
									add(po->name);
									fprintf(xml, "\t\t\t\t<poname name=\"%s\" />\n", encodeForXml(po->name).c_str());
								}
								if (po->poa)
								{
									add(po->poa);
									add(po->poa->actions);
									fprintf(xml, "\t\t\t\t<clipactions>\n");
									for (unsigned int k = 0; k < po->poa->clipactioncount; k++)
									{
										fprintf(xml, "\t\t\t\t<clipaction flags=\"%d\" flags2=\"%d\">\n", po->poa->actions[k].flags, po->poa->actions[k].flags2);
										add(po->poa->actions[k].actiondata);
										ProcessActions(xml, po->poa->actions[k].actiondata, aptaptdata, "\t\t\t\t\t");
										fprintf(xml, "\t\t\t\t</clipaction>\n");
									}
									fprintf(xml, "\t\t\t\t</clipactions>\n");
								}
								fprintf(xml, "\t\t\t</placeobject>\n");
							}
							break;
							case REMOVEOBJECT:
							{
								RemoveObject* ro = (RemoveObject*)sp->frames[i].frameitems[j];
								fprintf(xml, "\t\t\t<removeobject depth=\"%d\" />\n", ro->depth);
							}
							break;
							case BACKGROUNDCOLOR:
							{
								BackgroundColor* bg = (BackgroundColor*)sp->frames[i].frameitems[j];
								fprintf(xml, "\t\t\t<background color=\"%d\" />\n", bg->color);
							}
							break;
							case INITACTION:
							{
								OutputInitAction* oa = (OutputInitAction*)sp->frames[i].frameitems[j];
								add(oa->actionbytes);
								fprintf(xml, "\t\t\t<initaction sprite=\"%d\">\n", oa->sprite);
								ProcessActions(xml, oa->actionbytes, aptaptdata, "\t\t\t\t\t");
								fprintf(xml, "\t\t\t</initaction>\n");
							}
							break;
							}
						}
						fprintf(xml, "\t\t</frame>\n");
					}
					fprintf(xml, "\t</frames>\n");
				}
				fprintf(xml, "\t</sprite>\n");
			}
			break;
			case IMAGE:
			{
				Image* im = (Image*)m->characters[ch];
				fprintf(xml, "\t<image id=\"%d\" image=\"%d\" />\n", ch, im->texture);
			}
			break;
			case MORPH:
			{
				Morph* mo = (Morph*)m->characters[ch];
				fprintf(xml, "\t<morph id=\"%d\" start=\"%d\" end=\"%d\" />\n", ch, mo->startshape, mo->endshape);
			}
			break;
			case TEXT:
			{
				OutputText* te = (OutputText*)m->characters[ch];
				fprintf(xml, "\t<text id=\"%d\" top=\"%f\" left=\"%f\" bottom=\"%f\" right=\"%f\" rotm00=\"%f\" rotm01=\"%f\" rotm10=\"%f\" rotm11=\"%f\" tx=\"%f\" ty=\"%f\" >\n", ch, te->bounds.top, te->bounds.left, te->bounds.bottom, te->bounds.right, te->rotateandscale.m00, te->rotateandscale.m01, te->rotateandscale.m10, te->rotateandscale.m11, te->translate.X, te->translate.Y);
				if (te->recordcount)
				{
					add(te->records);
					fprintf(xml, "\t\t<records>\n");
					for (unsigned int i = 0; i < te->recordcount; i++)
					{
						fprintf(xml, "\t\t<record font=\"%d\" red=\"%f\" green=\"%f\" blue=\"%f\" alpha=\"%f\" u1=\"%f\" u2=\"%f\" u3=\"%f\" u4=\"%f\" tx=\"%f\" ty=\"%f\" height=\"%f\">\n", te->records[i].font, te->records[i].color.top, te->records[i].color.left, te->records[i].color.bottom, te->records[i].color.right, te->records[i].unknown.top, te->records[i].unknown.left, te->records[i].unknown.bottom, te->records[i].unknown.right, te->records[i].offset.X, te->records[i].offset.Y, te->records[i].textheight);
						if (te->records[i].glyphcount)
						{
							fprintf(xml, "\t\t<glyphs>\n");
							add(te->records[i].glyphs);
							for (unsigned int j = 0; j < te->records[i].glyphcount; j++)
							{
								fprintf(xml, "\t\t\t<glyph index=\"%d\" advance=\"%d\" />\n", te->records[i].glyphs[j].index, te->records[i].glyphs[j].advance);
							}
							fprintf(xml, "\t\t</glyphs>\n");
						}
						fprintf(xml, "\t\t</record>\n");
					}
					fprintf(xml, "\t\t</records>\n");
				}
				fprintf(xml, "\t</text>\n");
			}
			break;
			default:
				fprintf(xml, "\t\n");
				break;
			}
		}
		else
		{
			fprintf(xml, "\t<empty id = \"%d\" />\n", ch);
		}
	}
	fprintf(xml, "</aptdata>\n");
	fclose(xml);
	return 0;
}
