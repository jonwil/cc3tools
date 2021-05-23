/*
	Command & Conquer 3 Tools
	stream file read and write code
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

Manifest CC3TOOLS_DLL_EXPORT *StreamOpen(const char *filename, const char *bigfilepath,Manifest *patch)
{
	Manifest *mf;
	if (patch)
	{
		mf = patch;
	}
	else
	{
		mf = new Manifest;
	}
	char *fname = _strdup(filename);
	mf->FileName.push_back(fname);
	unsigned char *manifestdata;
	char fn[MAX_PATH];
	char *sn = 0;
	char *an = 0;
	char *mft = 0;
	Asset *a = 0;
	AssetId *id = 0;
	unsigned char *btx = 0;
	unsigned char *rtx = 0;
	unsigned char *itx = 0;
	if (!bigfilepath)
	{
		sprintf(fn,"%s.manifest",filename);
		HANDLE h = CreateFile(fn,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		int size = GetFileSize(h,NULL);
		manifestdata = new unsigned char[size];
		unsigned long i;
		if (!ReadFile(h,manifestdata,size,&i,NULL))
		{
			CloseHandle(h);
			return 0;
		}
		CloseHandle(h);
		sprintf(fn,"%s.relo",filename);
		h = CreateFile(fn,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		size = GetFileSize(h,NULL);
		rtx = new unsigned char[size];
		if (!ReadFile(h,rtx,size,&i,NULL))
		{
			CloseHandle(h);
			return 0;
		}
		CloseHandle(h);
		mf->RelocationData.push_back(rtx);
		sprintf(fn,"%s.imp",filename);
		h = CreateFile(fn,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		size = GetFileSize(h,NULL);
		itx = new unsigned char[size];
		if (!ReadFile(h,itx,size,&i,NULL))
		{
			CloseHandle(h);
			return 0;
		}
		CloseHandle(h);
		mf->ImportsData.push_back(itx);
		sprintf(fn,"%s.bin",filename);
		FileClass *f = new DiskFileClass(fn);
		mf->FileClasses.push_back(f);
	}
	else
	{
		BIGFile *bf = BigOpen(bigfilepath);
		mf->bf.push_back(bf);
		sprintf(fn,"%s.manifest",filename);
		BIGFileEntry *bfe = BigLoadEntryByName(bf,fn);
		manifestdata = bfe->fc->ReadAll();
		sprintf(fn,"%s.relo",filename);
		bfe = BigLoadEntryByName(bf,fn);
		rtx = bfe->fc->ReadAll();
		mf->RelocationData.push_back(rtx);
		sprintf(fn,"%s.imp",filename);
		bfe = BigLoadEntryByName(bf,fn);
		itx = bfe->fc->ReadAll();
		mf->ImportsData.push_back(itx);
		sprintf(fn,"%s.bin",filename);
		bfe = BigLoadEntryByName(bf,fn);
		FileClass *f = bfe->fc;
		mf->FileClasses.push_back(f);
	}
	char *mt = (char *)manifestdata;
	unsigned char *bt = btx;
	unsigned char *it = itx;
	unsigned char *rt = rtx;
	bt += 4;
	it += 4;
	rt += 4;
	ManifestHeader *h = new ManifestHeader;
	mf->Header.push_back(h);
	if ((mt[0] == 0) && (mt[1] == 0) && (mt[2] == 0) && (mt[3] == 0))
	{
		mt += 4;
		bt += 4;
		it += 4;
		rt += 4;
	}
	memcpy(h,mt,sizeof(ManifestHeader));
	mt += sizeof(ManifestHeader);
	if (h->AssetCount)
	{
		a = new Asset[h->AssetCount];
		mf->Assets.push_back(a);
		for (unsigned int i = 0;i < h->AssetCount;i++)
		{
			a[i].InstanceDataLoaded = false;
			a[i].ManifestName = fname;
			a[i].fc = mf->FileClasses[mf->FileClasses.size()-1];
			memcpy(&a[i].Header,mt,sizeof(a[i].Header));
			mt += sizeof(a[i].Header);
			if ((h->Version == 6) || (h->Version == 0x100))
			{
				a[i].IsTokenized = *((unsigned int *)mt);
				mt += 4;
			}
			else
			{
				a[i].IsTokenized = 0;
			}
		}
	}
	else
	{
		mf->Assets.push_back(0);
	}
	if (h->AssetReferenceBufferSize)
	{
		id = new AssetId[h->AssetReferenceBufferSize/sizeof(AssetId)];
		mf->AssetReferences.push_back(id);
		memcpy(id,mt,h->AssetReferenceBufferSize);
		mt += h->AssetReferenceBufferSize;
	}
	else
	{
		mf->AssetReferences.push_back(0);
	}
	if (h->ReferencedManifestNameBufferSize)
	{
		mft = mt;
		mt += h->ReferencedManifestNameBufferSize;
		std::vector<ManifestReference *> *mrv = new std::vector<ManifestReference *>;
		mf->ReferencedManifests.push_back(mrv);
		while (mft < mt)
		{
			ManifestReference *mr = new ManifestReference;
			mrv->push_back(mr);
			if (mft[0] == 1)
			{
				mr->IsPatch = false;
			}
			else
			{
				mr->IsPatch = true;
			}
			mft++;
			mr->Path = _strdup(mft);
			mft += strlen(mft);
			mft++;
		}
	}
	if (h->AssetNameBufferSize)
	{
		an = new char[h->AssetNameBufferSize];
		mf->AssetNames.push_back(an);
		memcpy(an,mt,h->AssetNameBufferSize);
		mt += h->AssetNameBufferSize;
	}
	else
	{
		mf->AssetNames.push_back(0);
	}
	if (h->SourceFileNameBufferSize)
	{
		sn = new char[h->SourceFileNameBufferSize];
		mf->SourceFileNames.push_back(sn);
		memcpy(sn,mt,h->SourceFileNameBufferSize);
		mt += h->SourceFileNameBufferSize;
	}
	else
	{
		mf->SourceFileNames.push_back(0);
	}
	for (unsigned int i = 0;i < h->AssetCount;i++)
	{
		a[i].AssetReferences = id + (a[i].Header.AssetReferenceOffset / sizeof(AssetId));
		a[i].InstanceData = bt;
		a[i].ImportsData = (unsigned int *)it;
		a[i].RelocationData = (unsigned int *)rt;
		bt += a[i].Header.InstanceDataSize;
		it += a[i].Header.ImportsDataSize;
		rt += a[i].Header.RelocationDataSize;
		a[i].Name = an + a[i].Header.NameOffset;
		a[i].SourceFileName = sn + a[i].Header.SourceFileNameOffset;
	}
	if (mf->Header.size() >= 2)
	{
		unsigned int c1 = mf->Header[mf->Header.size()-1]->AssetCount;
		unsigned int c2 = mf->Header[mf->Header.size()-2]->AssetCount;
		Asset *as1 = mf->Assets[mf->Assets.size()-1];
		Asset *as2 = mf->Assets[mf->Assets.size()-2];
		for (unsigned int i = 0;i < c1;i++)
		{
			Asset *a1 = &(as1[i]);
			if (!a1->Header.InstanceDataSize)
			{
				for (unsigned int j = 0;j < c2;j++)
				{
					Asset *a2 = &(as2[j]);
					if ((a1->Header.InstanceId == a2->Header.InstanceId) && (a1->Header.TypeId == a2->Header.TypeId))
					{
						memcpy(a1,a2,sizeof(Asset));
						break;
					}
				}
			}
		}
	}
	return mf;
}

void CC3TOOLS_DLL_EXPORT StreamClose(Manifest *mf)
{
	for (unsigned int i = 0;i < mf->FileClasses.size();i++)
	{
		delete mf->FileClasses[i];
	}
	for (unsigned int i = 0;i < mf->FileName.size();i++)
	{
		delete mf->FileName[i];
	}
	for (unsigned int i = 0;i < mf->Assets.size();i++)
	{
		delete mf->Assets[i];
	}
	for (unsigned int i = 0;i < mf->AssetReferences.size();i++)
	{
		delete mf->AssetReferences[i];
	}
	for (unsigned int i = 0;i < mf->ReferencedManifests.size();i++)
	{
		for (unsigned int j = 0;j < mf->ReferencedManifests[i]->size();j++)
		{
			delete (*mf->ReferencedManifests[i])[j]->Path;
			delete (*mf->ReferencedManifests[i])[j];
		}
		delete mf->ReferencedManifests[i];
	}
	for (unsigned int i = 0;i < mf->AssetNames.size();i++)
	{
		delete mf->AssetNames[i];
	}
	for (unsigned int i = 0;i < mf->SourceFileNames.size();i++)
	{
		delete mf->SourceFileNames[i];
	}
	for (unsigned int i = 0;i < mf->bf.size();i++)
	{
		BigClose(mf->bf[i]);
	}
	delete mf;
}

void CC3TOOLS_DLL_EXPORT StreamParseRelocations(Asset *as)
{
	unsigned int imp;
	unsigned int *id = as->RelocationData;
	if (as->Header.RelocationDataSize)
	{
		while ((imp = *id) != 0xFFFFFFFF)
		{
			(*((unsigned int *)(&as->InstanceData[imp]))) = (*((unsigned int *)(&as->InstanceData[imp]))) + *((unsigned int *)(&as->InstanceData));
			id++;
		}
	}
}

unsigned char CC3TOOLS_DLL_EXPORT *StreamGetCdata(Asset *as, Manifest *mf)
{
	char cdata[MAX_PATH];
	sprintf(cdata,"%s\\cdata\\%08x.%08x.%08x.%08x.cdata",as->ManifestName,as->Header.TypeId,as->Header.TypeHash,as->Header.InstanceId,as->Header.InstanceHash);
	for (unsigned int i = (unsigned int)mf->bf.size();i > 0;i--)
	{
		BIGFileEntry *cdfile = BigLoadEntryByName(mf->bf[i-1],cdata);
		if (cdfile)
		{
			return cdfile->fc->ReadAll();
		}
	}
	DiskFileClass *disk = new DiskFileClass(cdata);
	if (disk->Is_Present())
	{
		unsigned char *data = disk->ReadAll();
		delete disk;
		return data;
	}
	delete disk;
	return 0;
}

unsigned int CC3TOOLS_DLL_EXPORT StreamGetCdataSize(Asset *as, Manifest *mf)
{
	char cdata[MAX_PATH];
	sprintf(cdata,"%s\\cdata\\%08x.%08x.%08x.%08x.cdata",as->ManifestName,as->Header.TypeId,as->Header.TypeHash,as->Header.InstanceId,as->Header.InstanceHash);
	for (unsigned int i = (unsigned int)mf->bf.size();i > 0;i--)
	{
		BIGFileEntry *cdfile = BigLoadEntryByName(mf->bf[i-1],cdata);
		if (cdfile)
		{
			return cdfile->un_size;
		}
	}
	DiskFileClass *disk = new DiskFileClass(cdata);
	if (disk->Is_Present())
	{
		int size = disk->Size();
		delete disk;
		return size;
	}
	delete disk;
	return 0;
}

Asset CC3TOOLS_DLL_EXPORT *StreamFindAsset(Manifest *mf,unsigned int TypeId,unsigned int InstanceId)
{
	unsigned int ac = mf->Header[mf->Header.size()-1]->AssetCount;
	Asset *as = mf->Assets[mf->Assets.size()-1];
	for (unsigned int i = 0;i < ac;i++)
	{
		if ((as[i].Header.TypeId == TypeId) && (as[i].Header.InstanceId == InstanceId))
		{
			return &as[i];
		}
	}
	return NULL;
}

bool CC3TOOLS_DLL_EXPORT OutputAssetExists(OutputManifest *mf,unsigned int TypeId,unsigned int InstanceId)
{
	unsigned int ac = (unsigned int)mf->Assets.size();
	for (unsigned int i = 0;i < ac;i++)
	{
		if ((mf->Assets[i]->Header.TypeId == TypeId) && (mf->Assets[i]->Header.InstanceId == InstanceId))
		{
			return true;
		}
	}
	return false;
}

bool CC3TOOLS_DLL_EXPORT StreamLoad(Asset *as)
{
	if (!as->InstanceDataLoaded)
	{
		if (as->Header.InstanceDataSize)
		{
			as->fc->Seek((LONG)as->InstanceData);
			as->InstanceData = new unsigned char[as->Header.InstanceDataSize];
			as->fc->Read(as->InstanceData,as->Header.InstanceDataSize);
			as->InstanceDataLoaded = true;
			return true;
		}
		return false;
	}
	return true;
}

int CC3TOOLS_DLL_EXPORT StreamParseImports(Asset *as,Manifest *mf)
{
	StreamParseRelocations(as);
	for (unsigned int i = 0;i < as->Header.AssetReferenceCount;i++)
	{
		Asset *impas = StreamFindAsset(mf,as->AssetReferences[i].TypeId,as->AssetReferences[i].InstanceId);
		if (impas)
		{
			if (impas->Header.TypeId != 0xe3181c04)
			{
				if (StreamLoad(impas))
				{
					if (!StreamParseImports(impas,mf))
					{
						return 0;
					}
					*((unsigned char**)(as->InstanceData+as->ImportsData[i])) = impas->InstanceData;
				}
			}
		}
		else
		{
			return 0;
		}
	}
	return 1;
}
