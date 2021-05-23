/*
	Command & Conquer 3 Tools
	cc3tools shared dll include file
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
#ifndef CC3TOOLS_H_INCLUDED
#define CC3TOOLS_H_INCLUDED

#ifdef CC3TOOLS_EXPORTS
#define CC3TOOLS_DLL_EXPORT __declspec(dllexport)
#else
#define CC3TOOLS_DLL_EXPORT __declspec(dllimport)
#endif

#include <vector>
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include "fileio.h"

//structure representing a file within a .big file
struct BIGFile;
struct BIGFileEntry {
	FileClass *fc;
	BIGFile *container;
	unsigned int offset;
	unsigned int size;
	unsigned int un_size;
	const char *name;
};

//structure representing a .big file
struct BIGFile {
	FileClass *fc;
	char *filename;
	unsigned int magic;
	unsigned int num_files;
	BIGFileEntry *entries;
};

//structure representing the header of a .manifest file
struct ManifestHeader {
	bool IsBigEndian;
	bool IsLinked;
	unsigned short Version;
	unsigned int StreamChecksum;
	unsigned int AllTypesHash;
	unsigned int AssetCount;
	unsigned int TotalInstanceDataSize;
	unsigned int MaxInstanceChunkSize;
	unsigned int MaxRelocationChunkSize;
	unsigned int MaxImportsChunkSize;
	unsigned int AssetReferenceBufferSize;
	unsigned int ReferencedManifestNameBufferSize;
	unsigned int AssetNameBufferSize;
	unsigned int SourceFileNameBufferSize;
};

//structure representing an asset in a .manifest file
struct AssetEntry {
	unsigned int TypeId;
	unsigned int InstanceId;
	unsigned int TypeHash;
	unsigned int InstanceHash;
	unsigned int AssetReferenceOffset;
	unsigned int AssetReferenceCount;
	unsigned int NameOffset;
	unsigned int SourceFileNameOffset;
	unsigned int InstanceDataSize;
	unsigned int RelocationDataSize;
	unsigned int ImportsDataSize;
};

//structure representing a reference to an asset
struct AssetId {
	unsigned int TypeId;
	unsigned int InstanceId;
};

//structure representing a reference to another manifest
struct ManifestReference {
	bool IsPatch;
	char *Path;
};

//structure representing an asset
struct Asset {
	FileClass *fc;
	AssetEntry Header;
	unsigned int IsTokenized;
	AssetId *AssetReferences;
	char *Name;
	char *SourceFileName;
	char *ManifestName;
	bool InstanceDataLoaded;
	unsigned char *InstanceData;
	unsigned int *RelocationData;
	unsigned int *ImportsData;
};

//structure representing an asset to be output to a new stream
struct OutputAsset {
	AssetEntry Header;
	unsigned int IsTokenized;
	AssetId *AssetReferences;
	char *Name;
	char *SourceFileName;
	unsigned char *InstanceData;
	unsigned int *RelocationData;
	unsigned int *ImportsData;
};

//structure represneting a manifest
struct Manifest {
	std::vector<char *> FileName;
	std::vector<FileClass *> FileClasses;
	std::vector<ManifestHeader *> Header;
	std::vector<Asset *> Assets;
	std::vector<AssetId *> AssetReferences;
	std::vector<std::vector<ManifestReference *>*> ReferencedManifests;
	std::vector<char *> AssetNames;
	std::vector<char *> SourceFileNames;
	std::vector<unsigned char *> ImportsData;
	std::vector<unsigned char *> RelocationData;
	std::vector<BIGFile *> bf;
};

//structure represneting a manifest to be output as a new stream
struct OutputManifest {
	ManifestHeader Header;
	std::vector<OutputAsset *> Assets;
	std::vector<ManifestReference *> ReferencedManifests;
};

//.big file related functions
BIGFile CC3TOOLS_DLL_EXPORT *BigOpen(const char *filename); //open a .big file
void CC3TOOLS_DLL_EXPORT BigClose(BIGFile *bf); //close a .big file
unsigned int CC3TOOLS_DLL_EXPORT BigGetNumElements(BIGFile *bf); //get entry count for a .big file
BIGFileEntry CC3TOOLS_DLL_EXPORT *BigLoadEntry(BIGFile *bf, int entry); //load a .big file entry by index
const char CC3TOOLS_DLL_EXPORT *BigGetFilename(BIGFile *bf, int entry); //get the filename of an entry
BIGFileEntry CC3TOOLS_DLL_EXPORT *BigLoadEntryByName(BIGFile *bf, const char *name); //load a .big file entry by name

//stream related functions
Manifest CC3TOOLS_DLL_EXPORT *StreamOpen(const char *filename, const char *bigfilepath,Manifest *patch); //open a stream
void CC3TOOLS_DLL_EXPORT StreamClose(Manifest *mf); //close a stream
bool CC3TOOLS_DLL_EXPORT StreamLoad(Asset *as); //load an asset into memory if it isnt already loaded
void CC3TOOLS_DLL_EXPORT StreamParseRelocations(Asset *as); //parse relocations for an asset
unsigned char CC3TOOLS_DLL_EXPORT *StreamGetCdata(Asset *as, Manifest *mf); //Get the CDATA for an asset, only works for assets within .big files for now
unsigned int CC3TOOLS_DLL_EXPORT StreamGetCdataSize(Asset *as, Manifest *mf); //Get the CDATA size for an asset, only works for assets within .big files for now
Asset CC3TOOLS_DLL_EXPORT *StreamFindAsset(Manifest *mf,unsigned int TypeId,unsigned int InstanceId); //find an asset in a stream
bool CC3TOOLS_DLL_EXPORT OutputAssetExists(OutputManifest *mf,unsigned int TypeId,unsigned int InstanceId); //identifies if an asset has been added to an output stream or not
int CC3TOOLS_DLL_EXPORT StreamParseImports(Asset *as,Manifest *mf); //parse all relocations and imports for an asset

//C&C3 path related functions
void CC3TOOLS_DLL_EXPORT GetInstallPath(char *installpath,unsigned long size); //get the C&C3 install path
void CC3TOOLS_DLL_EXPORT GetModPath(char *modpath,unsigned long size); //get the C&C3 mod path
void CC3TOOLS_DLL_EXPORT GetMapPath(char *mappath,unsigned long size); //get the C&C3 map path
void CC3TOOLS_DLL_EXPORT GetSDKPath(char *sdkpath,unsigned long size); //read the SDK path from the registry, if it is not there, ask the user for it
void CC3TOOLS_DLL_EXPORT GetLanguage(char *language,unsigned long size); //get the C&C3 language

//misc functions
BOOL CC3TOOLS_DLL_EXPORT XBrowseForFolder(HWND hWnd,LPCTSTR lpszInitialFolder,LPTSTR lpszBuf,DWORD dwBufSize); //get an output folder path
unsigned long CC3TOOLS_DLL_EXPORT ComputeCC3Hash(const char *input, int length); //compute hash of data using C&C3 algoritihm

#endif // CC3TOOLS_H_INCLUDED
