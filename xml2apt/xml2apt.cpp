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
#include "tinyxml.h"
#define add(x) *((unsigned char **)&x) += (unsigned int)aptaptdata;
AptConstData *c;

void ProcessActions(TiXmlNode* entry,ActionBytes *ab)
{
	TiXmlNode *entryt;
	for (entryt = entry->FirstChild(); entryt != 0; entryt = entryt->NextSibling())
	{
		if (!_stricmp(entryt->Value(),"goto"))
		{
			unsigned int action;
			unsigned int pos;
			entryt->ToElement()->Attribute("action",&action);
			entryt->ToElement()->Attribute("pos",&pos);
			AddIntAction((action_type)action,ab,pos);
		}
		if (!_stricmp(entryt->Value(),"geturl"))
		{
			const char *str1;
			const char *str2;
			str1 = _strdup(entryt->ToElement()->Attribute("str1"));
			str2 = _strdup(entryt->ToElement()->Attribute("str2"));
			AddURLAction(ACTION_GETURL,ab,str1,str2);
		}
		if (!_stricmp(entryt->Value(),"constantpool"))
		{
			TiXmlNode *entryt2;
			for (entryt2 = entryt->FirstChild(); entryt2 != 0; entryt2 = entryt2->NextSibling())
			{
				ab->constantcount++;
				unsigned int val;
				entryt2->ToElement()->Attribute("val",&val);
				ab->constants.push_back(val);
			}
			AddConstantPoolAction(ACTION_CONSTANTPOOL,ab,ab->constantcount);
		}
		if (!_stricmp(entryt->Value(),"pushdata"))
		{
			ab->pushdatacount++;
			ActionPushData *pd = new ActionPushData;
			ab->actionpushdatas.push_back(pd);
			pd->pushdataoffset = ab->actionbytecount;
			pd->pushdatacount = 0;
			TiXmlNode *entryt2;
			for (entryt2 = entryt->FirstChild(); entryt2 != 0; entryt2 = entryt2->NextSibling())
			{
				pd->pushdatacount++;
				unsigned int value;
				entryt2->ToElement()->Attribute("value",&value);
				pd->pushdata.push_back(value);
			}
			AddPushDataAction(ACTION_PUSHDATA,ab,pd);
		}
		if (!_stricmp(entryt->Value(),"definefunction2"))
		{
			ab->definefunction2count++;
			ActionDefineFunction2 *pd = new ActionDefineFunction2;
			ab->actiondefinefunction2s.push_back(pd);
			pd->definefunction2offset = ab->actionbytecount;
			pd->argumentcount = 0;
			TiXmlNode *entryt2;
			unsigned int flags;
			unsigned int size;
			const char *name = _strdup(entryt->ToElement()->Attribute("name"));
			entryt->ToElement()->Attribute("flags",&flags);
			entryt->ToElement()->Attribute("size",&size);
			for (entryt2 = entryt->FirstChild(); entryt2 != 0; entryt2 = entryt2->NextSibling())
			{
				pd->argumentcount++;
				FunctionArgument *a = new FunctionArgument;
				entryt2->ToElement()->Attribute("reg",&a->reg);
				a->name = _strdup(entryt2->ToElement()->Attribute("name"));
				pd->arguments.push_back(a);
			}
			AddDefineFunction2Action(ACTION_DEFINEFUNCTION2,ab,pd,flags,size,name);
		}
		if (!_stricmp(entryt->Value(),"definefunction"))
		{
			ab->definefunctioncount++;
			ActionDefineFunction *adf = new ActionDefineFunction;
			ab->actiondefinefunctions.push_back(adf);
			adf->definefunctionoffset = ab->actionbytecount;
			adf->argumentcount = 0;
			TiXmlNode *entryt2;
			unsigned int size;
			const char *name = _strdup(entryt->ToElement()->Attribute("name"));
			entryt->ToElement()->Attribute("size",&size);
			for (entryt2 = entryt->FirstChild(); entryt2 != 0; entryt2 = entryt2->NextSibling())
			{
				adf->argumentcount++;
				char *s = _strdup(entryt2->ToElement()->Attribute("name"));
				adf->arguments.push_back(s);
			}
			AddDefineFunctionAction(ACTION_DEFINEFUNCTION,ab,adf,size,name);
		}
		if (!_stricmp(entryt->Value(),"string"))
		{
			unsigned int action;
			const char *str;
			entryt->ToElement()->Attribute("action",&action);
			str = _strdup(entryt->ToElement()->Attribute("str"));
			AddStringAction((action_type)action,ab,str);
		}
		if (!_stricmp(entryt->Value(),"byte"))
		{
			unsigned int action;
			unsigned int val;
			entryt->ToElement()->Attribute("action",&action);
			entryt->ToElement()->Attribute("val",&val);
			AddByteAction((action_type)action,ab,(unsigned char)val);
		}
		if (!_stricmp(entryt->Value(),"short"))
		{
			unsigned int action;
			unsigned int val;
			entryt->ToElement()->Attribute("action",&action);
			entryt->ToElement()->Attribute("val",&val);
			AddShortAction((action_type)action,ab,(unsigned short)val);
		}
		if (!_stricmp(entryt->Value(),"float"))
		{
			unsigned int action;
			float val;
			entryt->ToElement()->Attribute("action",&action);
			entryt->ToElement()->Attribute("val",&val);
			AddFloatAction((action_type)action,ab,val);
		}
		if (!_stricmp(entryt->Value(),"long"))
		{
			unsigned int action;
			unsigned int val;
			entryt->ToElement()->Attribute("action",&action);
			entryt->ToElement()->Attribute("val",&val);
			AddLongAction((action_type)action,ab,val);
		}
		if (!_stricmp(entryt->Value(),"noarg"))
		{
			unsigned int action;
			entryt->ToElement()->Attribute("action",&action);
			AddAction((action_type)action,ab);
		}
	}
}

Movie *GenerateApt(char *xmln)
{
	TiXmlDocument doc(xmln);
	doc.LoadFile();
	TiXmlNode* node = doc.FirstChild();
	while ((node->Type() != TiXmlNode::ELEMENT) && (_stricmp(node->Value(),"aptdata")))
	{
		node = node->NextSibling();
	}
	TiXmlNode *entry, *entry2, *entry3, *entry4, *entry5, *entry6;
	Movie *m = new Movie;
	m->type = MOVIE;
	m->signature = CHAR_SIG;
	m->pointer = 0;
	m->screensizex = 1024;
	m->screensizey = 768;
	m->unknown = 33;
	m->exportcount = 0;
	m->importcount = 0;
	m->charactercount = 1;
	m->characters.push_back(m);
	m->count = 0;
	for (entry = node->FirstChild(); entry != 0; entry = entry->NextSibling())
	{
		if (entry->Type() == TiXmlNode::ELEMENT)
		{
			if (!_stricmp(entry->Value(),"movieclip"))
			{
				unsigned int xx;
				entry->ToElement()->Attribute("screensizex", &xx);
				m->screensizex = xx;
				entry->ToElement()->Attribute("screensizey", &xx);
				m->screensizey = xx;
				entry->ToElement()->Attribute("unknown", &xx);
				m->unknown = xx;
				entry2 = entry->FirstChild();
				for (entry3 = entry2->FirstChild(); entry3 != 0; entry3 = entry3->NextSibling())
				{
					m->importcount++;
					Import *im = new Import;
					unsigned int x;
					entry3->ToElement()->Attribute("character",&x);
					im->character = x;
					im->movie = _strdup(entry3->ToElement()->Attribute("movie"));
					im->name = _strdup(entry3->ToElement()->Attribute("name"));
					im->pointer = 0;
					m->imports.push_back(im);
				}
				entry2 = entry2->NextSibling();
				for (entry3 = entry2->FirstChild(); entry3 != 0; entry3 = entry3->NextSibling())
				{
					m->exportcount++;
					Export *im = new Export;
					unsigned int x;
					entry3->ToElement()->Attribute("character",&x);
					im->character = x;
					im->name = _strdup(entry3->ToElement()->Attribute("name"));
					m->exports.push_back(im);
				}
				entry2 = entry2->NextSibling();
				m->framecount = 0;
				for (entry3 = entry2->FirstChild(); entry3 != 0; entry3 = entry3->NextSibling())
				{
					m->framecount++;
					Frame *f = new Frame;
					f->frameitemcount = 0;
					m->frames.push_back(f);
					for (entry4 = entry3->FirstChild(); entry4 != 0; entry4 = entry4->NextSibling())
					{
						if (!_stricmp(entry4->Value(),"action"))
						{
							f->frameitemcount++;
							Action *a = new Action;
							a->type = ACTION;
							a->ab.constantcount = 0;
							a->ab.stringcount = 0;
							a->ab.pushdatacount = 0;
							a->ab.actionbytecount = 0;
							a->ab.definefunction2count = 0;
							a->ab.definefunctioncount = 0;
							ProcessActions(entry4,&a->ab);
							f->frameitems.push_back(a);
						}
						if (!_stricmp(entry4->Value(),"framelabel"))
						{
							FrameLabel *l;
							l = new FrameLabel;
							l->type = FRAMELABEL;
							l->label = _strdup(entry4->ToElement()->Attribute("label"));
							entry4->ToElement()->Attribute("frame",&l->frame);
							l->flags = 0x70000;
							f->frameitems.push_back(l);
							f->frameitemcount++;
						}
						if (!_stricmp(entry4->Value(),"placeobject"))
						{
							f->frameitemcount++;
							PlaceObject *po = new PlaceObject;
							po->type = PLACEOBJECT;
							f->frameitems.push_back(po);
							entry4->ToElement()->Attribute("flags",&po->flags);
							entry4->ToElement()->Attribute("depth",&po->depth);
							entry4->ToElement()->Attribute("character",&po->character);
							entry4->ToElement()->Attribute("rotm00",&po->rotateandscale.m00);
							entry4->ToElement()->Attribute("rotm01",&po->rotateandscale.m01);
							entry4->ToElement()->Attribute("rotm10",&po->rotateandscale.m10);
							entry4->ToElement()->Attribute("rotm11",&po->rotateandscale.m11);
							entry4->ToElement()->Attribute("tx",&po->translate.X);
							entry4->ToElement()->Attribute("ty",&po->translate.Y);
							int red,green,blue,alpha;
							entry4->ToElement()->Attribute("red",&red);
							entry4->ToElement()->Attribute("green",&green);
							entry4->ToElement()->Attribute("blue",&blue);
							entry4->ToElement()->Attribute("alpha",&alpha);
							po->colortransform = red | (green * 0x100) | (blue * 0x10000) | (alpha * 0x1000000);
							entry4->ToElement()->Attribute("ratio",&po->ratio);
							entry4->ToElement()->Attribute("clipdepth",&po->clipdepth);
							entry4->ToElement()->Attribute("unknown",&po->unknown);
							entry5 = entry4->FirstChild("poname");
							if (entry5)
							{
								po->name = _strdup(entry5->ToElement()->Attribute("name"));
							}
							else
							{
								po->name = 0;
							}
							entry5 = entry4->FirstChild("clipactions");
							if (entry5)
							{
								po->poa = new PlaceObjectActions;
								po->poa->clipactioncount = 0;
								for (entry6 = entry5->FirstChild(); entry6 != 0; entry6 = entry6->NextSibling())
								{
									po->poa->clipactioncount++;
									PlaceObjectAction *poa = new PlaceObjectAction;
									po->poa->actions.push_back(poa);
									entry6->ToElement()->Attribute("flags",&poa->flags);
									entry6->ToElement()->Attribute("flags2",&poa->flags2);
									poa->actiondata = new Action;
									poa->actiondata->ab.constantcount = 0;
									poa->actiondata->ab.stringcount = 0;
									poa->actiondata->ab.pushdatacount = 0;
									poa->actiondata->ab.actionbytecount = 0;
									poa->actiondata->ab.definefunction2count = 0;
									poa->actiondata->ab.definefunctioncount = 0;
									ProcessActions(entry6,&poa->actiondata->ab);
								}
							}
							else
							{
								po->poa = 0;
							}
						}
						if (!_stricmp(entry4->Value(),"removeobject"))
						{
							RemoveObject *ro;
							ro = new RemoveObject;
							ro->type = REMOVEOBJECT;
							entry4->ToElement()->Attribute("depth",&ro->depth);
							f->frameitems.push_back(ro);
							f->frameitemcount++;
						}
						if (!_stricmp(entry4->Value(),"background"))
						{
							BackgroundColor *b;
							b = new BackgroundColor;
							b->type = BACKGROUNDCOLOR;
							entry4->ToElement()->Attribute("color",&b->color);
							f->frameitems.push_back(b);
							f->frameitemcount++;
						}
						if (!_stricmp(entry4->Value(),"initaction"))
						{
							f->frameitemcount++;
							InitAction *a = new InitAction;
							entry4->ToElement()->Attribute("sprite",&a->sprite);
							a->type = INITACTION;
							a->ab.constantcount = 0;
							a->ab.stringcount = 0;
							a->ab.pushdatacount = 0;
							a->ab.actionbytecount = 0;
							a->ab.definefunction2count = 0;
							a->ab.definefunctioncount = 0;
							ProcessActions(entry4,&a->ab);
							f->frameitems.push_back(a);
						}
					}
				}
			}
			if (!_stricmp(entry->Value(),"shape"))
			{
				m->charactercount++;
				Shape *sh = new Shape;
				sh->type = SHAPE;
				sh->signature = CHAR_SIG;
				entry->ToElement()->Attribute("top",&sh->bounds.top);
				entry->ToElement()->Attribute("left",&sh->bounds.left);
				entry->ToElement()->Attribute("bottom",&sh->bounds.bottom);
				entry->ToElement()->Attribute("right",&sh->bounds.right);
				entry->ToElement()->Attribute("geometry",&sh->geometry);
				m->characters.push_back(sh);
			}
			if (!_stricmp(entry->Value(),"edittext"))
			{
				m->charactercount++;
				EditText *sh = new EditText;
				sh->type = EDITTEXT;
				sh->signature = CHAR_SIG;
				entry->ToElement()->Attribute("top",&sh->bounds.top);
				entry->ToElement()->Attribute("left",&sh->bounds.left);
				entry->ToElement()->Attribute("bottom",&sh->bounds.bottom);
				entry->ToElement()->Attribute("right",&sh->bounds.right);
				entry->ToElement()->Attribute("font",&sh->font);
				entry->ToElement()->Attribute("alignment",&sh->alignment);
				int red,green,blue,alpha;
				entry->ToElement()->Attribute("red",&red);
				entry->ToElement()->Attribute("green",&green);
				entry->ToElement()->Attribute("blue",&blue);
				entry->ToElement()->Attribute("alpha",&alpha);
				sh->color = red | (green * 0x100) | (blue * 0x10000) | (alpha * 0x1000000);
				entry->ToElement()->Attribute("height",&sh->fontheight);
				entry->ToElement()->Attribute("readonly",&sh->readonly);
				entry->ToElement()->Attribute("multiline",&sh->multiline);
				entry->ToElement()->Attribute("wordwrap",&sh->wordwrap);
				entry2 = entry->FirstChild("ettext");
				if (entry2)
				{
					sh->text = _strdup(entry2->ToElement()->Attribute("text"));
					for (unsigned int len = 0;len < strlen(sh->text);len++)
					{
						if (sh->text[len] == 0x0A)
						{
							((char *)sh->text)[len] = 0x0D;
						}
					}
				}
				else
				{
					sh->text = 0;
				}
				entry2 = entry->FirstChild("etvar");
				if (entry2)
				{
					sh->variable = _strdup(entry2->ToElement()->Attribute("variable"));
				}
				else
				{
					sh->variable = 0;
				}
				m->characters.push_back(sh);
			}
			if (!_stricmp(entry->Value(),"font"))
			{
				m->charactercount++;
				Font *sh = new Font;
				sh->type = FONT;
				sh->signature = CHAR_SIG;
				sh->name = _strdup(entry->ToElement()->Attribute("name"));
				sh->glyphcount = 0;
				entry2 = entry->FirstChild("glyphs");
				if (entry2)
				{
					for (entry3 = entry2->FirstChild(); entry3 != 0; entry3 = entry3->NextSibling())
					{
						unsigned int glyph;
						entry3->ToElement()->Attribute("id",&glyph);
						sh->glyphs.push_back(glyph);
						sh->glyphcount++;
					}
				}
				m->characters.push_back(sh);
			}
			if (!_stricmp(entry->Value(),"button"))
			{
				m->charactercount++;
				Button *b = new Button;
				b->unknown = 0;
				b->unknown2 = 0;
				m->characters.push_back(b);
				b->type = BUTTON;
				b->signature = CHAR_SIG;
				entry->ToElement()->Attribute("top",&b->bounds.top);
				entry->ToElement()->Attribute("left",&b->bounds.left);
				entry->ToElement()->Attribute("bottom",&b->bounds.bottom);
				entry->ToElement()->Attribute("right",&b->bounds.right);
				entry2 = entry->FirstChild("vertexes");
				b->vertexcount = 0;
				if (entry2)
				{
					for (entry3 = entry2->FirstChild(); entry3 != 0; entry3 = entry3->NextSibling())
					{
						b->vertexcount++;
						Vector2 *v = new Vector2;
						entry3->ToElement()->Attribute("x",&v->X);
						entry3->ToElement()->Attribute("y",&v->Y);
						b->vertexes.push_back(v);
					}
				}
				entry2 = entry->FirstChild("triangles");
				b->trianglecount = 0;
				if (entry2)
				{
					for (entry3 = entry2->FirstChild(); entry3 != 0; entry3 = entry3->NextSibling())
					{
						b->trianglecount++;
						Triangle *v = new Triangle;
						entry3->ToElement()->Attribute("v1",(unsigned int *)&v->v1);
						entry3->ToElement()->Attribute("v2",(unsigned int *)&v->v2);
						entry3->ToElement()->Attribute("v3",(unsigned int *)&v->v3);
						b->triangles.push_back(v);
					}
				}
				b->recordcount = 0;
				entry2 = entry->FirstChild("buttonrecords");
				if (entry2)
				{
					for (entry3 = entry2->FirstChild(); entry3 != 0; entry3 = entry3->NextSibling())
					{
						b->recordcount++;
						ButtonRecord *br = new ButtonRecord;
						b->buttonrecords.push_back(br);
						entry3->ToElement()->Attribute("flags",&br->flags);
						entry3->ToElement()->Attribute("character",&br->character);
						entry3->ToElement()->Attribute("depth",&br->depth);
						entry3->ToElement()->Attribute("rotm00",&br->rotateandscale.m00);
						entry3->ToElement()->Attribute("rotm01",&br->rotateandscale.m01);
						entry3->ToElement()->Attribute("rotm10",&br->rotateandscale.m10);
						entry3->ToElement()->Attribute("rotm11",&br->rotateandscale.m11);
						entry3->ToElement()->Attribute("tx",&br->translate.X);
						entry3->ToElement()->Attribute("ty",&br->translate.Y);
						br->color.bottom = 1;
						br->color.top = 1;
						br->color.left = 1;
						br->color.right = 1;
						br->unknown.bottom = 0;
						br->unknown.top = 0;
						br->unknown.left = 0;
						br->unknown.right = 0;
					}
				}
				b->buttonactioncount = 0;
				entry2 = entry->FirstChild("buttonactions");
				if (entry2)
				{
					for (entry3 = entry2->FirstChild(); entry3 != 0; entry3 = entry3->NextSibling())
					{
						b->buttonactioncount++;
						ButtonAction *br = new ButtonAction;
						br->actiondata = new Action;
						entry3->ToElement()->Attribute("flags",&br->flags);
						br->actiondata->ab.constantcount = 0;
						br->actiondata->ab.stringcount = 0;
						br->actiondata->ab.pushdatacount = 0;
						br->actiondata->ab.actionbytecount = 0;
						br->actiondata->ab.definefunction2count = 0;
						br->actiondata->ab.definefunctioncount = 0;
						ProcessActions(entry3,&br->actiondata->ab);
						b->buttonactionrecords.push_back(br);
					}
				}
			}
			if (!_stricmp(entry->Value(),"sprite"))
			{
				m->charactercount++;
				Sprite *sp = new Sprite;
				m->characters.push_back(sp);
				sp->type = SPRITE;
				sp->signature = CHAR_SIG;
				sp->framecount = 0;
				sp->pointer = 0;
				entry2 = entry->FirstChild();
				if (entry2)
				{
					for (entry3 = entry2->FirstChild(); entry3 != 0; entry3 = entry3->NextSibling())
					{
						sp->framecount++;
						Frame *f = new Frame;
						f->frameitemcount = 0;
						sp->frames.push_back(f);
						for (entry4 = entry3->FirstChild(); entry4 != 0; entry4 = entry4->NextSibling())
						{
							if (!_stricmp(entry4->Value(),"action"))
							{
								f->frameitemcount++;
								Action *a = new Action;
								a->type = ACTION;
								a->ab.constantcount = 0;
								a->ab.stringcount = 0;
								a->ab.pushdatacount = 0;
								a->ab.actionbytecount = 0;
								a->ab.definefunction2count = 0;
								a->ab.definefunctioncount = 0;
								ProcessActions(entry4,&a->ab);
								f->frameitems.push_back(a);
							}
							if (!_stricmp(entry4->Value(),"framelabel"))
							{
								FrameLabel *l;
								l = new FrameLabel;
								l->type = FRAMELABEL;
								l->label = _strdup(entry4->ToElement()->Attribute("label"));
								entry4->ToElement()->Attribute("frame",&l->frame);
								l->flags = 0x70000;
								f->frameitems.push_back(l);
								f->frameitemcount++;
							}
							if (!_stricmp(entry4->Value(),"placeobject"))
							{
								f->frameitemcount++;
								PlaceObject *po = new PlaceObject;
								po->type = PLACEOBJECT;
								f->frameitems.push_back(po);
								entry4->ToElement()->Attribute("flags",&po->flags);
								entry4->ToElement()->Attribute("depth",&po->depth);
								entry4->ToElement()->Attribute("character",&po->character);
								entry4->ToElement()->Attribute("rotm00",&po->rotateandscale.m00);
								entry4->ToElement()->Attribute("rotm01",&po->rotateandscale.m01);
								entry4->ToElement()->Attribute("rotm10",&po->rotateandscale.m10);
								entry4->ToElement()->Attribute("rotm11",&po->rotateandscale.m11);
								entry4->ToElement()->Attribute("tx",&po->translate.X);
								entry4->ToElement()->Attribute("ty",&po->translate.Y);
								int red,green,blue,alpha;
								entry4->ToElement()->Attribute("red",&red);
								entry4->ToElement()->Attribute("green",&green);
								entry4->ToElement()->Attribute("blue",&blue);
								entry4->ToElement()->Attribute("alpha",&alpha);
								po->colortransform = red | (green * 0x100) | (blue * 0x10000) | (alpha * 0x1000000);
								entry4->ToElement()->Attribute("ratio",&po->ratio);
								entry4->ToElement()->Attribute("clipdepth",&po->clipdepth);
								entry4->ToElement()->Attribute("unknown",&po->unknown);
								entry5 = entry4->FirstChild("poname");
								if (entry5)
								{
									po->name = _strdup(entry5->ToElement()->Attribute("name"));
								}
								else
								{
									po->name = 0;
								}
								entry5 = entry4->FirstChild("clipactions");
								if (entry5)
								{
									po->poa = new PlaceObjectActions;
									po->poa->clipactioncount = 0;
									for (entry6 = entry5->FirstChild(); entry6 != 0; entry6 = entry6->NextSibling())
									{
										po->poa->clipactioncount++;
										PlaceObjectAction *poa = new PlaceObjectAction;
										po->poa->actions.push_back(poa);
										entry6->ToElement()->Attribute("flags",&poa->flags);
										entry6->ToElement()->Attribute("flags2",&poa->flags2);
										poa->actiondata = new Action;
										poa->actiondata->ab.constantcount = 0;
										poa->actiondata->ab.stringcount = 0;
										poa->actiondata->ab.pushdatacount = 0;
										poa->actiondata->ab.actionbytecount = 0;
										poa->actiondata->ab.definefunction2count = 0;
										poa->actiondata->ab.definefunctioncount = 0;
										ProcessActions(entry6,&poa->actiondata->ab);
									}
								}
								else
								{
									po->poa = 0;
								}
							}
							if (!_stricmp(entry4->Value(),"removeobject"))
							{
								RemoveObject *ro;
								ro = new RemoveObject;
								ro->type = REMOVEOBJECT;
								entry4->ToElement()->Attribute("depth",&ro->depth);
								f->frameitems.push_back(ro);
								f->frameitemcount++;
							}
							if (!_stricmp(entry4->Value(),"background"))
							{
								BackgroundColor *b;
								b = new BackgroundColor;
								b->type = BACKGROUNDCOLOR;
								entry4->ToElement()->Attribute("color",&b->color);
								f->frameitems.push_back(b);
								f->frameitemcount++;
							}
							if (!_stricmp(entry4->Value(),"initaction"))
							{
								f->frameitemcount++;
								InitAction *a = new InitAction;
								entry4->ToElement()->Attribute("sprite",&a->sprite);
								a->type = INITACTION;
								a->ab.constantcount = 0;
								a->ab.stringcount = 0;
								a->ab.pushdatacount = 0;
								a->ab.actionbytecount = 0;
								a->ab.definefunction2count = 0;
								a->ab.definefunctioncount = 0;
								ProcessActions(entry4,&a->ab);
								f->frameitems.push_back(a);
							}
						}
					}
				}
			}
			if (!_stricmp(entry->Value(),"image"))
			{
				m->charactercount++;
				Image *sh = new Image;
				sh->type = IMAGE;
				sh->signature = CHAR_SIG;
				entry->ToElement()->Attribute("image",&sh->texture);
				m->characters.push_back(sh);
			}
			if (!_stricmp(entry->Value(),"morph"))
			{
				m->charactercount++;
				Morph *sh = new Morph;
				sh->type = MORPH;
				sh->signature = CHAR_SIG;
				entry->ToElement()->Attribute("start",&sh->startshape);
				entry->ToElement()->Attribute("end",&sh->endshape);
				m->characters.push_back(sh);
			}
			if (!_stricmp(entry->Value(),"text"))
			{
				m->charactercount++;
				Text *sh = new Text;
				sh->type = TEXT;
				sh->signature = CHAR_SIG;
				entry->ToElement()->Attribute("top",&sh->bounds.top);
				entry->ToElement()->Attribute("left",&sh->bounds.left);
				entry->ToElement()->Attribute("bottom",&sh->bounds.bottom);
				entry->ToElement()->Attribute("right",&sh->bounds.right);
				entry->ToElement()->Attribute("rotm00",&sh->rotateandscale.m00);
				entry->ToElement()->Attribute("rotm01",&sh->rotateandscale.m01);
				entry->ToElement()->Attribute("rotm10",&sh->rotateandscale.m10);
				entry->ToElement()->Attribute("rotm11",&sh->rotateandscale.m11);
				entry->ToElement()->Attribute("tx",&sh->translate.X);
				entry->ToElement()->Attribute("ty",&sh->translate.Y);
				entry2 = entry->FirstChild("records");
				sh->recordcount = 0;
				if (entry2)
				{
					for (entry3 = entry2->FirstChild(); entry3 != 0; entry3 = entry3->NextSibling())
					{
						TextRecord *tr = new TextRecord;
						sh->recordcount++;
						sh->records.push_back(tr);
						entry3->ToElement()->Attribute("font",&tr->font);
						entry3->ToElement()->Attribute("red",&tr->color.top);
						entry3->ToElement()->Attribute("green",&tr->color.left);
						entry3->ToElement()->Attribute("blue",&tr->color.bottom);
						entry3->ToElement()->Attribute("alpha",&tr->color.right);
						entry3->ToElement()->Attribute("u1",&tr->unknown.top);
						entry3->ToElement()->Attribute("u2",&tr->unknown.left);
						entry3->ToElement()->Attribute("u3",&tr->unknown.bottom);
						entry3->ToElement()->Attribute("u4",&tr->unknown.right);
						entry3->ToElement()->Attribute("tx",&tr->offset.X);
						entry3->ToElement()->Attribute("ty",&tr->offset.Y);
						entry3->ToElement()->Attribute("height",&tr->textheight);
						entry4 = entry3->FirstChild("glyphs");
						tr->glyphcount = 0;
						if (entry4)
						{
							for (entry5 = entry4->FirstChild(); entry5 != 0; entry5 = entry5->NextSibling())
							{
								Glyph *gl = new Glyph;
								entry5->ToElement()->Attribute("index",&gl->index);
								entry5->ToElement()->Attribute("advance",&gl->advance);
								tr->glyphs.push_back(gl);
								tr->glyphcount++;
							}
						}
					}
				}
				m->characters.push_back(sh);
			}
			if (!_stricmp(entry->Value(),"empty"))
			{
				m->charactercount++;
				m->characters.push_back(0);
			}
		}
	}
	return m;
}

int main(int argc, char* argv[])
{
	char aptapt[MAX_PATH];
	char xmln[MAX_PATH];
	sprintf(xmln,"%s.xml",argv[1]);
	sprintf(aptapt,"%s.apt",argv[1]);
	Movie *m = GenerateApt(xmln);
	GenerateAptAptFile(m,aptapt);
	return 0;
}
