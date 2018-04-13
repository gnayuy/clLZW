// tiffimageio.cpp

//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <set>
#include <time.h>
#include <errno.h>
#include <queue>
#include <dirent.h>
#include <cstring>
#include <math.h>

//#include <omp.h>

#include "tiffio.h"
#include "tiffio.hxx"

using namespace std;






//
int main(int argc, char*argv[])
{
    //
    char *filename = const_cast<char*>(string(argv[1]).c_str());

    //
    TIFFSetWarningHandler(0);
    TIFFSetErrorHandler(0);

    //
    TIFF *input = TIFFOpen(filename,"r");
    if (!input)
    {
        cout<<"cannot open TIFF file "<<filename<<endl;
        return -1;
    }

    //
    uint32 width, length;

    if (!TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &width))
    {
        TIFFClose(input);
        return -1;
    }

    if (!TIFFGetField(input, TIFFTAG_IMAGELENGTH, &length))
    {
        TIFFClose(input);
        return -1;
    }

    uint32 rowsperstrip;

    if (!TIFFGetField(input, TIFFTAG_ROWSPERSTRIP, &rowsperstrip))
    {
        TIFFClose(input);
        return -1;
    }

    tsize_t stripsize  = TIFFStripSize(input);

    cout<<"image size "<<width<<"x"<<length<<endl;
    cout<<"strip size "<<stripsize<<endl;
    cout<<"rowsperstrip "<<rowsperstrip<<endl;

    uint32 bitspersample;
    if (!TIFFGetField(input, TIFFTAG_BITSPERSAMPLE, &bitspersample))
    {
        TIFFClose(input);
        return -1;
    }

    cout<<"bitspersample "<<bitspersample<<endl;

    tstrip_t s, ns = TIFFNumberOfStrips(input);
    uint32 row = 0;
    //tdata_t buf = _TIFFmalloc(stripsize);
    for (s = 0; s < ns && row < length; s++)
    {
        tsize_t cc = (row + rowsperstrip > length) ? TIFFVStripSize(input, length - row) : stripsize;

        cout<<"cc "<<cc<<" - "<<s<<endl;

//        if(TIFFReadEncodedStrip(input, s, buf, cc) < 0)
//        {
//            _TIFFfree(buf);
//            TIFFClose(input);
//            return -1;
//        }

        row += rowsperstrip;
    }


    //
    //_TIFFfree(buf);

    //
    TIFFClose(input);


    //
    return 0;
}
