/*
	Command & Conquer 3 Tools
	big file read code
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
#include <string>
#pragma warning( disable : 6211 )
int GetUncompressedSize(const unsigned char *buffer);
void DecompressData(unsigned char *inbuffer,unsigned char *outbuffer);

inline unsigned int reverse(unsigned int v)
{
	_asm {
		mov eax, v
		xchg al, ah
		rol eax, 16
		xchg al, ah
	}
}

const char CC3TOOLS_DLL_EXPORT *BigGetFilename(BIGFile *bf, int entry)
{
	return bf->entries[entry].name;
}

BIGFileEntry CC3TOOLS_DLL_EXPORT *BigLoadEntry(BIGFile *bf, int entry)
{
	unsigned char *tmp;
	if(bf->entries[entry].fc)
	{
		return &bf->entries[entry];
	}
	bf->fc->Seek(bf->entries[entry].offset);
	tmp = new unsigned char[20];
	bf->fc->Read(tmp, 20);
	bf->entries[entry].un_size = bf->entries[entry].size;
	int x = GetUncompressedSize(tmp);
	if (x)
	{
		delete[] tmp;
		unsigned char *data = new unsigned char[x];
		tmp = new unsigned char[bf->entries[entry].size];
		bf->fc->Seek(bf->entries[entry].offset);
		bf->fc->Read(tmp,bf->entries[entry].size);
		DecompressData(tmp,data);
		delete[] tmp;
		bf->entries[entry].fc = new MemFileClass(data,x);
		bf->entries[entry].un_size = x;
	}
	else if (bf->entries[entry].size < 100000000)
	{
		delete[] tmp;
		tmp = new unsigned char[bf->entries[entry].size];
		bf->fc->Seek(bf->entries[entry].offset);
		bf->fc->Read(tmp,bf->entries[entry].size);
		bf->entries[entry].fc = new MemFileClass(tmp,bf->entries[entry].size);
	}
	else
	{
		delete[] tmp;
		bf->entries[entry].fc = new DiskFileClass(bf->filename,bf->entries[entry].offset);
	}
	return &bf->entries[entry];
}

void CC3TOOLS_DLL_EXPORT BigClose(BIGFile *bf)
{
	for(unsigned int i=0; i<bf->num_files; i++)
	{
		if (bf->entries[i].fc)
		{
			delete[] bf->entries[i].fc;
		}
		delete[] bf->entries[i].name;
	}
	delete[] bf->entries;
	delete[] bf->filename;
	delete bf;
}

BIGFileEntry CC3TOOLS_DLL_EXPORT *BigLoadEntryByName(BIGFile *bf, const char *name)
{
	for(unsigned int i=0; i<bf->num_files; i++)
	{
		if(_stricmp(name, bf->entries[i].name) == 0)
		{
			return BigLoadEntry(bf, i);
		}
	}
	return NULL;
}

unsigned int CC3TOOLS_DLL_EXPORT BigGetNumElements(BIGFile *bf)
{
	return bf->num_files;
}

BIGFile CC3TOOLS_DLL_EXPORT *BigOpen(const char *filename)
{
	BIGFile *bf = NULL;
	try 
	{
		bf = new BIGFile();
		if (bf==NULL)
		{
			return NULL;
		}
		bf->filename = _strdup(filename);
		bf->fc = new DiskFileClass(filename);
		bf->fc->Read(&bf->magic, 4);
		if (reverse(bf->magic) != 0x42494734)
		{
			delete bf->fc;
			delete bf;
			return NULL;
		}
		bf->fc->Seek(0x8);
		bf->fc->Read(&bf->num_files, 4);
		bf->num_files = reverse(bf->num_files);
		bf->entries = new BIGFileEntry[bf->num_files];
		bf->fc->Seek(0x10);
		for (unsigned int i=0; i<bf->num_files; i++)
		{
			bf->fc->Read(&bf->entries[i].offset, 4);
			bf->entries[i].offset = reverse(bf->entries[i].offset);
			bf->fc->Read(&bf->entries[i].size, 4);
			bf->entries[i].size = reverse(bf->entries[i].size);
			char n[2];
			n[1] = 0x00;
			bf->fc->Read(n, 1);
			std::string s;
			while (*n != 0x00)
			{
				s.append(n);
				bf->fc->Read(n, 1);
			}
			bf->entries[i].name = _strdup(s.c_str());
			bf->entries[i].container = bf;
			bf->entries[i].fc = 0;
		}
	}
	catch (std::bad_alloc &)
	{
		if (bf)
		{
			delete bf;
			return NULL;
		}
	}
	return bf;
}
