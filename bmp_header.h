#ifndef BMP_HEADER_H_INCLUDED
#define BMP_HEADER_H_INCLUDED

#include <stdexcept>

struct BMPFileHeader{
    int signature;
    int sizeOfFile;
    int pixelArrayOffset;
} header;

void parseBMPHeader(unsigned char fileBuffer[])
{
    BMPFileHeader result;

    if(fileBuffer[0]!='B'||fileBuffer[1]!='M')
    {
        throw std::runtime_error("File not BMP type!");
    }

    result.signature = fileBuffer[1]*256 + fileBuffer[0];
    result.sizeOfFile = fileBuffer[5]*16777216+fileBuffer[4]*65536+fileBuffer[3]*256+fileBuffer[2];
    result.pixelArrayOffset = fileBuffer[13]*16777216+fileBuffer[12]*65536+fileBuffer[11]*256+fileBuffer[10];

    header = result;
}

#endif // BMP_HEADER_H_INCLUDED
