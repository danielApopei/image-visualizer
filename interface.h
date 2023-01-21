#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include <winbgim.h>
#include "bmp_dib.h"
#include "bmp.h"
#include <stdio.h>
#include <windows.h>

enum imageType
{
    bmp,
    png
} openedImageType;

int extraWidth = 275, extraHeight = 50;

int imageOffsetX = 25, imageOffsetY = 25;

int width = 512, height = 512;

int canvasWidth = 512, canvasHeight = 512;

int openedFiles = 0;

enum ImageType
{
    None,
    BMP,
    PNG
} imageType;

struct Button
{
    int x,y,width,height;
    bool hovered;

    void (*onClick)();
} openButton, previousButton, nextButton;
// Am definit astfel butonul pentru a adauga
// Si alte butoane ulterior, de ex: Next, Previous


//Desenez background
void background()
{
    setbkcolor(COLOR(55,71,79));

    clearviewport();
}

// Desenez canvas, sau mai bine spus
// Zona delimitata pentru afisarea imaginii
void canvas(int x, int y, int width, int height)
{
    int padding = 5;

    setfillstyle(1, COLOR(38,50,56));

    bar(x - padding, y - padding, x + width + padding, y + height + padding);

    setfillstyle(1, COLOR(84,110,122));

    bar(x, y, x + width, y + height);
}

// Deseneaza un dreptunghi cu border radius si thickness
// Nu am folosit vreo formula anume, pur si simplu am
// facut trial and error pana mi-a iesit ce am vrut
void drawRoundedRect(int x, int y, int width, int height, int borderRadius, int thickness, int color)
{
    setcolor(color);
    setfillstyle(1, color);

    // Colturile rotunde
    arc(x + borderRadius, y + borderRadius, 90, 180, borderRadius);
    arc(x + borderRadius, y + borderRadius, 90, 180, borderRadius - thickness);

    arc(x + width - borderRadius + 1, y + borderRadius, 0, 90, borderRadius);
    arc(x + width - borderRadius + 1, y + borderRadius, 0, 90, borderRadius - thickness);

    arc(x + borderRadius, y + height - borderRadius + 1, 180, 270, borderRadius);
    arc(x + borderRadius, y + height - borderRadius + 1, 180, 270, borderRadius - thickness);

    arc(x + width - borderRadius + 1, y + height - borderRadius + 1, 270, 360, borderRadius);
    arc(x + width - borderRadius + 1, y + height - borderRadius + 1, 270, 360, borderRadius - thickness);

    // Liniile drepte
    for(int i = 0; i <= thickness; i+=thickness)
    {
        line(x + borderRadius, y + i, x + width - borderRadius, y + i);
        line(x + i, y + borderRadius, x + i, y + height - borderRadius);
        line(x + borderRadius, y + height - i, x + width - borderRadius, y + height - i);
        line(x + width - i, y + borderRadius, x + width - i, y + height - borderRadius);
    }

    // Fill cu culoare
    floodfill(x + borderRadius, y + thickness / 2, color);
    floodfill(x + width / 2, y + height / 2, color);
}

void drawLeftTriangle(int x, int y, int width, int height, int color)
{
    setcolor(color);
    setfillstyle(1, color);

    line(x, y + height / 2, x + width, y + height);
    line(x, y + height / 2, x + width, y);
    line(x + width, y + height, x + width, y);

    floodfill(x + width / 2, y + height / 2, color);
}

void drawRightTriangle(int x, int y, int width, int height, int color)
{
    setcolor(color);
    setfillstyle(1, color);

    line(x + width, y + height / 2, x, y + height);
    line(x + width, y + height / 2, x, y);
    line(x, y + height, x, y);

    floodfill(x + width / 2, y + height / 2, color);
}

// Verifica daca mouse-ul este sau nu peste buton
// Daca state-ul difera de cel anterior, returneaza true
// Si actualizeaza starea butonului
bool checkHover(Button &button)
{
    int mouseX = mousex(), mouseY = mousey();

    bool difference = false;

    bool hover = mouseX >= button.x && mouseX <= (button.x + button.width) &&
    mouseY >= button.y && mouseY <= (button.y + button.height);

    if(hover != button.hovered)
        difference = true;

    button.hovered = hover;

    return difference;
}

void drawPreviousButton()
{
    int color = COLOR(84,110,122);

    if(previousButton.hovered)
    {
        color = COLOR(120,144,156);
    }

    drawLeftTriangle(previousButton.x, previousButton.y, previousButton.width, previousButton.height, color);
}

void drawNextButton()
{
    int color = COLOR(84,110,122);

    if(nextButton.hovered)
    {
        color = COLOR(120,144,156);
    }

    drawRightTriangle(nextButton.x, nextButton.y, nextButton.width, nextButton.height, color);
}

void drawButton()
{
    int border = 3;

    int color = COLOR(84,110,122);
    int borderColor = COLOR(38, 50, 56);
    int textColor = COLOR(236,239,241);

    if(openButton.hovered)
    {
        color = borderColor = COLOR(120,144,156);
    }

    //Obtin border-ul prin desenarea a doua dreptunghiuri
    drawRoundedRect(openButton.x - border, openButton.y - border, openButton.width + border * 2, openButton.height + border * 2, 25 + border, 5, borderColor);
    drawRoundedRect(openButton.x, openButton.y, openButton.width, openButton.height, 25, 5, color);

    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
    setbkcolor(color);
    setcolor(textColor);

    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(openButton.x + openButton.width/2, openButton.y + openButton.height/2 + 6, "Open Image");
}

bool uiUpdate()
{
    //Loop-ul pentru click si butoane

    // Daca am o stare noua, redraw la buton
    // Nu redesenez tot pentru ca dispare imaginea
    if(checkHover(openButton))
        drawButton();

    if(checkHover(previousButton))
        drawPreviousButton();

    if(checkHover(nextButton))
        drawNextButton();

    int previousFileCount = openedFiles;

    // Daca un buton are hover si dau click
    // Apelez functia butonului
    if(ismouseclick(WM_LBUTTONDOWN))
    {
        if(openButton.hovered)
            openButton.onClick();

        if(previousButton.hovered)
            previousButton.onClick();

        if(nextButton.hovered)
            nextButton.onClick();

        clearmouseclick(WM_LBUTTONDOWN);
    }

    return previousFileCount != openedFiles;
}

void initializeWindow()
{
    // Inchid fereastra anterioara, in caz ca exista
    closegraph();

    extraWidth = 285, extraHeight = 50;

    imageOffsetX = 30, imageOffsetY = 25;

    initwindow(width + extraWidth, height + extraHeight, "Image Visualizer");
}

void draw()
{
    // Desenez background-ul, canvas-ul si butonul
    background();

    canvas(imageOffsetX, imageOffsetY, canvasWidth, canvasHeight);

    openButton.width = 200;
    openButton.height = 75;

    openButton.x = imageOffsetX + width + 30;
    openButton.y = imageOffsetY + height - openButton.height;

    previousButton.width = 15;
    previousButton.height = 25;

    previousButton.x = 5;
    previousButton.y = height / 2;

    nextButton.width = 15;
    nextButton.height = 25;

    nextButton.x = imageOffsetX + width + 11;
    nextButton.y = height / 2;

    drawButton();

    drawPreviousButton();

    drawNextButton();
}

char lastOpenedFile[260] = {'\0'};

void redrawImage(char path[])
{
    if(isbmp(path))
    {
        // Daca am deschis imaginea, aflu date despre bmp
        // Reinitializez fereastra cu noile marimi si
        // Afisez imaginea

        openedImageType = bmp;

        openbmp(path);

        openedFiles++;

        strcpy(lastOpenedFile, path);

        width = max(256, dibHeader.width);
        height = max(256, dibHeader.height);

        canvasWidth = dibHeader.width;
        canvasHeight = dibHeader.height;

        initializeWindow();

        draw();

        displaybmp(path, imageOffsetX, imageOffsetY, uiUpdate);
    }
    else if(ispng(path))
    {
        // Daca am deschis imaginea, aflu date despre bmp
        // Reinitializez fereastra cu noile marimi si
        // Afisez imaginea

        openedImageType = png;

        readPNG(path);

        openedFiles++;

        strcpy(lastOpenedFile, path);

        width = max(256, myPNGfile.width);
        height = max(256, myPNGfile.height);

        canvasWidth = myPNGfile.width;
        canvasHeight = myPNGfile.height;

        initializeWindow();

        draw();

        decompressDataStream(imageOffsetX, imageOffsetY, uiUpdate);
    }
}

void previousFile()
{
    int index = strlen(lastOpenedFile) - 1;

    if(index == 0)
        return;

    while(lastOpenedFile[index] != '/' && lastOpenedFile[index] != '\\')
        index--;

    char dirPath[260] = {'\0'};

    char searchDir[260] = {'\0'};

    strncpy(dirPath, lastOpenedFile, index + 1);

    strncpy(searchDir, lastOpenedFile, index + 1);

    strcat(searchDir, "*");

    WIN32_FIND_DATA data;
    HANDLE file = FindFirstFile(searchDir, &data);

    char previousImageFile[260] = {'\0'};

    if (file != INVALID_HANDLE_VALUE) {

        do
        {
            int fileLength = strlen(data.cFileName);

            if(fileLength >= 4)
            {
                if((data.cFileName[fileLength - 4] == '.' &&
                   data.cFileName[fileLength - 3] == 'b' &&
                   data.cFileName[fileLength - 2] == 'm' &&
                   data.cFileName[fileLength - 1] == 'p') || (data.cFileName[fileLength - 4] == '.' &&
                   data.cFileName[fileLength - 3] == 'p' &&
                   data.cFileName[fileLength - 2] == 'n' &&
                   data.cFileName[fileLength - 1] == 'g'))
                {
                    if(strstr(lastOpenedFile, data.cFileName) != 0 && strlen(previousImageFile) > 0)
                    {
                        FindClose(file);

                        redrawImage(previousImageFile);

                        return;
                    }

                    strcpy(previousImageFile, dirPath);

                    strcat(previousImageFile, data.cFileName);
                }
            }
        }
        while (FindNextFile(file, &data));

        FindClose(file);
    }
}

void nextFile()
{
    int index = strlen(lastOpenedFile) - 1;

    if(index == 0)
        return;

    while(lastOpenedFile[index] != '/' && lastOpenedFile[index] != '\\')
        index--;

    char dirPath[260] = {'\0'};

    char searchDir[260] = {'\0'};

    strncpy(dirPath, lastOpenedFile, index + 1);

    strncpy(searchDir, lastOpenedFile, index + 1);

    strcat(searchDir, "*");

    WIN32_FIND_DATA data;
    HANDLE file = FindFirstFile(searchDir, &data);

    bool foundFile = false;

    if (file != INVALID_HANDLE_VALUE) {

        do
        {
            int fileLength = strlen(data.cFileName);

            if(fileLength >= 4)
            {
                if((data.cFileName[fileLength - 4] == '.' &&
                   data.cFileName[fileLength - 3] == 'b' &&
                   data.cFileName[fileLength - 2] == 'm' &&
                   data.cFileName[fileLength - 1] == 'p') || (data.cFileName[fileLength - 4] == '.' &&
                   data.cFileName[fileLength - 3] == 'p' &&
                   data.cFileName[fileLength - 2] == 'n' &&
                   data.cFileName[fileLength - 1] == 'g'))
                {
                    if(strstr(lastOpenedFile, data.cFileName) != 0)
                    {
                        foundFile = true;
                    }

                    else if(foundFile)
                    {
                        strcat(dirPath, data.cFileName);

                        FindClose(file);

                        redrawImage(dirPath);

                        return;
                    }
                }
            }
        }
        while (FindNextFile(file, &data));

        FindClose(file);
    }
}

void openFile()
{
    OPENFILENAME ofn = {0};

    char szFile[260]={0};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetFocus();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Images\0*.PNG;*.BMP\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (!GetOpenFileName ((LPOPENFILENAME)&ofn))
    {
        return; // Failed or cancelled
    }
    else
    {
        redrawImage(ofn.lpstrFile);
    }
}


void initializeInterface()
{
    openButton.onClick = openFile; // Asignez functia butonului
    previousButton.onClick = previousFile;
    nextButton.onClick = nextFile;

    initializeWindow();

    draw();

    while(true)
    {
        uiUpdate();
    }
}

#endif // INTERFACE_H_INCLUDED
