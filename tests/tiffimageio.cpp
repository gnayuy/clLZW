// tiffimageio.cpp

//
#include "tiffimageio.h"

//
TIFFIO::TIFFIO(string filepath)
{
    TIFFSetWarningHandler(0);
    TIFFSetErrorHandler(0);

    filename = filepath;

    p = NULL;

    readHeaders = false;
}
TIFFIO::~TIFFIO()
{
    if(p)
    {
        delete []p;
    }
}

int TIFFIO::getHeader()
{
    //
    TIFF *input = TIFFOpen(filename.c_str(),"r");
    if (!input)
    {
        cout<<"cannot open TIFF file "<<filename<<endl;
        return -1;
    }

    //
    if(!TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &width))
    {
        TIFFClose(input);
        return -1;
    }

    if(!TIFFGetField(input, TIFFTAG_IMAGELENGTH, &height))
    {
        TIFFClose(input);
        return -1;
    }

    if(!TIFFGetField(input, TIFFTAG_ROWSPERSTRIP, &rowsperstrip))
    {
        TIFFClose(input);
        return -1;
    }

    stripsize = TIFFStripSize(input);
    nstrips = TIFFNumberOfStrips(input);

    if(!TIFFGetField(input, TIFFTAG_BITSPERSAMPLE, &bitspersample))
    {
        TIFFClose(input);
        return -1;
    }

    if(!TIFFGetField(input, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel))
    {
        samplesperpixel = 1;
    }

    if(!TIFFGetField(input, TIFFTAG_PAGENUMBER, &currentpage, &depth))
    {
        depth = 1;
        while ( TIFFReadDirectory(input) )
        {
            depth++;
        }
    }

    //
    if(!TIFFGetFieldDefaulted(input, TIFFTAG_COMPRESSION, &compression))
    {
        TIFFClose(input);
        return -1;
    }

    //
    if(TIFFIsTiled(input))
    {
        TIFFClose(input);
        cout<<"Tiled TIFF image is not supported"<<endl;
        return -1;
    }

    //
    if(!TIFFGetField(input, TIFFTAG_PLANARCONFIG, &config))
    {
        TIFFClose(input);
        return -1;
    }

    if(config!=PLANARCONFIG_CONTIG || !(compression==COMPRESSION_NONE || compression==COMPRESSION_LZW))
    {
        cout<<"Only supports PLANARCONFIG_CONTIG and COMPRESSION_LZW"<<endl;
        return -1;
    }

    //
    TIFFClose(input);

    //
    readHeaders = true;

    //
    return 0;
}

int TIFFIO::read()
{
    //
    if(readHeaders==false)
    {
        getHeader();
    }

    //
    long size = width*height*depth*samplesperpixel*bitspersample/8;

    try
    {
        p = new unsigned char [size];
    }
    catch(...)
    {
        cout<<"failed alloc memory"<<endl;
        return -1;
    }

    //
    TIFF *input = TIFFOpen(filename.c_str(),"r");
    if (!input)
    {
        cout<<"cannot open TIFF file "<<filename<<endl;
        return -1;
    }

    //
    for(uint32 z=0; z<depth; z++)
    {
        //
        uint8* bufp = p + z*width*height*samplesperpixel*bitspersample/8;

        _TIFFmemset(bufp, 0, stripsize);

        tstrip_t s;
        uint32 row = 0;

        for (s = 0; s < nstrips; s++)
        {
            tsize_t cc = (row + rowsperstrip > height) ? TIFFVStripSize(input, height - row) : stripsize;
            if (TIFFReadEncodedStrip(input, s, bufp, cc) < 0)
            {
                TIFFError(TIFFFileName(input), "Error, can't read strip %lu", (unsigned long) s);
            }
            row += rowsperstrip;
            bufp += cc;

            //cout<<"stripsize "<<cc<<" at strip #"<<s<<endl;
        }

        if(depth > 1)
        {
            TIFFReadDirectory(input);
        }
    }

    //
    TIFFClose(input);

    //
    return 0;
}

int TIFFIO::write()
{
    //
    TIFF *output = TIFFOpen(filename.c_str(),"w");
    if (!output)
    {
        cout<<"cannot open TIFF file "<<filename<<endl;
        return -1;
    }

    //
    if(depth > 1)
    {
        TIFFCreateDirectory(output);
    }

    for (uint32 page = 0; page < depth; page++ )
    {
        //
        uint8* bufp = p + page*width*height*samplesperpixel*bitspersample/8;

        //
        TIFFSetDirectory(output, page);
        TIFFSetField(output, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(output, TIFFTAG_IMAGELENGTH, height);
        TIFFSetField(output, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        TIFFSetField(output, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
        TIFFSetField(output, TIFFTAG_BITSPERSAMPLE, bitspersample);
        TIFFSetField(output, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

        if(bitspersample==8)
        {
            TIFFSetField(output, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
        }
        else if(bitspersample==16)
        {
            TIFFSetField(output, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
        }
        else
        {
            cout<<"Data type is not supported"<<endl;
            return -1;
        }

        TIFFSetField(output, TIFFTAG_SOFTWARE, "clTIFFimageIO gnayuy 2018"); //

        TIFFSetField(output, TIFFTAG_COMPRESSION, compression);

        uint16 photometric = ( samplesperpixel == 1 ) ? PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB;

        int predictor;
        if ( compression == COMPRESSION_LZW )
        {
            predictor = 2;
            TIFFSetField(output, TIFFTAG_PREDICTOR, predictor);
        }

        TIFFSetField(output, TIFFTAG_PHOTOMETRIC, photometric);

        //TIFFSetField( output, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(output, rowsperstrip) );
        TIFFSetField( output, TIFFTAG_ROWSPERSTRIP, (rowsperstrip == -1) ? height : rowsperstrip);

        if ( depth > 1 )
        {
            TIFFSetField(output, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
            TIFFSetField(output, TIFFTAG_PAGENUMBER, page, depth);
        }
        if ( rowsperstrip == -1 )
        {
            TIFFSetDirectory(output,0);
            if(!TIFFWriteEncodedStrip(output, 0, bufp, width*height*samplesperpixel*bitspersample/8))
            {
                cout<<"fail to write encoded strip"<<endl;
                return -1;
            }
        }
        else
        {
            tstrip_t s;
            uint32 row = 0;

            for (s = 0; s < nstrips; s++)
            {
                tsize_t cc = (row + rowsperstrip > height) ? TIFFVStripSize(output, height - row) : stripsize;
                if (TIFFWriteEncodedStrip(output, s, bufp, cc) < 0)
                {
                    TIFFError(TIFFFileName(output), "Error, can't read strip %lu", (unsigned long) s);
                }
                row += rowsperstrip;
                bufp += cc;
            }
        }

        if ( depth > 1)
        {
            TIFFWriteDirectory(output);
        }
    }

    //
    TIFFClose(output);

    //
    return 0;
}

//
int main(int argc, char*argv[])
{
    if(argc<1)
    {
        cout<<"please put a tiff file name as an input"<<endl;
        return -1;
    }

    //
    TIFFIO tiffimage((string(argv[1])));
    tiffimage.read();

    if(argc>3)
    {
        tiffimage.filename = string(argv[2]);

        tiffimage.compression = atoi(argv[3]); // 1 or 5

        tiffimage.write();
    }

    //
    return 0;
}
