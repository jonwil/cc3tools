/*
	Command & Conquer 3 Tools
	eaf to swf conversion program
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
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <zlib.h>
#include <sstream>
#include <vector>
#include "resource.h"
enum tag_type
{
    END                   =  0,
    SHOWFRAME             =  1,
    DEFINESHAPE           =  2,
    FREECHARACTER  =  3,
    PLACEOBJECT           =  4,
    REMOVEOBJECT          =  5,
    DEFINEBITS            =  6,
    DEFINEBUTTON          =  7,
    JPEGTABLES            =  8,
    SETBACKGROUNDCOLOR    =  9,
    DEFINEFONT            = 10,
    DEFINETEXT            = 11,
    DOACTION              = 12,
    DEFINEFONTINFO        = 13,
    DEFINESOUND           = 14,
    STARTSOUND            = 15,
    DEFINEBUTTONSOUND     = 17,
    SOUNDSTREAMHEAD       = 18,
    SOUNDSTREAMBLOCK      = 19,
    DEFINELOSSLESS        = 20,
    DEFINEBITSJPEG2       = 21,
    DEFINESHAPE2          = 22,
    DEFINEBUTTONCXFORM    = 23,
    PROTECT               = 24,
    PATHSAREPOSTSCRIPT  = 25,
    PLACEOBJECT2          = 26,
    REMOVEOBJECT2         = 28,
    SYNCFRAME  = 29,
    FREEALL  = 31,
    DEFINESHAPE3          = 32,
    DEFINETEXT2           = 33,
    DEFINEBUTTON2         = 34,
    DEFINEBITSJPEG3       = 35,
    DEFINELOSSLESS2       = 36,
    DEFINEEDITTEXT  = 37,
    DEFINEVIDEO  = 38,
    DEFINESPRITE          = 39,
    NAMECHARACTER  = 40,
    SERIALNUMBER          = 41,
    DEFINETEXTFORMAT  = 42,
    FRAMELABEL            = 43,
    SOUNDSTREAMHEAD2      = 45,
    DEFINEMORPHSHAPE      = 46,
    FRAMETAG  = 47,
    DEFINEFONT2           = 48,
    GENCOMMAND  = 49,
    DEFINECOMMANDOBJ  = 50,
    CHARACTERSET  = 51,
    FONTREF  = 52,
    EXPORTASSETS          = 56,
    IMPORTASSETS          = 57,

    ENABLEDEBUGGER  = 58,

    INITACTION  = 59,

    DEFINEVIDEOSTREAM  = 60,
    VIDEOFRAME  = 61,

    DEFINEFONTINFO2       = 62,

    ENABLEDEBUGGER2  = 64,

    FILEATTRIBUTES        = 69,

    PLACEOBJECT3          = 70,

    IMPORTASSETS2         = 71,

    DEFINEALIGNZONES      = 73,

    CSMTEXTSETTINGS       = 74,

    DEFINEFONT3           = 75,

    METADATA              = 77,

    DEFINESCALINGGRID     = 78,

    DEFINESHAPE4          = 83,

    DEFINEMORPHSHAPE2     = 84,

    REFLEX                = 777,

    DEFINEBITSPTR         = 1023
};

enum action_type
{
    ACTION_END                     = 0x00,
    ACTION_NEXTFRAME               = 0x04,
    ACTION_PREVFRAME               = 0x05,
    ACTION_PLAY                    = 0x06,
    ACTION_STOP                    = 0x07,
    ACTION_TOGGLEQUALITY           = 0x08,
    ACTION_STOPSOUNDS              = 0x09,
    ACTION_GOTOFRAME               = 0x81,
    ACTION_GETURL                  = 0x83,
    ACTION_WAITFORFRAME            = 0x8A,
    ACTION_SETTARGET               = 0x8B,
    ACTION_GOTOLABEL               = 0x8C,
    ACTION_ADD                     = 0x0A,
    ACTION_SUBTRACT                = 0x0B,
    ACTION_MULTIPLY                = 0x0C,
    ACTION_DIVIDE                  = 0x0D,

    ACTION_EQUAL                   = 0x0E,

    ACTION_LESSTHAN                = 0x0F,
    ACTION_LOGICALAND              = 0x10,
    ACTION_LOGICALOR               = 0x11,
    ACTION_LOGICALNOT              = 0x12,
    ACTION_STRINGEQ                = 0x13,
    ACTION_STRINGLENGTH            = 0x14,
    ACTION_SUBSTRING               = 0x15,
    ACTION_POP                     = 0x17,
    ACTION_INT                     = 0x18,
    ACTION_GETVARIABLE             = 0x1C,
    ACTION_SETVARIABLE             = 0x1D,
    ACTION_SETTARGETEXPRESSION     = 0x20,
    ACTION_STRINGCONCAT            = 0x21,
    ACTION_GETPROPERTY             = 0x22,
    ACTION_SETPROPERTY             = 0x23,
    ACTION_DUPLICATECLIP           = 0x24,
    ACTION_REMOVECLIP              = 0x25,
    ACTION_TRACE                   = 0x26,
    ACTION_STARTDRAGMOVIE          = 0x27,
    ACTION_STOPDRAGMOVIE           = 0x28,
    ACTION_STRINGCOMPARE           = 0x29,
    ACTION_THROW                   = 0x2A,

    ACTION_CASTOP                  = 0x2B,

    ACTION_IMPLEMENTSOP            = 0x2C,

    ACTION_RANDOM                  = 0x30,
    ACTION_MBLENGTH                = 0x31,
    ACTION_ORD                     = 0x32,
    ACTION_CHR                     = 0x33,
    ACTION_GETTIMER                = 0x34,
    ACTION_MBSUBSTRING             = 0x35,
    ACTION_MBORD                   = 0x36,
    ACTION_MBCHR                   = 0x37,
    ACTION_WAITFORFRAMEEXPRESSION  = 0x8D,
    ACTION_PUSHDATA                = 0x96,
    ACTION_BRANCHALWAYS            = 0x99,
    ACTION_GETURL2                 = 0x9A,
    ACTION_BRANCHIFTRUE            = 0x9D,
    ACTION_CALLFRAME               = 0x9E,
    ACTION_GOTOEXPRESSION          = 0x9F,

    ACTION_DELETE                  = 0x3A,

    ACTION_DELETE2                 = 0x3B,

    ACTION_DEFINELOCAL             = 0x3C,
    ACTION_CALLFUNCTION            = 0x3D,
    ACTION_RETURN                  = 0x3E,
    ACTION_MODULO                  = 0x3F,

    ACTION_NEW                     = 0x40,

    ACTION_VAR                     = 0x41,
    ACTION_INITARRAY               = 0x42,


    ACTION_INITOBJECT              = 0x43,

    ACTION_TYPEOF                  = 0x44,

    ACTION_TARGETPATH              = 0x45,

    ACTION_ENUMERATE               = 0x46,

    ACTION_NEWADD                  = 0x47,
    ACTION_NEWLESSTHAN             = 0x48,

    ACTION_NEWEQUALS               = 0x49,

    ACTION_TONUMBER                = 0x4A,

    ACTION_TOSTRING                = 0x4B,

    ACTION_DUP                     = 0x4C,

    ACTION_SWAP                    = 0x4D,

    ACTION_GETMEMBER               = 0x4E,

    ACTION_SETMEMBER               = 0x4F,

    ACTION_INCREMENT               = 0x50,

    ACTION_DECREMENT               = 0x51,

    ACTION_CALLMETHOD              = 0x52,

    ACTION_NEWMETHOD               = 0x53, 

    ACTION_INSTANCEOF              = 0x54,

    ACTION_ENUM2                   = 0x55,

    ACTION_BITWISEAND              = 0x60,
    ACTION_BITWISEOR               = 0x61,
    ACTION_BITWISEXOR              = 0x62,
    ACTION_SHIFTLEFT               = 0x63,
    ACTION_SHIFTRIGHT              = 0x64,
    ACTION_SHIFTRIGHT2             = 0x65,
    ACTION_STRICTEQ                = 0x66,

    ACTION_GREATER                 = 0x67,

    ACTION_STRINGGREATER           = 0x68,

    ACTION_EXTENDS                 = 0x69,

    ACTION_CONSTANTPOOL            = 0x88,


    ACTION_DEFINEFUNCTION2         = 0x8E,

    ACTION_TRY                     = 0x8F,
    ACTION_WITH                    = 0x94,
    ACTION_DEFINEFUNCTION          = 0x9B,

    ACTION_SETREGISTER             = 0x87,
	EA_ACTION56 = 0x56,
	EA_ACTION58 = 0x58,
	EA_PUSHZERO = 0x59,
	EA_PUSHONE = 0x5A,
	EA_CALLFUNCTIONPOP = 0x5B,
	EA_CALLFUNCTION = 0x5C,
	EA_CALLMETHODPOP = 0x5D,
	EA_CALLMETHOD = 0x5E,
	EA_PUSHTHIS = 0x70,
	EA_PUSHGLOBAL = 0x71,
	EA_ZEROVARIABLE = 0x72,
	EA_PUSHTRUE = 0x73,
	EA_PUSHFALSE = 0x74,
	EA_PUSHNULL = 0x75,
	EA_PUSHUNDEFINED = 0x76,
	EA_ACTION77 = 0x77,
	EA_PUSHSTRING = 0xA1,
	EA_PUSHCONSTANT = 0xA2,
	EA_PUSHWORDCONSTANT = 0xA3,
	EA_GETSTRINGVAR = 0xA4,
	EA_GETSTRINGMEMBER = 0xA5,
	EA_SETSTRINGVAR = 0xA6,
	EA_SETSTRINGMEMBER = 0xA7,
	EA_PUSHVALUEOFVAR = 0xAE,
	EA_GETNAMEDMEMBER = 0xAF,
	EA_CALLNAMEDFUNCTIONPOP = 0xB0,
	EA_CALLNAMEDFUNCTION = 0xB1,
	EA_CALLNAMEDMETHODPOP = 0xB2,
	EA_CALLNAMEDMETHOD = 0xB3,
	EA_PUSHFLOAT = 0xB4,
	EA_PUSHBYTE = 0xB5,
	EA_PUSHSHORT = 0xB6,
	EA_PUSHLONG = 0xB7,
	EA_BRANCHIFFALSE = 0xB8,
	EA_PUSHREGISTER = 0xB9
};

struct PlaceObjectFlags {
	bool PlaceFlagMove:1;
	bool PlaceFlagHasCharacter:1;
	bool PlaceFlagHasMatrix:1;
	bool PlaceFlagHasColorTransform:1;
	bool PlaceFlagHasRatio:1;
	bool PlaceFlagHasName:1;
	bool PlaceFlagHasClipDepth:1;
	bool PlaceFlagHasClipActions:1;
};

struct ClipEventFlags {
	bool ClipEventLoad:1;
	bool ClipEventEnterFrame:1;
	bool ClipEventUnload:1;
	bool ClipEventMouseMove:1;
	bool ClipEventMouseDown:1;
	bool ClipEventMouseUp:1;
	bool ClipEventKeyDown:1;
	bool ClipEventKeyUp:1;
	bool ClipEventData:1;
	bool ClipEventInitialize:1;
	bool ClipEventPress:1;
	bool ClipEventRelease:1;
	bool ClipEventReleaseOutside:1;
	bool ClipEventRollOver:1;
	bool ClipEventRollOut:1;
	bool ClipEventDragOver:1;
	bool ClipEventDragOut:1;
	bool ClipEventKeyPress:1;
	bool ClipEventConstruct:1;
	char Reserved:5;
	char Reserved2:8;
};

#include <pshpack1.h>
struct Header {
	unsigned int version;
	unsigned int length;
	char size[9];
	unsigned short framerate;
	unsigned short framecount;
};
#include <poppack.h>
char **constantpool;
char *cpdata;
unsigned short cpcount;
unsigned char globalconst;
struct jump {
	unsigned int jumppos;
	unsigned char *jumpend;
	short jumpval;
};
std::vector <jump> jumps;

int GetActionSize(action_type action,unsigned char *input,unsigned int *insize)
{
	int size = 0;
	unsigned char *p = input;
	switch (action)
	{
	case EA_PUSHZERO:
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		p++;
		break;
	case EA_PUSHONE:
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		p++;
		break;
	case EA_CALLFUNCTIONPOP:
		size++;
		size++;
		p++;
		break;
	case EA_CALLMETHOD:
		size++;
		p++;
		break;
	case EA_CALLMETHODPOP:
		size++;
		size++;
		p++;
		break;
	case EA_PUSHGLOBAL:
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		p++;
		break;
	case EA_ZEROVARIABLE:
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		size++;
		p++;
		break;
	case EA_PUSHTRUE:
		size++;
		size++;
		size++;
		size++;
		size++;
		p++;
		break;
	case EA_PUSHFALSE:
		size++;
		size++;
		size++;
		size++;
		size++;
		p++;
		break;
	case EA_PUSHNULL:
		size++;
		size++;
		size++;
		size++;
		p++;
		break;
	case EA_PUSHUNDEFINED:
		size++;
		size++;
		size++;
		size++;
		p++;
		break;
	case EA_PUSHSTRING:
		{
			p++;
			unsigned short asize = *(unsigned short *)(p);
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size += asize;
			p += asize;
		}
		break;
	case EA_PUSHCONSTANT:
		{
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			p++;
		}
		break;
	case EA_PUSHWORDCONSTANT:
		{
			p++;
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			size++;
			p++;
		}
		break;
	case EA_GETSTRINGVAR:
		{
			p++;
			unsigned short asize = *(unsigned short *)(p);
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size += asize;
			p += asize;
			size++;
		}
		break;
	case EA_PUSHVALUEOFVAR:
		{
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			p++;
			size++;
		}
		break;
	case EA_GETNAMEDMEMBER:
		{
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			p++;
			size++;
		}
		break;
	case EA_CALLNAMEDFUNCTIONPOP:
		{
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			p++;
			size++;
			size++;
		}
		break;
	case EA_CALLNAMEDFUNCTION:
		{
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			p++;
			size++;
		}
		break;
	case EA_CALLNAMEDMETHODPOP:
		{
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			p++;
			size++;
			size++;
		}
		break;
	case EA_CALLNAMEDMETHOD:
		{
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			p++;
			size++;
		}
		break;
	case EA_PUSHFLOAT:
		{
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			p++;
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
		}
		break;
	case EA_PUSHREGISTER:
		{
			p++; //actioncode
			p++; //size
			p++; //size
			size++; //actioncode
			size++; //size
			size++; //size
			size++; //type
			p++; //register #
			size++; //register #
		}
		break;
	case EA_PUSHBYTE:
		{
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			p++;
			size++;
			size++;
			size++;
		}
		break;
	case EA_PUSHSHORT:
		{
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			p++;
			size++;
			p++;
			size++;
			size++;
		}
		break;
	case EA_PUSHLONG:
		{
			p++;
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			p++;
			size++;
			p++;
			size++;
			p++;
			size++;
			p++;
		}
		break;
	case EA_PUSHTHIS:
		{
			p++;
			size++;
			size++;
			size++;
			size++;
			size++;
			size++;
			size++;
			size++;
			size++;
			size++;
		}
		break;
	case EA_GETSTRINGMEMBER:
		{
			p++;
			unsigned short asize = *(unsigned short *)(p);
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size += asize;
			p += asize;
			size++;
		}
		break;
	case EA_SETSTRINGMEMBER:
		{
			p++;
			unsigned short asize = *(unsigned short *)(p);
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size += asize;
			p += asize;
			size++;
		}
		break;
	case EA_SETSTRINGVAR:
		{
			p++;
			unsigned short asize = *(unsigned short *)(p);
			p++;
			p++;
			size++;
			size++;
			size++;
			size++;
			size += asize;
			p += asize;
			size++;
		}
		break;
	default:
		size++;
		p++;
		if (action > 0x80)
		{
			unsigned short asize = *(unsigned short *)(p);
			size++;	
			p++;
			size++;
			p++;
			size += asize;
			p += asize;
		}
		break;
	}
	*insize = (unsigned int)(p - input);
	return size;
}

int ConvertAction(action_type action,unsigned char *input,std::stringstream *stream,unsigned int *insize)
{
	int size = 0;
	unsigned char c = 0;
	unsigned char *p = input;
	for (unsigned int ji = 0;ji < jumps.size();ji++)
	{
		if (p == jumps[ji].jumpend)
		{
			jumps[ji].jumpval = (*insize) - (jumps[ji].jumppos + 2);
		}
	}
	switch (action)
	{
	case ACTION_CONSTANTPOOL:
		{
			(*stream) << (*p);
			size++;
			p++;
			unsigned short asize = *(unsigned short *)(p);
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			cpcount = *(unsigned short *)(p);
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			asize -= 2;
			cpdata = new char[asize];
			memcpy(cpdata,p,asize);
			char *cpdatax = cpdata;
			constantpool = new char*[cpcount];
			for (unsigned int i = 0;i < cpcount;i++)
			{
				constantpool[i] = cpdatax;
				if (!_stricmp(cpdatax,"_global"))
				{
					globalconst = i;
				}
				cpdatax += strlen(cpdatax);
				cpdatax++;
			}
			for (unsigned int i = 0;i < asize;i++)
			{
				(*stream) << (*p);
				size++;
				p++;
			}
		}
		break;
	case EA_PUSHZERO:
		c = 0x96;
		(*stream) << c;
		size++;
		c = 0x09;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		c = 0x06;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		p++;
		break;
	case EA_PUSHONE:
		c = 0x96;
		(*stream) << c;
		size++;
		c = 0x05;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		c = 0x07;
		(*stream) << c;
		size++;
		c = 0x01;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		p++;
		break;
	case EA_CALLFUNCTIONPOP:
		c = 0x3D;
		(*stream) << c;
		size++;
		c = 0x17;
		(*stream) << c;
		size++;
		p++;
		break;
	case EA_CALLMETHOD:
		c = 0x52;
		(*stream) << c;
		size++;
		p++;
		break;
	case EA_CALLMETHODPOP:
		c = 0x52;
		(*stream) << c;
		size++;
		c = 0x17;
		(*stream) << c;
		size++;
		p++;
		break;
	case EA_PUSHGLOBAL:
		c = 0x96;
		(*stream) << c;
		size++;
		c = 0x02;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		c = 0x08;
		(*stream) << c;
		size++;
		(*stream) << globalconst;
		size++;
		c = 0x1C;
		(*stream) << c;
		size++;
		p++;
		break;
	case EA_ZEROVARIABLE:
		c = 0x96;
		(*stream) << c;
		size++;
		c = 0x09;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		c = 0x06;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		(*stream) << c;
		size++;
		c = 0x1D;
		(*stream) << c;
		size++;
		p++;
		break;
	case EA_PUSHTRUE:
		c = 0x96;
		(*stream) << c;
		size++;
		c = 0x02;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		c = 0x05;
		(*stream) << c;
		size++;
		c = 0x01;
		(*stream) << c;
		size++;
		p++;
		break;
	case EA_PUSHFALSE:
		c = 0x96;
		(*stream) << c;
		size++;
		c = 0x02;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		c = 0x05;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		p++;
		break;
	case EA_PUSHNULL:
		c = 0x96;
		(*stream) << c;
		size++;
		c = 0x01;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		c = 0x02;
		(*stream) << c;
		size++;
		p++;
		break;
	case EA_PUSHUNDEFINED:
		c = 0x96;
		(*stream) << c;
		size++;
		c = 0x01;
		(*stream) << c;
		size++;
		c = 0x00;
		(*stream) << c;
		size++;
		c = 0x03;
		(*stream) << c;
		size++;
		p++;
		break;
	case EA_PUSHSTRING:
		{
			p++;
			unsigned short asize = *(unsigned short *)(p);
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			unsigned char sz = asize + 1;
			(*stream) << sz;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			for (unsigned int i = 0;i < asize;i++)
			{
				(*stream) << (*p);
				size++;
				p++;
			}
		}
		break;
	case EA_PUSHCONSTANT:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x02;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x08;
			(*stream) << c;
			size++;
			unsigned char constant = (*p);
			(*stream) << constant;
			size++;
			p++;
		}
		break;
	case EA_PUSHWORDCONSTANT:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x03;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x09;
			(*stream) << c;
			size++;
			unsigned char constant = (*p);
			(*stream) << constant;
			size++;
			p++;
			constant = (*p);
			(*stream) << constant;
			size++;
			p++;
		}
		break;
	case EA_GETSTRINGVAR:
		{
			p++;
			unsigned short asize = *(unsigned short *)(p);
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			unsigned char sz = asize + 1;
			(*stream) << sz;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			for (unsigned int i = 0;i < asize;i++)
			{
				(*stream) << (*p);
				size++;
				p++;
			}
			c = 0x1C;
			(*stream) << c;
			size++;
		}
		break;
	case EA_PUSHVALUEOFVAR:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x02;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x08;
			(*stream) << c;
			size++;
			unsigned char constant = (*p);
			(*stream) << constant;
			size++;
			p++;
			c = 0x1C;
			(*stream) << c;
			size++;
		}
		break;
	case EA_GETNAMEDMEMBER:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x02;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x08;
			(*stream) << c;
			size++;
			unsigned char constant = (*p);
			(*stream) << constant;
			size++;
			p++;
			c = 0x4E;
			(*stream) << c;
			size++;
		}
		break;
	case EA_CALLNAMEDFUNCTIONPOP:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x02;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x08;
			(*stream) << c;
			size++;
			unsigned char constant = (*p);
			(*stream) << constant;
			size++;
			p++;
			c = 0x3D;
			(*stream) << c;
			size++;
			c = 0x17;
			(*stream) << c;
			size++;
		}
		break;
	case EA_CALLNAMEDFUNCTION:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x02;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x08;
			(*stream) << c;
			size++;
			unsigned char constant = (*p);
			(*stream) << constant;
			size++;
			p++;
			c = 0x3D;
			(*stream) << c;
			size++;
		}
		break;
	case EA_CALLNAMEDMETHOD:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x02;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x08;
			(*stream) << c;
			size++;
			unsigned char constant = (*p);
			(*stream) << constant;
			size++;
			p++;
			c = 0x52;
			(*stream) << c;
			size++;
		}
		break;
	case EA_CALLNAMEDMETHODPOP:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x02;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x08;
			(*stream) << c;
			size++;
			unsigned char constant = (*p);
			(*stream) << constant;
			size++;
			p++;
			c = 0x52;
			(*stream) << c;
			size++;
			c = 0x17;
			(*stream) << c;
			size++;
		}
		break;
	case EA_PUSHFLOAT:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x05;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x01;
			(*stream) << c;
			size++;
			unsigned char byte = (*p);
			(*stream) << byte;
			size++;
			p++;
			byte = (*p);
			(*stream) << byte;
			size++;
			p++;
			byte = (*p);
			(*stream) << byte;
			size++;
			p++;
			byte = (*p);
			(*stream) << byte;
			size++;
			p++;
		}
		break;
	case EA_PUSHREGISTER:
		{
			p++; //actioncode
			p++; //size
			p++; //size
			c = 0x96;
			(*stream) << c;
			size++; //actioncode
			c = 0x02;
			(*stream) << c;
			size++; //size
			c = 0x00;
			(*stream) << c;
			size++; //size
			c = 0x04;
			(*stream) << c;
			size++; //type
			unsigned char byte = (*p);
			(*stream) << byte;
			p++; //register #
			size++; //register #
		}
		break;
	case EA_PUSHBYTE:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x05;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x07;
			(*stream) << c;
			size++;
			unsigned char byte = (*p);
			(*stream) << byte;
			size++;
			p++;
			c = 0x00;
			(*stream) << c;
			size++;
			(*stream) << c;
			size++;
			(*stream) << c;
			size++;
		}
		break;
	case EA_PUSHSHORT:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x05;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x07;
			(*stream) << c;
			size++;
			unsigned char byte = (*p);
			(*stream) << byte;
			size++;
			p++;
			byte = (*p);
			(*stream) << byte;
			size++;
			p++;
			c = 0x00;
			(*stream) << c;
			size++;
			(*stream) << c;
			size++;
		}
		break;
	case EA_PUSHLONG:
		{
			p++;
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x05;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x07;
			(*stream) << c;
			size++;
			unsigned char byte = (*p);
			(*stream) << byte;
			size++;
			p++;
			byte = (*p);
			(*stream) << byte;
			size++;
			p++;
			byte = (*p);
			(*stream) << byte;
			size++;
			p++;
			byte = (*p);
			(*stream) << byte;
			size++;
			p++;
		}
		break;
	case ACTION_BRANCHALWAYS:
	case ACTION_BRANCHIFTRUE:
		{
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			short jv = *(short *)(p);
			if (jv < 0)
			{
				unsigned char *x = p;
				x += jv;
				x += 2;
				int loutsz = 0;
				unsigned int linsz = 0;
				for (unsigned char *i = x;i < p + 2;)
				{
					unsigned char cx = (*i);
					loutsz += GetActionSize((action_type)cx,i,&linsz);
					i += linsz;
				}
				loutsz = -loutsz;
				(*stream) << (LOBYTE(loutsz));
				size++;
				p++;
				(*stream) << (HIBYTE(loutsz));
				size++;
				p++;
			}
			else
			{
				jump j;
				j.jumppos = (*insize) + size;
				j.jumpend = p + jv + 2;
				j.jumpval = 0;
				(*stream) << (*p);
				size++;
				p++;
				(*stream) << (*p);
				size++;
				p++;
				jumps.push_back(j);
			}
		}
		break;
	case EA_PUSHTHIS:
		{
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			c = 0x06;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x74;
			(*stream) << c;
			size++;
			c = 0x68;
			(*stream) << c;
			size++;
			c = 0x69;
			(*stream) << c;
			size++;
			c = 0x73;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x1C;
			(*stream) << c;
			size++;
		}
		break;
	case EA_GETSTRINGMEMBER:
		{
			p++;
			unsigned short asize = *(unsigned short *)(p);
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			unsigned char sz = asize + 1;
			(*stream) << sz;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			for (unsigned int i = 0;i < asize;i++)
			{
				(*stream) << (*p);
				size++;
				p++;
			}
			c = 0x4E;
			(*stream) << c;
			size++;
		}
		break;
	case EA_SETSTRINGMEMBER:
		{
			p++;
			unsigned short asize = *(unsigned short *)(p);
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			unsigned char sz = asize + 1;
			(*stream) << sz;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			for (unsigned int i = 0;i < asize;i++)
			{
				(*stream) << (*p);
				size++;
				p++;
			}
			c = 0x4F;
			(*stream) << c;
			size++;
		}
		break;
	case EA_SETSTRINGVAR:
		{
			p++;
			unsigned short asize = *(unsigned short *)(p);
			p++;
			p++;
			c = 0x96;
			(*stream) << c;
			size++;
			unsigned char sz = asize + 1;
			(*stream) << sz;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			c = 0x00;
			(*stream) << c;
			size++;
			for (unsigned int i = 0;i < asize;i++)
			{
				(*stream) << (*p);
				size++;
				p++;
			}
			c = 0x1D;
			(*stream) << c;
			size++;
		}
		break;
	case ACTION_DEFINEFUNCTION:
		{
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			char fch = 0;
			do
			{
				fch = (*p);
				(*stream) << (*p);
				size++;
				p++;
			} while (fch != 0);
			unsigned short asize = *(unsigned short *)(p);
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			for (unsigned short i = 0;i < asize;i++)
			{
				do
				{
					fch = (*p);
					(*stream) << (*p);
					size++;
					p++;
				} while (fch != 0);
			}
			short jv = *(short *)(p);
			jump j;
			j.jumppos = (*insize) + size;
			j.jumpend = p + jv + 2;
			j.jumpval = 0;
			jumps.push_back(j);
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
		}
		break;
	case ACTION_DEFINEFUNCTION2:
		{
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			char fch = 0;
			do
			{
				fch = (*p);
				(*stream) << (*p);
				size++;
				p++;
			} while (fch != 0);
			unsigned short asize = *(unsigned short *)(p);
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
			for (unsigned short i = 0;i < asize;i++)
			{
				(*stream) << (*p);
				size++;
				p++;
				do
				{
					fch = (*p);
					(*stream) << (*p);
					size++;
					p++;
				} while (fch != 0);
			}
			jump j;
			short jv = *(short *)(p);
			j.jumppos = (*insize) + size;
			j.jumpend = p + jv + 2;
			j.jumpval = 0;
			jumps.push_back(j);
			(*stream) << (*p);
			size++;
			p++;
			(*stream) << (*p);
			size++;
			p++;
		}
		break;
	default:
		(*stream) << (*p);
		size++;
		p++;
		if (action > 0x80)
		{
			unsigned short asize = *(unsigned short *)(p);
			(*stream) << (*p);
			size++;	
			p++;
			(*stream) << (*p);
			size++;
			p++;
			for (unsigned int i = 0;i < asize;i++)
			{
				(*stream) << (*p);
				size++;
				p++;
			}
		}
		break;
	}
	*insize = (unsigned int)(p - input);
	return size;
}
unsigned char current_byte;
unsigned char unused_bits;
unsigned int read_bits(int bitcount,unsigned char **p)
{
	unsigned int value = 0;
	unsigned short bits_needed = bitcount;
	while (bits_needed > 0)
	{
		if (unused_bits) {
			if (bits_needed >= unused_bits) {
				value |= (current_byte << (bits_needed - unused_bits));
				bits_needed -= unused_bits;
				current_byte = 0;
				unused_bits = 0;
			} else {
				value |= (current_byte >> (unused_bits - bits_needed));
				current_byte &= ((1 << (unused_bits - bits_needed)) - 1);
				unused_bits -= bits_needed;
				bits_needed = 0;
			}
		} else {
			current_byte = (**p);
			(*p)++;
			unused_bits = 8;
		}
	}
	return value;
}
int read_bits_signed(int bitcount,unsigned char **p)
{
	int x = (int)read_bits(bitcount,p);
	if (x & (1 << (bitcount - 1)))
	{
		x |= -1 << bitcount;
	}
	return x;
}

unsigned int read_matrix_size(unsigned char *p)
{
	current_byte = 0;
	unused_bits = 0;
	int total_bits = 0;
	int scale = read_bits(1,&p);
	total_bits++;
	if (scale)
	{
		int scalebits = read_bits(5,&p);
		total_bits += 5;
		float scalex = read_bits_signed(scalebits,&p) / 65536.0f;
		total_bits += scalebits;
		float scaley = read_bits_signed(scalebits,&p) / 65536.0f;
		total_bits += scalebits;
	}
	int rotate = read_bits(1,&p);
	total_bits++;
	if (rotate)
	{
		int rotatebits = read_bits(5,&p);
		total_bits += 5;
		float rotatex = read_bits_signed(rotatebits,&p) / 65536.0f;
		total_bits += rotatebits;
		float rotatey = read_bits_signed(rotatebits,&p) / 65536.0f;
		total_bits += rotatebits;
	}
	int translatebits = read_bits(5,&p);
	total_bits += 5;
	int translatex = read_bits_signed(translatebits,&p);
	total_bits += translatebits;
	int translatey = read_bits_signed(translatebits,&p);
	total_bits += translatebits;
	total_bits = 8 * ((total_bits + 7)/8);
	return total_bits / 8;
}

unsigned int read_color_transform_alpha_size(unsigned char *p)
{
	current_byte = 0;
	unused_bits = 0;
	int total_bits = 0;
	int hasaddterms = read_bits(1,&p);
	total_bits++;
	int hasmultterms = read_bits(1,&p);
	total_bits++;
	int nbits = read_bits(4,&p);
	total_bits += 4;
	if (hasmultterms)
	{
		total_bits += nbits;
		total_bits += nbits;
		total_bits += nbits;
		total_bits += nbits;
	}
	if (hasaddterms)
	{
		total_bits += nbits;
		total_bits += nbits;
		total_bits += nbits;
		total_bits += nbits;
	}
	total_bits = 8 * ((total_bits + 7) / 8);
	return total_bits / 8;
}

int main(int argc, char* argv[])
{
	FILE *f = fopen(argv[1],"rb");
	if (!f)
	{
		return 1;
	}
	int x = fseek(f,0,SEEK_END);
	int fsize = ftell(f);
	x = fseek(f,0,SEEK_SET);
	unsigned char *c = new unsigned char[fsize];
	fread(c,fsize,1,f);
	unsigned char *p = c;
	fclose(f);
	Header *h = (Header *)p;
	if ((h->version == 0x07535743) || (h->version == 0x06535743))
	{
		p += 8;
		unsigned char *z = new unsigned char[h->length];
		memcpy(z,h,sizeof(Header));
		z[0] = 'F';
		unsigned char *o = z;
		o += 8;
		z_stream s;
		s.total_out = 0;
		s.zalloc = (alloc_func)0;
		s.zfree = (free_func)0;
		s.opaque = (voidpf)0;
		s.next_in = (Bytef*)p;
		s.avail_in = (uInt)(fsize - 8);
		s.next_out = (Bytef*)o;
		s.avail_out = (uInt)(h->length - 8);
		inflateInit(&s);
		inflate(&s,Z_FINISH);
		inflateEnd(&s);
		fsize = h->length;
		c = z;
		p = z;
	}
	h = (Header *)p;
	p += sizeof(Header);
	std::stringstream streamf;
	unsigned int sizef = sizeof(Header);
	tag_type tag;
	do
	{
		unsigned short tag_header = *(unsigned short *)(p);
		unsigned int hl = 2;
		tag = (tag_type)(tag_header >> 6);
		unsigned int tag_length = tag_header & 0x3F;
		if (tag_length == 0x3F)
		{
			tag_length = *(unsigned int *)(p + 2);
			hl += 4;
		}
		if ((tag == DOACTION) || (tag == INITACTION))
		{
			constantpool = 0;
			cpdata = 0;
			cpcount = 0;
			p += hl;
			unsigned short sprite = 0;
			unsigned int size = 0;
			std::stringstream stream;
			if (tag == INITACTION)
			{
				sprite = *(unsigned short *)(p);
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
			}
			action_type action;
			do
			{
				action = (action_type)(*p);
				unsigned int sz = size;
				size += ConvertAction(action,p,&stream,&sz);
				p += sz;
			} while (action != ACTION_END);
			unsigned short outtag;
			unsigned short ot;
			if (tag == DOACTION)
			{
				ot = 0x300;
			}
			else
			{
				ot = 0xEC0;
			}
			if (size >= 63)
			{
				outtag = ot + 0x3F;
				streamf << LOBYTE(outtag);
				streamf << HIBYTE(outtag);
				sizef++;
				sizef++;
				streamf << LOBYTE(LOWORD(size));
				streamf << HIBYTE(LOWORD(size));
				streamf << LOBYTE(HIWORD(size));
				streamf << HIBYTE(HIWORD(size));
				sizef += 4;
			}
			else
			{
				outtag = ot + size;
				streamf << LOBYTE(outtag);
				streamf << HIBYTE(outtag);
				sizef++;
				sizef++;
			}
			std::string str = stream.str();
			const char *cs = str.c_str();
			for (unsigned int ji = 0;ji < jumps.size();ji++)
			{
				memcpy((void *)&(cs[jumps[ji].jumppos]),&(jumps[ji].jumpval),2);
			}
			jumps.clear();
			for (unsigned int i = 0;i < size;i++)
			{
				streamf << cs[i];
				sizef++;
			}
		}
		else if (tag == PLACEOBJECT2)
		{
			constantpool = 0;
			cpdata = 0;
			cpcount = 0;
			p += hl;
			unsigned int size = 0;
			unsigned short character = 0;
			std::stringstream stream;
			PlaceObjectFlags flags = *((PlaceObjectFlags *)(p));
			stream << (*p);
			size++;
			p++;
			stream << (*p);
			size++;
			p++;
			stream << (*p);
			size++;
			p++;
			if (flags.PlaceFlagHasCharacter)
			{
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
			}
			if (flags.PlaceFlagHasMatrix)
			{
				unsigned int ms = read_matrix_size(p);
				for (unsigned int m = 0;m < ms;m++)
				{
					stream << (*p);
					size++;
					p++;
				}
			}
			if (flags.PlaceFlagHasColorTransform)
			{
				unsigned int cs = read_color_transform_alpha_size(p);
				for (unsigned int m = 0;m < cs;m++)
				{
					stream << (*p);
					size++;
					p++;
				}
			}
			if (flags.PlaceFlagHasRatio)
			{
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
			}
			if (flags.PlaceFlagHasName)
			{
				char nch;
				do
				{
					nch = (*p);
					stream << (*p);
					size++;
					p++;
				} while (nch != 0);
			}
			if (flags.PlaceFlagHasClipDepth)
			{
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
			}
			if (flags.PlaceFlagHasClipActions)
			{
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
				bool end = false;
				do
				{
					ClipEventFlags cf = *(ClipEventFlags *)(p);
					unsigned int fl = *(unsigned int *)(p);
					stream << (*p);
					size++;
					p++;
					stream << (*p);
					size++;
					p++;
					stream << (*p);
					size++;
					p++;
					stream << (*p);
					size++;
					p++;
					if (!fl)
					{
						break;
					}
					unsigned int recsize = *(unsigned int *)(p);
					p += 4;
					std::stringstream streama;
					unsigned int sizea = 0;
					unsigned char key = 0;
					if (cf.ClipEventKeyPress)
					{
						key = (*p);
						recsize--;
						streama << (*p);
						sizea++;
						p++;
					}
					action_type action;
					for (unsigned int a = 0;a < recsize;)
					{
						action = (action_type)(*p);
						unsigned int sz = sizea;
						sizea += ConvertAction(action,p,&streama,&sz);
						p += sz;
						a += sz;
					}
					stream << LOBYTE(LOWORD(sizea));
					stream << HIBYTE(LOWORD(sizea));
					stream << LOBYTE(HIWORD(sizea));
					stream << HIBYTE(HIWORD(sizea));
					size += 4;
					std::string arstr = streama.str();
					const char *ars = arstr.c_str();
					for (unsigned int ji = 0;ji < jumps.size();ji++)
					{
						memcpy((void *)&(ars[jumps[ji].jumppos]),&(jumps[ji].jumpval),2);
					}
					jumps.clear();
					for (unsigned int i = 0;i < sizea;i++)
					{
						stream << ars[i];
					}
					size += sizea;
				} while (!end);
			}
			unsigned short outtag;
			unsigned short ot;
			ot = 0x680;
			if (size >= 63)
			{
				outtag = ot + 0x3F;
				streamf << LOBYTE(outtag);
				streamf << HIBYTE(outtag);
				sizef++;
				sizef++;
				streamf << LOBYTE(LOWORD(size));
				streamf << HIBYTE(LOWORD(size));
				streamf << LOBYTE(HIWORD(size));
				streamf << HIBYTE(HIWORD(size));
				sizef += 4;
			}
			else
			{
				outtag = ot + size;
				streamf << LOBYTE(outtag);
				streamf << HIBYTE(outtag);
				sizef++;
				sizef++;
			}
			std::string str = stream.str();
			const char *cs = str.c_str();
			for (unsigned int ji = 0;ji < jumps.size();ji++)
			{
				memcpy((void *)&(cs[jumps[ji].jumppos]),&(jumps[ji].jumpval),2);
			}
			jumps.clear();
			for (unsigned int i = 0;i < size;i++)
			{
				streamf << cs[i];
				sizef++;
			}
		}
		else if (tag == DEFINEBUTTON2)
		{
			constantpool = 0;
			cpdata = 0;
			cpcount = 0;
			p += hl;
			unsigned int size = 0;
			unsigned short character = 0;
			std::stringstream stream;
			stream << (*p);
			size++;
			p++;
			stream << (*p);
			size++;
			p++;
			stream << (*p);
			size++;
			p++;
			unsigned short hasactions = *(unsigned short*)(p);
			stream << (*p);
			size++;
			p++;
			stream << (*p);
			size++;
			p++;
			bool end = false;
			do
			{
				unsigned char fl = *(p);
				stream << (*p);
				size++;
				p++;
				if (!fl)
				{
					break;
				}
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
				stream << (*p);
				size++;
				p++;
				unsigned int ms = read_matrix_size(p);
				for (unsigned int m = 0;m < ms;m++)
				{
					stream << (*p);
					size++;
					p++;
				}
				if ((*p) != 0x04)
				{
				}
				unsigned int cs = read_color_transform_alpha_size(p);
				for (unsigned int m = 0;m < cs;m++)
				{
					stream << (*p);
					size++;
					p++;
				}
			} while (!end);
			if (hasactions)
			{
				unsigned short offset;
				do
				{
					offset = *(unsigned short *)(p);
					unsigned short sizea = 0;
					unsigned int a = 0;
					std::stringstream streama;
					p++;
					p++;
					streama << (*p);
					sizea++;
					p++;
					a++;
					streama << (*p);
					sizea++;
					p++;
					a++;
					action_type action;
					do
					{
						action = (action_type)(*p);
						unsigned int sz = sizea;
						sizea += ConvertAction(action,p,&streama,&sz);
						p += sz;
						a += sz;
					} while (action != ACTION_END);
					if (offset)
					{
						sizea += 2;
						stream << LOBYTE(sizea);
						stream << HIBYTE(sizea);
						sizea -= 2;
					}
					else
					{
						char xc = 0;
						stream << xc;
						stream << xc;
					}
					size += 2;
					std::string arstr = streama.str();
					const char *ars = arstr.c_str();
					for (unsigned int ji = 0;ji < jumps.size();ji++)
					{
						memcpy((void *)&(ars[jumps[ji].jumppos]),&(jumps[ji].jumpval),2);
					}
					jumps.clear();
					for (unsigned int i = 0;i < sizea;i++)
					{
						stream << ars[i];
					}
					size += sizea;
				} while (offset != 0);
			}
			unsigned short outtag;
			unsigned short ot;
			ot = 0x880;
			if (size >= 63)
			{
				outtag = ot + 0x3F;
				streamf << LOBYTE(outtag);
				streamf << HIBYTE(outtag);
				sizef++;
				sizef++;
				streamf << LOBYTE(LOWORD(size));
				streamf << HIBYTE(LOWORD(size));
				streamf << LOBYTE(HIWORD(size));
				streamf << HIBYTE(HIWORD(size));
				sizef += 4;
			}
			else
			{
				outtag = ot + size;
				streamf << LOBYTE(outtag);
				streamf << HIBYTE(outtag);
				sizef++;
				sizef++;
			}
			std::string str = stream.str();
			const char *cs = str.c_str();
			for (unsigned int i = 0;i < size;i++)
			{
				streamf << cs[i];
				sizef++;
			}
		}
		else if (tag == DEFINESPRITE)
		{
			p += hl;
			unsigned char *sx = p;
			unsigned short spriteid = *(unsigned short *)(p);
			p++;
			p++;
			unsigned short framecount = *(unsigned short *)(p);
			p++;
			p++;
			std::stringstream streams;
			streams << LOBYTE(spriteid);
			streams << HIBYTE(spriteid);
			streams << LOBYTE(framecount);
			streams << HIBYTE(framecount);
			unsigned int sizes = 4;
			tag_type s_tag;
			do
			{
				unsigned short s_tag_header = *(unsigned short *)(p);
				unsigned int s_hl = 2;
				s_tag = (tag_type)(s_tag_header >> 6);
				unsigned int s_tag_length = s_tag_header & 0x3F;
				if (s_tag_length == 0x3F)
				{
					s_tag_length = *(unsigned int *)(p + 2);
					s_hl += 4;
				}
				if ((s_tag == DOACTION) || (s_tag == INITACTION))
				{
					constantpool = 0;
					cpdata = 0;
					cpcount = 0;
					p += s_hl;
					unsigned short sprite = 0;
					unsigned int size = 0;
					std::stringstream stream;
					if (s_tag == INITACTION)
					{
						sprite = *(unsigned short *)(p);
						stream << (*p);
						size++;
						p++;
						stream << (*p);
						size++;
						p++;
					}
					action_type action;
					do
					{
						action = (action_type)(*p);
						unsigned int sz = size;
						size += ConvertAction(action,p,&stream,&sz);
						p += sz;
					} while (action != ACTION_END);
					unsigned short outtag;
					unsigned short ot;
					if (s_tag == DOACTION)
					{
						ot = 0x300;
					}
					else
					{
						ot = 0xEC0;
					}
					if (size >= 63)
					{
						outtag = ot + 0x3F;
						streams << LOBYTE(outtag);
						streams << HIBYTE(outtag);
						sizes++;
						sizes++;
						streams << LOBYTE(LOWORD(size));
						streams << HIBYTE(LOWORD(size));
						streams << LOBYTE(HIWORD(size));
						streams << HIBYTE(HIWORD(size));
						sizes += 4;
					}
					else
					{
						outtag = ot + size;
						streams << LOBYTE(outtag);
						streams << HIBYTE(outtag);
						sizes++;
						sizes++;
					}
					std::string str = stream.str();
					const char *cs = str.c_str();
					for (unsigned int ji = 0;ji < jumps.size();ji++)
					{
						memcpy((void *)&(cs[jumps[ji].jumppos]),&(jumps[ji].jumpval),2);
					}
					jumps.clear();
					for (unsigned int i = 0;i < size;i++)
					{
						streams << cs[i];
						sizes++;
					}
				}
				else if (s_tag == PLACEOBJECT2)
				{
					constantpool = 0;
					cpdata = 0;
					cpcount = 0;
					p += s_hl;
					unsigned int size = 0;
					unsigned short character = 0;
					std::stringstream stream;
					PlaceObjectFlags flags = *((PlaceObjectFlags *)(p));
					stream << (*p);
					size++;
					p++;
					stream << (*p);
					size++;
					p++;
					stream << (*p);
					size++;
					p++;
					if (flags.PlaceFlagHasCharacter)
					{
						stream << (*p);
						size++;
						p++;
						stream << (*p);
						size++;
						p++;
					}
					if (flags.PlaceFlagHasMatrix)
					{
						unsigned int ms = read_matrix_size(p);
						for (unsigned int m = 0;m < ms;m++)
						{
							stream << (*p);
							size++;
							p++;
						}
					}
					if (flags.PlaceFlagHasColorTransform)
					{
						unsigned int cs = read_color_transform_alpha_size(p);
						for (unsigned int m = 0;m < cs;m++)
						{
							stream << (*p);
							size++;
							p++;
						}
					}
					if (flags.PlaceFlagHasRatio)
					{
						stream << (*p);
						size++;
						p++;
						stream << (*p);
						size++;
						p++;
					}
					if (flags.PlaceFlagHasName)
					{
						char nch;
						do
						{
							nch = (*p);
							stream << (*p);
							size++;
							p++;
						} while (nch != 0);
					}
					if (flags.PlaceFlagHasClipDepth)
					{
						stream << (*p);
						size++;
						p++;
						stream << (*p);
						size++;
						p++;
					}
					if (flags.PlaceFlagHasClipActions)
					{
						stream << (*p);
						size++;
						p++;
						stream << (*p);
						size++;
						p++;
						stream << (*p);
						size++;
						p++;
						stream << (*p);
						size++;
						p++;
						stream << (*p);
						size++;
						p++;
						stream << (*p);
						size++;
						p++;
						bool end = false;
						do
						{
							ClipEventFlags cf = *(ClipEventFlags *)(p);
							unsigned int fl = *(unsigned int *)(p);
							stream << (*p);
							size++;
							p++;
							stream << (*p);
							size++;
							p++;
							stream << (*p);
							size++;
							p++;
							stream << (*p);
							size++;
							p++;
							if (!fl)
							{
								break;
							}
							unsigned int recsize = *(unsigned int *)(p);
							p += 4;
							std::stringstream streama;
							unsigned int sizea = 0;
							unsigned char key = 0;
							if (cf.ClipEventKeyPress)
							{
								key = (*p);
								recsize--;
								streama << (*p);
								sizea++;
								p++;
							}
							action_type action;
							for (unsigned int a = 0;a < recsize;)
							{
								action = (action_type)(*p);
								unsigned int sz = sizea;
								sizea += ConvertAction(action,p,&streama,&sz);
								p += sz;
								a += sz;
							}
							stream << LOBYTE(LOWORD(sizea));
							stream << HIBYTE(LOWORD(sizea));
							stream << LOBYTE(HIWORD(sizea));
							stream << HIBYTE(HIWORD(sizea));
							size += 4;
							std::string arstr = streama.str();
							const char *ars = arstr.c_str();
							for (unsigned int ji = 0;ji < jumps.size();ji++)
							{
								memcpy((void *)&(ars[jumps[ji].jumppos]),&(jumps[ji].jumpval),2);
							}
							jumps.clear();
							for (unsigned int i = 0;i < sizea;i++)
							{
								stream << ars[i];
							}
							size += sizea;
						} while (!end);
					}
					unsigned short outtag;
					unsigned short ot;
					ot = 0x680;
					if (size >= 63)
					{
						outtag = ot + 0x3F;
						streams << LOBYTE(outtag);
						streams << HIBYTE(outtag);
						sizes++;
						sizes++;
						streams << LOBYTE(LOWORD(size));
						streams << HIBYTE(LOWORD(size));
						streams << LOBYTE(HIWORD(size));
						streams << HIBYTE(HIWORD(size));
						sizes += 4;
					}
					else
					{
						outtag = ot + size;
						streams << LOBYTE(outtag);
						streams << HIBYTE(outtag);
						sizes++;
						sizes++;
					}
					std::string str = stream.str();
					const char *cs = str.c_str();
					for (unsigned int ji = 0;ji < jumps.size();ji++)
					{
						memcpy((void *)&(cs[jumps[ji].jumppos]),&(jumps[ji].jumpval),2);
					}
					jumps.clear();
					for (unsigned int i = 0;i < size;i++)
					{
						streams << cs[i];
						sizes++;
					}
				}
				else
				{
					for (unsigned int i = 0;i < (s_tag_length + s_hl);i++)
					{
						streams << (*p);
						sizes++;
						p++;
					}
				}
			} while (p < (sx + tag_length));
			unsigned short outtag;
			unsigned short ot;
			ot = 0x9C0;
			outtag = ot + 0x3F;
			streamf << LOBYTE(outtag);
			streamf << HIBYTE(outtag);
			sizef++;
			sizef++;
			streamf << LOBYTE(LOWORD(sizes));
			streamf << HIBYTE(LOWORD(sizes));
			streamf << LOBYTE(HIWORD(sizes));
			streamf << HIBYTE(HIWORD(sizes));
			sizef += 4;
			std::string strs = streams.str();
			const char *cs = strs.c_str();
			for (unsigned int i = 0;i < sizes;i++)
			{
				streamf << cs[i];
				sizef++;
			}
		}
		else
		{
			for (unsigned int i = 0;i < (tag_length + hl);i++)
			{
				streamf << (*p);
				sizef++;
				p++;
			}
		}
	} while (p < (c + fsize));
	char *ofn = _strdup(argv[1]);
	if (!ofn)
	{
		return 1;
	}
	ofn[strlen(ofn) - 1] = 'f';
	ofn[strlen(ofn) - 2] = 'w';
	ofn[strlen(ofn) - 3] = 's';
	FILE *f2 = fopen(ofn,"wb");
	if (!f2)
	{
		return 1;
	}
	h->length = sizef;
	fwrite(h,sizeof(Header),1,f2);
	std::string strf = streamf.str();
	const char *cs = strf.c_str();
	fwrite(cs,sizef - sizeof(Header),1,f2);
	fclose(f);
	fclose(f2);
	return 0;
}
