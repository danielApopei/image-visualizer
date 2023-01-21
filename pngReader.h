#ifndef PNGREADER_H_INCLUDED
#define PNGREADER_H_INCLUDED
#include <cstring>
#include <stdexcept>
#include <iostream>
#include "tinf.h"
#include <graphics.h>
#include <winbgim.h>
#include <stdio.h>

/// PUTPIXEL PE LINIILE 295, 349, 396, 417

struct color
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
};

void putPixelRBGA(int x, int y, unsigned char foreground_red, unsigned char foreground_green, unsigned char foreground_blue, unsigned char alpha)
{
    int background = getpixel(x, y);

    int background_red = RED_VALUE(background);
    int background_green = GREEN_VALUE(background);
    int background_blue = BLUE_VALUE(background);

    float normalizedAlpha = alpha / 255.;

    int alphaRed = normalizedAlpha * foreground_red + (1 - normalizedAlpha) * background_red;
    int alphaGreen = normalizedAlpha * foreground_green + (1 - normalizedAlpha) * background_green;
    int alphaBlue = normalizedAlpha * foreground_blue + (1 - normalizedAlpha) * background_blue;

    putpixel(x, y, COLOR(alphaRed, alphaGreen, alphaBlue));
}

struct PNG
{
    ///general
    bool hasIHDR, hasPLTE, hasIEND;
    bool hastRNS;
    bool hascHRM, hasgAMA, hasiCCP, hassBIT, hassRGB;
    bool hastEXt, haszTXt, hasiTXt;
    bool hasbKGD, hashIST, haspHYs, hassPLT;
    bool hastIME;
    ///IHDR
    int width;
    int height;
    int bitDepth; //1 byte
    int colourType; //1 byte
    int compressionMethod; //1 byte
    int filterMethod; //1 byte
    int interlaceMethod; //1 byte
    ///PLTE
    color colorPalette[256]; // 3 bytes each
    int paletteSize;
    ///IDAT (concatenated data)
    unsigned char dataStream[6000000]; ///
    int dataStreamSize;
    ///sRGB
    int renderingIntent; // 1 byte
    ///gAMA
    float gamma;
    ///pHYs
    unsigned int pixelsPerUnitX;
    unsigned int pixelsPerUnitY;
    unsigned int unitSpecifier;
    ///cHRM
    float whitePointX;
    float whitePointY;
    float redX;
    float redY;
    float greenX;
    float greenY;
    float blueX;
    float blueY;
    ///tIME
    int year; //2 bytes
    int month; //1 byte
    int day; //1 byte
    int hour; //1 byte
    int minute; //1 byte
    int second; //1 byte

} myPNGfile;


void extractIHDR(unsigned char fileBuffer[], int offset, int totalSize)
{
    myPNGfile.hasIHDR = true;
    //for other chunk types: int dataSize = ((int)fileBuffer[offset]*16777216 + (int)fileBuffer[offset + 1]*65536 + (int)fileBuffer[offset + 2]*256 + (int)fileBuffer[offset + 3]);
    myPNGfile.width = ((int)fileBuffer[offset + 8]*16777216 + (int)fileBuffer[offset + 9]*65536 + (int)fileBuffer[offset + 10]*256 + (int)fileBuffer[offset + 11]);
    myPNGfile.height = ((int)fileBuffer[offset + 12]*16777216 + (int)fileBuffer[offset + 13]*65536 + (int)fileBuffer[offset + 14]*256 + (int)fileBuffer[offset + 15]);

    myPNGfile.bitDepth = (int)fileBuffer[offset + 16];
    myPNGfile.colourType = (int)fileBuffer[offset + 17];
    myPNGfile.compressionMethod = (int)fileBuffer[offset + 18];
    myPNGfile.filterMethod = (int)fileBuffer[offset + 19];
    myPNGfile.interlaceMethod = (int)fileBuffer[offset + 20];
    ///validating values for colorType & bitDepth
    if(myPNGfile.bitDepth!=1 && myPNGfile.bitDepth!=2 && myPNGfile.bitDepth!=4 && myPNGfile.bitDepth!=8 && myPNGfile.bitDepth!=16)
        std::runtime_error("IHDR ERROR - invalid bitDepth!");
    if(myPNGfile.colourType!=0 && myPNGfile.colourType!=2 && myPNGfile.colourType!=3 && myPNGfile.colourType!=4 && myPNGfile.colourType!=6)
        std::runtime_error("IHDR ERROR - invalid colorType!");
    if(myPNGfile.colourType == 3 && myPNGfile.bitDepth == 16)
        std::runtime_error("IHDR ERROR - invalid colorType-bitDepth combination!");
    if(myPNGfile.colourType == 2 || myPNGfile.colourType == 4 || myPNGfile.colourType == 6)
    {
        if(myPNGfile.bitDepth != 8 && myPNGfile.bitDepth != 16)
            std::runtime_error("IHDR ERROR - invalid colorType-bitDepth combination!");
    }
    unsigned int CRC32 = ((int)fileBuffer[offset + 21]*16777216 + (int)fileBuffer[offset + 22]*65536 + (int)fileBuffer[offset + 23]*256 + (int)fileBuffer[offset + 24]);
    //std::cout<<CRC32<<std::endl;
}

void extractPLTE(unsigned char fileBuffer[], int offset, int totalSize)
{
    myPNGfile.hasPLTE = true;
    ///dataSize must be a multiple of 3 (1 byte for each R G B)
    int dataSize = ((int)fileBuffer[offset]*16777216 + (int)fileBuffer[offset + 1]*65536 + (int)fileBuffer[offset + 2]*256 + (int)fileBuffer[offset + 3]);
    if(dataSize %3 != 0)
        std::runtime_error("PLTE ERROR - invalid palette size!");
    myPNGfile.paletteSize = dataSize / 3;
    std::cout<<"------- PLTE DEBUG: "<<myPNGfile.paletteSize<<"\n";
    for(int i=0; i<myPNGfile.paletteSize; i++)
    {
        myPNGfile.colorPalette[i].red = (int)fileBuffer[offset + 8 + i*3];
        myPNGfile.colorPalette[i].green = (int)fileBuffer[offset + 8 + i*3 + 1];
        myPNGfile.colorPalette[i].blue = (int)fileBuffer[offset + 8 + i*3 + 2];
    }
}

void extractIDAT(unsigned char fileBuffer[], int offset, int totalSize)
{
    //std::cout<<"Before IDAT\n";
    //std::cout<<"size of file buffer: "<<sizeof(fileBuffer)<<std::endl;
    ///a lot of work to do here...
    int dataSize = ((int)fileBuffer[offset]*16777216 + (int)fileBuffer[offset + 1]*65536 + (int)fileBuffer[offset + 2]*256 + (int)fileBuffer[offset + 3]);
    //std::cout<<"dataSize: "<<dataSize<<std::endl;
    for(int i=offset + 8; i<offset + 8 + dataSize; i++)
    {
        //std::cout<<i<<" "<<myPNGfile.dataStreamSize<<std::endl;
        myPNGfile.dataStream[myPNGfile.dataStreamSize] = fileBuffer[i];
        //std::cout<<"op done\n";
        myPNGfile.dataStreamSize++;
    }
    //std::cout<<"After IDAT\n";
}

void extractIEND(unsigned char fileBuffer[], int offset, int totalSize)
{
    myPNGfile.hasIEND = true;
    int dataSize = ((int)fileBuffer[offset]*16777216 + (int)fileBuffer[offset + 1]*65536 + (int)fileBuffer[offset + 2]*256 + (int)fileBuffer[offset + 3]);
    if(dataSize!=0)
        std::runtime_error("IEND ERROR - invalid dataSize!");
    unsigned int CRC32 = ((int)fileBuffer[offset + dataSize + 8]*16777216 + (int)fileBuffer[offset + dataSize + 9]*65536 + (int)fileBuffer[offset + dataSize + 10]*256 + (int)fileBuffer[offset + dataSize + 11]);
    if(CRC32 != 2923585666)
        std::runtime_error("IEND ERROR - invalid CRC-32!");
}

void extractsRGB(unsigned char fileBuffer[], int offset, int totalSize)
{
    myPNGfile.hassRGB = true;
    myPNGfile.renderingIntent = fileBuffer[offset+8];
    unsigned int CRC32 = ((int)fileBuffer[offset+9]*16777216 + (int)fileBuffer[offset+10]*65536 + (int)fileBuffer[offset+11]*256 + (int)fileBuffer[offset+12]);
}

void extractgAMA(unsigned char fileBuffer[], int offset, int totalSize)
{
    myPNGfile.hasgAMA = true;
    int preConversionGamma = ((int)fileBuffer[offset + 8]*16777216 + (int)fileBuffer[offset + 9]*65536 + (int)fileBuffer[offset + 10]*256 + (int)fileBuffer[offset + 11]);
    myPNGfile.gamma = preConversionGamma / 100000.0;
    unsigned int CRC32 = ((int)fileBuffer[offset+12]*16777216 + (int)fileBuffer[offset+13]*65536 + (int)fileBuffer[offset+14]*256 + (int)fileBuffer[offset+15]);
}

void extractpHYs(unsigned char fileBuffer[], int offset, int totalSize)
{
    myPNGfile.haspHYs = true;
    myPNGfile.pixelsPerUnitX = ((int)fileBuffer[offset + 8]*16777216 + (int)fileBuffer[offset + 9]*65536 + (int)fileBuffer[offset + 10]*256 + (int)fileBuffer[offset + 11]);
    myPNGfile.pixelsPerUnitY = ((int)fileBuffer[offset + 12]*16777216 + (int)fileBuffer[offset + 13]*65536 + (int)fileBuffer[offset + 14]*256 + (int)fileBuffer[offset + 15]);
    myPNGfile.unitSpecifier = (int)fileBuffer[offset + 16];
    unsigned int CRC32 = ((int)fileBuffer[offset+17]*16777216 + (int)fileBuffer[offset+18]*65536 + (int)fileBuffer[offset+19]*256 + (int)fileBuffer[offset+20]);
}

void extractcHRM(unsigned char fileBuffer[], int offset, int totalSize)
{
    myPNGfile.hascHRM = true;
    unsigned int whitePointX = ((int)fileBuffer[offset + 8]*16777216 + (int)fileBuffer[offset + 9]*65536 + (int)fileBuffer[offset + 10]*256 + (int)fileBuffer[offset + 11]);
    unsigned int whitePointY = ((int)fileBuffer[offset + 12]*16777216 + (int)fileBuffer[offset + 13]*65536 + (int)fileBuffer[offset + 14]*256 + (int)fileBuffer[offset + 15]);
    unsigned int redX = ((int)fileBuffer[offset + 16]*16777216 + (int)fileBuffer[offset + 17]*65536 + (int)fileBuffer[offset + 18]*256 + (int)fileBuffer[offset + 19]);
    unsigned int redY = ((int)fileBuffer[offset + 20]*16777216 + (int)fileBuffer[offset + 21]*65536 + (int)fileBuffer[offset + 22]*256 + (int)fileBuffer[offset + 23]);
    unsigned int greenX = ((int)fileBuffer[offset + 24]*16777216 + (int)fileBuffer[offset + 25]*65536 + (int)fileBuffer[offset + 26]*256 + (int)fileBuffer[offset + 27]);
    unsigned int greenY = ((int)fileBuffer[offset + 28]*16777216 + (int)fileBuffer[offset + 29]*65536 + (int)fileBuffer[offset + 30]*256 + (int)fileBuffer[offset + 31]);
    unsigned int blueX = ((int)fileBuffer[offset + 32]*16777216 + (int)fileBuffer[offset + 33]*65536 + (int)fileBuffer[offset + 34]*256 + (int)fileBuffer[offset + 35]);
    unsigned int blueY = ((int)fileBuffer[offset + 36]*16777216 + (int)fileBuffer[offset + 37]*65536 + (int)fileBuffer[offset + 38]*256 + (int)fileBuffer[offset + 39]);
    myPNGfile.whitePointX = whitePointX / 100000.0;
    myPNGfile.whitePointY = whitePointY / 100000.0;
    myPNGfile.redX = redX / 100000.0;
    myPNGfile.redY = redY / 100000.0;
    myPNGfile.greenX = greenX / 100000.0;
    myPNGfile.greenY = greenY / 100000.0;
    myPNGfile.blueX = blueX / 100000.0;
    myPNGfile.blueY = blueY / 100000.0;
    unsigned int CRC32 = ((int)fileBuffer[offset+40]*16777216 + (int)fileBuffer[offset+41]*65536 + (int)fileBuffer[offset+42]*256 + (int)fileBuffer[offset+43]);
}

void extracttIME(unsigned char fileBuffer[], int offset, int totalSize)
{
    myPNGfile.hastIME = true;
    myPNGfile.year = ((int)fileBuffer[offset + 8]*256 + (int)fileBuffer[offset + 9]);
    myPNGfile.month = (int)fileBuffer[offset + 10];
    myPNGfile.day = (int)fileBuffer[offset + 11];
    myPNGfile.hour = (int)fileBuffer[offset + 12];
    myPNGfile.minute = (int)fileBuffer[offset + 13];
    myPNGfile.second = (int)fileBuffer[offset + 14];
    unsigned int CRC32 = ((int)fileBuffer[offset+15]*16777216 + (int)fileBuffer[offset+16]*65536 + (int)fileBuffer[offset+17]*256 + (int)fileBuffer[offset+18]);
}

int paeth(int a, int b, int c)
{
    int p = a+b-c;
    int pa = abs(p-a);
    int pb = abs(p-b);
    int pc = abs(p-c);
    if(pa<=pb && pa <= pc) return a;
    if(pb<=pc) return b;
    return c;
}

void decompressDataStream(int x, int y, bool (*onPixelDisplayed)())
{
    std::cout<<"data stream size: "<<myPNGfile.dataStreamSize;
    ///afisare compresata
    /*std::cout<<std::endl<<std::endl;
    for(int i=0; i<myPNGfile.dataStreamSize; i++)
        std::cout<<i<<" - "<<(int)myPNGfile.dataStream[i]<<std::endl;*/

    unsigned int decompressedDataStreamSize = 150*myPNGfile.dataStreamSize;

    unsigned char *decompressedDataStream = new unsigned char[150*myPNGfile.dataStreamSize];

    tinf_uncompress(decompressedDataStream, &decompressedDataStreamSize, myPNGfile.dataStream+2, myPNGfile.dataStreamSize);

    std::cout<<"\nChecking decompression ("<<decompressedDataStreamSize<<"): \n";
    for(int i=0; i<1000; i++)
    {
        //std::cout<<i<<" > "<<(int)decompressedDataStream[i]<<std::endl;
        //std::cout<<(int)decompressedDataStream[9190+i]<<std::endl;
    }
    color lastRow[3000];

    memset(lastRow, 0, sizeof(lastRow));
    int myRed = 0, myGreen = 0, myBlue = 0, myAlpha = 0;
    int i = 1;
    int pixelX = 0, pixelY = 0;
    //initwindow(myPNGfile.width, myPNGfile.height);

    switch(myPNGfile.colourType)
    {
    case 6:
    {
        ///RGBA

        i = 0; int chunkType = 0;

        for(int pixelY = 0; pixelY < myPNGfile.height; pixelY++)
        {
            myRed = 0;
            myGreen = 0;
            myBlue = 0;
            myAlpha = 0;
            chunkType = decompressedDataStream[i];
            //std::cout<<pixelY<<" chunkType: "<<chunkType<<std::endl;
            i++;
            for(int pixelX = 0;pixelX < myPNGfile.width;pixelX++)
            {
                int auxBlue = myBlue;
                int lastlastRed, lastlastGreen, lastlastBlue, lastlastAlpha;
                int newRed = decompressedDataStream[i];
                int newGreen = decompressedDataStream[i+1];
                int newBlue = decompressedDataStream[i+2];
                int newAlpha = decompressedDataStream[i+3];
                if(chunkType == 4)
                {
                    // thisPixel = Paeth (pixelLeft, pixelAbove, pixelAboveLeft) + thisValue
                    if(pixelX>0)
                        myRed = (paeth(myRed, lastRow[pixelX].red, lastlastRed) + newRed)%256;
                    else
                        myRed = (paeth(myRed, lastRow[pixelX].red, 0) + newRed)%256;
                    if(pixelX>0)
                        myGreen = (paeth(myGreen, lastRow[pixelX].green, lastlastGreen) + newGreen)%256;
                    else
                        myGreen = (paeth(myGreen, lastRow[pixelX].green, 0) + newGreen)%256;
                    if(pixelX>0)
                    {
                        //std::cout<<"paeth("<<(int)myBlue<<", "<<(int)lastRow[pixelX].blue<<", "<<(int)lastRow[pixelX-1].blue<<") + "<<(int)newBlue<<std::endl;
                        myBlue = (paeth(myBlue, lastRow[pixelX].blue, lastlastBlue) + newBlue)%256;
                        //std::cout<<" = "<<(int)myBlue<<std::endl;
                    }
                    else
                        myBlue = (paeth(myBlue, lastRow[pixelX].blue, 0) + newBlue)%256;
                    if(pixelX>0)
                        myAlpha = (paeth(myAlpha, lastRow[pixelX].alpha, lastlastAlpha) + newAlpha)%256;
                    else
                        myAlpha = (paeth(myAlpha, lastRow[pixelX].alpha, 0) + newAlpha)%256;

                }
                else if(chunkType == 2)
                {
                    // thisPixel = pixelAbove + thisValue
                    myRed = (lastRow[pixelX].red + newRed) % 256;
                    myGreen = (lastRow[pixelX].green + newGreen) % 256;
                    myBlue = (lastRow[pixelX].blue + newBlue) % 256;
                    myAlpha = (lastRow[pixelX].alpha + newAlpha) % 256;
                }
                else if(chunkType == 3)
                {
                    // thisPixel = (pixelAbove + pixelLeft) // 2 + thisValue
                    myRed = ((myRed + lastRow[pixelX].red)/2 + newRed) % 256;
                    myGreen = ((myGreen + lastRow[pixelX].green)/2 + newGreen) % 256;
                    myBlue = ((myBlue + lastRow[pixelX].blue)/2 + newBlue) % 256;
                    myAlpha = ((myAlpha + lastRow[pixelX].alpha)/2 + newAlpha) % 256;
                }
                else if(chunkType == 1)
                {
                    // thisPixel = pixelLeft + thisValue
                    myRed = (myRed + newRed) % 256;
                    myGreen = (myGreen + newGreen) % 256;
                    myBlue = (myBlue + newBlue) % 256;
                    myAlpha = (myAlpha + newAlpha) % 256;
                }
                else
                {
                    //chunkType = 0
                    //thisPixel = thisValue (1 -> turns to 0, 0 -> value stays the same)
                    if(newRed == 1)
                        myRed = 0;
                    else if(newRed != 0)
                        myRed = newRed;
                    if(newGreen == 1)
                        myGreen = 0;
                    else if(newGreen != 0)
                        myGreen = newGreen;
                    if(newBlue == 1)
                        myBlue = 0;
                    else if(newBlue != 0)
                        myBlue = newBlue;
                    if(newAlpha == 1)
                        myAlpha = 0;
                    else if(newAlpha != 0)
                        myAlpha = newAlpha;

                }

                ///combining new color with last
                putPixelRBGA(x + pixelX, y + pixelY, myRed, myGreen, myBlue, myAlpha); /// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< AFISARE

                //putpixel(pixelX,pixelY,COLOR(myRed, myGreen, myBlue));
                //std::cout<<"PIXEL ("<<pixelX<<","<<pixelY<<") ("<<myRed<<", "<<myGreen<<", "<<myBlue<<", "<<myAlpha<<")\n";
                //std::cout<<myBlue<<" = Paeth("<<(int)auxBlue<<", "<<(int)lastRow[pixelX].blue<<", "<<(int)lastRow[pixelX-1].blue<<") + "<<(int)newBlue<<std::endl;
                //std::cout<<"adica "<<paeth(auxBlue, lastRow[pixelX].blue, lastRow[pixelX-1].blue) << " + "<<(int)newBlue <<" = "<<paeth(auxBlue, lastRow[pixelX].blue, lastRow[pixelX-1].blue) + newBlue<<std::endl;
                lastlastRed = lastRow[pixelX].red;
                lastlastGreen = lastRow[pixelX].green;
                lastlastBlue = lastRow[pixelX].blue;
                lastlastAlpha = lastRow[pixelX].alpha;

                lastRow[pixelX].red = myRed;
                lastRow[pixelX].green = myGreen;
                lastRow[pixelX].blue = myBlue;
                lastRow[pixelX].alpha = myAlpha;
                //std::cout<<"CHECK: "<<(int)lastRow[14].red<<" "<<(int)lastRow[14].green<<" "<<(int)lastRow[14].blue<<" "<<(int)lastRow[14].alpha<<"\n";
                i+=4;

                if(onPixelDisplayed())
                {
                    free(decompressedDataStream);
                    free(lastRow);

                    return;
                }
            }
        }
        break;
    }
    case 2:
    {
        ///RGB
        i = 0; int chunkType = 0;

        for(int pixelY = 0; pixelY < myPNGfile.height; pixelY++)
        {
            myRed = 0;
            myGreen = 0;
            myBlue = 0;
            chunkType = decompressedDataStream[i];
            //std::cout<<pixelY<<" chunkType: "<<chunkType<<std::endl;
            i++;
            for(int pixelX = 0;pixelX < myPNGfile.width;pixelX++)
            {
                int lastlastRed, lastlastGreen, lastlastBlue;
                int newRed = decompressedDataStream[i];
                int newGreen = decompressedDataStream[i+1];
                int newBlue = decompressedDataStream[i+2];
                if(chunkType == 4)
                {
                    // thisPixel = Paeth (pixelLeft, pixelAbove, pixelAboveLeft) + thisValue
                    if(pixelX>0)
                        myRed = (paeth(myRed, lastRow[pixelX].red, lastlastRed) + newRed)%256;
                    else
                        myRed = (paeth(myRed, lastRow[pixelX].red, 0) + newRed)%256;
                    if(pixelX>0)
                        myGreen = (paeth(myGreen, lastRow[pixelX].green, lastlastGreen) + newGreen)%256;
                    else
                        myGreen = (paeth(myGreen, lastRow[pixelX].green, 0) + newGreen)%256;
                    if(pixelX>0)
                    {
                        //std::cout<<"paeth("<<(int)myBlue<<", "<<(int)lastRow[pixelX].blue<<", "<<(int)lastRow[pixelX-1].blue<<") + "<<(int)newBlue<<std::endl;
                        myBlue = (paeth(myBlue, lastRow[pixelX].blue, lastlastBlue) + newBlue)%256;
                        //std::cout<<" = "<<(int)myBlue<<std::endl;
                    }
                    else
                        myBlue = (paeth(myBlue, lastRow[pixelX].blue, 0) + newBlue)%256;

                }
                else if(chunkType == 2)
                {
                    // thisPixel = pixelAbove + thisValue
                    myRed = (lastRow[pixelX].red + newRed) % 256;
                    myGreen = (lastRow[pixelX].green + newGreen) % 256;
                    myBlue = (lastRow[pixelX].blue + newBlue) % 256;
                }
                else if(chunkType == 3)
                {
                    // thisPixel = (pixelAbove + pixelLeft) // 2 + thisValue
                    myRed = ((myRed + lastRow[pixelX].red)/2 + newRed) % 256;
                    myGreen = ((myGreen + lastRow[pixelX].green)/2 + newGreen) % 256;
                    myBlue = ((myBlue + lastRow[pixelX].blue)/2 + newBlue) % 256;
                }
                else if(chunkType == 1)
                {
                    // thisPixel = pixelLeft + thisValue
                    myRed = (myRed + newRed) % 256;
                    myGreen = (myGreen + newGreen) % 256;
                    myBlue = (myBlue + newBlue) % 256;
                }
                else
                {
                    if(newRed == 1)
                        myRed = 0;
                    else if(newRed != 0)
                        myRed = newRed;
                    if(newGreen == 1)
                        myGreen = 0;
                    else if(newGreen != 0)
                        myGreen = newGreen;
                    if(newBlue == 1)
                        myBlue = 0;
                    else if(newBlue != 0)
                        myBlue = newBlue;

                }



                putpixel(x+pixelX,y+pixelY,COLOR(myRed, myGreen, myBlue)); /// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< AFISARE

                lastRow[pixelX].red = myRed;
                lastRow[pixelX].green = myGreen;
                lastRow[pixelX].blue = myBlue;
                lastRow[pixelX].alpha = myAlpha;
                //std::cout<<"PIXEL ("<<pixelX<<","<<pixelY<<") ("<<myRed<<", "<<myGreen<<", "<<myBlue<<")\n";
                i+=3;

                if(onPixelDisplayed())
                {
                    free(decompressedDataStream);
                    free(lastRow);

                    return;
                }
            }
        }
        break;
    }
    case 3:
    {
        ///Palette
        std::cout<<"The Palette of the image:\n";
        std::cout<<"palette size: "<<myPNGfile.paletteSize<<std::endl;
        for(int j=0; j<myPNGfile.paletteSize; j++)
            std::cout<<"pcolor "<<j<<" - "<<(int)myPNGfile.colorPalette[j].red<<" "<<(int)myPNGfile.colorPalette[j].green<<" "<<(int)myPNGfile.colorPalette[j].blue<<"\n";
        std::cout<<std::endl;
        switch(myPNGfile.bitDepth)
        {
        case 4:
        {
            //1 color is 4 bit (so really the index because this is palette)
            //00001111 index
            int parity = 0;
            while(pixelY < myPNGfile.height)
            {
                int index1 = ((decompressedDataStream[i] & 240) >> 4);
                int index2 = (decompressedDataStream[i] & 15);
                myRed = myPNGfile.colorPalette[index1].red;
                myGreen = myPNGfile.colorPalette[index1].green;
                myBlue = myPNGfile.colorPalette[index1].blue;
                std::cout<<"PIXEL ("<<pixelX<<","<<pixelY<<") ("<<myRed<<", "<<myGreen<<", "<<myBlue<<") (index1: "<<(int)((decompressedDataStream[i] & 240) >> 4)<<")\n";
                putpixel(x+pixelX, y+pixelY, COLOR(myRed, myGreen, myBlue)); /// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< AFISARE
                pixelX++;
                if(pixelX >= myPNGfile.width)
                {
                    pixelX=0;
                    pixelY++;
                    i++;
                }
                myRed = myPNGfile.colorPalette[index2].red;
                myGreen = myPNGfile.colorPalette[index2].green;
                myBlue = myPNGfile.colorPalette[index2].blue;
                //std::cout<<"PIXEL ("<<pixelX<<","<<pixelY<<") ("<<myRed<<", "<<myGreen<<", "<<myBlue<<") (index2: "<<(int)(decompressedDataStream[i] & 15)<<")\n";
                putpixel(x+pixelX, y+pixelY, COLOR(myRed, myGreen, myBlue)); /// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< AFISARE
                pixelX++;
                if(pixelX >= myPNGfile.width)
                {
                    pixelX=0;
                    pixelY++;
                    i++;
                }
                i++;

                if(onPixelDisplayed())
                {
                    free(decompressedDataStream);
                    free(lastRow);

                    return;
                }
            }
        }
        case 8:
        {
            while(pixelY < myPNGfile.height)
            {
                //00000000 index
                int index = decompressedDataStream[i];
                myRed = myPNGfile.colorPalette[index].red;
                myGreen = myPNGfile.colorPalette[index].green;
                myBlue = myPNGfile.colorPalette[index].blue;
                //std::cout<<"PIXEL ("<<pixelX<<","<<pixelY<<") ("<<myRed<<", "<<myGreen<<", "<<myBlue<<") (ds[index]: "<<(int)decompressedDataStream[i]<<")\n";
                putpixel(x+pixelX, y+pixelY, COLOR(myRed, myGreen, myBlue)); /// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< AFISARE
                pixelX++;
                if(pixelX >= myPNGfile.width)
                {
                    pixelX=0;
                    pixelY++;
                    i++;
                }
                i++;

                if(onPixelDisplayed())
                {
                    free(decompressedDataStream);
                    free(lastRow);

                    return;
                }
            }
            break;

        }
        }
    }
    }
    delete decompressedDataStream;
}

bool ispng(const char* filePath)
{
    FILE* file = fopen(filePath, "rb");
    if(file == NULL)
        throw std::runtime_error("File does not exist!");

    int fileSize = 4;
    unsigned char* fileBuffer = new unsigned char[fileSize];
    delete fileBuffer;
    fseek(file, 0, SEEK_SET);
    fread(fileBuffer, fileSize, 1, file);

    if(fileBuffer[1]!='P' || fileBuffer[2]!='N' || fileBuffer[3]!='G')
        return false;

    return true;
}

void readPNG(const char* filePath)
{
    myPNGfile.dataStreamSize = 0;

    ///OPEN FILE, COPY INTO BUFFER
    FILE* file = fopen(filePath, "rb");
    if(file == NULL)
        throw std::runtime_error("File does not exist!");
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    unsigned char* fileBuffer = new unsigned char[fileSize];
    fseek(file, 0, SEEK_SET);
    fread(fileBuffer, fileSize, 1, file);


    ///PREPARE FOR META-DATA EXTRACTION

    ///check if file has correct signature
    if(fileBuffer[1]!='P' || fileBuffer[2]!='N' || fileBuffer[3]!='G')
        throw std::runtime_error("File not PNG type!");

    ///jumping over 8-byte PNG signature
    int offset = 8;

    while(offset < fileSize)
    {
        std::cout<<">>> Program has reached here! <<<\n";
        std::cout<<offset<<" / "<<fileSize<<std::endl;
        ///calculating type and total size of current chunk
        int currentChunkSize = 12 + ((int)fileBuffer[offset]*16777216 + (int)fileBuffer[offset + 1]*65536 + (int)fileBuffer[offset + 2]*256 + (int)fileBuffer[offset + 3]);
        char currentChunkType[5] = {fileBuffer[offset+4],fileBuffer[offset+5], fileBuffer[offset+6],fileBuffer[offset+7], 0};
        std::cout<<currentChunkType<<" starting at "<<offset<<std::endl;

        ///selecting respective extractor to extract data
        if(strcmp(currentChunkType, "IHDR") == 0)
            extractIHDR(fileBuffer, offset, currentChunkSize);
        else if(strcmp(currentChunkType, "IDAT") == 0)
            extractIDAT(fileBuffer, offset, currentChunkSize);
        else if(strcmp(currentChunkType, "IEND") == 0)
            extractIEND(fileBuffer, offset, currentChunkSize);
        else if(strcmp(currentChunkType, "PLTE") == 0)
            extractPLTE(fileBuffer, offset, currentChunkSize);
        else if(strcmp(currentChunkType, "sRGB") == 0)
            extractsRGB(fileBuffer, offset, currentChunkSize);
        else if(strcmp(currentChunkType, "gAMA") == 0)
            extractgAMA(fileBuffer, offset, currentChunkSize);
        else if(strcmp(currentChunkType, "pHYs") == 0)
            extractpHYs(fileBuffer, offset, currentChunkSize);
        else if(strcmp(currentChunkType, "cHRM") == 0)
            extractcHRM(fileBuffer, offset, currentChunkSize);
        else if(strcmp(currentChunkType, "tIME") == 0)
            extracttIME(fileBuffer, offset, currentChunkSize);
        else std::cout<<"UNSUPPORTED CHUNK TYPE! Skipping...\n";
        offset += currentChunkSize;
    }

    ///AFTER META EXTRACT DEBUG
    std::cout<<"\n";
    std::cout<<"width: "<<myPNGfile.width<<std::endl;
    std::cout<<"height: "<<myPNGfile.height<<std::endl;
    std::cout<<"bitDepth: "<<myPNGfile.bitDepth<<std::endl;
    switch(myPNGfile.colourType)
    {
    case 0:
    {
        std::cout<<"colorType: 0 (Grayscale)\n";
        break;
    }
    case 2:
    {
        std::cout<<"colorType: 2 (RGB)\n";
        break;
    }
    case 3:
    {
        std::cout<<"colorType: 3 (Pallette)\n";
        break;
    }
    case 4:
    {
        std::cout<<"colorType: 4 (Grayscale + Alpha)\n";
        break;
    }
    case 6:
    {
        std::cout<<"colorType: 6 (RGBA)\n";
        break;
    }
    }
    std::cout<<"compressionMethod: "<<myPNGfile.compressionMethod<<std::endl;
    std::cout<<"filterMethod: "<<myPNGfile.filterMethod<<std::endl;
    std::cout<<"interlaceMethod: "<<myPNGfile.interlaceMethod<<std::endl;
    std::cout<<std::endl;

    delete fileBuffer;

    ///PREPARE FOR REVERSING FILTER-COMPRESS PROCESS ON dataStream
    //decompressDataStream();

    //getch();
}

#endif // PNGREADER_H_INCLUDED
