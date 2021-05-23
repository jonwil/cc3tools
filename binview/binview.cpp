/*
	Command & Conquer 3 Tools
	Bin viewer tool
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
#include "resource.h"
#include <windowsx.h>
#include <shlobj.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "w3x_file.h"
#include "Bitstream.h"
#include <deque>
#include "MpegGenerator.h"
#include "ParserVersion6.h"
#include "PcmOutputStream.h"

char *cmdline;
HINSTANCE hInst;
HACCEL accel;
HHOOK hook;
HWND hdlg;
Manifest *mf;
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
char outputfolder[MAX_PATH];
unsigned int nv = 0;
unsigned int pc = 0;

#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#define SAFE_DELETE(p) { if (p) { delete (p); (p)=NULL; } }
#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2
#define DXUT_ERR(str,hr) (hr)

static const int ea_adpcm_table[] = {
	0, 240, 460, 392, 0, 0, -208, -220, 0, 1,
	3, 4, 7, 8, 10, 11, 0, -1, -3, -4
};

class CWaveFile
{
public:
	WAVEFORMATEX* m_pwfx;
	HMMIO m_hmmio;
	MMCKINFO m_ck;
	MMCKINFO m_ckRiff;
	DWORD m_dwSize;
	MMIOINFO m_mmioinfoOut;
	DWORD m_dwFlags;
	BOOL m_bIsReadingFromMemory;
	BYTE* m_pbData;
	BYTE* m_pbDataCur;
	ULONG m_ulDataSize;
	CHAR* m_pResourceBuffer;
protected:
	HRESULT ReadMMIO();
	HRESULT WriteMMIO( WAVEFORMATEX *pwfxDest );
public:
	CWaveFile();
	~CWaveFile();
	HRESULT Open( LPSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
	HRESULT OpenFromMemory( BYTE* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, DWORD dwFlags );
	HRESULT Close();
	HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
	HRESULT Write( UINT nSizeToWrite, BYTE* pbData, UINT* pnSizeWrote );
	DWORD   GetSize();
	HRESULT ResetFile();
	WAVEFORMATEX* GetFormat() { return m_pwfx; };
};

CWaveFile::CWaveFile()
{
	m_pwfx = NULL;
	m_hmmio = NULL;
	m_pResourceBuffer = NULL;
	m_dwSize = 0;
	m_bIsReadingFromMemory = FALSE;
}

CWaveFile::~CWaveFile()
{
	Close();
	if( !m_bIsReadingFromMemory )
	{
		SAFE_DELETE_ARRAY( m_pwfx );
	}
}

HRESULT CWaveFile::Open( LPSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
	HRESULT hr;
	m_dwFlags = dwFlags;
	m_bIsReadingFromMemory = FALSE;
	if( m_dwFlags == WAVEFILE_READ )
	{
		if( strFileName == NULL )
		{
			return E_INVALIDARG;
		}
		SAFE_DELETE_ARRAY( m_pwfx );
		m_hmmio = mmioOpen( strFileName, NULL, MMIO_ALLOCBUF | MMIO_READ );
		if( NULL == m_hmmio )
		{
			HRSRC hResInfo;
			HGLOBAL hResData;
			DWORD dwSize;
			VOID* pvRes;
			if( NULL == ( hResInfo = FindResource( NULL, strFileName, "WAVE" ) ) )
			{
				if( NULL == ( hResInfo = FindResource( NULL, strFileName, "WAV" ) ) )
				{
					return DXUT_ERR( "FindResource", E_FAIL );
				}
			}
			if( NULL == ( hResData = LoadResource( GetModuleHandle(NULL), hResInfo ) ) )
			{
				return DXUT_ERR( "LoadResource", E_FAIL );
			}
			if( 0 == ( dwSize = SizeofResource( GetModuleHandle(NULL), hResInfo ) ) )
			{
				return DXUT_ERR( "SizeofResource", E_FAIL );
			}
			if( NULL == ( pvRes = LockResource( hResData ) ) )
			{
				return DXUT_ERR( "LockResource", E_FAIL );
			}
			m_pResourceBuffer = new CHAR[ dwSize ];
			if( m_pResourceBuffer == NULL )
			{
				return DXUT_ERR( "new", E_OUTOFMEMORY );
			}
			memcpy( m_pResourceBuffer, pvRes, dwSize );
			MMIOINFO mmioInfo;
			ZeroMemory( &mmioInfo, sizeof(mmioInfo) );
			mmioInfo.fccIOProc = FOURCC_MEM;
			mmioInfo.cchBuffer = dwSize;
			mmioInfo.pchBuffer = (CHAR*) m_pResourceBuffer;
			m_hmmio = mmioOpen( NULL, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ );
		}
		if( FAILED( hr = ReadMMIO() ) )
		{
			mmioClose( m_hmmio, 0 );
			return DXUT_ERR( "ReadMMIO", hr );
		}
		if( FAILED( hr = ResetFile() ) )
		{
			return DXUT_ERR( "ResetFile", hr );
		}
		m_dwSize = m_ck.cksize;
	}
	else
	{
		m_hmmio = mmioOpen( strFileName, NULL, MMIO_ALLOCBUF  |
												MMIO_READWRITE |
												MMIO_CREATE );
		if( NULL == m_hmmio )
		{
			return DXUT_ERR( "mmioOpen", E_FAIL );
		}
		if( FAILED( hr = WriteMMIO( pwfx ) ) )
		{
			mmioClose( m_hmmio, 0 );
			return DXUT_ERR( "WriteMMIO", hr );
		}
		if( FAILED( hr = ResetFile() ) )
		{
			return DXUT_ERR( "ResetFile", hr );
		}
	}
	return hr;
}

HRESULT CWaveFile::OpenFromMemory( BYTE* pbData, ULONG ulDataSize,
								WAVEFORMATEX* pwfx, DWORD dwFlags )
{
	m_pwfx = pwfx;
	m_ulDataSize = ulDataSize;
	m_pbData = pbData;
	m_pbDataCur = m_pbData;
	m_bIsReadingFromMemory = TRUE;
	if( dwFlags != WAVEFILE_READ )
	{
		return E_NOTIMPL;
	}
	return S_OK;
}

HRESULT CWaveFile::ReadMMIO()
{
	MMCKINFO ckIn;
	PCMWAVEFORMAT pcmWaveFormat;
	m_pwfx = NULL;
	if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
	{
		return DXUT_ERR( "mmioDescend", E_FAIL );
	}
	if( (m_ckRiff.ckid != FOURCC_RIFF) ||
		(m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
	{
		return DXUT_ERR( "mmioFOURCC", E_FAIL );
	}
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
	{
		return DXUT_ERR( "mmioDescend", E_FAIL );
	}
	if( ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT) )
	{
		return DXUT_ERR( "sizeof(PCMWAVEFORMAT)", E_FAIL );
	}
	if( mmioRead( m_hmmio, (HPSTR) &pcmWaveFormat,
				sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
	{
		return DXUT_ERR( "mmioRead", E_FAIL );
	}
	if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
	{
		m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
		if( NULL == m_pwfx )
		{
			return DXUT_ERR( "m_pwfx", E_FAIL );
		}
		memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
		m_pwfx->cbSize = 0;
	}
	else
	{
		WORD cbExtraBytes = 0L;
		if( mmioRead( m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
		{
			return DXUT_ERR( "mmioRead", E_FAIL );
		}
		m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) + cbExtraBytes ];
		if( NULL == m_pwfx )
		{
			return DXUT_ERR( "new", E_FAIL );
		}
		memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
		m_pwfx->cbSize = cbExtraBytes;
		if( mmioRead( m_hmmio, (CHAR*)(((BYTE*)&(m_pwfx->cbSize))+sizeof(WORD)),
					  cbExtraBytes ) != cbExtraBytes )
		{
			SAFE_DELETE( m_pwfx );
			return DXUT_ERR( "mmioRead", E_FAIL );
		}
	}
	if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
	{
		SAFE_DELETE( m_pwfx );
		return DXUT_ERR( "mmioAscend", E_FAIL );
	}
	return S_OK;
}

DWORD CWaveFile::GetSize()
{
	return m_dwSize;
}

HRESULT CWaveFile::ResetFile()
{
	if( m_bIsReadingFromMemory )
	{
		m_pbDataCur = m_pbData;
	}
	else
	{
		if( m_hmmio == NULL )
		{
			return CO_E_NOTINITIALIZED;
		}
		if( m_dwFlags == WAVEFILE_READ )
		{
			if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC),
							SEEK_SET ) )
			{
				return DXUT_ERR( "mmioSeek", E_FAIL );
			}
			m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
			if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
			{
				return DXUT_ERR( "mmioDescend", E_FAIL );
			}
		}
		else
		{
			m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
			m_ck.cksize = 0;
			if( 0 != mmioCreateChunk( m_hmmio, &m_ck, 0 ) )
			{
				return DXUT_ERR( "mmioCreateChunk", E_FAIL );
			}
			if( 0 != mmioGetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
			{
				return DXUT_ERR( "mmioGetInfo", E_FAIL );
			}
		}
	}
	return S_OK;
}

HRESULT CWaveFile::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
	if( m_bIsReadingFromMemory )
	{
		if( m_pbDataCur == NULL )
		{
			return CO_E_NOTINITIALIZED;
		}
		if( pdwSizeRead != NULL )
		{
			*pdwSizeRead = 0;
		}
		if( (BYTE*)(m_pbDataCur + dwSizeToRead) >
			(BYTE*)(m_pbData + m_ulDataSize) )
		{
			dwSizeToRead = m_ulDataSize - (DWORD)(m_pbDataCur - m_pbData);
		}
		CopyMemory( pBuffer, m_pbDataCur, dwSizeToRead );
		if( pdwSizeRead != NULL )
		{
			*pdwSizeRead = dwSizeToRead;
		}
		return S_OK;
	}
	else
	{
		MMIOINFO mmioinfoIn;
		if( m_hmmio == NULL )
		{
			return CO_E_NOTINITIALIZED;
		}
		if( pBuffer == NULL || pdwSizeRead == NULL )
		{
			return E_INVALIDARG;
		}
		if( pdwSizeRead != NULL )
		{
			*pdwSizeRead = 0;
		}
		if( 0 != mmioGetInfo( m_hmmio, &mmioinfoIn, 0 ) )
		{
			return DXUT_ERR( "mmioGetInfo", E_FAIL );
		}
		UINT cbDataIn = dwSizeToRead;
		if( cbDataIn > m_ck.cksize )
		{
			cbDataIn = m_ck.cksize;
		}
		m_ck.cksize -= cbDataIn;
		for( DWORD cT = 0; cT < cbDataIn; cT++ )
		{
			if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
			{
				if( 0 != mmioAdvance( m_hmmio, &mmioinfoIn, MMIO_READ ) )
				{
					return DXUT_ERR( "mmioAdvance", E_FAIL );
				}
				if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
				{
					return DXUT_ERR( "mmioinfoIn.pchNext", E_FAIL );
				}
			}
			*((BYTE*)pBuffer+cT) = *((BYTE*)mmioinfoIn.pchNext);
			mmioinfoIn.pchNext++;
		}
		if( 0 != mmioSetInfo( m_hmmio, &mmioinfoIn, 0 ) )
		{
			return DXUT_ERR( "mmioSetInfo", E_FAIL );
		}
		if( pdwSizeRead != NULL )
		{
			*pdwSizeRead = cbDataIn;
		}
		return S_OK;
	}
}

HRESULT CWaveFile::Close()
{
	if( m_dwFlags == WAVEFILE_READ )
	{
		mmioClose( m_hmmio, 0 );
		m_hmmio = NULL;
		SAFE_DELETE_ARRAY( m_pResourceBuffer );
	}
	else
	{
		m_mmioinfoOut.dwFlags |= MMIO_DIRTY;
		if( m_hmmio == NULL )
		{
			return CO_E_NOTINITIALIZED;
		}
		if( 0 != mmioSetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
		{
			return DXUT_ERR( "mmioSetInfo", E_FAIL );
		}
		if( 0 != mmioAscend( m_hmmio, &m_ck, 0 ) )
		{
			return DXUT_ERR( "mmioAscend", E_FAIL );
		}
		if( 0 != mmioAscend( m_hmmio, &m_ckRiff, 0 ) )
		{
			return DXUT_ERR( "mmioAscend", E_FAIL );
		}
		mmioSeek( m_hmmio, 0, SEEK_SET );
		if( 0 != (INT)mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) )
		{
			return DXUT_ERR( "mmioDescend", E_FAIL );
		}
		m_ck.ckid = mmioFOURCC('f', 'a', 'c', 't');
		if( 0 == mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
		{
			DWORD dwSamples = 0;
			mmioWrite( m_hmmio, (HPSTR)&dwSamples, sizeof(DWORD) );
			mmioAscend( m_hmmio, &m_ck, 0 );
		}
		if( 0 != mmioAscend( m_hmmio, &m_ckRiff, 0 ) )
		{
			return DXUT_ERR( "mmioAscend", E_FAIL );
		}
		mmioClose( m_hmmio, 0 );
		m_hmmio = NULL;
	}
	return S_OK;
}

HRESULT CWaveFile::WriteMMIO( WAVEFORMATEX *pwfxDest )
{
	m_ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	m_ckRiff.cksize = 0;
	if( 0 != mmioCreateChunk( m_hmmio, &m_ckRiff, MMIO_CREATERIFF ) )
	{
		return DXUT_ERR( "mmioCreateChunk", E_FAIL );
	}
	m_ck.ckid = mmioFOURCC('f', 'm', 't', ' ');
	m_ck.cksize = sizeof(PCMWAVEFORMAT);
	if( 0 != mmioCreateChunk( m_hmmio, &m_ck, 0 ) )
	{
		return DXUT_ERR( "mmioCreateChunk", E_FAIL );
	}
	if( pwfxDest->wFormatTag == WAVE_FORMAT_PCM )
	{
		if( mmioWrite( m_hmmio, (HPSTR) pwfxDest,
					   sizeof(PCMWAVEFORMAT)) != sizeof(PCMWAVEFORMAT))
		{
			return DXUT_ERR( "mmioWrite", E_FAIL );
		}
	}
	else
	{
		if( (UINT)mmioWrite( m_hmmio, (HPSTR) pwfxDest,
							 sizeof(*pwfxDest) + pwfxDest->cbSize ) !=
							 ( sizeof(*pwfxDest) + pwfxDest->cbSize ) )
		{
			return DXUT_ERR( "mmioWrite", E_FAIL );
		}
	}
	if( 0 != mmioAscend( m_hmmio, &m_ck, 0 ) )
	{
		return DXUT_ERR( "mmioAscend", E_FAIL );
	}
	return S_OK;
}

HRESULT CWaveFile::Write( UINT nSizeToWrite, BYTE* pbSrcData, UINT* pnSizeWrote )
{
	UINT cT;
	if( m_bIsReadingFromMemory )
	{
		return E_NOTIMPL;
	}
	if( m_hmmio == NULL )
	{
		return CO_E_NOTINITIALIZED;
	}
	if( pnSizeWrote == NULL || pbSrcData == NULL )
	{
		return E_INVALIDARG;
	}
	*pnSizeWrote = 0;
	for( cT = 0; cT < nSizeToWrite; cT++ )
	{
		if( m_mmioinfoOut.pchNext == m_mmioinfoOut.pchEndWrite )
		{
			m_mmioinfoOut.dwFlags |= MMIO_DIRTY;
			if( 0 != mmioAdvance( m_hmmio, &m_mmioinfoOut, MMIO_WRITE ) )
			{
				return DXUT_ERR( "mmioAdvance", E_FAIL );
			}
		}
		*((BYTE*)m_mmioinfoOut.pchNext) = *((BYTE*)pbSrcData+cT);
		(BYTE*)m_mmioinfoOut.pchNext++;
		(*pnSizeWrote)++;
	}
	return S_OK;
}

struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	FLOAT rhw;
	DWORD color;
	FLOAT tu, tv;   // The texture coordinates
};

struct AudioFileRuntime {
	unsigned int zeros;
	unsigned int subtitlelen;
	char *subtitle;
	unsigned int numberofsamples;
	unsigned int samplerate;
	void *headerdata;
	unsigned int headerdatasize;
	unsigned char numberofchannels;
};


struct AudioFileRuntime2 {
	unsigned int zeros;
	unsigned int subtitlelen;
	char *subtitle;
	unsigned int numberofsamples;
	unsigned int samplerate;
	void *headerdata;
	unsigned int headerdatasize;
	int unk1;
	int unk2;
	unsigned char numberofchannels;
};

short av_clip_int16(int a)
{
	if ((a + 32768) & ~65535)
	{
		return (short)((a>>31) ^ 32767);
	}
	else
	{
		return (short)a;
	}
}

IDirect3DVertexDeclaration9 *decl;
IDirect3DTexture9 *tex;
LPDIRECT3DVERTEXBUFFER9 m_pVB;
LPDIRECT3DINDEXBUFFER9 m_pIB;
LRESULT CALLBACK AccelHook(int code, WPARAM wParam, LPARAM lParam)
{
	if (code == MSGF_DIALOGBOX)
	{
		MSG* msg = (MSG*)lParam;
		if (msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST)
		{
			if (TranslateAccelerator(hdlg, accel, msg))
			{
				return true;
			}
		}
		return CallNextHookEx(hook, code, wParam, lParam);
	}
	return false;
}

BOOL CALLBACK ViewWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
	case WM_PAINT:
		{
			d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0,0,0,255), 0.0f, 0);
			d3ddev->BeginScene();
			if (tex && m_pVB && decl)
			{
				d3ddev->SetTexture(0, tex);
				d3ddev->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX));
				d3ddev->SetVertexDeclaration(decl);
				if (m_pIB)
				{
					d3ddev->SetIndices(m_pIB);
					d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,nv,0,pc);
				}
				else
				{
					d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,0,pc);
				}
				d3ddev->SetTexture(0, NULL);
				d3ddev->SetStreamSource(0,0,0,0);
				d3ddev->SetIndices(0);
			}
			d3ddev->EndScene();
			d3ddev->Present(0,0,0,0);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void ParseADPCM(CWaveFile *f,unsigned char *block,unsigned int channels)
{
	int coeff1[4], coeff2[4], shift[4];
	short *samples = new short[128*channels*2];
	for(unsigned int x = 0;x < channels;x++)
	{
		for (unsigned int n = 0;n < 4;n++) 
		{
			coeff1[n] = ea_adpcm_table[(block[0]&0x0F)];
			coeff2[n] = ea_adpcm_table[(block[0]&0x0F)+4];
			shift[n] = (block[2]&0x0F) + 8;
#define SMP(offset)   samples[ ( n*32 + (offset) )*channels + x ]
			SMP(0) = ((block[1]<<24)>>16) + (block[0]&0xF0);
			SMP(1) = ((block[3]<<24)>>16) + (block[2]&0xF0);
#undef SMP
			block += 4;
		}
		for (unsigned int m = 0;m < 15;m++)
		{
			for (unsigned int n = 0; n < 4; n++)
			{
#define SMP(offset)   samples[ ( n*32 + (m+1)*2 + (offset) )*channels + x ]
				int level1, level2;
				level1 = ((block[n] & 0xF0) << 24) >> shift[n];
				level2 = ((block[n] & 0x0F) << 28) >> shift[n];
				SMP(0) = av_clip_int16((level1 + SMP(-1)*coeff1[n] + SMP(-2)*coeff2[n] + 0x80) >> 8);
				SMP(1) = av_clip_int16((level2 + SMP( 0)*coeff1[n] + SMP(-1)*coeff2[n] + 0x80) >> 8);
#undef SMP
			}
			block += 4;
		}
	}
	unsigned int sz;
	f->Write(128*channels*2,(BYTE *)samples,&sz);
}

void ParseRawBlock(CWaveFile *f,unsigned char *block,unsigned int channels,unsigned int numsamples)
{
	for (unsigned int i = 0;i < channels * numsamples;i++)
	{
		signed short sample = *(signed short *)block;
		if (sample < 0)
		{
			sample++;
		}
		sample = sample << 8;
		unsigned int sz;
		f->Write(2,(BYTE *)&sample,&sz);
		block += 2;
	}
}

void ParseADPCMBlock(CWaveFile *f,unsigned char *block,unsigned int channels,unsigned int numsamples)
{
	for (unsigned int sc = 0;sc < numsamples;sc += 0x80)
	{
		ParseADPCM(f,block,channels);
		block += 0x4C * channels;
	}
}

inline void Swap(unsigned short& Value)
{
	Value = (Value & 0xFF00) >> 8 | (Value & 0x00FF) << 8;
	return;
}

inline void Swap(unsigned int& Value)
{
	Value = (Value & 0xFF000000) >> 24 | (Value & 0x00FF0000) >> 8 |
		(Value & 0x000000FF) << 24 | (Value & 0x0000FF00) << 8;
	return;
}

void ParseADPCMStream(CWaveFile *f,unsigned char *block,unsigned int channels,unsigned int numsamples)
{
	unsigned int sc = 0;
	do
	{
		unsigned int size = *((unsigned int *)block);
		Swap(size);
		block += 4;
		unsigned int samps = *((unsigned int *)block);
		Swap(samps);
		block += 4;
		size -= 8;
		ParseADPCMBlock(f,block,channels,samps);
		block += size;
		sc += samps;
	} while (sc < numsamples);
}

void ParseRawStream(CWaveFile *f,unsigned char *block,unsigned int channels,unsigned int numsamples)
{
	unsigned int sc = 0;
	do
	{
		unsigned int size = *((unsigned int *)block);
		Swap(size);
		block += 4;
		unsigned int samps = *((unsigned int *)block);
		Swap(samps);
		block += 4;
		size -= 8;
		ParseRawBlock(f,block,channels,samps);
		block += size;
		sc += samps;
	} while (sc < numsamples);
}

void ParseEL31Block(CWaveFile *f,unsigned char *block,unsigned int channels,unsigned int numsamples,unsigned int size)
{
	elMpegGenerator Gen;
	elBlock FirstBlock;
	shared_array<uint8_t> Data(new uint8_t[size]);
	for (unsigned int i = 0;i < size;i++)
	{
		Data[i] = block[i];
	}
	FirstBlock.Data = Data;
	FirstBlock.SampleCount = numsamples;
	FirstBlock.Size = size;
	Gen.Initialize(FirstBlock, make_shared<elParser>());
	Gen.ParseBlock(FirstBlock);
	Gen.DoneParsingBlocks();
	std::vector< shared_ptr<elPcmOutputStream> > Streams;
	unsigned int ChannelCount = 0;
	for (unsigned int i = 0; i < Gen.GetStreamCount(); i++)
	{
		Streams.push_back(Gen.CreatePcmStream(i));
		ChannelCount += Gen.GetChannels(i);
	}
	if (!Streams.size())
	{
		return;
	}
	shared_array<short> ReadBuffer(new short[ChannelCount * elPcmOutputStream::RecommendBufferSize()]);
	const unsigned int PcmBufferSamples = elPcmOutputStream::RecommendBufferSize();
	shared_array<short> PcmBuffer(new short[PcmBufferSamples]);
	while (!Streams[0]->Eos())
	{
		unsigned int Ch = 0;
		unsigned int Frames = 0;
		for (unsigned int i = 0; i < Gen.GetStreamCount(); i++)
		{
			unsigned int Read;
			Read = Streams[i]->Read(PcmBuffer.get(), PcmBufferSamples);
			Frames = Read / Streams[i]->GetChannels();
			unsigned int OldCh = Ch;
			for (unsigned int j = 0; j < Frames; j++)
			{
				Ch = OldCh;
				for (unsigned int k = 0; k < Streams[i]->GetChannels(); k++)
				{
					ReadBuffer[j * ChannelCount + Ch] = PcmBuffer[j * Streams[i]->GetChannels() + k];
					Ch++;
				}
			}
		}
		unsigned int sz;
		f->Write(Frames * ChannelCount * sizeof(short),(BYTE *) ReadBuffer.get(),&sz);
	}
}

void ParseEL31Stream(CWaveFile *f,unsigned char *block,unsigned int channels,unsigned int numsamples,unsigned int size)
{
	unsigned int sz = 0;
	elMpegGenerator Gen;
	elBlock FirstBlock;
	bool LastPacket = false;
    uint16_t Flags = *(uint16_t*)block;
	block += sizeof(uint16_t);
	sz += sizeof(uint16_t);
    uint16_t BlockSize = *(uint16_t*)block;
	block += sizeof(uint16_t);
	sz += sizeof(uint16_t);
    uint32_t Samples = *(uint32_t*)block;
	block += sizeof(uint32_t);
	sz += sizeof(uint32_t);
    Swap(Flags);
    Swap(BlockSize);
    Swap(Samples);
	BlockSize -= 8;
	shared_array<uint8_t> Data(new uint8_t[BlockSize]);
	for (unsigned int i = 0;i < BlockSize;i++)
	{
		Data[i] = block[i];
	}
	block += BlockSize;
	sz += BlockSize;
	FirstBlock.Clear();
	FirstBlock.Data = Data;
	FirstBlock.SampleCount = Samples;
	FirstBlock.Size = BlockSize;
    if (Flags & 0x8000)
    {
        LastPacket = true;
    }
	Gen.Initialize(FirstBlock, make_shared<elParser>());
	Gen.ParseBlock(FirstBlock);
	bool x = true;
    while (x)
    {
        elBlock Block;
        if (LastPacket || sz == size)
        {
            break;
        }
	    uint16_t Flags2 = *(uint16_t*)block;
		block += sizeof(uint16_t);
		sz += sizeof(uint16_t);
	    uint16_t BlockSize2 = *(uint16_t*)block;
		block += sizeof(uint16_t);
		sz += sizeof(uint16_t);
	    uint32_t Samples2 = *(uint32_t*)block;
		block += sizeof(uint32_t);
		sz += sizeof(uint32_t);
        if (sz == size)
        {
            break;
        }
	    Swap(Flags2);
	    Swap(BlockSize2);
	    Swap(Samples2);
		BlockSize2 -= 8;
		shared_array<uint8_t> Datax(new uint8_t[BlockSize2]);
		for (unsigned int i = 0;i < BlockSize2;i++)
		{
			Datax[i] = block[i];
		}
		block += BlockSize2;
		sz += BlockSize2;
		Block.Clear();
		Block.Data = Datax;
		Block.SampleCount = Samples2;
		Block.Size = BlockSize2;
		if (Flags2 & 0x8000)
		{
			LastPacket = true;
	    }
        Gen.ParseBlock(Block);
    }
	Gen.DoneParsingBlocks();
	std::vector< shared_ptr<elPcmOutputStream> > Streams;
	unsigned int ChannelCount = 0;
	for (unsigned int i = 0; i < Gen.GetStreamCount(); i++)
	{
		Streams.push_back(Gen.CreatePcmStream(i));
		ChannelCount += Gen.GetChannels(i);
	}
	if (!Streams.size())
	{
		return;
	}
	shared_array<short> ReadBuffer(new short[ChannelCount * elPcmOutputStream::RecommendBufferSize()]);
	const unsigned int PcmBufferSamples = elPcmOutputStream::RecommendBufferSize();
	shared_array<short> PcmBuffer(new short[PcmBufferSamples]);
	while (!Streams[0]->Eos())
	{
		unsigned int Ch = 0;
		unsigned int Frames = 0;
		for (unsigned int i = 0; i < Gen.GetStreamCount(); i++)
		{
			unsigned int Read;
			Read = Streams[i]->Read(PcmBuffer.get(), PcmBufferSamples);
			Frames = Read / Streams[i]->GetChannels();
			unsigned int OldCh = Ch;
			for (unsigned int j = 0; j < Frames; j++)
			{
				Ch = OldCh;
				for (unsigned int k = 0; k < Streams[i]->GetChannels(); k++)
				{
					ReadBuffer[j * ChannelCount + Ch] = PcmBuffer[j * Streams[i]->GetChannels() + k];
					Ch++;
				}
			}
		}
		unsigned int szx;
		f->Write(Frames * ChannelCount * sizeof(short),(BYTE *) ReadBuffer.get(),&szx);
	}
}

void ParseL32PBlock(CWaveFile *f,unsigned char *block,unsigned int channels,unsigned int numsamples,unsigned int size)
{
	elMpegGenerator Gen;
	elBlock FirstBlock;
	shared_array<uint8_t> Data(new uint8_t[size]);
	for (unsigned int i = 0;i < size;i++)
	{
		Data[i] = block[i];
	}
	FirstBlock.Data = Data;
	FirstBlock.SampleCount = numsamples;
	FirstBlock.Size = size;
	Gen.Initialize(FirstBlock, make_shared<elParserVersion6>());
	Gen.ParseBlock(FirstBlock);
	Gen.DoneParsingBlocks();
	std::vector< shared_ptr<elPcmOutputStream> > Streams;
	unsigned int ChannelCount = 0;
	for (unsigned int i = 0; i < Gen.GetStreamCount(); i++)
	{
		Streams.push_back(Gen.CreatePcmStream(i));
		ChannelCount += Gen.GetChannels(i);
	}
	if (!Streams.size())
	{
		return;
	}
	shared_array<short> ReadBuffer(new short[ChannelCount * elPcmOutputStream::RecommendBufferSize()]);
	const unsigned int PcmBufferSamples = elPcmOutputStream::RecommendBufferSize();
	shared_array<short> PcmBuffer(new short[PcmBufferSamples]);
	while (!Streams[0]->Eos())
	{
		unsigned int Ch = 0;
		unsigned int Frames = 0;
		for (unsigned int i = 0; i < Gen.GetStreamCount(); i++)
		{
			unsigned int Read;
			Read = Streams[i]->Read(PcmBuffer.get(), PcmBufferSamples);
			Frames = Read / Streams[i]->GetChannels();
			unsigned int OldCh = Ch;
			for (unsigned int j = 0; j < Frames; j++)
			{
				Ch = OldCh;
				for (unsigned int k = 0; k < Streams[i]->GetChannels(); k++)
				{
					ReadBuffer[j * ChannelCount + Ch] = PcmBuffer[j * Streams[i]->GetChannels() + k];
					Ch++;
				}
			}
		}
		unsigned int sz;
		f->Write(Frames * ChannelCount * sizeof(short),(BYTE *) ReadBuffer.get(),&sz);
	}
}

void ParseL32PStream(CWaveFile *f,unsigned char *block,unsigned int channels,unsigned int numsamples,unsigned int size)
{
	unsigned int sz = 0;
	elMpegGenerator Gen;
	elBlock FirstBlock;
	bool LastPacket = false;
    uint16_t Flags = *(uint16_t *)block;
	block += sizeof(uint16_t);
	sz += sizeof(uint16_t);
    uint16_t BlockSize = *(uint16_t *)block;
	block += sizeof(uint16_t);
	sz += sizeof(uint16_t);
    uint32_t Samples = *(uint32_t *)block;
	block += sizeof(uint32_t);
	sz += sizeof(uint32_t);
    Swap(Flags);
    Swap(BlockSize);
    Swap(Samples);
	BlockSize -= 8;
	shared_array<uint8_t> Data(new uint8_t[BlockSize]);
	for (unsigned int i = 0;i < BlockSize;i++)
	{
		Data[i] = block[i];
	}
	block += BlockSize;
	sz += BlockSize;
	FirstBlock.Clear();
	FirstBlock.Data = Data;
	FirstBlock.SampleCount = Samples;
	FirstBlock.Size = BlockSize;
    if (Flags & 0x8000)
    {
        LastPacket = true;
    }
	Gen.Initialize(FirstBlock, make_shared<elParserVersion6>());
	Gen.ParseBlock(FirstBlock);
	bool x = true;
    while (x)
    {
        elBlock Block;
        if (LastPacket || sz == size)
        {
            break;
        }
	    uint16_t Flags2 = *(uint16_t *)block;
		block += sizeof(uint16_t);
		sz += sizeof(uint16_t);
	    uint16_t BlockSize2 = *(uint16_t *)block;
		block += sizeof(uint16_t);
		sz += sizeof(uint16_t);
	    uint32_t Samples2 = *(uint32_t *)block;
		block += sizeof(uint32_t);
		sz += sizeof(uint32_t);
        if (sz == size)
        {
            break;
        }
	    Swap(Flags2);
	    Swap(BlockSize2);
	    Swap(Samples2);
		BlockSize2 -= 8;
		shared_array<uint8_t> Datax(new uint8_t[BlockSize2]);
		for (unsigned int i = 0;i < BlockSize2;i++)
		{
			Datax[i] = block[i];
		}
		block += BlockSize2;
		sz += BlockSize2;
		Block.Clear();
		Block.Data = Datax;
		Block.SampleCount = Samples2;
		Block.Size = BlockSize2;
		if (Flags2 & 0x8000)
		{
			LastPacket = true;
	    }
        Gen.ParseBlock(Block);
    }
	Gen.DoneParsingBlocks();
	std::vector< shared_ptr<elPcmOutputStream> > Streams;
	unsigned int ChannelCount = 0;
	for (unsigned int i = 0; i < Gen.GetStreamCount(); i++)
	{
		Streams.push_back(Gen.CreatePcmStream(i));
		ChannelCount += Gen.GetChannels(i);
	}
	if (!Streams.size())
	{
		return;
	}
	shared_array<short> ReadBuffer(new short[ChannelCount * elPcmOutputStream::RecommendBufferSize()]);
	const unsigned int PcmBufferSamples = elPcmOutputStream::RecommendBufferSize();
	shared_array<short> PcmBuffer(new short[PcmBufferSamples]);
	while (!Streams[0]->Eos())
	{
		unsigned int Ch = 0;
		unsigned int Frames = 0;
		for (unsigned int i = 0; i < Gen.GetStreamCount(); i++)
		{
			unsigned int Read;
			Read = Streams[i]->Read(PcmBuffer.get(), PcmBufferSamples);
			Frames = Read / Streams[i]->GetChannels();
			unsigned int OldCh = Ch;
			for (unsigned int j = 0; j < Frames; j++)
			{
				Ch = OldCh;
				for (unsigned int k = 0; k < Streams[i]->GetChannels(); k++)
				{
					ReadBuffer[j * ChannelCount + Ch] = PcmBuffer[j * Streams[i]->GetChannels() + k];
					Ch++;
				}
			}
		}
		unsigned int szx;
		f->Write(Frames * ChannelCount * sizeof(short),(BYTE *) ReadBuffer.get(),&szx);
	}
}

BOOL CALLBACK ExtractDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
	case WM_INITDIALOG:
		{
			hdlg = hwnd;
			d3d = Direct3DCreate9(D3D_SDK_VERSION);
			D3DPRESENT_PARAMETERS d3dpp;
			ZeroMemory(&d3dpp, sizeof(d3dpp));
			d3dpp.Windowed = TRUE;
			d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
			d3dpp.hDeviceWindow = GetDlgItem(hwnd,IDC_3DVIEW);
			d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
			d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
			d3dpp.EnableAutoDepthStencil = TRUE;
			d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetDlgItem(hwnd,IDC_3DVIEW), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
			SubclassWindow(GetDlgItem(hwnd,IDC_3DVIEW),ViewWndProc);
		}
		break;
	case WM_CLOSE:
		if (mf)
		{
			StreamClose(mf);
		}
		EndDialog(hwnd, 0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_FILE_EXIT:
			if (mf)
			{
				StreamClose(mf);
			}
			EndDialog(hwnd, 0);
			break;
		case ID_FILE_OPEN:
			{
				OPENFILENAME ofn;
				char sfile[MAX_PATH] = "";
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFilter = "Streams (*.manifest)\0*.manifest\0";
				ofn.lpstrFile = sfile;
				ofn.lpstrTitle = "Open Stream";
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrDefExt = ".manifest";
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT | OFN_NOCHANGEDIR;
				if (GetOpenFileName(&ofn))
				{
					if (mf)
					{
						StreamClose(mf);
					}
					ListBox_ResetContent(GetDlgItem(hwnd,IDC_FILELIST));
					strrchr(sfile,'.')[0] = 0;
					mf = StreamOpen(sfile,0,0);
					for (unsigned int i = 0;i < mf->Header[mf->Assets.size()-1]->AssetCount;i++)
					{
						if ((mf->Assets[mf->Assets.size()-1][i].Header.TypeId == 0x21e727da) || (mf->Assets[mf->Assets.size()-1][i].Header.TypeId == 0xF0F08712) || (mf->Assets[mf->Assets.size()-1][i].Header.TypeId == 0x166B084D))
						{
							if (mf->Assets[mf->Assets.size()-1][i].Header.InstanceDataSize)
							{
								const char *name = mf->Assets[mf->Assets.size()-1][i].Name;
								int index = ListBox_AddString(GetDlgItem(hwnd,IDC_FILELIST),name);
								ListBox_SetItemData(GetDlgItem(hwnd,IDC_FILELIST),index,i);
							}
						}
					}
				}
			}
			break;
		case ID_FILE_SAVE:
			{
				int index = ListBox_GetCurSel(GetDlgItem(hwnd,IDC_FILELIST));
				if (index != -1)
				{
					OPENFILENAME ofn;
					char sfile[MAX_PATH] = "";
					int i = ListBox_GetItemData(GetDlgItem(hwnd,IDC_FILELIST),index);
					if ((mf->Assets[mf->Assets.size()-1][i].Header.TypeId == 0x21e727da) && (tex))
					{
						const char *name = mf->Assets[mf->Assets.size()-1][i].Name;
						strcpy(sfile,strchr(name,':')+1);
						strcat(sfile,".dds");
						ZeroMemory(&ofn, sizeof(ofn));
						ofn.lStructSize = sizeof(ofn);
						ofn.hwndOwner = hwnd;
						ofn.lpstrFilter = "Texture (*.dds)\0*.dds\0";
						ofn.lpstrFile = sfile;
						ofn.lpstrTitle = "Save texture";
						ofn.lpstrDefExt = ".dds";
						ofn.nMaxFile = MAX_PATH;
						ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
						if (GetSaveFileName(&ofn))
						{
							D3DXSaveTextureToFile(sfile,D3DXIFF_DDS,tex,NULL);
						}
					}
					if (mf->Assets[mf->Assets.size()-1][i].Header.TypeId == 0x166B084D)
					{
						if (mf->Assets[mf->Assets.size()-1][i].InstanceData)
						{
							unsigned char *outdata = mf->Assets[mf->Assets.size()-1][i].InstanceData;
							StreamParseImports(&(mf->Assets[mf->Assets.size()-1][i]),mf);
							AudioFileRuntime *afr = (AudioFileRuntime *)outdata;
							if (mf->Assets[mf->Assets.size()-1][i].Header.TypeHash == 0xa372b6c3)
							{
								AudioFileRuntime2 *afr2 = (AudioFileRuntime2 *)outdata;
								afr->numberofchannels = afr2->numberofchannels;
							}
							unsigned char *cd = StreamGetCdata(&(mf->Assets[mf->Assets.size()-1][i]),mf);
							int size = StreamGetCdataSize(&(mf->Assets[mf->Assets.size()-1][i]),mf);
							const char *name = mf->Assets[mf->Assets.size()-1][i].Name;
							strcpy(sfile,strchr(name,':')+1);
							strcat(sfile,".wav");
							ZeroMemory(&ofn, sizeof(ofn));
							ofn.lStructSize = sizeof(ofn);
							ofn.hwndOwner = hwnd;
							ofn.lpstrFilter = "Audio File (*.wav)\0*.wav\0";
							ofn.lpstrFile = sfile;
							ofn.lpstrTitle = "Save Audio File";
							ofn.lpstrDefExt = ".wav";
							ofn.nMaxFile = MAX_PATH;
							ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
							if (GetSaveFileName(&ofn))
							{
								CWaveFile *f = new CWaveFile;
								WAVEFORMATEX fmt;
								memset(&fmt,0,sizeof(fmt));
								fmt.wFormatTag = WAVE_FORMAT_PCM;
								fmt.nSamplesPerSec = afr->samplerate;
								fmt.wBitsPerSample = 16;
								fmt.nChannels = afr->numberofchannels;
								fmt.nBlockAlign = fmt.nChannels * (fmt.wBitsPerSample / 8);
								fmt.nAvgBytesPerSec = fmt.nBlockAlign * fmt.nSamplesPerSec;
								if (FAILED(f->Open(sfile,&fmt,WAVEFILE_WRITE)))
								{
									f->Close();
									delete f;
									return false;
								}
								if (afr->headerdatasize)
								{
									switch (((char *)afr->headerdata)[0])
									{
									case 2:
										ParseRawStream(f,cd,afr->numberofchannels,afr->numberofsamples);
										break;
									case 4:
										ParseADPCMStream(f,cd,afr->numberofchannels,afr->numberofsamples);
										break;
									case 5:
										ParseEL31Stream(f,cd,afr->numberofchannels,afr->numberofsamples,size);
										break;
									case 6:
									case 7:
										ParseL32PStream(f,cd,afr->numberofchannels,afr->numberofsamples,size);
										break;
									}
								}
								else
								{
									switch (cd[0])
									{
									case 2:
										cd += 0x10;
										ParseRawBlock(f,cd,afr->numberofchannels,afr->numberofsamples);
										break;
									case 4:
										cd += 0x10;
										ParseADPCMBlock(f,cd,afr->numberofchannels,afr->numberofsamples);
										break;
									case 5:
										cd += 0x10;
										size -= 0x10;
										ParseEL31Block(f,cd,afr->numberofchannels,afr->numberofsamples,size);
										break;
									case 6:
									case 7:
										cd += 0x10;
										size -= 0x10;
										ParseL32PBlock(f,cd,afr->numberofchannels,afr->numberofsamples,size);
										break;
									}
								}
								f->Close();
								delete f;
							}
						}
					}
				}
			}
			break;
		case IDC_FILELIST:
			{
				if (HIWORD(wParam) == LBN_SELCHANGE)
				{
					EnableMenuItem(GetMenu(hwnd),ID_FILE_SAVE,MF_GRAYED);
					int index = ListBox_GetCurSel(GetDlgItem(hwnd,IDC_FILELIST));
					if (index != -1)
					{
						int i = ListBox_GetItemData(GetDlgItem(hwnd,IDC_FILELIST),index);
						if (tex)
						{
							tex->Release();
							tex = 0;
						}
						if (m_pVB)
						{
							m_pVB->Release();
							m_pVB = 0;
						}
						if (m_pIB)
						{
							m_pIB->Release();
							m_pIB = 0;
						}
						if (decl)
						{
							decl->Release();
							decl = 0;
						}
						if (StreamLoad(&(mf->Assets[mf->Assets.size()-1][i])))
						{
							if (mf->Assets[mf->Assets.size()-1][i].Header.TypeId == 0x21e727da)
							{
								if (mf->Assets[mf->Assets.size()-1][i].InstanceData)
								{
									EnableMenuItem(GetMenu(hwnd),ID_FILE_SAVE,MF_ENABLED);
									unsigned char *outdata = mf->Assets[mf->Assets.size()-1][i].InstanceData;
									outdata += 12;
									nv = 6;
									pc = 2;
									D3DVERTEXELEMENT9 elements[4] = {{0,0,D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITIONT,0},{0,16,D3DDECLTYPE_D3DCOLOR,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_COLOR,0},{0,20,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},{0xFF,0,D3DDECLTYPE_UNUSED,0,0,0}};
									d3ddev->CreateVertexBuffer(6 * sizeof(CUSTOMVERTEX),0 /* Usage */, 0,D3DPOOL_MANAGED, &m_pVB, NULL);
									d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
									d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
									d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
									d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
									d3ddev->CreateVertexDeclaration(elements,&decl);
									D3DXCreateTextureFromFileInMemoryEx(d3ddev,outdata,mf->Assets[mf->Assets.size()-1][i].Header.InstanceDataSize - 12,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_FROM_FILE,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_FILTER_NONE,0,0,0,&tex);
									D3DSURFACE_DESC sd;
									tex->GetLevelDesc(0, &sd);
									RECT r;
									GetClientRect(GetDlgItem(hwnd,IDC_3DVIEW),&r);
									int w = sd.Width;
									int h = sd.Height;
									if (w > r.right)
									{
										w = r.right;
									}
									if (h > r.bottom)
									{
										h = r.bottom;
									}
									CUSTOMVERTEX vertexArray[] =
									{
										{ 0,					 0,					  0.5f, 1.0f, 0xffffffff, 0.0, 0.0, },
										{ (FLOAT)w, 0,					  0.5f, 1.0f, 0xffffffff, 1.0, 0.0, },
										{ (FLOAT)w, (FLOAT)h, 0.5f, 1.0f, 0xffffffff, 1.0, 1.0, },
										{ (FLOAT)w, (FLOAT)h, 0.5f, 1.0f, 0xffffffff, 1.0, 1.0, },
										{ 0,					 (FLOAT)h, 0.5f, 1.0f, 0xffffffff, 0.0, 1.0, },
										{ 0,					 0,					  0.5f, 1.0f, 0xffffffff, 0.0, 0.0, },
									};
									VOID* pVertices;
									m_pVB->Lock( 0, sizeof(vertexArray), (void**)&pVertices, 0 );
									memcpy(pVertices,vertexArray,sizeof(vertexArray));
									m_pVB->Unlock();
								}
							}
							else if (mf->Assets[mf->Assets.size()-1][i].Header.TypeId == 0xF0F08712)
							{
								StreamParseImports(&(mf->Assets[mf->Assets.size()-1][i]),mf);
							}
							else if (mf->Assets[mf->Assets.size()-1][i].Header.TypeId == 0x166B084D)
							{
								EnableMenuItem(GetMenu(hwnd),ID_FILE_SAVE,MF_ENABLED);
							}
						}
					}
				}
			}
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	hInst = hInstance;
	if (lpCmdLine)
	{
		cmdline = _strdup(lpCmdLine);
	}
	else
	{
		cmdline = 0;
	}
	mf = 0;
	accel = LoadAccelerators(hInst,MAKEINTRESOURCE(IDR_BIGEXTRACT));
	hook = SetWindowsHookEx(WH_MSGFILTER, AccelHook, NULL, GetCurrentThreadId());
	DialogBox(hInst, MAKEINTRESOURCE(IDD_EXTRACT), NULL,(DLGPROC) ExtractDlgProc);
	UnhookWindowsHookEx(hook);
	if (tex)
	{
		tex->Release();
	}
	if (m_pVB)
	{
		m_pVB->Release();
	}
	if (decl)
	{
		decl->Release();
	}
	if (d3ddev)
	{
		d3ddev->Release();
	}
	if (d3d)
	{
		d3d->Release();
	}
	return 0;
}
