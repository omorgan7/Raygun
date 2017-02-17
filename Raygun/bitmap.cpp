//
//  bitmap.cpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017->
//  Copyright © 2017 Owen Morgan-> All rights reserved->
//

#include <stdio.h>
#include <fstream>
#include <iostream>
#include "bitmap.hpp"

void fillBitmapStruct(WINBMPFILEHEADER * FileHeader, WIN3XBITMAPHEADER *BitmapHeader, int width, int height){
    FileHeader->FileType = 0x4d42;//BMP magic number
    FileHeader->FileSize = sizeof(_Win3xBitmapHeader)+ sizeof(_WinBMPFileHeader) + width*height*3;
    FileHeader->Reserved1 = 0;
    FileHeader->Reserved2 = 0;
    FileHeader->BitmapOffset = sizeof(_WinBMPFileHeader)+sizeof(_Win3xBitmapHeader);
    BitmapHeader->Size = sizeof(_Win3xBitmapHeader);
    BitmapHeader->Width = width;
    BitmapHeader->Height = height;
    BitmapHeader->Planes = 1;
    BitmapHeader->BitsPerPixel = 24;
    BitmapHeader->Compression = 0;
    BitmapHeader->SizeOfBitmap = FileHeader->FileSize;
    BitmapHeader->HorzResolution = 2835;
    BitmapHeader->VertResolution = 2835;
    BitmapHeader->ColorsUsed = 0;
    BitmapHeader->ColorsImportant = 0;
}
void writeBitmapHeaderToStream(WINBMPFILEHEADER * FileHeader, WIN3XBITMAPHEADER *BitmapHeader, std::ofstream *ofs){
    *ofs<<(FileHeader->FileType);
    *ofs<<(FileHeader->FileSize);
    *ofs<<(FileHeader->Reserved1);
    *ofs<<(FileHeader->Reserved2);
    *ofs<<(FileHeader->BitmapOffset);
    *ofs<<(BitmapHeader->Size);
    *ofs<<(BitmapHeader->Width);
    *ofs<<(BitmapHeader->Height);
    *ofs<<(BitmapHeader->Planes);
    *ofs<<(BitmapHeader->BitsPerPixel);
    *ofs<<(BitmapHeader->Compression);
    *ofs<<(BitmapHeader->SizeOfBitmap);
    *ofs<<(BitmapHeader->HorzResolution);
    *ofs<<(BitmapHeader->VertResolution);
    *ofs<<(BitmapHeader->ColorsUsed);
    *ofs<<(BitmapHeader->ColorsImportant);
}
