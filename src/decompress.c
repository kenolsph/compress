#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../include/decompress.h"
#include "../include/huffman.h"
#include "../include/bitio.h"

void decompressFile(const char *inputFile, const char *outputFile)
{
	FILE *input = fopen(inputFile, "rb");

	if (input == NULL)
	{
		printf("Error: Cannot open input file %s\n", inputFile);
		return;
	}

	FILE *output = fopen(outputFile, "wb");

	if (output == NULL)
	{
		printf("Error: Cannot open output file %s\n", outputFile);
		fclose(input);
		return;
	}

	char header[5];

	if (fread(header, 1, 4, input) != 4)
	{
		printf("Invalid compressed file.\n");
		fclose(input);
		fclose(output);
		return;
	}

	header[4] = '\0';

	if (strcmp(header, "HUF1") != 0)
	{
		printf("Invalid compressed file.\n");
		fclose(input);
		fclose(output);
		return;
	}

	uint64_t fileSize;

	if (fread(&fileSize, sizeof(uint64_t), 1, input) != 1)
	{
		printf("Error: Corrupted compressed file (missing size).\n");
		fclose(input);
		fclose(output);
		return;
	}

	uint64_t freq64[256];

	if (fread(freq64, sizeof(uint64_t), 256, input) != 256)
	{
		printf("Error: Corrupted compressed file (missing frequency table).\n");
		fclose(input);
		fclose(output);
		return;
	}

	unsigned long frequency[256];
	int distinctCount = 0;
	int onlyByte = -1;

	for (int i = 0; i < 256; i++)
	{
		frequency[i] = (unsigned long)freq64[i];

		if (frequency[i] > 0)
		{
			distinctCount++;
			onlyByte = i;
		}
	}

	if (fileSize == 0)
	{
		fclose(input);
		fclose(output);
		return;
	}

	if (distinctCount == 1)
	{
		for (uint64_t i = 0; i < fileSize; i++)
		{
			fputc(onlyByte, output);
		}

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
		printf("Error: failed to rebuild Huffman tree.\n");
		fclose(input);
		fclose(output);
		return;
	}

	BitReader reader;
	initBitReader(&reader, input);

	HuffmanNode *node = root;
	uint64_t written = 0;

	while (written < fileSize)
	{
		int bit = readBit(&reader);

		if (bit == -1)
		{
			printf("Error: compressed file ended unexpectedly.\n");
			break;
		}

		node = bit ? node->right : node->left;

		if (node->left == NULL && node->right == NULL)
		{
			fputc(node->byte, output);
			written++;
			node = root;
		}
	}

	freeHuffmanTree(root);

	fclose(input);
	fclose(output);
}
