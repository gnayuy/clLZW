// data.h

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

using namespace std;

// read binary/compressed data
int readBinaryData(char *filename, unsigned char *&p, unsigned int &size);

// write binary/compressed data
int writeBinaryData(char *filename, unsigned char *p, unsigned int size);
