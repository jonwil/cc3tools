/*
	Command & Conquer 3 Tools
	file i/o classes
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

DiskFileClass::DiskFileClass(const char *filename,LONG off)
{
	file = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (file)
	{
		SetFilePointer(file,off,NULL,FILE_BEGIN);
		offset = off;
	}
}

DiskFileClass::~DiskFileClass()
{
	if (file)
	{
		CloseHandle(file);
	}
}

DWORD DiskFileClass::Size()
{
	if (file)
	{
		return GetFileSize(file,NULL);
	}
	return 0;
}

void DiskFileClass::Seek(LONG location)
{
	if (file)
	{
		SetFilePointer(file,location,NULL,FILE_BEGIN);
	}
}

void DiskFileClass::Read(void *buffer,int size)
{
	if (file)
	{
		DWORD i;
		if (!ReadFile(file,buffer,size,&i,NULL))
		{
			CloseHandle(file);
		}
	}
}

MemFileClass::MemFileClass(unsigned char *buf,unsigned int sz)
{
	buffer = buf;
	size = sz;
	currentpos = buffer;
}

MemFileClass::~MemFileClass()
{
	delete[] buffer;
}

DWORD MemFileClass::Size()
{
	return size;
}

void MemFileClass::Seek(LONG location)
{
	currentpos = buffer + location;
}

void MemFileClass::Read(void *buf,int sz)
{
	memcpy(buf,currentpos,sz);
	currentpos += sz;
}

unsigned char *DiskFileClass::ReadAll()
{
	unsigned char *buf = new unsigned char[Size()];
	Read(buf,Size());
	return buf;
}

unsigned char *MemFileClass::ReadAll()
{
	unsigned char *buf = new unsigned char[size];
	memcpy(buf,buffer,size);
	return buf;
}
