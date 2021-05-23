/*
	Command & Conquer 3 Tools
	apt binary structure definitions and functions
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
#ifndef APT_FILE_H_INCLUDED
#define APT_FILE_H_INCLUDED

#define STRLENGTH(x) (4 * ((((unsigned int)strlen(x) + 1) + 3)/4))
#define GETALIGN(x) ((4 * ((x + 3) / 4)) - x)
#define ALIGN(x) x = ((unsigned char *)(4 * ((((unsigned int)x) + 3) / 4)))
#define B(x) x?"true":"false"

#include <vector>
#include <sstream>

struct Vector2 {
	float X;
	float Y;
};

struct Transform {
	float m00;
	float m01;
	float m10;
	float m11;
};

struct Rect {
	float top;
	float left;
	float bottom;
	float right;
};

enum CharacterType {
	SHAPE = 1,
	EDITTEXT = 2,
	FONT = 3,
	BUTTON = 4,
	SPRITE = 5,
	IMAGE = 7,
	MORPH = 8,
	MOVIE = 9,
	TEXT = 10
};

#define CHAR_SIG 0x09876543

struct Character {
	unsigned int type;
	unsigned int signature;
};

enum action_type
{
	ACTION_END					 = 0x00,
	ACTION_NEXTFRAME			   = 0x04,
	ACTION_PREVFRAME			   = 0x05,
	ACTION_PLAY					= 0x06,
	ACTION_STOP					= 0x07,
	ACTION_TOGGLEQUALITY		   = 0x08,
	ACTION_STOPSOUNDS			  = 0x09,
	ACTION_GOTOFRAME			   = 0x81,
	ACTION_GETURL				  = 0x83,
	ACTION_WAITFORFRAME			= 0x8A,
	ACTION_SETTARGET			   = 0x8B,
	ACTION_GOTOLABEL			   = 0x8C,
	ACTION_ADD					 = 0x0A,
	ACTION_SUBTRACT				= 0x0B,
	ACTION_MULTIPLY				= 0x0C,
	ACTION_DIVIDE				  = 0x0D,

	ACTION_EQUAL				   = 0x0E,

	ACTION_LESSTHAN				= 0x0F,
	ACTION_LOGICALAND			  = 0x10,
	ACTION_LOGICALOR			   = 0x11,
	ACTION_LOGICALNOT			  = 0x12,
	ACTION_STRINGEQ				= 0x13,
	ACTION_STRINGLENGTH			= 0x14,
	ACTION_SUBSTRING			   = 0x15,
	ACTION_POP					 = 0x17,
	ACTION_INT					 = 0x18,
	ACTION_GETVARIABLE			 = 0x1C,
	ACTION_SETVARIABLE			 = 0x1D,
	ACTION_SETTARGETEXPRESSION	 = 0x20,
	ACTION_STRINGCONCAT			= 0x21,
	ACTION_GETPROPERTY			 = 0x22,
	ACTION_SETPROPERTY			 = 0x23,
	ACTION_DUPLICATECLIP		   = 0x24,
	ACTION_REMOVECLIP			  = 0x25,
	ACTION_TRACE				   = 0x26,
	ACTION_STARTDRAGMOVIE		  = 0x27,
	ACTION_STOPDRAGMOVIE		   = 0x28,
	ACTION_STRINGCOMPARE		   = 0x29,
	ACTION_THROW				   = 0x2A,

	ACTION_CASTOP				  = 0x2B,

	ACTION_IMPLEMENTSOP			= 0x2C,

	ACTION_RANDOM				  = 0x30,
	ACTION_MBLENGTH				= 0x31,
	ACTION_ORD					 = 0x32,
	ACTION_CHR					 = 0x33,
	ACTION_GETTIMER				= 0x34,
	ACTION_MBSUBSTRING			 = 0x35,
	ACTION_MBORD				   = 0x36,
	ACTION_MBCHR				   = 0x37,
	ACTION_WAITFORFRAMEEXPRESSION  = 0x8D,
	ACTION_PUSHDATA				= 0x96,
	ACTION_BRANCHALWAYS			= 0x99,
	ACTION_GETURL2				 = 0x9A,
	ACTION_BRANCHIFTRUE			= 0x9D,
	ACTION_CALLFRAME			   = 0x9E,
	ACTION_GOTOEXPRESSION		  = 0x9F,

	ACTION_DELETE				  = 0x3A,

	ACTION_DELETE2				 = 0x3B,

	ACTION_DEFINELOCAL			 = 0x3C,
	ACTION_CALLFUNCTION			= 0x3D,
	ACTION_RETURN				  = 0x3E,
	ACTION_MODULO				  = 0x3F,

	ACTION_NEW					 = 0x40,

	ACTION_VAR					 = 0x41,
	ACTION_INITARRAY			   = 0x42,


	ACTION_INITOBJECT			  = 0x43,

	ACTION_TYPEOF				  = 0x44,

	ACTION_TARGETPATH			  = 0x45,

	ACTION_ENUMERATE			   = 0x46,

	ACTION_NEWADD				  = 0x47,
	ACTION_NEWLESSTHAN			 = 0x48,

	ACTION_NEWEQUALS			   = 0x49,

	ACTION_TONUMBER				= 0x4A,

	ACTION_TOSTRING				= 0x4B,

	ACTION_DUP					 = 0x4C,

	ACTION_SWAP					= 0x4D,

	ACTION_GETMEMBER			   = 0x4E,

	ACTION_SETMEMBER			   = 0x4F,

	ACTION_INCREMENT			   = 0x50,

	ACTION_DECREMENT			   = 0x51,

	ACTION_CALLMETHOD			  = 0x52,

	ACTION_NEWMETHOD			   = 0x53, 

	ACTION_INSTANCEOF			  = 0x54,

	ACTION_ENUM2				   = 0x55,

	ACTION_BITWISEAND			  = 0x60,
	ACTION_BITWISEOR			   = 0x61,
	ACTION_BITWISEXOR			  = 0x62,
	ACTION_SHIFTLEFT			   = 0x63,
	ACTION_SHIFTRIGHT			  = 0x64,
	ACTION_SHIFTRIGHT2			 = 0x65,
	ACTION_STRICTEQ				= 0x66,

	ACTION_GREATER				 = 0x67,

	ACTION_STRINGGREATER		   = 0x68,

	ACTION_EXTENDS				 = 0x69,

	ACTION_CONSTANTPOOL			= 0x88,


	ACTION_DEFINEFUNCTION2		 = 0x8E,

	ACTION_TRY					 = 0x8F,
	ACTION_WITH					= 0x94,
	ACTION_DEFINEFUNCTION		  = 0x9B,

	ACTION_SETREGISTER			 = 0x87,
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

struct ButtonFlags {
	bool ButtonStateUp:1;
	bool ButtonStateOver:1;
	bool ButtonStateDown:1;
	bool ButtonStateHitTest:1;
	char Reserved:4;
};

struct ButtonActionFlags {
	bool CondOverDownToIdle:1;
	char KeyPress:7;
	bool CondIdleToOverUp:1;
	bool CondOverUpToIdle:1;
	bool CondOverUpToOverDown:1;
	bool CondOverDownToOverUp:1;
	bool CondOverDownToOutDown:1;
	bool CondOutDownToOverDown:1;
	bool CondOutDownToIdle:1;
	bool CondIdleToOverDown:1;
};

enum FrameItemType {
	ACTION = 1,
	FRAMELABEL = 2,
	PLACEOBJECT = 3,
	REMOVEOBJECT = 4,
	BACKGROUNDCOLOR = 5,
	INITACTION = 8
};

struct FrameItem {
	unsigned int type;
};

struct Frame {
	unsigned int frameitemcount;
	std::vector<FrameItem *> frameitems;
};

struct OutputFrame {
	unsigned int frameitemcount;
	FrameItem **frameitems;
};

struct Shape : public Character {
	Rect bounds;
	unsigned int geometry;
};

struct Tri {
	Vector2 v1;
	Vector2 v2;
	Vector2 v3;
};

struct Line {
	Vector2 v1;
	Vector2 v2;
};

struct SolidStyle {
	unsigned int red;
	unsigned int green;
	unsigned int blue;
	unsigned int alpha;
};

struct LineStyle {
	unsigned int width;
	unsigned int red;
	unsigned int green;
	unsigned int blue;
	unsigned int alpha;
};

struct TextureStyle {
	unsigned int red;
	unsigned int green;
	unsigned int blue;
	unsigned int alpha;
	unsigned int TextureCharacter;
	Transform rotateandscale;
	Vector2 translate;
};

struct GeometryEntry {
	unsigned int tricount;
	std::vector<Tri *> tris;
	unsigned int linecount;
	std::vector <Line *> lines;
	unsigned int solidstylecount;
	std::vector <SolidStyle *> solidstyles;
	unsigned int linestylecount;
	std::vector <LineStyle *> linestyles;
	unsigned int texturestylecount;
	std::vector <TextureStyle *> texturestyles;
};

struct Geometry {
	unsigned int entrycount;
	std::vector<GeometryEntry *> entries;
};

struct ParsedShape : public Character {
	Rect bounds;
	Geometry *geometry;
};

struct EditText : public Character {
	Rect bounds;
	unsigned int font;
	unsigned int alignment;
	unsigned int color;
	float fontheight;
	unsigned int readonly;
	unsigned int multiline;
	unsigned int wordwrap;
	const char *text;
	const char *variable;
};

struct Font : public Character {
	const char *name;
	unsigned int glyphcount;
	std::vector<unsigned int> glyphs;
};

struct OutputFont : public Character {
	const char *name;
	unsigned int glyphcount;
	unsigned int *glyphs; //offset of glyph data
};

struct ButtonRecord {
	unsigned int flags; //matches with ButtonFlags
	unsigned int character;
	unsigned int depth;
	Transform rotateandscale;
	Vector2 translate;
	Rect color; ///always 1.0,1.0,1.0,1.0 as far as I can see
	Rect unknown; ///always zero as far as I can see
};

struct Triangle {
	unsigned short v1;
	unsigned short v2;
	unsigned short v3;
};

struct ActionString {
	unsigned int stringoffset;
	const char *string;
};

struct FunctionArgument {
	unsigned int reg;
	const char *name;
};

struct ActionDefineFunction2 {
	unsigned int definefunction2offset;
	unsigned int argumentcount;
	std::vector<FunctionArgument *> arguments;
};

struct ActionDefineFunction {
	unsigned int definefunctionoffset;
	unsigned int argumentcount;
	std::vector<const char *> arguments;
};

struct ActionPushData {
	unsigned int pushdataoffset;
	unsigned int pushdatacount;
	std::vector<unsigned int> pushdata;
};

struct ActionBytes {
	unsigned int actionbytecount;
	std::stringstream actionbytes;
	unsigned int constantcount;
	std::vector<unsigned int> constants;
	unsigned int stringcount;
	std::vector<ActionString *> actionstrings;
	unsigned int pushdatacount;
	std::vector<ActionPushData *> actionpushdatas;
	unsigned int definefunction2count;
	std::vector<ActionDefineFunction2 *> actiondefinefunction2s;
	unsigned int definefunctioncount;
	std::vector<ActionDefineFunction *> actiondefinefunctions;
};

struct Action : public FrameItem {
	ActionBytes ab;
};

struct OutputAction : public FrameItem {
	unsigned char *actionbytes;
};

struct ButtonAction {
	unsigned int flags; //matches with ButtonActionFlags;
	Action *actiondata;
};

struct OutputButtonAction {
	unsigned int flags; //matches with ButtonActionFlags;
	unsigned char *actiondata;
};

struct Button : public Character {
	unsigned int unknown; ///always zero as far as I can see
	Rect bounds;
	unsigned int trianglecount;
	unsigned int vertexcount;
	std::vector <Vector2 *> vertexes;
	std::vector <Triangle *> triangles;
	unsigned int recordcount;
	std::vector <ButtonRecord *> buttonrecords;
	unsigned int buttonactioncount;
	std::vector <ButtonAction *> buttonactionrecords;
	unsigned int unknown2; ///always zero as far as I can see
};

struct OutputButton : public Character {
	unsigned int unknown; //always zero as far as I can see
	Rect bounds;
	unsigned int trianglecount;
	unsigned int vertexcount;
	Vector2 *vertexes;
	Triangle *triangles;
	unsigned int recordcount;
	ButtonRecord *buttonrecords;
	unsigned int buttonactioncount;
	OutputButtonAction *buttonactionrecords;
	unsigned int unknown2; //always zero as far as I can see
};

struct Sprite : public Character {
	unsigned int framecount;
	std::vector<Frame *> frames;
	unsigned int pointer; //always zero, used at runtime
};

struct OutputSprite : public Character {
	unsigned int framecount;
	OutputFrame *frames; //offset of frame data
	unsigned int pointer; //always zero, used at runtime
};

struct Image : public Character {
	unsigned int texture;
};

struct Texture {
	unsigned int texture;
	bool hasuv;
	unsigned int top;
	unsigned int left;
	unsigned int bottom;
	unsigned int right;
};

struct D3DImage : public Character {
	Texture *texture;
};

struct Morph : public Character {
	unsigned int startshape;
	unsigned int endshape;
};

struct Import {
	const char *movie;
	const char *name;
	unsigned int character;
	unsigned int pointer; //always zero, used at runtime
};

struct Export {
	const char *name;
	unsigned int character;
};

struct Movie : public Character {
	unsigned int framecount;
	std::vector<Frame *> frames;
	unsigned int pointer; //always zero, used at runtime
	unsigned int charactercount;
	std::vector<Character *> characters;
	unsigned int screensizex;
	unsigned int screensizey;
	unsigned int unknown; ///always 33 as far as I can see
	unsigned int importcount;
	std::vector<Import *> imports;
	unsigned int exportcount;
	std::vector<Export *> exports;
	unsigned int count; //always zero, used at runtime
};

struct OutputMovie : public Character {
	unsigned int framecount;
	OutputFrame *frames; //offset of frame data
	unsigned int pointer; //always zero, used at runtime
	unsigned int charactercount;
	Character **characters; //offset of character data
	unsigned int screensizex;
	unsigned int screensizey;
	unsigned int unknown; //always 33 as far as I can see
	unsigned int importcount;
	Import *imports; //offset of imports data
	unsigned int exportcount;
	Export *exports; //offset of exports data
	unsigned int count; //always zero, used at runtime
};

struct Glyph {
	unsigned int index;
	int advance;
};

struct TextRecord {
	unsigned int font;
	Rect color;
	Rect unknown; ///always zero as far as I can see
	Vector2 offset;
	float textheight;
	unsigned int glyphcount;
	std::vector<Glyph *> glyphs;
};

struct OutputTextRecord {
	unsigned int font;
	Rect color;
	Rect unknown; //always zero as far as I can see
	Vector2 offset;
	float textheight;
	unsigned int glyphcount;
	Glyph *glyphs;
};

struct Text : public Character {
	Rect bounds;
	Transform rotateandscale;
	Vector2 translate;
	unsigned int recordcount;
	std::vector<TextRecord *> records;
};

struct OutputText : public Character {
	Rect bounds;
	Transform rotateandscale;
	Vector2 translate;
	unsigned int recordcount;
	OutputTextRecord *records;
};

struct FrameLabel : public FrameItem {
	const char *label;
	unsigned int flags; ///always 0x70000 as far as I can see
	unsigned int frame;
};

struct PlaceObjectAction {
	unsigned int flags;
	unsigned int flags2;
	Action *actiondata;
};

struct OutputPlaceObjectAction {
	unsigned int flags;
	unsigned int flags2;
	unsigned char *actiondata;
};

struct PlaceObjectActions {
	unsigned int clipactioncount;
	std::vector<PlaceObjectAction *> actions;
};

struct OutputPlaceObjectActions {
	unsigned int clipactioncount;
	OutputPlaceObjectAction *actions;
};

struct PlaceObject : public FrameItem {
	unsigned int flags; //matches with PlaceObjectFlags
	unsigned int depth;
	unsigned int character;
	Transform rotateandscale;
	Vector2 translate;
	unsigned int colortransform;
	unsigned int unknown; ///always zero as far as I can see
	float ratio;
	const char *name;
	unsigned int clipdepth;
	PlaceObjectActions *poa;
};

struct OutputPlaceObject : public FrameItem {
	unsigned int flags; //matches with PlaceObjectFlags
	unsigned int depth;
	unsigned int character;
	Transform rotateandscale;
	Vector2 translate;
	unsigned int colortransform;
	unsigned int unknown; ///always zero as far as I can see
	float ratio;
	const char *name;
	unsigned int clipdepth;
	OutputPlaceObjectActions *poa;
};

struct RemoveObject : public FrameItem {
	unsigned int depth;
};

struct BackgroundColor : public FrameItem {
	unsigned int color;
};

struct InitAction : public FrameItem {
	unsigned int sprite;
	ActionBytes ab;
};

struct OutputInitAction : public FrameItem {
	unsigned int sprite;
	unsigned char *actionbytes;
};

enum AptConstItemType {
	TYPE_UNDEF=0,
	TYPE_STRING=1,
	TYPE_NUMBER=4,
};

struct AptConstItem {
	AptConstItemType type;
	union {
		const char *strvalue;
		unsigned int numvalue;
	};
};

struct AptConstData {
	unsigned int aptdataoffset;
	unsigned int itemcount;
	std::vector<AptConstItem *> items;
};

void CC3TOOLS_DLL_EXPORT GenerateAptAptFile(Movie *m,const char *filename);
void CC3TOOLS_DLL_EXPORT GenerateAptConstFile(AptConstData *c,const char *filename);
/* 
use this for:
ACTION_END
ACTION_NEXTFRAME
ACTION_PREVFRAME
ACTION_PLAY
ACTION_STOP
ACTION_ADD
ACTION_SUBTRACT
ACTION_MULTIPLY
ACTION_DIVIDE
ACTION_EQUAL
ACTION_LESSTHAN
ACTION_LOGICALAND
ACTION_LOGICALOR
ACTION_LOGICALNOT
ACTION_STRINGEQ
ACTION_STRINGLENGTH
ACTION_SUBSTRING
ACTION_POP
ACTION_INT
ACTION_GETVARIABLE
ACTION_SETVARIABLE
ACTION_SETTARGETEXPRESSION
ACTION_STRINGCONCAT
ACTION_GETPROPERTY
ACTION_SETPROPERTY
ACTION_DUPLICATECLIP
ACTION_REMOVECLIP
ACTION_TRACE
ACTION_STARTDRAGMOVIE
ACTION_STOPDRAGMOVIE
ACTION_CASTOP
ACTION_IMPLEMENTSOP
ACTION_RANDOM
ACTION_CHR
ACTION_GETTIMER
ACTION_DELETE
ACTION_DELETE2
ACTION_DEFINELOCAL
ACTION_CALLFUNCTION
ACTION_RETURN
ACTION_MODULO
ACTION_NEW
ACTION_VAR
ACTION_INITARRAY
ACTION_INITOBJECT
ACTION_TYPEOF
ACTION_TARGETPATH
ACTION_ENUMERATE
ACTION_NEWADD
ACTION_NEWLESSTHAN
ACTION_NEWEQUALS
ACTION_TONUMBER
ACTION_TOSTRING
ACTION_DUP
ACTION_SWAP
ACTION_GETMEMBER
ACTION_SETMEMBER
ACTION_INCREMENT
ACTION_DECREMENT
ACTION_CALLMETHOD
ACTION_NEWMETHOD
ACTION_INSTANCEOF
ACTION_ENUM2
EA_PUSHZERO
EA_PUSHONE
EA_CALLFUNCTIONPOP
EA_CALLFUNCTION
EA_CALLMETHODPOP
EA_CALLMETHOD
ACTION_BITWISEAND
ACTION_BITWISEOR
ACTION_BITWISEXOR
ACTION_SHIFTLEFT
ACTION_SHIFTRIGHT
ACTION_STRICTEQ
ACTION_GREATER
ACTION_EXTENDS
EA_PUSHTHIS
EA_PUSHGLOBAL
EA_ZEROVARIABLE
EA_PUSHTRUE
EA_PUSHFALSE
EA_PUSHNULL
EA_PUSHUNDEFINED
ACTION_GETURL2
ACTION_CALLFRAME
*/
void CC3TOOLS_DLL_EXPORT AddAction(action_type action,ActionBytes *ab);
/*
use this for:
ACTION_GOTOFRAME
ACTION_SETREGISTER
ACTION_BRANCHALWAYS
ACTION_BRANCHIFTRUE
EA_BRANCHIFFALSE
ACTION_GOTOEXPRESSION
ACTION_WITH
*/
void CC3TOOLS_DLL_EXPORT AddIntAction(action_type action,ActionBytes *ab,int actionvalue);
/*
use this for:
ACTION_SETTARGET
ACTION_GOTOLABEL
EA_PUSHSTRING
EA_GETSTRINGVAR
EA_GETSTRINGMEMBER
EA_SETSTRINGVAR
EA_SETSTRINGMEMBER
*/
void CC3TOOLS_DLL_EXPORT AddStringAction(action_type action,ActionBytes *ab,const char *actionstring);
/*
use this for:
EA_PUSHCONSTANT
EA_PUSHVALUEOFVAR
EA_GETNAMEDMEMBER
EA_CALLNAMEDFUNCTIONPOP
EA_CALLNAMEDFUNCTION
EA_CALLNAMEDMETHODPOP
EA_CALLNAMEDMETHOD
EA_PUSHBYTE
EA_PUSHREGISTER
*/
void CC3TOOLS_DLL_EXPORT AddByteAction(action_type action,ActionBytes *ab,unsigned char number);
/*
use this for:
EA_PUSHSHORT
*/
void CC3TOOLS_DLL_EXPORT AddShortAction(action_type action,ActionBytes *ab,unsigned short number);
/*
use this for:
EA_PUSHFLOAT
*/
void CC3TOOLS_DLL_EXPORT AddFloatAction(action_type action,ActionBytes *ab,float number);
/*
use this for:
EA_PUSHLONG
*/
void CC3TOOLS_DLL_EXPORT AddLongAction(action_type action,ActionBytes *ab,unsigned long number);
/*
use this for:
ACTION_GETURL
*/
void CC3TOOLS_DLL_EXPORT AddURLAction(action_type action,ActionBytes *ab,const char *str1,const char *str2);
/*
use this for:
ACTION_CONSTANTPOOL
*/
void CC3TOOLS_DLL_EXPORT AddConstantPoolAction(action_type action,ActionBytes *ab,unsigned int constantcount);
/*
use this for:
ACTION_DEFINEFUNCTION2
*/
void CC3TOOLS_DLL_EXPORT AddDefineFunction2Action(action_type action,ActionBytes *ab,ActionDefineFunction2 *pd,unsigned int flags,unsigned int size,const char *name);
/*
use this for:
ACTION_DEFINEFUNCTION
*/
void CC3TOOLS_DLL_EXPORT AddDefineFunctionAction(action_type action,ActionBytes *ab,ActionDefineFunction *pd,unsigned int size,const char *name);
/*
use this for:
ACTION_PUSHDATA
*/
void CC3TOOLS_DLL_EXPORT AddPushDataAction(action_type action,ActionBytes *ab,ActionPushData *pd);
#endif // APT_FILE_H_INCLUDED
