// data.cpp

#include "data.h"

//
int readBinaryData(char *filename, unsigned char *&p, unsigned int &size)
{
    FILE *fp;

    if((fp = fopen(filename , "rb" ))==NULL)
    {
        printf("fopen FAILURE\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    try
    {
        p = (unsigned char *)malloc(sizeof(unsigned char)*size);
    }
    catch(...)
    {
        cout<<"failed to alloc memory\n";
        return -1;
    }

    if(p == NULL)
    {
        printf("malloc FAILURE\n");
        return -1;
    }

    fread(p,sizeof(unsigned char),size,fp);

    fclose(fp);

    //
    return 0;
}

//
int writeBinaryData(char *filename, unsigned char *p, unsigned int size)
{
    //
    if(p == NULL)
    {
        printf("Invalid data pointer\n");
        return -1;
    }

    //
    FILE *fp;

    if((fp = fopen(filename , "wb" ))==NULL)
    {
        printf("fopen FAILURE\n");
        return -1;
    }

    fwrite(p,sizeof(unsigned char),size,fp);

    fclose(fp);

    //
    if(p)
    {
        delete []p;
    }

    //
    return 0;
}
