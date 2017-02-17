//
//  bitmap.hpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#ifndef bitmap_hpp
#define bitmap_hpp

typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef long LONG;
typedef unsigned char BYTE;
typedef struct _WinBMPFileHeader
{
    WORD   FileType;     /* File type, always 4D42h ("BM") */
    DWORD  FileSize;     /* Size of the file in bytes */
    WORD   Reserved1;    /* Always 0 */
    WORD   Reserved2;    /* Always 0 */
    DWORD  BitmapOffset; /* Starting position of image data in bytes */
} WINBMPFILEHEADER;

typedef struct _Win3xBitmapHeader
{
    DWORD Size;            /* Size of this header in bytes */
    LONG  Width;           /* Image width in pixels */
    LONG  Height;          /* Image height in pixels */
    WORD  Planes;          /* Number of color planes */
    WORD  BitsPerPixel;    /* Number of bits per pixel */
    /* Fields added for Windows 3.x follow this line */
    DWORD Compression;     /* Compression methods used */
    DWORD SizeOfBitmap;    /* Size of bitmap in bytes */
    LONG  HorzResolution;  /* Horizontal resolution in pixels per meter */
    LONG  VertResolution;  /* Vertical resolution in pixels per meter */
    DWORD ColorsUsed;      /* Number of colors in the image */
    DWORD ColorsImportant; /* Minimum number of important colors */
} WIN3XBITMAPHEADER;

typedef struct _Win3xPaletteElement
{
    BYTE Blue;      /* Blue component */
    BYTE Green;     /* Green component */
    BYTE Red;       /* Red component */
    BYTE Reserved;  /* Padding (always 0) */
} WIN3XPALETTEELEMENT;

void fillBitmapStruct(_WinBMPFileHeader * FileHeader, _Win3xBitmapHeader *BitmapHeader, int width, int height);
void writeBitmapHeaderToStream(_WinBMPFileHeader * FileHeader, _Win3xBitmapHeader *BitmapHeader, std::ofstream *ofs);

#endif /* bitmap_hpp */
