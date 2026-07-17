#include "../include/bitio.h"

void initBitWriter(BitWriter *writer, FILE *file)
{
	writer->file = file;
	writer->buffer = 0;
	writer->bitCount = 0;
}

void writeBit(BitWriter *writer, int bit)
{
	writer->buffer <<= 1;

	if (bit)
	{
		writer->buffer |= 1;
	}

	writer->bitCount++;

	if (writer->bitCount == 8)
	{
		fputc(writer->buffer, writer->file);

		writer->buffer = 0;
		writer->bitCount = 0;
	}
}

void flushBits(BitWriter *writer)
{
	if (writer->bitCount > 0)
	{
		writer->buffer <<= (8 - writer->bitCount);

		fputc(writer->buffer, writer->file);

		writer->buffer = 0;
		writer->bitCount = 0;
	}
}

void initBitReader(BitReader *reader, FILE *file)
{
	reader->file = file;
	reader->buffer = 0;
	reader->bitCount = 0;
}

int readBit(BitReader *reader)
{
	if (reader->bitCount == 0)
	{
		int byte = fgetc(reader->file);

		if (byte == EOF)
		{
			return -1;
		}

		reader->buffer = (unsigned char)byte;
		reader->bitCount = 8;
	}

	int bit = (reader->buffer & 0x80) ? 1 : 0;

	reader->buffer <<= 1;
	reader->bitCount--;

	return bit;
}
