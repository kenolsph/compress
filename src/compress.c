#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../include/compress.h"
#include "../include/huffman.h"
#include "../include/bitio.h"

void compressFile(const char *inputFile, const char *outputFile)
{
	FILE *input = fopen(inputFile, "rb");

	if (input == NULL)
	{
		printf("Error: could not open file %s\n", inputFile);
		return;
	}

	FILE *output = fopen(outputFile, "wb");

	if (output == NULL)
	{
		printf("Error: Cannot create output file %s\n", outputFile);
		fclose(input);
		return;
	}

	/* Work out the original file size up front, we store it in the header */
	fseek(input, 0, SEEK_END);
	long fileSizeLong = ftell(input);
	fseek(input, 0, SEEK_SET);

	if (fileSizeLong < 0)
	{
		printf("Error: could not determine size of %s\n", inputFile);
		fclose(input);
		fclose(output);
		return;
	}

	uint64_t fileSize = (uint64_t)fileSizeLong;

	/* Count byte frequencies */
	unsigned long frequency[256] = {0};
	int byte;

	while ((byte = fgetc(input)) != EOF)
	{
		frequency[(unsigned char)byte]++;
	}

	/* --- Header: magic bytes, original file size, 256-entry frequency table --- */
	fwrite("HUF1", 1, 4, output);
	fwrite(&fileSize, sizeof(uint64_t), 1, output);

	uint64_t freq64[256];

	for (int i = 0; i < 256; i++)
	{
		freq64[i] = (uint64_t)frequency[i];
	}

	fwrite(freq64, sizeof(uint64_t), 256, output);

	if (fileSize == 0)
	{
		/* Empty input, nothing more to write */
		fclose(input);
		fclose(output);
		return;
	}

	int distinctCount = 0;

	for (int i = 0; i < 256; i++)
	{
		if (frequency[i] > 0)
		{
			distinctCount++;
		}
	}

	if (distinctCount == 1)
	{
		/* Only one distinct byte value in the whole file. A real Huffman
		   tree needs at least two symbols, so there's nothing to encode -
		   the decompressor can rebuild the file from size + frequency alone. */
		fclose(input);
		fclose(output);
		return;
	}

	PriorityQueue *queue = buildPriorityQueueFromFrequency(frequency);

	if (queue == NULL)
	{
		fclose(input);
		fclose(output);
		return;
	}

	HuffmanNode *root = buildHuffmanTree(queue);
	free(queue);

	if (root == NULL)
	{
		printf("Error: failed to build Huffman tree.\n");
		fclose(input);
		fclose(output);
		return;
	}

	char code[256];
	generateCodes(root, code, 0);

	/* Re-read the file from the start and write the Huffman-coded bits */
	fseek(input, 0, SEEK_SET);

	BitWriter writer;
	initBitWriter(&writer, output);

	while ((byte = fgetc(input)) != EOF)
	{
		char *bits = huffmanCodes[(unsigned char)byte];

		for (int i = 0; bits[i] != '\0'; i++)
		{
			writeBit(&writer, bits[i] == '1' ? 1 : 0);
		}
	}

	flushBits(&writer);

	freeHuffmanTree(root);
	freeHuffmanCodes();

	fclose(input);
	fclose(output);
}
