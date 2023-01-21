#ifndef BMP_COLORTABLE_H_INCLUDED
#define BMP_COLORTABLE_H_INCLUDED

#include <cmath>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include "bmp_dib.h"

/*struct color
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
};*/

void parseColorTable(unsigned char *fileBuffer, color colors[], int colorsUsed)
{
    //colorTable present
    for(int i = 0; i<=colorsUsed;i++)
    {
        colors[i].blue = fileBuffer[14 + dibHeader.headerSize + i*4];
        colors[i].green = fileBuffer[14 + dibHeader.headerSize + i*4 + 1];
        colors[i].red = fileBuffer[14 + dibHeader.headerSize + i*4 + 2];
    }
}

#endif // BMP_COLORTABLE_H_INCLUDED

