#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>

FILE *openInputFile(const char *filename);
FILE *createOutputFile(const char *filename);

#endif
