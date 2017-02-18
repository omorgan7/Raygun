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
    FileHeader->FileSize = 54 + width*height*24;
    FileHeader->Reserved1 = 0;
    FileHeader->Reserved2 = 0;
    FileHeader->BitmapOffset = 54;
    BitmapHeader->Size = 40;
    BitmapHeader->Width = width;
    BitmapHeader->Height = height;
    BitmapHeader->Planes = 1;
    BitmapHeader->BitsPerPixel = 24;
    BitmapHeader->Compression = 0;
    BitmapHeader->SizeOfBitmap = width*height*24;
    BitmapHeader->HorzResolution = 2835;
    BitmapHeader->VertResolution = 2835;
    BitmapHeader->ColorsUsed = 0;
    BitmapHeader->ColorsImportant = 0;
}
void writeBitmapHeaderToStream(WINBMPFILEHEADER * FileHeader, WIN3XBITMAPHEADER *BitmapHeader, std::ofstream *ofs){
    ofs->write((const char*) (&FileHeader->FileType),sizeof(FileHeader->FileType));
    ofs->write((const char*)(&FileHeader->FileSize),sizeof(FileHeader->FileSize));
    ofs->write((const char*)(&FileHeader->Reserved1),sizeof(FileHeader->Reserved1));
    ofs->write((const char*)(&FileHeader->Reserved2),sizeof(FileHeader->Reserved2));
    ofs->write((const char*)(&FileHeader->BitmapOffset),sizeof(FileHeader->BitmapOffset));
    ofs->write((const char*) BitmapHeader,sizeof(WIN3XBITMAPHEADER));
}
