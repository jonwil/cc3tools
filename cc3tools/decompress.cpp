/*
	Command & Conquer 3 Tools
	refpack decompression code
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
unsigned int swapbytes(const unsigned char *buffer,unsigned int count)
{
	int result = 0;
	switch (count)
	{
	case 1:
		result = buffer[0];
		break;
	case 2:
		result = buffer[0] << 8;
		result |= buffer[1];
		break;
	case 3:
		result = buffer[0] << 0x10;
		result |= buffer[1] << 8;
		result |= buffer[2];
		break;
	case 4:
		result = buffer[0] << 0x18;
		result |= buffer[1] << 0x10;
		result |= buffer[2] << 8;
		result |= buffer[3];
		break;
	}
	return result;
}

int GetUncompressedSize(const unsigned char *buffer)
{
	int compressedsize = 0;
	unsigned short headerflags = (unsigned short)swapbytes(buffer,2);
	if ((headerflags & 0x3EFF) == 0x10FB)
	{
		unsigned int x = headerflags & 0x8000;
		if (x)
		{
			x = 1;
		}
		x += 3;
		compressedsize = swapbytes(buffer + 2,x);
	}
	return compressedsize;
}

void DecompressData(unsigned char *inbuffer,unsigned char *outbuffer)
{
	unsigned char *inbuffer2 = inbuffer;
	unsigned char *outbuffer2 = outbuffer;
	unsigned int flags = 0;
	unsigned char code = 0;
	unsigned char code2 = 0;
	unsigned int count = 0;
	unsigned char *tempbuf = 0;
	unsigned char code3 = 0;
	unsigned char code4 = 0;
	flags = *inbuffer2;
	inbuffer2++;
	flags = (flags << 8) + *inbuffer2;
	inbuffer2++;
	if (flags & 0x8000)
	{
		if (flags & 0x100)
		{
			inbuffer2 += 4;
		}
		inbuffer2 += 4;
	}
	else
	{
		if (flags & 0x100)
		{
			inbuffer2 += 3;
		}
		inbuffer2 += 3;
	}
	while (true)
	{
		code = *inbuffer2;
		inbuffer2++;
		if (!(code & 0x80))
		{
			code2 = *inbuffer2;
			inbuffer2++;
			count = code & 3;
			for (unsigned int i = 0;i < count;i++)
			{
				*outbuffer2 = *inbuffer2;
				outbuffer2++;
				inbuffer2++;
			}
			tempbuf = (outbuffer2 - 1) - (code2 + (code & 0x60) * 8);
			count = (((code & 0x1C) / 4) + 2);
			for (unsigned int i = 0;i <= count;i++)
			{
				*outbuffer2 = *tempbuf;
				outbuffer2++;
				tempbuf++;
			}
		}
		else if (!(code & 0x40))
		{
			code2 = *inbuffer2;
			inbuffer2++;
			code3 = *inbuffer2;
			inbuffer2++;
			count = code2 >> 6;
			for (unsigned int i = 0;i < count;i++)
			{
				*outbuffer2 = *inbuffer2;
				outbuffer2++;
				inbuffer2++;
			}
			tempbuf = (outbuffer2 - 1) - (((code2 & 0x3F) << 8) + code3);
			count = (code & 0x3F) + 3;
			for (unsigned int i = 0;i <= count;i++)
			{
				*outbuffer2 = *tempbuf;
				outbuffer2++;
				tempbuf++;
			}
		}
		else if (!(code & 0x20))
		{
			code2 = *inbuffer2;
			inbuffer2++;
			code3 = *inbuffer2;
			inbuffer2++;
			code4 = *inbuffer2;
			inbuffer2++;
			count = code & 3;
			for (unsigned int i = 0;i < count;i++)
			{
				*outbuffer2 = *inbuffer2;
				outbuffer2++;
				inbuffer2++;
			}
			tempbuf = (outbuffer2 - 1) - ((((code & 0x10) >> 4) << 0x10) + (code2 << 8) + code3);
			count = (((code & 0x0C) >> 2) << 8) + code4 + 4;
			for (unsigned int i = 0;i <= count;i++)
			{
				*outbuffer2 = *tempbuf;
				outbuffer2++;
				tempbuf++;
			}
		}
		else
		{
			count = ((code & 0x1F) * 4) + 4;
			if (count <= 0x70)
			{
				for (unsigned int i = 0;i < count;i++)
				{
					*outbuffer2 = *inbuffer2;
					outbuffer2++;
					inbuffer2++;
				}
			}
			else
			{
				count = code & 3;
				for (unsigned int i = 0;i < count;i++)
				{
					*outbuffer2 = *inbuffer2;
					outbuffer2++;
					inbuffer2++;
				}
				return;
			}
		}
	}
}
