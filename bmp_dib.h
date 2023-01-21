#ifndef BMP_DIB_H_INCLUDED
#define BMP_DIB_H_INCLUDED

#include <cstring>

enum DIBType
{
    CORE,
    OS2,
    INFO,
    V2INFO,
    V3INFO,
    V4,
    V5
};

struct XYZ
{
    int X,Y,Z;
};

struct XYZTriple
{
    XYZ red;
    XYZ green;
    XYZ blue;
};

struct BMPDibHeader
{
    DIBType dibType;

    int headerSize;
    int width;
    int height;
    short planes;
    short bitCount;
    int compression;
    int imageSize;
    int xPixelsPerM;
    int yPixelsPerM;
    int colorsUsed;
    int colorsImportant;
    int redMask = 0x00FF0000;
    int greenMask = 0x0000FF00;
    int blueMask = 0x000000FF;
    int alphaMask = 0xFF000000;
    int csType;
    XYZTriple endPoints;
    int gammaRed;
    int gammaGreen;
    int gammaBlue;
    int intent;
    int profileData;
    int profileSize;
    int reserved;
} dibHeader;

void parseDIB(unsigned char *fileBuffer)
{
    memcpy(&dibHeader.headerSize, fileBuffer + 14, 4);
    memcpy(&dibHeader.width, fileBuffer + 18, 4);
    memcpy(&dibHeader.height, fileBuffer + 22, 4);
    memcpy(&dibHeader.planes, fileBuffer + 26, 2);
    memcpy(&dibHeader.bitCount, fileBuffer + 28, 2);

    switch(dibHeader.headerSize)
    {
        case 12:
            dibHeader.dibType = CORE;
            break;

            case 64:
            case 16:
            dibHeader.dibType = OS2;
            break;

            case 40:
            dibHeader.dibType = INFO;
            break;

            case 52:
            dibHeader.dibType = V2INFO;
            break;

            case 56:
            dibHeader.dibType = V3INFO;
            break;

            case 108:
            dibHeader.dibType = V4;
            break;

            case 124:
            dibHeader.dibType = V5;
            break;
    }

    if(dibHeader.dibType < INFO)
        return;

    memcpy(&dibHeader.compression, fileBuffer + 30, 4);
    memcpy(&dibHeader.imageSize, fileBuffer + 34, 4);
    memcpy(&dibHeader.xPixelsPerM, fileBuffer + 38, 4);
    memcpy(&dibHeader.yPixelsPerM, fileBuffer + 42, 4);
    memcpy(&dibHeader.colorsUsed, fileBuffer + 46, 4);
    memcpy(&dibHeader.colorsImportant, fileBuffer + 50, 4);

    if(dibHeader.dibType < V2INFO)
        return;

    memcpy(&dibHeader.redMask, fileBuffer + 54, 4);
    memcpy(&dibHeader.greenMask, fileBuffer + 58, 4);
    memcpy(&dibHeader.blueMask, fileBuffer + 62, 4);

    if(dibHeader.dibType < V3INFO)
        return;

    memcpy(&dibHeader.alphaMask, fileBuffer + 66, 4);

    if(dibHeader.dibType < V4)
        return;

    memcpy(&dibHeader.endPoints.red.X, fileBuffer + 70, 4);
    memcpy(&dibHeader.endPoints.red.Y, fileBuffer + 74, 4);
    memcpy(&dibHeader.endPoints.red.Z, fileBuffer + 78, 4);

    memcpy(&dibHeader.endPoints.green.X, fileBuffer + 82, 4);
    memcpy(&dibHeader.endPoints.green.Y, fileBuffer + 86, 4);
    memcpy(&dibHeader.endPoints.green.Z, fileBuffer + 90, 4);

    memcpy(&dibHeader.endPoints.blue.X, fileBuffer + 94, 4);
    memcpy(&dibHeader.endPoints.blue.Y, fileBuffer + 98, 4);
    memcpy(&dibHeader.endPoints.blue.Z, fileBuffer + 102, 4);

    memcpy(&dibHeader.gammaRed, fileBuffer + 106, 4);
    memcpy(&dibHeader.gammaGreen, fileBuffer + 110, 4);
    memcpy(&dibHeader.gammaBlue, fileBuffer + 114, 4);

    if(dibHeader.dibType < V5)
        return;

    memcpy(&dibHeader.intent, fileBuffer + 118, 4);
    memcpy(&dibHeader.profileData, fileBuffer + 122, 4);
    memcpy(&dibHeader.profileSize, fileBuffer + 126, 4);
    memcpy(&dibHeader.reserved, fileBuffer + 130, 4);
}

#endif // BMP_DIB_H_INCLUDED
