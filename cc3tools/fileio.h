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

class FileClass
{
public:
	virtual ~FileClass() {};
	virtual DWORD Size() = 0;
	virtual void Seek(LONG location) = 0;
	virtual void Read(void *buffer,int size) = 0;
	virtual unsigned char *ReadAll() = 0;
	virtual bool Is_Present() = 0;
};

class DiskFileClass : public FileClass
{
	HANDLE file;
	LONG offset;
public:
	DiskFileClass(const char *filename,LONG offset = 0);
	virtual ~DiskFileClass();
	DWORD Size();
	void Seek(LONG location);
	void Read(void *buffer,int size);
	unsigned char *ReadAll();
	bool Is_Present()
	{
		if (file)
		{
			return true;
		}
		return false;
	}
};

class MemFileClass : public FileClass
{
	unsigned char *buffer;
	unsigned char *currentpos;
	unsigned int size;
public:
	MemFileClass(unsigned char *buffer,unsigned int size);
	virtual ~MemFileClass();
	DWORD Size();
	void Seek(LONG location);
	void Read(void *buffer,int size);
	unsigned char *ReadAll();
	bool Is_Present()
	{
		if (buffer)
		{
			return true;
		}
		return false;
	}
};
