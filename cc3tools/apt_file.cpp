/*
	Command & Conquer 3 Tools
	apt write code
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

unsigned int GetFrameItemSize(Frame *fr)
{
	unsigned int framesize = 0;
	for (unsigned int i = 0;i < fr->frameitemcount;i++)
	{
		switch(fr->frameitems[i]->type)
		{
		case ACTION:
			framesize += sizeof(OutputAction);
			break;
		case FRAMELABEL:
			framesize += sizeof(FrameLabel);
			break;
		case PLACEOBJECT:
			framesize += sizeof(OutputPlaceObject);
			break;
		case REMOVEOBJECT:
			framesize += sizeof(RemoveObject);
			break;
		case BACKGROUNDCOLOR:
			framesize += sizeof(BackgroundColor);
			break;
		case INITACTION:
			framesize += sizeof(OutputInitAction);
			break;
		}
	}
	return framesize;
}

unsigned int GetFrameItemPointerSize(Frame *fr)
{
	unsigned int framesize = 0;
	for (unsigned int i = 0;i < fr->frameitemcount;i++)
	{
		switch(fr->frameitems[i]->type)
		{
		case ACTION:
			{
				Action *a = ((Action *)fr->frameitems[i]);
				framesize += a->ab.actionbytecount;
				framesize += GETALIGN(a->ab.actionbytecount);
			}
			break;
		case FRAMELABEL:
			{
				FrameLabel *f = ((FrameLabel *)fr->frameitems[i]);
				framesize += STRLENGTH(f->label);
			}
			break;
		case PLACEOBJECT:
			{
				PlaceObject *pl = ((PlaceObject *)fr->frameitems[i]);
				if (pl->name)
				{
					framesize += STRLENGTH(pl->name);
				}
				if (pl->poa)
				{
					framesize += sizeof(OutputPlaceObjectActions);
				}
			}
			break;
		case INITACTION:
			{
				InitAction *a = ((InitAction *)fr->frameitems[i]);
				framesize += a->ab.actionbytecount;
				framesize += GETALIGN(a->ab.actionbytecount);
			}
			break;
		}
	}
	return framesize;
}

unsigned int GetFrameItemPointerPointerSize(Frame *fr)
{
	unsigned int framesize = 0;
	for (unsigned int i = 0;i < fr->frameitemcount;i++)
	{
		switch(fr->frameitems[i]->type)
		{
		case ACTION:
			{
				Action *a = ((Action *)fr->frameitems[i]);
				framesize += a->ab.constantcount * 4;
				for (unsigned int j = 0;j < a->ab.stringcount;j++)
				{
					framesize += STRLENGTH(a->ab.actionstrings[j]->string);
				}
				for (unsigned int j = 0;j < a->ab.pushdatacount;j++)
				{
					framesize += a->ab.actionpushdatas[j]->pushdatacount * 4;
				}
				for (unsigned int j = 0;j < a->ab.definefunction2count;j++)
				{
					for (unsigned int arg = 0;arg < a->ab.actiondefinefunction2s[j]->argumentcount;arg++)
					{
						framesize += 8;
						framesize += STRLENGTH(a->ab.actiondefinefunction2s[j]->arguments[arg]->name);
					}
				}
				for (unsigned int j = 0;j < a->ab.definefunctioncount;j++)
				{
					for (unsigned int arg = 0;arg < a->ab.actiondefinefunctions[j]->argumentcount;arg++)
					{
						framesize += 4;
						framesize += STRLENGTH(a->ab.actiondefinefunctions[j]->arguments[arg]);
					}
				}
			}
			break;
		case PLACEOBJECT:
			{
				PlaceObject *pl = ((PlaceObject *)fr->frameitems[i]);
				if (pl->poa)
				{
					framesize += pl->poa->clipactioncount * sizeof(OutputPlaceObjectAction);
				}
			}
			break;
		case INITACTION:
			{
				InitAction *a = ((InitAction *)fr->frameitems[i]);
				framesize += a->ab.constantcount * 4;
				for (unsigned int j = 0;j < a->ab.stringcount;j++)
				{
					framesize += STRLENGTH(a->ab.actionstrings[j]->string);
				}
				for (unsigned int j = 0;j < a->ab.pushdatacount;j++)
				{
					framesize += a->ab.actionpushdatas[j]->pushdatacount * 4;
				}
				for (unsigned int j = 0;j < a->ab.definefunction2count;j++)
				{
					for (unsigned int arg = 0;arg < a->ab.actiondefinefunction2s[j]->argumentcount;arg++)
					{
						framesize += 8;
						framesize += STRLENGTH(a->ab.actiondefinefunction2s[j]->arguments[arg]->name);
					}
				}
				for (unsigned int j = 0;j < a->ab.definefunctioncount;j++)
				{
					for (unsigned int arg = 0;arg < a->ab.actiondefinefunctions[j]->argumentcount;arg++)
					{
						framesize += 4;
						framesize += STRLENGTH(a->ab.actiondefinefunctions[j]->arguments[arg]);
					}
				}
			}
			break;
		}
	}
	return framesize;
}

unsigned int GetFrameItemPointerPointerPointerSize(Frame *fr)
{
	unsigned int framesize = 0;
	for (unsigned int i = 0;i < fr->frameitemcount;i++)
	{
		switch(fr->frameitems[i]->type)
		{
		case PLACEOBJECT:
			{
				PlaceObject *pl = ((PlaceObject *)fr->frameitems[i]);
				if (pl->poa)
				{
					for (unsigned int j = 0;j < pl->poa->clipactioncount;j++)
					{
						framesize += pl->poa->actions[j]->actiondata->ab.actionbytecount;
						framesize += GETALIGN(pl->poa->actions[j]->actiondata->ab.actionbytecount);
					}
				}
			}
			break;
		}
	}
	return framesize;
}

unsigned int GetFrameItemPointerPointerPointerPointerSize(Frame *fr)
{
	unsigned int framesize = 0;
	for (unsigned int i = 0;i < fr->frameitemcount;i++)
	{
		switch(fr->frameitems[i]->type)
		{
		case PLACEOBJECT:
			{
				PlaceObject *pl = ((PlaceObject *)fr->frameitems[i]);
				if (pl->poa)
				{
					for (unsigned int j = 0;j < pl->poa->clipactioncount;j++)
					{
						framesize += pl->poa->actions[j]->actiondata->ab.constantcount * 4;
						for (unsigned int k = 0;k < pl->poa->actions[j]->actiondata->ab.stringcount;k++)
						{
							framesize += STRLENGTH(pl->poa->actions[j]->actiondata->ab.actionstrings[k]->string);
						}
						for (unsigned int k = 0;k < pl->poa->actions[j]->actiondata->ab.pushdatacount;k++)
						{
							framesize += pl->poa->actions[j]->actiondata->ab.actionpushdatas[k]->pushdatacount * 4;
						}
						for (unsigned int k = 0;k < pl->poa->actions[j]->actiondata->ab.definefunction2count;k++)
						{
							for (unsigned int arg = 0;arg < pl->poa->actions[j]->actiondata->ab.actiondefinefunction2s[k]->argumentcount;arg++)
							{
								framesize += 8;
								framesize += STRLENGTH(pl->poa->actions[j]->actiondata->ab.actiondefinefunction2s[k]->arguments[arg]->name);
							}
						}
						for (unsigned int k = 0;k < pl->poa->actions[j]->actiondata->ab.definefunctioncount;k++)
						{
							for (unsigned int arg = 0;arg < pl->poa->actions[j]->actiondata->ab.actiondefinefunctions[k]->argumentcount;arg++)
							{
								framesize += 4;
								framesize += STRLENGTH(pl->poa->actions[j]->actiondata->ab.actiondefinefunctions[k]->arguments[arg]);
							}
						}
					}
				}
			}
			break;
		}
	}
	return framesize;
}

void CC3TOOLS_DLL_EXPORT GenerateAptAptFile(Movie *m,const char *filename)
{
	unsigned int aptdatasize = 16;
	unsigned int characterdatasize = 0;
	unsigned int framesize = 0;
	unsigned int frameitemlistsize = 0;
	unsigned int frameitemsize = 0;
	unsigned int frameitempointersize = 0;
	unsigned int frameitempointerpointersize = 0;
	unsigned int frameitempointerpointerpointersize = 0;
	for (unsigned int i = 0;i < m->importcount;i++)
	{
		aptdatasize += sizeof(Import);
		aptdatasize += STRLENGTH(m->imports[i]->movie);
		aptdatasize += STRLENGTH(m->imports[i]->name);
	}
	for (unsigned int i = 0;i < m->exportcount;i++)
	{
		aptdatasize += sizeof(Export);
		aptdatasize += STRLENGTH(m->exports[i]->name);
	}
	aptdatasize += m->charactercount * 4;
	for (unsigned int i = 0;i < m->charactercount;i++)
	{
		if (m->characters[i])
		{
			switch(m->characters[i]->type)
			{
			case BUTTON:
				{
					aptdatasize += sizeof(OutputButton);
					characterdatasize += sizeof(OutputButton);
					Button *b = (Button *)m->characters[i];
					frameitemsize += b->vertexcount * sizeof(Vector2);
					aptdatasize += b->vertexcount * sizeof(Vector2);
					frameitemsize += b->trianglecount * sizeof(Triangle);
					aptdatasize += b->trianglecount * sizeof(Triangle);
					frameitemsize += GETALIGN(b->trianglecount * sizeof(Triangle));
					aptdatasize += GETALIGN(b->trianglecount * sizeof(Triangle));
					frameitemsize += b->recordcount * sizeof(ButtonRecord);
					aptdatasize += b->recordcount * sizeof(ButtonRecord);
					frameitemsize += b->buttonactioncount * sizeof(OutputButtonAction);
					aptdatasize += b->buttonactioncount * sizeof(OutputButtonAction);
					unsigned int fipsize = 0;
					unsigned int fippsize = 0;
					for (unsigned int bu = 0;bu < b->buttonactioncount;bu++)
					{
						fipsize += b->buttonactionrecords[bu]->actiondata->ab.actionbytecount;
						fipsize += GETALIGN(b->buttonactionrecords[bu]->actiondata->ab.actionbytecount);
						fippsize += b->buttonactionrecords[bu]->actiondata->ab.constantcount * 4;
						for (unsigned int j = 0;j < b->buttonactionrecords[bu]->actiondata->ab.stringcount;j++)
						{
							fippsize += STRLENGTH(b->buttonactionrecords[bu]->actiondata->ab.actionstrings[j]->string);
						}
						for (unsigned int j = 0;j < b->buttonactionrecords[bu]->actiondata->ab.pushdatacount;j++)
						{
							fippsize += b->buttonactionrecords[bu]->actiondata->ab.actionpushdatas[j]->pushdatacount * 4;
						}
						for (unsigned int j = 0;j < b->buttonactionrecords[bu]->actiondata->ab.definefunction2count;j++)
						{
							for (unsigned int arg = 0;arg < b->buttonactionrecords[bu]->actiondata->ab.actiondefinefunction2s[j]->argumentcount;arg++)
							{
								fippsize += 8;
								fippsize += STRLENGTH(b->buttonactionrecords[bu]->actiondata->ab.actiondefinefunction2s[j]->arguments[arg]->name);
							}
						}
						for (unsigned int j = 0;j < b->buttonactionrecords[bu]->actiondata->ab.definefunctioncount;j++)
						{
							for (unsigned int arg = 0;arg < b->buttonactionrecords[bu]->actiondata->ab.actiondefinefunctions[j]->argumentcount;arg++)
							{
								fippsize += 4;
								fippsize += STRLENGTH(b->buttonactionrecords[bu]->actiondata->ab.actiondefinefunctions[j]->arguments[arg]);
							}
						}
					}
					frameitempointersize += fipsize;
					aptdatasize += fipsize;
					frameitempointerpointersize += fippsize;
					aptdatasize += fippsize;
				}
				break;
			case SHAPE:
				aptdatasize += sizeof(Shape);
				characterdatasize += sizeof(Shape);
				break;
			case EDITTEXT:
				aptdatasize += sizeof(EditText);
				characterdatasize += sizeof(EditText);
				if (((EditText *)m->characters[i])->text)
				{
					aptdatasize += STRLENGTH(((EditText *)m->characters[i])->text);
					framesize += STRLENGTH(((EditText *)m->characters[i])->text);
				}
				if (((EditText *)m->characters[i])->variable)
				{
					aptdatasize += STRLENGTH(((EditText *)m->characters[i])->variable);
					framesize += STRLENGTH(((EditText *)m->characters[i])->variable);
				}
				break;
			case FONT:
				aptdatasize += sizeof(OutputFont);
				characterdatasize += sizeof(OutputFont);
				aptdatasize += STRLENGTH(((Font *)m->characters[i])->name);
				framesize += STRLENGTH(((Font *)m->characters[i])->name);
				aptdatasize += ((Font *)m->characters[i])->glyphcount * 4;
				framesize += ((Font *)m->characters[i])->glyphcount * 4;
				break;
			case SPRITE:
				aptdatasize += sizeof(OutputSprite);
				characterdatasize += sizeof(OutputSprite);
				for (unsigned int j = 0;j < ((Sprite *)m->characters[i])->framecount;j++)
				{
					aptdatasize += 8;
					framesize += 8;
					if (((Sprite *)m->characters[i])->frames[j])
					{
						frameitemlistsize += ((Sprite *)m->characters[i])->frames[j]->frameitemcount * 4;
						aptdatasize += ((Sprite *)m->characters[i])->frames[j]->frameitemcount * 4;
						int frsize = GetFrameItemSize(((Sprite *)m->characters[i])->frames[j]);
						aptdatasize += frsize;
						frameitemsize += frsize;
						int frpsize = GetFrameItemPointerSize(((Sprite *)m->characters[i])->frames[j]);
						aptdatasize += frpsize;
						frameitempointersize += frpsize;
						int frppsize = GetFrameItemPointerPointerSize(((Sprite *)m->characters[i])->frames[j]);
						aptdatasize += frppsize;
						frameitempointerpointersize += frppsize;
						int frpppsize = GetFrameItemPointerPointerPointerSize(((Sprite *)m->characters[i])->frames[j]);
						aptdatasize += frpppsize;
						frameitempointerpointerpointersize += frpppsize;
						aptdatasize += GetFrameItemPointerPointerPointerPointerSize(((Sprite *)m->characters[i])->frames[j]);
					}
				}
				break;
			case IMAGE:
				aptdatasize += sizeof(Image);
				characterdatasize += sizeof(Image);
				break;
			case MORPH:
				aptdatasize += sizeof(Morph);
				characterdatasize += sizeof(Morph);
				break;
			case MOVIE:
				aptdatasize += sizeof(OutputMovie);
				characterdatasize += sizeof(OutputMovie);
				for (unsigned int j = 0;j < ((Movie *)m->characters[i])->framecount;j++)
				{
					aptdatasize += 8;
					framesize += 8;
					if (((Movie *)m->characters[i])->frames[j])
					{
						frameitemlistsize += ((Movie *)m->characters[i])->frames[j]->frameitemcount * 4;
						aptdatasize += ((Movie *)m->characters[i])->frames[j]->frameitemcount * 4;
						int frsize = GetFrameItemSize(((Movie *)m->characters[i])->frames[j]);
						aptdatasize += frsize;
						frameitemsize += frsize;
						int frpsize = GetFrameItemPointerSize(((Movie *)m->characters[i])->frames[j]);
						aptdatasize += frpsize;
						frameitempointersize += frpsize;
						int frppsize = GetFrameItemPointerPointerSize(((Movie *)m->characters[i])->frames[j]);
						aptdatasize += frppsize;
						frameitempointerpointersize += frppsize;
						int frpppsize = GetFrameItemPointerPointerPointerSize(((Movie *)m->characters[i])->frames[j]);
						aptdatasize += frpppsize;
						frameitempointerpointerpointersize += frpppsize;
						aptdatasize += GetFrameItemPointerPointerPointerPointerSize(((Movie *)m->characters[i])->frames[j]);
					}
				}
				break;
			case TEXT:
				{
					aptdatasize += sizeof(OutputText);
					characterdatasize += sizeof(OutputText);
					Text *t = (Text *)m->characters[i];
					frameitemsize += t->recordcount * sizeof(OutputTextRecord);
					aptdatasize += t->recordcount * sizeof(OutputTextRecord);
					unsigned int fipsize = 0;
					for (unsigned int te = 0;te < t->recordcount;te++)
					{
						fipsize += t->records[te]->glyphcount * sizeof(Glyph);
					}
					frameitempointersize += fipsize;
					aptdatasize += fipsize;
				}
				break;
			}
		}
	}
	unsigned char *aptdata = new unsigned char[aptdatasize];
	memset(aptdata,0,aptdatasize);
	unsigned char *aptpos = aptdata;
	std::vector <unsigned char **> relocations;
	memcpy(aptpos,"Apt Data:74\x1A\0\0\0\0",16);
	aptpos += 16;
	Import *imp = (Import *)aptpos;
	aptpos += m->importcount * sizeof(Import);
	for (unsigned int i = 0;i < m->importcount;i++)
	{
		imp[i].character = m->imports[i]->character;
		imp[i].pointer = m->imports[i]->pointer;
		strcpy((char *)aptpos,m->imports[i]->movie);
		imp[i].movie = (char *)aptpos;
		relocations.push_back((unsigned char **)(&imp[i].movie));
		aptpos += STRLENGTH(m->imports[i]->movie);
		strcpy((char *)aptpos,m->imports[i]->name);
		imp[i].name = (char *)aptpos;
		relocations.push_back((unsigned char **)(&imp[i].name));
		aptpos += STRLENGTH(m->imports[i]->name);
	}
	Export *ex = (Export *)aptpos;
	aptpos += m->exportcount * sizeof(Export);
	for (unsigned int i = 0;i < m->exportcount;i++)
	{
		ex[i].character = m->exports[i]->character;
		strcpy((char *)aptpos,m->exports[i]->name);
		ex[i].name = (char *)aptpos;
		relocations.push_back((unsigned char **)(&ex[i].name));
		aptpos += STRLENGTH(m->exports[i]->name);
	}
	unsigned char **ch = (unsigned char **)aptpos;
	aptpos += m->charactercount * 4;
	unsigned char *chd = aptpos;
	aptpos += characterdatasize;
	OutputFrame *fr = (OutputFrame *)aptpos;
	aptpos += framesize;
	unsigned char **fil = (unsigned char **)aptpos;
	aptpos += frameitemlistsize;
	unsigned char *fi = aptpos;
	aptpos += frameitemsize;
	unsigned char *fip = aptpos;
	aptpos += frameitempointersize;
	unsigned char *ads = aptpos;
	aptpos += frameitempointerpointersize;
	unsigned char *ads2 = aptpos;
	aptpos += frameitempointerpointerpointersize;
	unsigned char *ads3 = aptpos;
	for (unsigned int i = 0;i < m->charactercount;i++)
	{
		if (m->characters[i])
		{
			*ch = chd;
			relocations.push_back((unsigned char **)(ch));
			switch (m->characters[i]->type)
			{
			case BUTTON:
				{
					OutputButton *ob = (OutputButton *)chd;
					Button *b = (Button *)m->characters[i];
					chd += sizeof(OutputButton);
					ob->type = b->type;
					ob->signature = b->signature;
					ob->unknown = b->unknown;
					ob->unknown2 = b->unknown2;
					ob->bounds = b->bounds;
					ob->trianglecount = b->trianglecount;
					ob->vertexcount = b->vertexcount;
					if (ob->vertexcount)
					{
						Vector2 *v = (Vector2 *)fi;
						ob->vertexes = v;
						relocations.push_back((unsigned char **)(&ob->vertexes));
						for (unsigned int vc = 0;vc < ob->vertexcount;vc++)
						{
							*v = *(b->vertexes[vc]);
							v++;
						}
						fi = (unsigned char *)v;
					}
					if (ob->trianglecount)
					{
						Triangle *t = (Triangle *)fi;
						ob->triangles = t;
						relocations.push_back((unsigned char **)(&ob->triangles));
						for (unsigned int tc = 0;tc < ob->trianglecount;tc++)
						{
							*t = *(b->triangles[tc]);
							t++;
						}
						fi = (unsigned char *)t;
						fi += GETALIGN(ob->trianglecount * sizeof(Triangle));
					}
					ob->recordcount = b->recordcount;
					if (ob->recordcount)
					{
						ButtonRecord *r = (ButtonRecord *)fi;
						ob->buttonrecords = r;
						relocations.push_back((unsigned char **)(&ob->buttonrecords));
						for (unsigned int rc = 0;rc < ob->recordcount;rc++)
						{
							*r = *(b->buttonrecords[rc]);
							r++;
						}
						fi = (unsigned char *)r;
					}
					ob->buttonactioncount = b->buttonactioncount;
					if (ob->buttonactioncount)
					{
						OutputButtonAction *a = (OutputButtonAction *)fi;
						ob->buttonactionrecords = a;
						relocations.push_back((unsigned char **)(&ob->buttonactionrecords));
						for (unsigned int ac = 0;ac < ob->buttonactioncount;ac++)
						{
							ButtonAction *ba = (ButtonAction *)b->buttonactionrecords[ac];
							a->flags = ba->flags;
							Action *ai = ba->actiondata;
							relocations.push_back((unsigned char **)(&a->actiondata));
							std::string s = ai->ab.actionbytes.str();
							const char *sc = s.c_str();
							a->actiondata = fip;
							unsigned char *abcs = fip;
							memcpy(fip,sc,ai->ab.actionbytecount);
							fip += ai->ab.actionbytecount;
							fip += GETALIGN(ai->ab.actionbytecount);
							if (ai->ab.constantcount)
							{
								*(unsigned char **)(abcs + 8) = ads;
								relocations.push_back((unsigned char **)(abcs + 8));
								for (unsigned int l = 0;l < ai->ab.constantcount;l++)
								{
									*(unsigned int *)ads = ai->ab.constants[l];
									ads += 4;
								}
							}
							for (unsigned int l = 0;l < ai->ab.stringcount;l++)
							{
								*(unsigned char **)(abcs+ai->ab.actionstrings[l]->stringoffset) = ads;
								relocations.push_back((unsigned char **)(abcs+ai->ab.actionstrings[l]->stringoffset));
								strcpy((char *)ads,ai->ab.actionstrings[l]->string);
								ads += STRLENGTH(ai->ab.actionstrings[l]->string);
							}
							for (unsigned int l = 0;l < ai->ab.pushdatacount;l++)
							{
								*(unsigned char **)(abcs+ai->ab.actionpushdatas[l]->pushdataoffset) = ads;
								relocations.push_back((unsigned char **)(abcs+ai->ab.actionpushdatas[l]->pushdataoffset));
								for (unsigned int pd = 0;pd < ai->ab.actionpushdatas[l]->pushdatacount;pd++)
								{
									*(unsigned int *)ads = ai->ab.actionpushdatas[l]->pushdata[pd];
									ads += 4;
								}
							}
							for (unsigned int l = 0;l < ai->ab.definefunction2count;l++)
							{
								*(unsigned char **)(abcs+ai->ab.actiondefinefunction2s[l]->definefunction2offset) = ads;
								relocations.push_back((unsigned char **)(abcs+ai->ab.actiondefinefunction2s[l]->definefunction2offset));
								unsigned char *adst = ads;
								ads += (ai->ab.actiondefinefunction2s[l]->argumentcount * 8);
								for (unsigned int pd = 0;pd < ai->ab.actiondefinefunction2s[l]->argumentcount;pd++)
								{
									*(unsigned int *)adst = ai->ab.actiondefinefunction2s[l]->arguments[pd]->reg;
									adst += 4;
									*(unsigned char **)adst = ads;
									relocations.push_back((unsigned char **)(adst));
									adst += 4;
									strcpy((char *)ads,ai->ab.actiondefinefunction2s[l]->arguments[pd]->name);
									ads += STRLENGTH(ai->ab.actiondefinefunction2s[l]->arguments[pd]->name);
								}
							}
							for (unsigned int l = 0;l < ai->ab.definefunctioncount;l++)
							{
								*(unsigned char **)(abcs+ai->ab.actiondefinefunctions[l]->definefunctionoffset) = ads;
								relocations.push_back((unsigned char **)(abcs+ai->ab.actiondefinefunctions[l]->definefunctionoffset));
								unsigned char *adst = ads;
								ads += (ai->ab.actiondefinefunctions[l]->argumentcount * 4);
								for (unsigned int pd = 0;pd < ai->ab.actiondefinefunctions[l]->argumentcount;pd++)
								{
									*(unsigned char **)adst = ads;
									relocations.push_back((unsigned char **)(adst));
									adst += 4;
									strcpy((char *)ads,ai->ab.actiondefinefunctions[l]->arguments[pd]);
									ads += STRLENGTH(ai->ab.actiondefinefunctions[l]->arguments[pd]);
								}
							}
							a++;
						}
						fi = (unsigned char *)a;
					}
				}
				break;
			case SHAPE:
				{
					Shape *sh = (Shape *)chd;
					Shape *shi = (Shape *)m->characters[i];
					chd += sizeof(Shape);
					sh->type = shi->type;
					sh->bounds = shi->bounds;
					sh->geometry = shi->geometry;
					sh->signature = shi->signature;
				}
				break;
			case EDITTEXT:
				{
					EditText *et = (EditText *)chd;
					EditText *eti = (EditText *)m->characters[i];
					chd += sizeof(EditText);
					et->type = eti->type;
					et->signature = eti->signature;
					et->bounds = eti->bounds;
					et->font = eti->font;
					et->color = eti->color;
					et->alignment = eti->alignment;
					et->fontheight = eti->fontheight;
					et->readonly = eti->readonly;
					et->multiline = eti->multiline;
					et->wordwrap = eti->wordwrap;
					if (eti->text)
					{
						char *frx = (char *)fr;
						et->text = frx;
						relocations.push_back((unsigned char **)(&et->text));
						strcpy(frx,eti->text);
						frx += STRLENGTH(eti->text);
						fr = (OutputFrame *)frx;
					}
					else
					{
						et->text = 0;
					}
					if (eti->variable)
					{
						char *frx = (char *)fr;
						et->variable = frx;
						relocations.push_back((unsigned char **)(&et->variable));
						strcpy(frx,eti->variable);
						frx += STRLENGTH(eti->variable);
						fr = (OutputFrame *)frx;
					}
					else
					{
						et->variable = 0;
					}
				}
				break;
			case FONT:
				{
					OutputFont *fo = (OutputFont *)chd;
					Font *foi = (Font *)m->characters[i];
					chd += sizeof(OutputFont);
					fo->type = foi->type;
					fo->signature = foi->signature;
					char *frx = (char *)fr;
					fo->name = frx;
					relocations.push_back((unsigned char **)(&fo->name));
					strcpy((char *)frx,foi->name);
					frx += STRLENGTH(foi->name);
					fr = (OutputFrame *)frx;
					if (foi->glyphcount)
					{
						fo->glyphcount = foi->glyphcount;
						fo->glyphs = (unsigned int *)fr;
						unsigned int *frg = fo->glyphs;
						relocations.push_back((unsigned char **)(&fo->glyphs));
						for (unsigned int j = 0;j < foi->glyphcount;j++)
						{
							*frg = foi->glyphs[j];
							frg++;
						}
						fr = (OutputFrame *)frg;
					}
					else
					{
						fo->glyphs = 0;
					}
				}
				break;
			case SPRITE:
				{
					OutputSprite *spr = (OutputSprite *)chd;
					Sprite *spri = (Sprite *)m->characters[i];
					chd += sizeof(OutputSprite);
					spr->type = spri->type;
					spr->signature = spri->signature;
					spr->framecount = spri->framecount;
					if (spr->framecount)
					{
						spr->frames = fr;
						relocations.push_back((unsigned char **)(&spr->frames));
					}
					else
					{
						spr->frames = 0;
					}
					spr->pointer = spri->pointer;
					for (unsigned int j = 0;j < spri->framecount;j++)
					{
						if (spri->frames[j])
						{
							if (spri->frames[j]->frameitemcount)
							{
								fr->frameitemcount = spri->frames[j]->frameitemcount;
								fr->frameitems = (FrameItem**)fil;
								relocations.push_back((unsigned char**)(&fr->frameitems));
							}
							else
							{
								fr->frameitems = 0;
							}
							fr++;
							for (unsigned int k = 0;k < spri->frames[j]->frameitemcount;k++)
							{
								*fil = fi;
								relocations.push_back((unsigned char **)(fil));
								fil++;
								switch (spri->frames[j]->frameitems[k]->type)
								{
								case ACTION:
									{
										OutputAction *a = (OutputAction *)fi;
										Action *ai = (Action *)spri->frames[j]->frameitems[k];
										fi += sizeof(OutputAction);
										a->type = ai->type;
										relocations.push_back((unsigned char **)(&a->actionbytes));
										std::string s = ai->ab.actionbytes.str();
										const char *sc = s.c_str();
										a->actionbytes = fip;
										unsigned char *abcs = fip;
										memcpy(fip,sc,ai->ab.actionbytecount);
										fip += ai->ab.actionbytecount;
										fip += GETALIGN(ai->ab.actionbytecount);
										if (ai->ab.constantcount)
										{
											*(unsigned char **)(abcs + 8) = ads;
											relocations.push_back((unsigned char **)(abcs + 8));
											for (unsigned int l = 0;l < ai->ab.constantcount;l++)
											{
												*(unsigned int *)ads = ai->ab.constants[l];
												ads += 4;
											}
										}
										for (unsigned int l = 0;l < ai->ab.stringcount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actionstrings[l]->stringoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actionstrings[l]->stringoffset));
											strcpy((char *)ads,ai->ab.actionstrings[l]->string);
											ads += STRLENGTH(ai->ab.actionstrings[l]->string);
										}
										for (unsigned int l = 0;l < ai->ab.pushdatacount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actionpushdatas[l]->pushdataoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actionpushdatas[l]->pushdataoffset));
											for (unsigned int pd = 0;pd < ai->ab.actionpushdatas[l]->pushdatacount;pd++)
											{
												*(unsigned int *)ads = ai->ab.actionpushdatas[l]->pushdata[pd];
												ads += 4;
											}
										}
										for (unsigned int l = 0;l < ai->ab.definefunction2count;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actiondefinefunction2s[l]->definefunction2offset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actiondefinefunction2s[l]->definefunction2offset));
											unsigned char *adst = ads;
											ads += (ai->ab.actiondefinefunction2s[l]->argumentcount * 8);
											for (unsigned int pd = 0;pd < ai->ab.actiondefinefunction2s[l]->argumentcount;pd++)
											{
												*(unsigned int *)adst = ai->ab.actiondefinefunction2s[l]->arguments[pd]->reg;
												adst += 4;
												*(unsigned char **)adst = ads;
												relocations.push_back((unsigned char **)(adst));
												adst += 4;
												strcpy((char *)ads,ai->ab.actiondefinefunction2s[l]->arguments[pd]->name);
												ads += STRLENGTH(ai->ab.actiondefinefunction2s[l]->arguments[pd]->name);
											}
										}
										for (unsigned int l = 0;l < ai->ab.definefunctioncount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actiondefinefunctions[l]->definefunctionoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actiondefinefunctions[l]->definefunctionoffset));
											unsigned char *adst = ads;
											ads += (ai->ab.actiondefinefunctions[l]->argumentcount * 4);
											for (unsigned int pd = 0;pd < ai->ab.actiondefinefunctions[l]->argumentcount;pd++)
											{
												*(unsigned char **)adst = ads;
												relocations.push_back((unsigned char **)(adst));
												adst += 4;
												strcpy((char *)ads,ai->ab.actiondefinefunctions[l]->arguments[pd]);
												ads += STRLENGTH(ai->ab.actiondefinefunctions[l]->arguments[pd]);
											}
										}
									}
									break;
								case FRAMELABEL:
									{
										FrameLabel *fl = (FrameLabel *)fi;
										FrameLabel *fli = (FrameLabel *)spri->frames[j]->frameitems[k];
										fi += sizeof(FrameLabel);
										fl->type = fli->type;
										fl->flags = fli->flags;
										fl->frame = fli->frame;
										fl->label = (char *)fip;
										relocations.push_back((unsigned char **)(&fl->label));
										strcpy((char *)fip,fli->label);
										fip += STRLENGTH(fli->label);
									}
									break;
								case PLACEOBJECT:
									{
										OutputPlaceObject *pl = (OutputPlaceObject *)fi;
										PlaceObject *pli = (PlaceObject *)spri->frames[j]->frameitems[k];
										fi += sizeof(OutputPlaceObject);
										pl->type = pli->type;
										pl->flags = pli->flags;
										pl->depth = pli->depth;
										pl->character = pli->character;
										pl->rotateandscale = pli->rotateandscale;
										pl->translate = pli->translate;
										pl->colortransform = pli->colortransform;
										pl->unknown = pli->unknown;
										pl->ratio = pli->ratio;
										if (pli->name)
										{
											pl->name = (char *)fip;
											relocations.push_back((unsigned char **)(&pl->name));
											strcpy((char *)fip,pli->name);
											fip += STRLENGTH(pli->name);
										}
										else
										{
											pl->name = 0;
										}
										pl->clipdepth = pli->clipdepth;
										if (pli->poa)
										{
											pl->poa = (OutputPlaceObjectActions *)fip;
											relocations.push_back((unsigned char **)(&pl->poa));
											fip += sizeof(OutputPlaceObjectActions);
											pl->poa->clipactioncount = pli->poa->clipactioncount;
											pl->poa->actions = (OutputPlaceObjectAction *)ads;
											relocations.push_back((unsigned char **)(&pl->poa->actions));
											ads += pli->poa->clipactioncount * sizeof(OutputPlaceObjectAction);
											for (unsigned int l = 0;l < pli->poa->clipactioncount;l++)
											{
												pl->poa->actions[l].flags = pli->poa->actions[l]->flags;
												pl->poa->actions[l].flags2 = pli->poa->actions[l]->flags2;
												relocations.push_back((unsigned char **)&pl->poa->actions[l].actiondata);
												pl->poa->actions[l].actiondata = (unsigned char *)ads2;
												unsigned char *abcs = ads2;
												ads2 += pli->poa->actions[l]->actiondata->ab.actionbytecount;
												ads2 += GETALIGN(pli->poa->actions[l]->actiondata->ab.actionbytecount);
												std::string s = pli->poa->actions[l]->actiondata->ab.actionbytes.str();
												const char *sc = s.c_str();
												memcpy(abcs,sc,pli->poa->actions[l]->actiondata->ab.actionbytecount);
												if (pli->poa->actions[l]->actiondata->ab.constantcount)
												{
													*(unsigned char **)(abcs + 8) = ads3;
													relocations.push_back((unsigned char **)(abcs + 8));
													for (unsigned int mx = 0;mx < pli->poa->actions[l]->actiondata->ab.constantcount;mx++)
													{
														*(unsigned int *)ads3 = pli->poa->actions[l]->actiondata->ab.constants[mx];
														ads3 += 4;
													}
												}
												for (unsigned int mx = 0;mx < pli->poa->actions[l]->actiondata->ab.stringcount;mx++)
												{
													*(unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actionstrings[mx]->stringoffset) = ads3;
													relocations.push_back((unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actionstrings[mx]->stringoffset));
													strcpy((char *)ads3,pli->poa->actions[l]->actiondata->ab.actionstrings[mx]->string);
													ads3 += STRLENGTH(pli->poa->actions[l]->actiondata->ab.actionstrings[mx]->string);
												}
												for (unsigned int mx = 0;mx < pli->poa->actions[l]->actiondata->ab.pushdatacount;mx++)
												{
													*(unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actionpushdatas[mx]->pushdataoffset) = ads3;
													relocations.push_back((unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actionpushdatas[mx]->pushdataoffset));
													for (unsigned int pd = 0;pd < pli->poa->actions[l]->actiondata->ab.actionpushdatas[mx]->pushdatacount;pd++)
													{
														*(unsigned int *)ads3 = pli->poa->actions[l]->actiondata->ab.actionpushdatas[mx]->pushdata[pd];
														ads3 += 4;
													}
												}
												for (unsigned int mx = 0;mx < pli->poa->actions[l]->actiondata->ab.definefunction2count;mx++)
												{
													*(unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->definefunction2offset) = ads3;
													relocations.push_back((unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->definefunction2offset));
													unsigned char *adst = ads3;
													ads3 += (pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->argumentcount * 8);
													for (unsigned int pd = 0;pd < pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->argumentcount;pd++)
													{
														*(unsigned int *)adst = pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->arguments[pd]->reg;
														adst += 4;
														*(unsigned char **)adst = ads3;
														relocations.push_back((unsigned char **)(adst));
														adst += 4;
														strcpy((char *)ads3,pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->arguments[pd]->name);
														ads3 += STRLENGTH(pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->arguments[pd]->name);
													}
												}
												for (unsigned int mx = 0;mx < pli->poa->actions[l]->actiondata->ab.definefunctioncount;mx++)
												{
													*(unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[mx]->definefunctionoffset) = ads3;
													relocations.push_back((unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[mx]->definefunctionoffset));
													unsigned char *adst = ads3;
													ads += (pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[mx]->argumentcount * 4);
													for (unsigned int pd = 0;pd < pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[mx]->argumentcount;pd++)
													{
														*(unsigned char **)adst = ads3;
														relocations.push_back((unsigned char **)(adst));
														adst += 4;
														strcpy((char *)ads3,pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[mx]->arguments[pd]);
														ads3 += STRLENGTH(pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[mx]->arguments[pd]);
													}
												}
											}
										}
										else
										{
											pl->poa = 0;
										}
									}
									break;
								case REMOVEOBJECT:
									{
										RemoveObject *r = (RemoveObject *)fi;
										fi += sizeof(RemoveObject);
										RemoveObject *ri = (RemoveObject *)spri->frames[j]->frameitems[k];
										r->type = ri->type;
										r->depth = ri->depth;
									}
									break;
								case BACKGROUNDCOLOR:
									{
										BackgroundColor *bgc = (BackgroundColor *)fi;
										fi += sizeof(BackgroundColor);
										BackgroundColor *bgci = (BackgroundColor *)spri->frames[j]->frameitems[k];
										bgc->type = bgci->type;
										bgc->color = bgci->color;
									}
									break;
								case INITACTION:
									{
										OutputInitAction *a = (OutputInitAction *)fi;
										InitAction *ai = (InitAction *)spri->frames[j]->frameitems[k];
										fi += sizeof(OutputInitAction);
										a->type = ai->type;
										a->sprite = ai->sprite;
										relocations.push_back((unsigned char **)(&a->actionbytes));
										std::string s = ai->ab.actionbytes.str();
										const char *sc = s.c_str();
										a->actionbytes = fip;
										unsigned char *abcs = fip;
										memcpy(fip,sc,ai->ab.actionbytecount);
										fip += ai->ab.actionbytecount;
										fip += GETALIGN(ai->ab.actionbytecount);
										if (ai->ab.constantcount)
										{
											*(unsigned char **)(abcs + 8) = ads;
											relocations.push_back((unsigned char **)(abcs + 8));
											for (unsigned int l = 0;l < ai->ab.constantcount;l++)
											{
												*(unsigned int *)ads = ai->ab.constants[l];
												ads += 4;
											}
										}
										for (unsigned int l = 0;l < ai->ab.stringcount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actionstrings[l]->stringoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actionstrings[l]->stringoffset));
											strcpy((char *)ads,ai->ab.actionstrings[l]->string);
											ads += STRLENGTH(ai->ab.actionstrings[l]->string);
										}
										for (unsigned int l = 0;l < ai->ab.pushdatacount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actionpushdatas[l]->pushdataoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actionpushdatas[l]->pushdataoffset));
											for (unsigned int pd = 0;pd < ai->ab.actionpushdatas[l]->pushdatacount;pd++)
											{
												*(unsigned int *)ads = ai->ab.actionpushdatas[l]->pushdata[pd];
												ads += 4;
											}
										}
										for (unsigned int l = 0;l < ai->ab.definefunction2count;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actiondefinefunction2s[l]->definefunction2offset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actiondefinefunction2s[l]->definefunction2offset));
											unsigned char *adst = ads;
											ads += (ai->ab.actiondefinefunction2s[l]->argumentcount * 8);
											for (unsigned int pd = 0;pd < ai->ab.actiondefinefunction2s[l]->argumentcount;pd++)
											{
												*(unsigned int *)adst = ai->ab.actiondefinefunction2s[l]->arguments[pd]->reg;
												adst += 4;
												*(unsigned char **)adst = ads;
												relocations.push_back((unsigned char **)(adst));
												adst += 4;
												strcpy((char *)ads,ai->ab.actiondefinefunction2s[l]->arguments[pd]->name);
												ads += STRLENGTH(ai->ab.actiondefinefunction2s[l]->arguments[pd]->name);
											}
										}
										for (unsigned int l = 0;l < ai->ab.definefunctioncount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actiondefinefunctions[l]->definefunctionoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actiondefinefunctions[l]->definefunctionoffset));
											unsigned char *adst = ads;
											ads += (ai->ab.actiondefinefunctions[l]->argumentcount * 4);
											for (unsigned int pd = 0;pd < ai->ab.actiondefinefunctions[l]->argumentcount;pd++)
											{
												*(unsigned char **)adst = ads;
												relocations.push_back((unsigned char **)(adst));
												adst += 4;
												strcpy((char *)ads,ai->ab.actiondefinefunctions[l]->arguments[pd]);
												ads += STRLENGTH(ai->ab.actiondefinefunctions[l]->arguments[pd]);
											}
										}
									}
									break;
								}
							}
						}
						else
						{
							fr->frameitemcount = 0;
							fr->frameitems = 0;
							fr++;
						}
					}
				}
				break;
			case IMAGE:
				{
					Image *im = (Image *)chd;
					Image *imi = (Image *)m->characters[i];
					chd += sizeof(Image);
					im->type = imi->type;
					im->texture = imi->texture;
					im->signature = imi->signature;
				}
				break;
			case MORPH:
				{
					Morph *mo = (Morph *)chd;
					Morph *moi = (Morph *)m->characters[i];
					chd += sizeof(Morph);
					mo->type = moi->type;
					mo->signature = moi->signature;
					mo->startshape = moi->startshape;
					mo->endshape = moi->endshape;
				}
				break;
			case MOVIE:
				{
					OutputMovie *mov = (OutputMovie *)chd;
					Movie *movi = (Movie *)m->characters[i];
					chd += sizeof(OutputMovie);
					mov->type = movi->type;
					mov->signature = movi->signature;
					mov->framecount = movi->framecount;
					if (mov->framecount)
					{
						mov->frames = fr;
						relocations.push_back((unsigned char **)(&mov->frames));
					}
					else
					{
						mov->frames = 0;
					}
					mov->pointer = movi->pointer;
					mov->charactercount = movi->charactercount;
					mov->characters = (Character **)ch;
					relocations.push_back((unsigned char **)(&mov->characters));
					mov->screensizex = movi->screensizex;
					mov->screensizey = movi->screensizey;
					mov->importcount = movi->importcount;
					mov->unknown = movi->unknown;
					mov->imports = imp;
					relocations.push_back((unsigned char **)(&mov->imports));
					mov->exportcount = movi->exportcount;
					mov->exports = ex;
					relocations.push_back((unsigned char **)(&mov->exports));
					mov->count = movi->count;
					for (unsigned int j = 0;j < movi->framecount;j++)
					{
						if (movi->frames[j])
						{
							fr->frameitemcount = movi->frames[j]->frameitemcount;
							fr->frameitems = (FrameItem **)fil;
							relocations.push_back((unsigned char **)(&fr->frameitems));
							fr++;
							for (unsigned int k = 0;k < movi->frames[j]->frameitemcount;k++)
							{
								*fil = fi;
								relocations.push_back((unsigned char **)(fil));
								fil++;
								switch (movi->frames[j]->frameitems[k]->type)
								{
								case ACTION:
									{
										OutputAction *a = (OutputAction *)fi;
										Action *ai = (Action *)movi->frames[j]->frameitems[k];
										fi += sizeof(OutputAction);
										a->type = ai->type;
										relocations.push_back((unsigned char **)(&a->actionbytes));
										std::string s = ai->ab.actionbytes.str();
										const char *sc = s.c_str();
										a->actionbytes = fip;
										unsigned char *abcs = fip;
										memcpy(fip,sc,ai->ab.actionbytecount);
										fip += ai->ab.actionbytecount;
										fip += GETALIGN(ai->ab.actionbytecount);
										if (ai->ab.constantcount)
										{
											*(unsigned char **)(abcs + 8) = ads;
											relocations.push_back((unsigned char **)(abcs + 8));
											for (unsigned int l = 0;l < ai->ab.constantcount;l++)
											{
												*(unsigned int *)ads = ai->ab.constants[l];
												ads += 4;
											}
										}
										for (unsigned int l = 0;l < ai->ab.stringcount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actionstrings[l]->stringoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actionstrings[l]->stringoffset));
											strcpy((char *)ads,ai->ab.actionstrings[l]->string);
											ads += STRLENGTH(ai->ab.actionstrings[l]->string);
										}
										for (unsigned int l = 0;l < ai->ab.pushdatacount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actionpushdatas[l]->pushdataoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actionpushdatas[l]->pushdataoffset));
											for (unsigned int pd = 0;pd < ai->ab.actionpushdatas[l]->pushdatacount;pd++)
											{
												*(unsigned int *)ads = ai->ab.actionpushdatas[l]->pushdata[pd];
												ads += 4;
											}
										}
										for (unsigned int l = 0;l < ai->ab.definefunction2count;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actiondefinefunction2s[l]->definefunction2offset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actiondefinefunction2s[l]->definefunction2offset));
											unsigned char *adst = ads;
											ads += (ai->ab.actiondefinefunction2s[l]->argumentcount * 8);
											for (unsigned int pd = 0;pd < ai->ab.actiondefinefunction2s[l]->argumentcount;pd++)
											{
												*(unsigned int *)adst = ai->ab.actiondefinefunction2s[l]->arguments[pd]->reg;
												adst += 4;
												*(unsigned char **)adst = ads;
												relocations.push_back((unsigned char **)(adst));
												adst += 4;
												strcpy((char *)ads,ai->ab.actiondefinefunction2s[l]->arguments[pd]->name);
												ads += STRLENGTH(ai->ab.actiondefinefunction2s[l]->arguments[pd]->name);
											}
										}
										for (unsigned int l = 0;l < ai->ab.definefunctioncount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actiondefinefunctions[l]->definefunctionoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actiondefinefunctions[l]->definefunctionoffset));
											unsigned char *adst = ads;
											ads += (ai->ab.actiondefinefunctions[l]->argumentcount * 4);
											for (unsigned int pd = 0;pd < ai->ab.actiondefinefunctions[l]->argumentcount;pd++)
											{
												*(unsigned char **)adst = ads;
												relocations.push_back((unsigned char **)(adst));
												adst += 4;
												strcpy((char *)ads,ai->ab.actiondefinefunctions[l]->arguments[pd]);
												ads += STRLENGTH(ai->ab.actiondefinefunctions[l]->arguments[pd]);
											}
										}
									}
									break;
								case FRAMELABEL:
									{
										FrameLabel *fl = (FrameLabel *)fi;
										FrameLabel *fli = (FrameLabel *)movi->frames[j]->frameitems[k];
										fi += sizeof(FrameLabel);
										fl->type = fli->type;
										fl->flags = fli->flags;
										fl->frame = fli->frame;
										fl->label = (char *)fip;
										relocations.push_back((unsigned char **)(&fl->label));
										strcpy((char *)fip,fli->label);
										fip += STRLENGTH(fli->label);
									}
									break;
								case PLACEOBJECT:
									{
										OutputPlaceObject *pl = (OutputPlaceObject *)fi;
										PlaceObject *pli = (PlaceObject *)movi->frames[j]->frameitems[k];
										fi += sizeof(OutputPlaceObject);
										pl->type = pli->type;
										pl->flags = pli->flags;
										pl->depth = pli->depth;
										pl->character = pli->character;
										pl->rotateandscale = pli->rotateandscale;
										pl->translate = pli->translate;
										pl->colortransform = pli->colortransform;
										pl->unknown = pli->unknown;
										pl->ratio = pli->ratio;
										if (pli->name)
										{
											pl->name = (char *)fip;
											relocations.push_back((unsigned char **)(&pl->name));
											strcpy((char *)fip,pli->name);
											fip += STRLENGTH(pli->name);
										}
										else
										{
											pl->name = 0;
										}
										pl->clipdepth = pli->clipdepth;
										if (pli->poa)
										{
											pl->poa = (OutputPlaceObjectActions *)fip;
											relocations.push_back((unsigned char **)(&pl->poa));
											fip += sizeof(OutputPlaceObjectActions);
											pl->poa->clipactioncount = pli->poa->clipactioncount;
											pl->poa->actions = (OutputPlaceObjectAction *)ads;
											relocations.push_back((unsigned char **)(&pl->poa->actions));
											ads += pli->poa->clipactioncount * sizeof(OutputPlaceObjectAction);
											for (unsigned int l = 0;l < pli->poa->clipactioncount;l++)
											{
												pl->poa->actions[l].flags = pli->poa->actions[l]->flags;
												pl->poa->actions[l].flags2 = pli->poa->actions[l]->flags2;
												relocations.push_back((unsigned char **)&pl->poa->actions[l].actiondata);
												pl->poa->actions[l].actiondata = (unsigned char *)ads2;
												unsigned char *abcs = ads2;
												ads2 += pli->poa->actions[l]->actiondata->ab.actionbytecount;
												ads2 += GETALIGN(pli->poa->actions[l]->actiondata->ab.actionbytecount);
												std::string s = pli->poa->actions[l]->actiondata->ab.actionbytes.str();
												const char *sc = s.c_str();
												memcpy(abcs,sc,pli->poa->actions[l]->actiondata->ab.actionbytecount);
												if (pli->poa->actions[l]->actiondata->ab.constantcount)
												{
													*(unsigned char **)(abcs + 8) = ads3;
													relocations.push_back((unsigned char **)(abcs + 8));
													for (unsigned int mx = 0;mx < pli->poa->actions[l]->actiondata->ab.constantcount;mx++)
													{
														*(unsigned int *)ads3 = pli->poa->actions[l]->actiondata->ab.constants[mx];
														ads3 += 4;
													}
												}
												for (unsigned int mx = 0;mx < pli->poa->actions[l]->actiondata->ab.stringcount;mx++)
												{
													unsigned int so = pli->poa->actions[l]->actiondata->ab.actionstrings[mx]->stringoffset;
													*(unsigned char **)(abcs+so) = ads3;
													relocations.push_back((unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actionstrings[mx]->stringoffset));
													strcpy((char *)ads3,pli->poa->actions[l]->actiondata->ab.actionstrings[mx]->string);
													ads3 += STRLENGTH(pli->poa->actions[l]->actiondata->ab.actionstrings[mx]->string);
												}
												for (unsigned int mx = 0;mx < pli->poa->actions[l]->actiondata->ab.pushdatacount;mx++)
												{
													*(unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actionpushdatas[mx]->pushdataoffset) = ads3;
													relocations.push_back((unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actionpushdatas[mx]->pushdataoffset));
													for (unsigned int pd = 0;pd < pli->poa->actions[l]->actiondata->ab.actionpushdatas[mx]->pushdatacount;pd++)
													{
														*(unsigned int *)ads3 = pli->poa->actions[l]->actiondata->ab.actionpushdatas[mx]->pushdata[pd];
														ads3 += 4;
													}
												}
												for (unsigned int mx = 0;mx < pli->poa->actions[l]->actiondata->ab.definefunction2count;mx++)
												{
													*(unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->definefunction2offset) = ads3;
													relocations.push_back((unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->definefunction2offset));
													unsigned char *adst = ads3;
													ads3 += (pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->argumentcount * 8);
													for (unsigned int pd = 0;pd < pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->argumentcount;pd++)
													{
														*(unsigned int *)adst = pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->arguments[pd]->reg;
														adst += 4;
														*(unsigned char **)adst = ads3;
														relocations.push_back((unsigned char **)(adst));
														adst += 4;
														strcpy((char *)ads3,pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->arguments[pd]->name);
														ads3 += STRLENGTH(pli->poa->actions[l]->actiondata->ab.actiondefinefunction2s[mx]->arguments[pd]->name);
													}
												}
												for (unsigned int mx = 0;mx < pli->poa->actions[l]->actiondata->ab.definefunctioncount;mx++)
												{
													*(unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[l]->definefunctionoffset) = ads3;
													relocations.push_back((unsigned char **)(abcs+pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[mx]->definefunctionoffset));
													unsigned char *adst = ads3;
													ads += (pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[mx]->argumentcount * 4);
													for (unsigned int pd = 0;pd < pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[mx]->argumentcount;pd++)
													{
														*(unsigned char **)adst = ads3;
														relocations.push_back((unsigned char **)(adst));
														adst += 4;
														strcpy((char *)ads3,pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[mx]->arguments[pd]);
														ads3 += STRLENGTH(pli->poa->actions[l]->actiondata->ab.actiondefinefunctions[mx]->arguments[pd]);
													}
												}
											}
										}
										else
										{
											pl->poa = 0;
										}
									}
									break;
								case REMOVEOBJECT:
									{
										RemoveObject *r = (RemoveObject *)fi;
										fi += sizeof(RemoveObject);
										RemoveObject *ri = (RemoveObject *)movi->frames[j]->frameitems[k];
										r->type = ri->type;
										r->depth = ri->depth;
									}
									break;
								case BACKGROUNDCOLOR:
									{
										BackgroundColor *bgc = (BackgroundColor *)fi;
										fi += sizeof(BackgroundColor);
										BackgroundColor *bgci = (BackgroundColor *)movi->frames[j]->frameitems[k];
										bgc->type = bgci->type;
										bgc->color = bgci->color;
									}
									break;
								case INITACTION:
									{
										OutputInitAction *a = (OutputInitAction *)fi;
										InitAction *ai = (InitAction *)movi->frames[j]->frameitems[k];
										fi += sizeof(OutputInitAction);
										a->type = ai->type;
										a->sprite = ai->sprite;
										relocations.push_back((unsigned char **)(&a->actionbytes));
										std::string s = ai->ab.actionbytes.str();
										const char *sc = s.c_str();
										a->actionbytes = fip;
										unsigned char *abcs = fip;
										memcpy(fip,sc,ai->ab.actionbytecount);
										fip += ai->ab.actionbytecount;
										fip += GETALIGN(ai->ab.actionbytecount);
										if (ai->ab.constantcount)
										{
											*(unsigned char **)(abcs + 8) = ads;
											relocations.push_back((unsigned char **)(abcs + 8));
											for (unsigned int l = 0;l < ai->ab.constantcount;l++)
											{
												*(unsigned int *)ads = ai->ab.constants[l];
												ads += 4;
											}
										}
										for (unsigned int l = 0;l < ai->ab.stringcount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actionstrings[l]->stringoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actionstrings[l]->stringoffset));
											strcpy((char *)ads,ai->ab.actionstrings[l]->string);
											ads += STRLENGTH(ai->ab.actionstrings[l]->string);
										}
										for (unsigned int l = 0;l < ai->ab.pushdatacount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actionpushdatas[l]->pushdataoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actionpushdatas[l]->pushdataoffset));
											for (unsigned int pd = 0;pd < ai->ab.actionpushdatas[l]->pushdatacount;pd++)
											{
												*(unsigned int *)ads = ai->ab.actionpushdatas[l]->pushdata[pd];
												ads += 4;
											}
										}
										for (unsigned int l = 0;l < ai->ab.definefunction2count;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actiondefinefunction2s[l]->definefunction2offset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actiondefinefunction2s[l]->definefunction2offset));
											unsigned char *adst = ads;
											ads += (ai->ab.actiondefinefunction2s[l]->argumentcount * 8);
											for (unsigned int pd = 0;pd < ai->ab.actiondefinefunction2s[l]->argumentcount;pd++)
											{
												*(unsigned int *)adst = ai->ab.actiondefinefunction2s[l]->arguments[pd]->reg;
												adst += 4;
												*(unsigned char **)adst = ads;
												relocations.push_back((unsigned char **)(adst));
												adst += 4;
												strcpy((char *)ads,ai->ab.actiondefinefunction2s[l]->arguments[pd]->name);
												ads += STRLENGTH(ai->ab.actiondefinefunction2s[l]->arguments[pd]->name);
											}
										}
										for (unsigned int l = 0;l < ai->ab.definefunctioncount;l++)
										{
											*(unsigned char **)(abcs+ai->ab.actiondefinefunctions[l]->definefunctionoffset) = ads;
											relocations.push_back((unsigned char **)(abcs+ai->ab.actiondefinefunctions[l]->definefunctionoffset));
											unsigned char *adst = ads;
											ads += (ai->ab.actiondefinefunctions[l]->argumentcount * 4);
											for (unsigned int pd = 0;pd < ai->ab.actiondefinefunctions[l]->argumentcount;pd++)
											{
												*(unsigned char **)adst = ads;
												relocations.push_back((unsigned char **)(adst));
												adst += 4;
												strcpy((char *)ads,ai->ab.actiondefinefunctions[l]->arguments[pd]);
												ads += STRLENGTH(ai->ab.actiondefinefunctions[l]->arguments[pd]);
											}
										}
									}
									break;
								}
							}
						}
						else
						{
							fr->frameitemcount = 0;
							fr->frameitems = 0;
							fr++;
						}
					}
				}
				break;
			case TEXT:
				{
					OutputText *ot = (OutputText *)chd;
					Text *t = (Text *)m->characters[i];
					chd += sizeof(OutputText);
					ot->type = t->type;
					ot->signature = t->signature;
					ot->bounds = t->bounds;
					ot->rotateandscale = t->rotateandscale;
					ot->translate = t->translate;
					ot->recordcount = t->recordcount;
					if (ot->recordcount)
					{
						OutputTextRecord *tr = (OutputTextRecord *)fi;
						ot->records = tr;
						relocations.push_back((unsigned char **)(&ot->records));
						for (unsigned int rc = 0;rc < ot->recordcount;rc++)
						{
							tr->color = t->records[rc]->color;
							tr->font = t->records[rc]->font;
							tr->offset = t->records[rc]->offset;
							tr->textheight = t->records[rc]->textheight;
							tr->unknown = t->records[rc]->unknown;
							tr->glyphcount = t->records[rc]->glyphcount;
							Glyph *gl = (Glyph *)fip;
							tr->glyphs = gl;
							relocations.push_back((unsigned char**)(&tr->glyphs));
							for (unsigned int ac = 0;ac < t->records[rc]->glyphcount;ac++)
							{
								gl->advance = t->records[rc]->glyphs[ac]->advance;
								gl->index = t->records[rc]->glyphs[ac]->index;
								gl++;
							}
							fip = (unsigned char *)gl;
							tr++;
						}
						fi = (unsigned char *)tr;
					}
				}
				break;
			}
			ch++;
		}
		else
		{
			*ch = 0;
			ch++;
		}
	}
	for (unsigned int i = 0;i < relocations.size();i++)
	{
		unsigned char **c = relocations[i];
		(*c) = (unsigned char *)((*c) - aptdata);
	}
	FILE *f = fopen(filename,"wb");
	if (!f)
	{
		return;
	}
	fwrite(aptdata,aptdatasize,1,f);
	fclose(f);
}

void CC3TOOLS_DLL_EXPORT GenerateAptConstFile(AptConstData *c,const char *filename)
{
	unsigned int aptconstsize = 0x20 + (c->itemcount * 8);
	std::vector <unsigned char **> relocations;
	for (unsigned int i = 0;i < c->itemcount;i++)
	{
		if (c->items[i]->type == TYPE_STRING)
		{
			if (c->items[i]->strvalue)
			{
				aptconstsize += STRLENGTH(c->items[i]->strvalue);
			}
			else
			{
				aptconstsize += 1;
			}
		}
	}
	unsigned char *aptconstdata = new unsigned char[aptconstsize];
	memset(aptconstdata,0,aptconstsize);
	unsigned char *aptconst = aptconstdata;
	memcpy(aptconst,"Apt constant file\x1A\0\0",20);
	aptconst += 20;
	*(unsigned int *)aptconst = c->aptdataoffset;
	aptconst += 4;
	*(unsigned int *)aptconst = c->itemcount;
	aptconst += 4;
	unsigned int n = 0x20;
	*(unsigned int *)aptconst = n;
	aptconst += 4;
	AptConstItem *aci = (AptConstItem *)aptconst;
	aptconst += c->itemcount * 8;
	for (unsigned int i = 0;i < c->itemcount;i++)
	{
		aci->type = c->items[i]->type;
		if (c->items[i]->type == TYPE_STRING)
		{
			aci->strvalue = (char *)aptconst;
			relocations.push_back((unsigned char **)(&aci->strvalue));
			if (c->items[i]->strvalue)
			{
				strcpy((char *)aptconst,c->items[i]->strvalue);
				aptconst += STRLENGTH(c->items[i]->strvalue);
			}
			else
			{
				aptconst++;
			}
		}
		else
		{
			aci->numvalue = c->items[i]->numvalue;
		}
		aci++;
	}
	for (unsigned int i = 0;i < relocations.size();i++)
	{
		unsigned char **rc = relocations[i];
		(*rc) = (unsigned char *)((*rc) - aptconstdata);
	}
	FILE *f = fopen(filename,"wb");
	if (!f)
	{
		return;
	}
	fwrite(aptconstdata,aptconstsize,1,f);
	fclose(f);
}

void CC3TOOLS_DLL_EXPORT AddAction(action_type action,ActionBytes *ab)
{
	unsigned char c = (unsigned char)action;
	ab->actionbytes << c;
	ab->actionbytecount++;
}

void CC3TOOLS_DLL_EXPORT AddIntAction(action_type action,ActionBytes *ab,int actionvalue)
{
	unsigned char c = (unsigned char)action;
	ab->actionbytes << c;
	ab->actionbytecount++;
	unsigned int x = GETALIGN(ab->actionbytecount);
	c = 0;
	for (unsigned int i = 0;i < x;i++)
	{
		ab->actionbytes << c;
		ab->actionbytecount++;
	}
	c = LOBYTE(LOWORD(actionvalue));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(actionvalue));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(actionvalue));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(actionvalue));
	ab->actionbytes << c;
	ab->actionbytecount++;
}

void CC3TOOLS_DLL_EXPORT AddStringAction(action_type action,ActionBytes *ab,const char *actionstring)
{
	unsigned char c = (unsigned char)action;
	ab->actionbytes << c;
	ab->actionbytecount++;
	unsigned int x = GETALIGN(ab->actionbytecount);
	c = 0;
	for (unsigned int i = 0;i < x;i++)
	{
		ab->actionbytes << c;
		ab->actionbytecount++;
	}
	ActionString *s = new ActionString;
	s->stringoffset = ab->actionbytecount;
	s->string = _strdup(actionstring);
	ab->actionstrings.push_back(s);
	ab->stringcount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
}

void CC3TOOLS_DLL_EXPORT AddByteAction(action_type action,ActionBytes *ab,unsigned char number)
{
	unsigned char c = (unsigned char)action;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << number;
	ab->actionbytecount++;
}

void CC3TOOLS_DLL_EXPORT AddShortAction(action_type action,ActionBytes *ab,unsigned short number)
{
	unsigned char c = (unsigned char)action;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << LOBYTE(number);
	ab->actionbytecount++;
	ab->actionbytes << HIBYTE(number);
	ab->actionbytecount++;
}

void CC3TOOLS_DLL_EXPORT AddFloatAction(action_type action,ActionBytes *ab,float number)
{
	unsigned char c = (unsigned char)action;
	unsigned int n = *(unsigned int*)&number;
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(LOWORD(n));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(n));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(n));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(n));
	ab->actionbytes << c;
	ab->actionbytecount++;
}

void CC3TOOLS_DLL_EXPORT AddLongAction(action_type action,ActionBytes *ab,unsigned long number)
{
	unsigned char c = (unsigned char)action;
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(LOWORD(number));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(number));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(number));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(number));
	ab->actionbytes << c;
	ab->actionbytecount++;
}

void CC3TOOLS_DLL_EXPORT AddURLAction(action_type action,ActionBytes *ab,const char *str1,const char *str2)
{
	unsigned char c = (unsigned char)action;
	ab->actionbytes << c;
	ab->actionbytecount++;
	unsigned int x = GETALIGN(ab->actionbytecount);
	c = 0;
	for (unsigned int i = 0;i < x;i++)
	{
		ab->actionbytes << c;
		ab->actionbytecount++;
	}
	ActionString *s = new ActionString;
	s->stringoffset = ab->actionbytecount;
	s->string = _strdup(str1);
	ab->actionstrings.push_back(s);
	ab->stringcount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ActionString *s2 = new ActionString;
	s2->stringoffset = ab->actionbytecount;
	s2->string = _strdup(str2);
	ab->actionstrings.push_back(s2);
	ab->stringcount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
}

void CC3TOOLS_DLL_EXPORT AddConstantPoolAction(action_type action,ActionBytes *ab,unsigned int constantcount)
{
	unsigned char c = (unsigned char)action;
	ab->actionbytes << c;
	ab->actionbytecount++;
	unsigned int x = GETALIGN(ab->actionbytecount);
	c = 0;
	for (unsigned int i = 0;i < x;i++)
	{
		ab->actionbytes << c;
		ab->actionbytecount++;
	}
	c = LOBYTE(LOWORD(constantcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(constantcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(constantcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(constantcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = 0;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
}

void CC3TOOLS_DLL_EXPORT AddDefineFunction2Action(action_type action,ActionBytes *ab,ActionDefineFunction2 *pd,unsigned int flags,unsigned int size,const char *name)
{
	unsigned char c = (unsigned char)action;
	ab->actionbytes << c;
	ab->actionbytecount++;
	unsigned int x = GETALIGN(ab->actionbytecount);
	c = 0;
	for (unsigned int i = 0;i < x;i++)
	{
		ab->actionbytes << c;
		ab->actionbytecount++;
	}
	ActionString *s = new ActionString;
	s->stringoffset = ab->actionbytecount;
	s->string = _strdup(name);
	ab->actionstrings.push_back(s);
	ab->stringcount++;
	c = 0;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(LOWORD(pd->argumentcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(pd->argumentcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(pd->argumentcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(pd->argumentcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(LOWORD(flags));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(flags));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(flags));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(flags));
	ab->actionbytes << c;
	ab->actionbytecount++;
	pd->definefunction2offset = ab->actionbytecount;
	c = 0;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(LOWORD(size));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(size));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(size));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(size));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(LOWORD(0x98765432));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(0x98765432));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(0x98765432));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(0x98765432));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(LOWORD(0x12345678));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(0x12345678));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(0x12345678));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(0x12345678));
	ab->actionbytes << c;
	ab->actionbytecount++;
}

void CC3TOOLS_DLL_EXPORT AddDefineFunctionAction(action_type action,ActionBytes *ab,ActionDefineFunction *pd,unsigned int size,const char *name)
{
	unsigned char c = (unsigned char)action;
	ab->actionbytes << c;
	ab->actionbytecount++;
	unsigned int x = GETALIGN(ab->actionbytecount);
	c = 0;
	for (unsigned int i = 0;i < x;i++)
	{
		ab->actionbytes << c;
		ab->actionbytecount++;
	}
	ActionString *s = new ActionString;
	s->stringoffset = ab->actionbytecount;
	s->string = _strdup(name);
	ab->actionstrings.push_back(s);
	ab->stringcount++;
	c = 0;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(LOWORD(pd->argumentcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(pd->argumentcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(pd->argumentcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(pd->argumentcount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	pd->definefunctionoffset = ab->actionbytecount;
	c = 0;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(LOWORD(size));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(size));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(size));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(size));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(LOWORD(0x98765432));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(0x98765432));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(0x98765432));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(0x98765432));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(LOWORD(0x12345678));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(0x12345678));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(0x12345678));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(0x12345678));
	ab->actionbytes << c;
	ab->actionbytecount++;
}

void CC3TOOLS_DLL_EXPORT AddPushDataAction(action_type action,ActionBytes *ab,ActionPushData *pd)
{
	unsigned char c = (unsigned char)action;
	ab->actionbytes << c;
	ab->actionbytecount++;
	unsigned int x = GETALIGN(ab->actionbytecount);
	c = 0;
	for (unsigned int i = 0;i < x;i++)
	{
		ab->actionbytes << c;
		ab->actionbytecount++;
	}
	c = LOBYTE(LOWORD(pd->pushdatacount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(LOWORD(pd->pushdatacount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = LOBYTE(HIWORD(pd->pushdatacount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	c = HIBYTE(HIWORD(pd->pushdatacount));
	ab->actionbytes << c;
	ab->actionbytecount++;
	pd->pushdataoffset = ab->actionbytecount;
	c = 0;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
	ab->actionbytes << c;
	ab->actionbytecount++;
}
