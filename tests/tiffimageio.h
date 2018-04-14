// tiffimageio.h

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

#include "tiffio.h"
#include "tiffio.hxx"

using namespace std;

//
class TIFFIO
{
public:
    TIFFIO(string filepath);
    ~TIFFIO();

public:
    int getHeader();

    int read();
    int write();

public:
    uint16 config;
    uint16 bitspersample, samplesperpixel;
    uint16 currentpage, compression;
    uint32 width, height, depth;
    uint32 rowsperstrip, stripsize, nstrips;

    string filename;
    unsigned char *p;

    bool readHeaders;
};


