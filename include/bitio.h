#ifndef BITIO_H
#define BITIO_H

#include <stdio.h>

typedef struct
{
	FILE *file;
	unsigned char buffer;
	int bitCount;
} BitWriter;

typedef struct
{
	FILE *file;
	unsigned char buffer;
	int bitCount;
} BitReader;

void initBitWriter(BitWriter *Writer, FILE *file);
void writeBit(BitWriter *writer, int bit);
void flushBits(BitWriter *writer);

void initBitReader(BitReader *reader, FILE *file);
int readBit(BitReader *reader);

#endif
