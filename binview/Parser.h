/*
    EA Layer 3 Extractor/Decoder
    Copyright (C) 2010, Ben Moench.
    See License.txt
*/

#pragma once

#include "Internal.h"

// Some structures

struct elChannelInfo
{
    unsigned int Scfsi;
    unsigned int Size;

    uint32_t SideInfo[2];
};

struct elUncompressedSampleFrames
{
    elUncompressedSampleFrames() : Count(0), OffsetInOutput(0) {};
    unsigned int Count;
    unsigned int OffsetInOutput;
    shared_array<short> Data;
};

struct elGranule
{
    elGranule() : Used(false) {};

    bool Used;

    unsigned char Version;
    unsigned char SampleRateIndex;
    unsigned int SampleRate;
    unsigned char ChannelMode;
    unsigned char Channels;
    unsigned char ModeExtension;
    unsigned char Index;

    shared_array<uint8_t> Data;
    unsigned int DataSize;

    elUncompressedSampleFrames Uncomp;
    std::vector<elChannelInfo> ChannelInfo;
};

struct elFrame
{
    elGranule Gr[2];
};

typedef std::deque<elFrame> elStream;
typedef std::vector<elStream> elStreamVector;

enum
{
    MV_2_5 = 0,
    MV_RESERVED = 1,
    MV_2 = 2,
    MV_1 = 3
};

enum
{
    CM_STEREO = 0,
    CM_JOINT_STEREO = 1,
    CM_DUAL_CHANNEL = 2,
    CM_MONO = 3
};


class bsBitstream;


/// The EALayer3 parser class.
class elParser
{
public:
    elParser();
    virtual ~elParser();

    /// Parses the entire input stream and checks to see if it's a format that can be parsed.
    virtual bool Initialize(bsBitstream& IS);

    /// Parses the entire input stream and outputs an elStreamVector.
    virtual void Parse(elStreamVector& Streams, bsBitstream& IS);
    
protected:
    /// Read a granule and uncompressed samples if they exist from the stream.
    virtual bool ReadGranuleWithUncSamples(bsBitstream& IS, elGranule& Gr);
    
    /// Read a granule from the stream.
    virtual bool ReadGranule(bsBitstream& IS, elGranule& Gr);

    /// Read the actual uncompressed samples from the file.
    virtual void ReadUncSamples(bsBitstream& IS, elGranule& Gr);
    
    /// The current frame number for debugging purposes.
    unsigned int m_CurrentFrame;
};
