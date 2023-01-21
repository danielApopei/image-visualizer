#ifndef BMP_H_INCLUDED
#define BMP_H_INCLUDED

#include <stdio.h>
#include <iostream>
#include "bmp_header.h"
#include "bmp_dib.h"
#include "bmp_colorTable.h"
#include "bmp_display.h"
#include <stdexcept>

//Modificare: Am separat openimagefile in openbmp si displaybmp
//Pentru ca programul sa obtina mai intai date despre bmp de la openbmp
//Si sa dea display la UI cum trebuie, iar abia apoi da display
//Acolo unde cere

void displaybmp(const char* filePath, int x, int y, bool (*onPixelDisplayed)())
{
    //Adaugat: display la x si y

    FILE* file = fopen(filePath, "rb");

    if(file == NULL)
        throw std::runtime_error("File does not exist!");

    fseek(file, 0, SEEK_END);

    int fileSize = ftell(file);

    unsigned char* fileBuffer = new unsigned char[fileSize];

    fseek(file, 0, SEEK_SET);

    fread(fileBuffer, fileSize, 1, file);

    int bitCount = dibHeader.bitCount;
    int colorsUsed = 0;
    if(bitCount<=8)
        colorsUsed = pow(2,bitCount);
    color colors[colorsUsed];
    if(bitCount<=8)
        parseColorTable(fileBuffer, colors, colorsUsed);
    std::cout<<"Detected format: BMP "<<bitCount<<"-bit\n";

    displayBMP(fileBuffer, colors, x, y, onPixelDisplayed);

    fclose(file);

    delete(fileBuffer);
}

bool isbmp(const char* filePath)
{
    FILE* file = fopen(filePath, "rb");

    if(file == NULL)
        throw std::runtime_error("File does not exist!");

    int fileSize = 2;

    unsigned char* fileBuffer = new unsigned char[fileSize];

    fseek(file, 0, SEEK_SET);

    fread(fileBuffer, fileSize, 1, file);

    return fileBuffer[0] == 'B' && fileBuffer[1] == 'M';
}

void openbmp(const char* filePath)
{
    FILE* file = fopen(filePath, "rb");

    if(file == NULL)
        throw std::runtime_error("File does not exist!");

    fseek(file, 0, SEEK_END);

    int fileSize = ftell(file);

    unsigned char* fileBuffer = new unsigned char[fileSize];

    fseek(file, 0, SEEK_SET);

    fread(fileBuffer, fileSize, 1, file);

    parseBMPHeader(fileBuffer);

    parseDIB(fileBuffer);

    fclose(file);

    delete(fileBuffer);
}

#endif // BMP_H_INCLUDED

