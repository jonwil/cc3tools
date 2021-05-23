/*
	Command & Conquer 3 Tools
	w3x binary structure definitions
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
#ifndef W3X_FILE_H_INCLUDED
#define W3X_FILE_H_INCLUDED
#include <d3d9.h>

struct Vector2 {
	float X;
	float Y;
};

struct Vector3 {
	float X;
	float Y;
	float Z;
};

struct Vector4 {
	float X;
	float Y;
	float Z;
	float W;
};

struct Quaternion {
	float X;
	float Y;
	float Z;
	float W;
};

enum MeshGeometryType {
	Normal = 0,
	Skin,
	CameraAligned,
	CameraOriented
};

char *MeshGeometryTypes[4] = {"Normal","Skin","CameraAligned","CameraOriented"};

struct BoneInfluence {
	float Weight;
	unsigned short Bone;
};

struct BoxMinMax {
	Vector3 Min;
	Vector3 Max;
};

struct Sphere {
	float Radius;
	Vector3 Center;
};

#define CONST_FLOAT 0xDF08DF25
#define CONST_BOOL 0xA3F84C3D
#define CONST_INT 0x89181982
#define CONST_TEXTURE 0xA59096A6

struct FXShaderConstant {
	unsigned int Hash;
	unsigned int ConstantNameLength;
	char *ConstantName;
};

struct FXShaderConstantFloat : FXShaderConstant {
	unsigned int ValueCount;
	float *Value;
};

struct FXShaderConstantTexture : FXShaderConstant {
	unsigned char *Texture;
};

struct FXShaderConstantInt : FXShaderConstant {
	int Value;
};

struct FXShaderConstantBool : FXShaderConstant {
	bool Value;
};

struct Triangle {
	unsigned int VertexCount;
	unsigned int *Vertices;
	Vector3 Nrm;
	float Dist;
};

struct W3DMeshPipelineVertexData {
	unsigned int VertexCount;
	unsigned int VertexSize;
	unsigned char *Vertices;
	unsigned int VertexElementSize;
	D3DVERTEXELEMENT9 *VertexElements;
	unsigned int BoneNumbersCount;
	unsigned short *BoneNumbers;
};

struct Child {
	unsigned int Front;
	unsigned int Back;
};

struct Poly {
	unsigned int Begin;
	unsigned int Count;
};

struct Node {
	Vector3 Min;
	Vector3 Max;
	Child *Children;
	Poly *Polys;
};

struct AABTree {
	unsigned int PolyIndexCount;
	unsigned int *PolyIndices;
	unsigned int NodeCount;
	Node *Nodes;
};

struct W3DMesh {
	unsigned int Zeroes;
	W3DMeshPipelineVertexData *Vertices;
	MeshGeometryType GeometryType;
	BoxMinMax BoundingBox;
	Sphere BoundingSphere;
	unsigned int TriangleCount;
	Triangle *Triangles;
	unsigned int ShaderNameLength;
	char *ShaderName;
	unsigned int TechniqueNameLength;
	char *TechniqueName;
	unsigned int FXShaderConstantCount;
	FXShaderConstant **FXShaderConstants;
	unsigned char TechniqueIndex;
	AABTree *AABTree;
	bool Hidden;
	bool CastShadow;
	unsigned char SortLevel;
};

struct W3DCollisionBox {
	unsigned int Zeroes;
	Vector3 Center;
	Vector3 Extent;
	bool JoypadPickingOnly;
};

struct SubObject {
	unsigned int BoneIndex;
	unsigned int SubObjectNameLength;
	char *SubObjectName;
	W3DMesh *Mesh;
};

struct W3DHierarchy;
struct W3DContainer {
	unsigned int Zeroes;
	W3DHierarchy *Hierarchy;
	unsigned int SubObjectCount;
	SubObject *SubObjects;
};

struct Fixup {
	float M00;
	float M10;
	float M20;
	float M30;
	float M01;
	float M11;
	float M21;
	float M31;
	float M02;
	float M12;
	float M22;
	float M32;
	float M03;
	float M13;
	float M23;
	float M33;
};

struct Pivot {
	unsigned int PivotNameLength;
	char *PivotName;
	int Parent;
	Vector3 Translation;
	Quaternion Rotation;
	Fixup FixupMatrix;
};

struct W3DHierarchy {
	unsigned int Zeroes;
	unsigned int PivotCount;
	Pivot *Pivots;
};

#define CHAN_DELTA 0x77f97305;
#define CHAN_TIMECODE 0x60d8cf69;
enum AnimationChannelType {
	XTranslation = 0,
	YTranslation,
	ZTranslation,
	Orientation,
	Visibility
};

struct AnimationChannelBase {
	unsigned int Hash;
	AnimationChannelType Type;
	unsigned int NumFrames;
	unsigned int VectorLen;
	unsigned short Pivot;
};

struct AnimationChannelTimecoded : AnimationChannelBase {
	unsigned int FrameAndBinaryFlagCount;
	unsigned short *FrameAndBinaryFlags;
	unsigned int ValuesCount;
	float *Values;
};

struct AnimationChannelDelta : AnimationChannelBase {
	float Scale;
	unsigned int InitFloatCount;
	float *InitFloats;
	unsigned int NumBits;
	unsigned int CompressedValueCount;
	unsigned char *CompressedValues;
};

struct W3DAnimation {
	unsigned int Zeroes;
	W3DHierarchy *Hierarchy;
	unsigned int NumFrames;
	unsigned int FrameRate;
	unsigned int ChannelCount;
	AnimationChannelBase *Channels;
};

#endif // W3X_FILE_H_INCLUDED
