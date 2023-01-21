#ifndef BMP_DISPLAY_H_INCLUDED
#define BMP_DISPLAY_H_INCLUDED

#include <winbgim.h>
#include <stdio.h>
#include "bmp_header.h"
#include "bmp_colorTable.h"
#include "bmp_dib.h"
#include "utilities.h"

void displayBMP(unsigned char* fileBuffer, color colors[], int x, int y, bool (*onPixelDisplayed)())
{
    //Adaugare: Display la pozitia x,y
    //Adunam x si y la coordonatele calculate

    int pixelX = 0, pixelY = dibHeader.height;
    int currentPixel = header.pixelArrayOffset;
    int fileSize = header.sizeOfFile;

    int pixelIncrement = 1;

    switch(dibHeader.bitCount)
            {
                case 1:
                case 4:
                case 8:
                    {
                        pixelIncrement = 1;

                        break;
                    }

                case 16:
                    {
                        pixelIncrement=2;

                        break;
                    }

                case 24:
                    {
                        pixelIncrement=3;

                        break;
                    }

                case 32:
                    {
                        pixelIncrement=4;

                        break;
                    }
            }

    int padding = (4 - dibHeader.width * pixelIncrement % 4)%4;

    while(currentPixel < fileSize)
    {
        //Modificare: Calculam altfel currentPixel pentru bitCount < 8
        if(dibHeader.bitCount >= 8)
            currentPixel = header.pixelArrayOffset + (pixelX + (dibHeader.height - pixelY) * dibHeader.width) * pixelIncrement + padding * (dibHeader.height - pixelY);

        switch(dibHeader.bitCount)
        {
            case 1:
            {
                int pixelData = fileBuffer[currentPixel];

                for(int i = 7; i >= 0; i--)
                {
                        color color = colors[(pixelData & (1 << i)) >> i];

                        putpixel(x + pixelX, y + pixelY, COLOR(color.red, color.green, color.blue));

                        pixelX++;
                }

                break;
            }

            case 4:
                {
                    int pixelData = fileBuffer[currentPixel];

                    int leftPixel = ((pixelData & (1 << 4)) >> 4) +
                                    ((pixelData & (1 << 5)) >> 4)+
                                    ((pixelData & (1 << 6)) >> 4)+
                                    ((pixelData & (1 << 7)) >> 4);

                    int rightPixel = (pixelData & (1 << 0)) +
                                        (pixelData & (1 << 1))+
                                        (pixelData & (1 << 2))+
                                        (pixelData & (1 << 3));

                    color leftColor = colors[leftPixel];
                    color rightColor = colors[rightPixel];

                    putpixel(x + pixelX, y + pixelY, COLOR(leftColor.red, leftColor.green, leftColor.blue));
                    putpixel(x + pixelX + 1, y + pixelY, COLOR(rightColor.red, rightColor.green, rightColor.blue));

                    pixelX += 2;

                    break;
                }

            case 8:
                {
                    color color = colors[fileBuffer[currentPixel]];
                    putpixel(x + pixelX, y + pixelY, COLOR(color.red, color.green, color.blue));
                    pixelX++;

                    break;
                }

            case 16:
                {
                    int thisColor = fileBuffer[currentPixel]+fileBuffer[currentPixel+1]*256;
                    int blue = 8*((thisColor & 31));
                    int green = 8*((thisColor & 992)>>5);
                    int red = 8*((thisColor & 31744)>>10);

                    int alpha = 255;

                    if(dibHeader.compression == 3 && dibHeader.dibType > INFO)
                    {
                        red = bitmaskToValue(thisColor, dibHeader.redMask);
                        green = bitmaskToValue(thisColor, dibHeader.greenMask);
                        blue = bitmaskToValue(thisColor, dibHeader.blueMask);

                        if(dibHeader.dibType > V2INFO)
                            alpha = bitmaskToValue(thisColor, dibHeader.alphaMask);
                    }

                    putPixelRBGA(x + pixelX, y + pixelY, red, green, blue, alpha);

                    pixelX++;

                    break;
                }

            case 24:
                {
                    unsigned char blue = fileBuffer[currentPixel + 0];
                    unsigned char green = fileBuffer[currentPixel + 1];
                    unsigned char red = fileBuffer[currentPixel + 2];

                    putpixel(x + pixelX, y + pixelY, COLOR(red, green, blue));
                    pixelX++;

                    break;
                }

            case 32:
                {
                    //int red = fileBuffer[currentPixel];
                    //int green = fileBuffer[currentPixel + 3];
                    //int blue = fileBuffer[currentPixel + 2];

                    int thisColor;

                    memcpy(&thisColor, fileBuffer + currentPixel, 4);

                    int red = bitmaskToValue(thisColor, dibHeader.redMask);
                    int green = bitmaskToValue(thisColor, dibHeader.greenMask);
                    int blue = bitmaskToValue(thisColor, dibHeader.blueMask);

                    int alpha = bitmaskToValue(thisColor, dibHeader.alphaMask);

                    putPixelRBGA(x + pixelX, y + pixelY, red, green, blue, alpha);
                    pixelX++;

                    break;
                }

            default:{
                throw std::runtime_error("Unsupported bit count!");
                break;
            }
        }

        if(dibHeader.bitCount >= 8)
        {
            if(pixelX >= dibHeader.width)
            {
                pixelX = 0;
                pixelY--;
            }
        }
        else
        {
            //Modificare de la ultima data:
            //Am adaugat acest else ca sa calculez
            //Alfel currentPixel pentru bitCount < 8
            //Si apar corect imaginile

            if(pixelX >= dibHeader.width)
            {
                pixelX = 0;
                pixelY--;
                currentPixel = (currentPixel / 4 + 1) * 4;
            }
            else currentPixel++;
        }

        // Functiile care returneaza true intrerup display-ul
        // Fac asta pentru functiile care deschid noi fisiere
        if(onPixelDisplayed())
            return;
    }

    printf("Image succesfully displayed");
}
#endif // BMP_DISPLAY_H_INCLUDED
