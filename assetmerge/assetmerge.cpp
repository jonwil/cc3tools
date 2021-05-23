/*
	Command & Conquer 3 Tools
	Asset Merge tool
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
#include <shlwapi.h>
#include <algorithm>
#pragma warning(disable: 6211)
void *memmem(const void *b1, const void *b2, size_t len1, size_t len2)
{
	char *sp = (char *) b1;
	char *pp = (char *) b2;
	char *eos = sp + len1 - len2;
	if(!(b1 && b2 && len1 && len2))
	{
		return NULL;
	}
	while (sp <= eos)
	{
		if (*sp == *pp)
		{
			if (memcmp(sp, pp, len2) == 0)
			{
				return sp;
			}
		}
		sp++;
	}
	return NULL;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("usage: assetmerge [manifest] [path]");
		return 0;
	}
	Manifest *m = StreamOpen(argv[1],0,0);
	OutputManifest out;
	std::vector<OutputAsset *> aa;
	out.Header.IsBigEndian = m->Header[0]->IsBigEndian;
	out.Header.IsLinked = m->Header[0]->IsLinked;
	out.Header.Version = 6;
	out.Header.StreamChecksum = m->Header[0]->StreamChecksum;
	out.Header.AllTypesHash = 0x0a979693;
	out.Header.AssetCount = m->Header[0]->AssetCount;
	out.Header.TotalInstanceDataSize = 0;
	out.Header.MaxInstanceChunkSize = m->Header[0]->MaxInstanceChunkSize;
	out.Header.MaxRelocationChunkSize = m->Header[0]->MaxRelocationChunkSize;
	out.Header.MaxImportsChunkSize = m->Header[0]->MaxImportsChunkSize;
	out.Header.AssetReferenceBufferSize = 0;
	out.Header.ReferencedManifestNameBufferSize = m->Header[0]->ReferencedManifestNameBufferSize;
	out.Header.AssetNameBufferSize = 0;
	out.Header.SourceFileNameBufferSize = 0;
	std::vector<ManifestReference *> *mrs = m->ReferencedManifests[0];
	Asset *as = m->Assets[0];
	for (unsigned int i = 0;i < mrs->size();i++)
	{
		ManifestReference *mr = (*mrs)[i];
		out.ReferencedManifests.push_back(mr);
	}
	for (unsigned int i = 0;i < m->Header[0]->AssetCount;i++)
	{
		OutputAsset *oa = new OutputAsset;
		out.Assets.push_back(oa);
		StreamLoad(&as[i]);
		oa->Header.TypeId = as[i].Header.TypeId;
		oa->Header.InstanceId = as[i].Header.InstanceId;
		oa->Header.TypeHash = as[i].Header.TypeHash;
		oa->Header.InstanceHash = as[i].Header.InstanceHash;
		oa->Header.AssetReferenceOffset = 0;
		oa->Header.AssetReferenceCount = as[i].Header.AssetReferenceCount;
		oa->Header.NameOffset = 0;
		oa->Header.SourceFileNameOffset = 0;
		oa->Header.InstanceDataSize = as[i].Header.InstanceDataSize;
		oa->IsTokenized = as[i].IsTokenized;
		out.Header.TotalInstanceDataSize += as[i].Header.InstanceDataSize;
		if (out.Header.MaxInstanceChunkSize < oa->Header.InstanceDataSize)
		{
			out.Header.MaxInstanceChunkSize = oa->Header.InstanceDataSize;
		}
		oa->Header.RelocationDataSize = as[i].Header.RelocationDataSize;
		if (out.Header.MaxRelocationChunkSize < oa->Header.RelocationDataSize)
		{
			out.Header.MaxRelocationChunkSize = oa->Header.RelocationDataSize;
		}
		oa->Header.ImportsDataSize = as[i].Header.ImportsDataSize;
		if (out.Header.MaxImportsChunkSize < oa->Header.ImportsDataSize)
		{
			out.Header.MaxImportsChunkSize = oa->Header.ImportsDataSize;
		}
		oa->AssetReferences = as[i].AssetReferences;
		oa->Name = as[i].Name;
		oa->SourceFileName = as[i].SourceFileName;
		oa->InstanceData = as[i].InstanceData;
		oa->RelocationData = as[i].RelocationData;
		oa->ImportsData = as[i].ImportsData;
	}
	WIN32_FIND_DATA data;
	char fname[MAX_PATH];
	sprintf(fname,"%s\\*.asset",argv[2]);
	HANDLE h = FindFirstFile(fname,&data);
	if (h == INVALID_HANDLE_VALUE)
	{
		printf("No assets found\n");
		FindClose(h);
		return 0;
	}
	else
	{
		do {
			char fname2[MAX_PATH];
			sprintf(fname2,"%s\\%s",argv[2],data.cFileName);
			fname2[MAX_PATH-1] = 0;
			FILE *f = fopen(fname2,"rb");
			OutputAsset *oa = new OutputAsset;
			aa.push_back(oa);
			fread(&oa->Header.TypeId,4,1,f);
			fread(&oa->Header.InstanceId,4,1,f);
			fread(&oa->Header.TypeHash,4,1,f);
			fread(&oa->Header.InstanceHash,4,1,f);
			fread(&oa->Header.AssetReferenceCount,4,1,f);
			fread(&oa->Header.InstanceDataSize,4,1,f);
			fread(&oa->Header.RelocationDataSize,4,1,f);
			fread(&oa->Header.ImportsDataSize,4,1,f);
			oa->Header.AssetReferenceOffset = 0;
			oa->Header.NameOffset = 0;
			oa->Header.SourceFileNameOffset = 0;
			oa->IsTokenized = 0;
			if (oa->Header.AssetReferenceCount)
			{
				oa->AssetReferences = new AssetId[oa->Header.AssetReferenceCount];
				fread(oa->AssetReferences,oa->Header.AssetReferenceCount*sizeof(AssetId),1,f);
			}
			int nl = 0;
			fread(&nl,4,1,f);
			oa->Name = new char[nl+1];
			oa->Name[nl] = 0;
			fread(oa->Name,nl,1,f);
			int snl = 0;
			fread(&snl,4,1,f);
			oa->SourceFileName = new char[snl+1];
			oa->SourceFileName[snl] = 0;
			fread(oa->SourceFileName,snl,1,f);
			oa->InstanceData = new unsigned char[oa->Header.InstanceDataSize];
			fread(oa->InstanceData,oa->Header.InstanceDataSize,1,f);
			if (oa->Header.RelocationDataSize)
			{
				oa->RelocationData = new unsigned int [oa->Header.RelocationDataSize / 4];
				fread(oa->RelocationData,oa->Header.RelocationDataSize,1,f);
			}
			if (oa->Header.ImportsDataSize)
			{
				oa->ImportsData = new unsigned int [oa->Header.ImportsDataSize / 4];
				fread(oa->ImportsData,oa->Header.ImportsDataSize,1,f);
			}
			fclose(f);
		} while (FindNextFile(h,&data));
		FindClose(h);
	}
	for (unsigned int i = 0;i < out.Assets.size();i++)
	{
		for (unsigned int j = 0;j < out.Assets[i]->Header.AssetReferenceCount;j++)
		{
			if (out.Assets[i]->AssetReferences[j].TypeId == 0xb22f87c9)
			{
				for (unsigned int k = 0;k < aa.size();k++)
				{
					if (out.Assets[i]->AssetReferences[j].InstanceId == aa[k]->Header.InstanceId)
					{
						if ((aa[k]->Header.TypeId == 0xe3181c04) || (aa[k]->Header.TypeId == 0xf0f08712) || (aa[k]->Header.TypeId == 0xc2b1a262))
						{
							out.Assets[i]->AssetReferences[j].TypeId = aa[k]->Header.TypeId;
						}
					}
				}
			}
		}
	}
	std::vector<OutputAsset *> outs;
	for (unsigned int i = 0;i < aa.size();i++)
	{
		std::vector<OutputAsset *>::iterator result = std::find(out.Assets.begin(),out.Assets.end(),aa[i]);
		if (result == out.Assets.end())
		{
			outs.push_back(aa[i]);
			out.Header.TotalInstanceDataSize += aa[i]->Header.InstanceDataSize;
			out.Header.AssetCount++;
			if (out.Header.MaxInstanceChunkSize < aa[i]->Header.InstanceDataSize)
			{
				out.Header.MaxInstanceChunkSize = aa[i]->Header.InstanceDataSize;
			}
			if (out.Header.MaxRelocationChunkSize < aa[i]->Header.InstanceDataSize)
			{
				out.Header.MaxRelocationChunkSize = aa[i]->Header.InstanceDataSize;
			}
			if (out.Header.MaxImportsChunkSize < aa[i]->Header.InstanceDataSize)
			{
				out.Header.MaxImportsChunkSize = aa[i]->Header.InstanceDataSize;
			}
		}
	}
	for (unsigned int i = 0;i < out.Assets.size();i++)
	{
		outs.push_back(out.Assets[i]);
	}
	out.Assets.clear();
	for (unsigned int i = 0;i < outs.size();i++)
	{
		out.Assets.push_back(outs[i]);
	}
	std::vector<char *> SourceNames;
	for (unsigned int i = 0;i < out.Assets.size();i++)
	{
		out.Header.AssetReferenceBufferSize += out.Assets[i]->Header.AssetReferenceCount * sizeof(AssetId);
		out.Header.AssetNameBufferSize += (strlen(out.Assets[i]->Name) + 1);
		std::vector<char *>::iterator result = std::find(SourceNames.begin(),SourceNames.end(),out.Assets[i]->SourceFileName);
		if (result == SourceNames.end())
		{
			out.Header.SourceFileNameBufferSize += (strlen(out.Assets[i]->SourceFileName) + 1);
			SourceNames.push_back(out.Assets[i]->SourceFileName);
		}
	}
	char *AssetReferenceBuffer = new char[out.Header.AssetReferenceBufferSize];
	char *AssetReferenceOffset = AssetReferenceBuffer;
	char *NameBuffer = new char[out.Header.AssetNameBufferSize];
	char *NameOffset = NameBuffer;
	char *SourceNameBuffer = new char[out.Header.SourceFileNameBufferSize];
	char *SourceNameOffset = SourceNameBuffer;
	char *ReferencedManifestBuffer = new char[out.Header.ReferencedManifestNameBufferSize];
	char *ReferencedManifestOffset = ReferencedManifestBuffer;
	char manifest[MAX_PATH];
	char imp[MAX_PATH];
	char bin[MAX_PATH];
	char relo[MAX_PATH];
	sprintf(manifest,"%s_temp.manifest",argv[1]);
	sprintf(imp,"%s_temp.imp",argv[1]);
	sprintf(bin,"%s_temp.bin",argv[1]);
	sprintf(relo,"%s_temp.relo",argv[1]);
	FILE *mf = fopen(manifest,"wb");
	FILE *impf = fopen(imp,"wb");
	FILE *binf = fopen(bin,"wb");
	FILE *relof = fopen(relo,"wb");
	for (unsigned int i = 0;i < out.ReferencedManifests.size();i++)
	{
		if (out.ReferencedManifests[i]->IsPatch)
		{
			(*(unsigned int *)ReferencedManifestOffset) = 2;
		}
		else
		{
			(*(unsigned int *)ReferencedManifestOffset) = 1;
		}
		ReferencedManifestOffset += 1;
		strcpy(ReferencedManifestOffset,out.ReferencedManifests[i]->Path);
		ReferencedManifestOffset += strlen(out.ReferencedManifests[i]->Path);
		ReferencedManifestOffset++;
	}
	fwrite(&out.Header.StreamChecksum,4,1,binf);
	fwrite(&out.Header.StreamChecksum,4,1,impf);
	fwrite(&out.Header.StreamChecksum,4,1,relof);
	fwrite(&out.Header,sizeof(out.Header),1,mf);
	for (unsigned int i = 0;i < out.Assets.size();i++)
	{
		OutputAsset *oa = out.Assets[i];
		memcpy(AssetReferenceOffset,oa->AssetReferences,oa->Header.AssetReferenceCount * sizeof(AssetId));
		oa->Header.AssetReferenceOffset = AssetReferenceOffset - AssetReferenceBuffer;
		AssetReferenceOffset += oa->Header.AssetReferenceCount * sizeof(AssetId);
		strcpy(NameOffset,oa->Name);
		oa->Header.NameOffset = NameOffset - NameBuffer;
		NameOffset += (strlen(oa->Name) + 1);
		char *pos = (char *)memmem(SourceNameBuffer,oa->SourceFileName,SourceNameOffset-SourceNameBuffer,strlen(oa->SourceFileName));
		if (pos)
		{
			oa->Header.SourceFileNameOffset = pos - SourceNameBuffer;
		}
		else
		{
			strcpy(SourceNameOffset,oa->SourceFileName);
			oa->Header.SourceFileNameOffset = SourceNameOffset - SourceNameBuffer;
			SourceNameOffset += (strlen(oa->SourceFileName) + 1);
		}
		fwrite(&(oa->Header),sizeof(oa->Header),1,mf);
		fwrite(&(oa->IsTokenized),sizeof(oa->IsTokenized),1,mf);
		fwrite(oa->InstanceData,oa->Header.InstanceDataSize,1,binf);
		if (oa->Header.ImportsDataSize)
		{
			fwrite(oa->ImportsData,oa->Header.ImportsDataSize,1,impf);
		}
		if (oa->Header.RelocationDataSize)
		{
			fwrite(oa->RelocationData,oa->Header.RelocationDataSize,1,relof);
		}
	}
	fwrite(AssetReferenceBuffer,out.Header.AssetReferenceBufferSize,1,mf);
	fwrite(ReferencedManifestBuffer,out.Header.ReferencedManifestNameBufferSize,1,mf);
	fwrite(NameBuffer,out.Header.AssetNameBufferSize,1,mf);
	fwrite(SourceNameBuffer,out.Header.SourceFileNameBufferSize,1,mf);
	fclose(mf);
	fclose(binf);
	fclose(relof);
	fclose(impf);
	char to[MAX_PATH];
	sprintf(to,"%s\\cdata",argv[1]);
	if (!PathFileExists(to))
	{
		CreateDirectory(to,NULL);
	}
	WIN32_FIND_DATA datac;
	char fnamec[MAX_PATH];
	sprintf(fnamec,"%s\\*.cdata",argv[2]);
	HANDLE hc = FindFirstFile(fnamec,&datac);
	if (hc == INVALID_HANDLE_VALUE)
	{
		FindClose(hc);
	}
	else
	{
		do {
			char fnamec2[MAX_PATH];
			char fnamec3[MAX_PATH];
			sprintf(fnamec2,"%s\\%s",argv[2],datac.cFileName);
			sprintf(fnamec3,"%s\\%s",to,datac.cFileName);
			CopyFile(fnamec2,fnamec3,false);
		} while (FindNextFile(hc,&datac));
		FindClose(hc);
	}
	char manifesto[MAX_PATH];
	char impo[MAX_PATH];
	char bino[MAX_PATH];
	char reloo[MAX_PATH];
	sprintf(manifesto,"%s.manifest",argv[1]);
	sprintf(impo,"%s.imp",argv[1]);
	sprintf(bino,"%s.bin",argv[1]);
	sprintf(reloo,"%s.relo",argv[1]);
	StreamClose(m);
	DeleteFile(manifesto);
	DeleteFile(impo);
	DeleteFile(bino);
	DeleteFile(reloo);
	MoveFile(manifest,manifesto);
	MoveFile(bin,bino);
	MoveFile(imp,impo);
	MoveFile(relo,reloo);
}
