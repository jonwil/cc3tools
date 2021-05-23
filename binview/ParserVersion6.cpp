/*
    EA Layer 3 Extractor/Decoder
    Copyright (C) 2010, Ben Moench.
    See License.txt
*/

#include "Internal.h"
#include "ParserVersion6.h"
#include "Bitstream.h"

elParserVersion6::elParserVersion6()
{
    return;
}

elParserVersion6::~elParserVersion6()
{
    return;
}

bool elParserVersion6::ReadGranuleWithUncSamples(bsBitstream& IS, elGranule& Gr)
{
    if (IS.Eos())
    {
        return false;
    }

    if (IS.GetCountBitsLeft() < 16)
    {
        return false;
    }

    const unsigned int StartOffset = IS.Tell();

    // Read the uncompressed sample information
    unsigned int HasSecondPart = 0;
    unsigned int Unknown = 0;
    unsigned int TotalGranuleSize = 0;
    unsigned int Mode = 0;
    unsigned int BeforeUnc = 0;
    unsigned int UncSampleCount = 0;
    unsigned int MpegGranuleSize = 0;
    unsigned int HeaderSize = 0;

    HasSecondPart = IS.ReadBit();
    Unknown = IS.ReadBit();
    IS.ReadBits(2); // unused
    TotalGranuleSize = IS.ReadBits(12);

    if (HasSecondPart)
    {
        HeaderSize = 6;

        Mode = IS.ReadBits(2);
        BeforeUnc = IS.ReadBits(10);
        UncSampleCount = IS.ReadBits(10);
        MpegGranuleSize = IS.ReadBits(10);
        HeaderSize = 6;
    }
    else
    {
        HeaderSize = 2;
        if (TotalGranuleSize)
        {
            MpegGranuleSize = TotalGranuleSize - HeaderSize;
        }
    }

    Gr.Uncomp.Count = UncSampleCount;
    Gr.Uncomp.OffsetInOutput = BeforeUnc;

    // Read the granule if there is any
    if (MpegGranuleSize)
    {
        if (!ReadGranule(IS, Gr))
        {
            return false;
        }
        IS.SeekToNextByte();

    }

    // Check if this is the last granule in the block
    if (Gr.Version == 0 && Gr.SampleRateIndex == 0 && Gr.ChannelMode == 0 &&
        Gr.ModeExtension == 0 && Gr.Index == 0)
    {
        return false;
    }

    if (!TotalGranuleSize)
    {
        return false;
    }
    
    // Read the uncompressed samples
    if (UncSampleCount)
    {
        IS.SeekAbsolute(StartOffset + (MpegGranuleSize + HeaderSize) * 8);
        ReadUncSamples(IS, Gr);
    }

    IS.SeekAbsolute(StartOffset + TotalGranuleSize * 8);

    Gr.Used = true;
    return true;
}
