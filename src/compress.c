#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/compress.h"
#include "../include/codec.h"
#include "../include/bitio.h"

/*
 * File format (CMP2), all numbers little-endian:
 *
 *   4 bytes   "CMP2"
 *   2 bytes   N, the number of distinct byte values (0 to 256)
 *   N times   1 byte value + 4 byte count, in ascending byte order
 *   rest      the Huffman-coded bits, padded to a whole byte
 *
 * The original size is the sum of the counts. The decoder rebuilds the
 * same tree from the counts, so no tree has to be stored.
 */

static void putLE(FILE *file, unsigned long long value, int bytes)
{
	for (int i = 0; i < bytes; i++)
	{
		fputc((int)((value >> (8 * i)) & 0xFF), file);
	}
}

/* Give every leaf its bit string: left edge = '0', right edge = '1'. */
static int assignCodes(const HuffmanNode *node, char *path, int depth, char *codes[256])
{
	if (node->left == NULL && node->right == NULL)
	{
		path[depth] = '\0';
		codes[node->byte] = (char *)malloc((size_t)depth + 1);

		if (codes[node->byte] == NULL)
		{
			return -1;
		}

		memcpy(codes[node->byte], path, (size_t)depth + 1);
		return 0;
	}

	path[depth] = '0';

	if (node->left != NULL && assignCodes(node->left, path, depth + 1, codes) != 0)
	{
		return -1;
	}

	path[depth] = '1';

	if (node->right != NULL && assignCodes(node->right, path, depth + 1, codes) != 0)
	{
		return -1;
	}

	return 0;
}

int compressFile(const char *inputFile, const char *outputFile)
{
	int status = 1;
	int created = 0;
	int distinct = 0;
	FILE *input = NULL;
	FILE *output = NULL;
	HuffmanNode *root = NULL;
	char *codes[256] = {0};
	char path[257];
	unsigned long frequency[256] = {0};
	unsigned char buffer[4096];
	size_t count;
	BitWriter writer;

	input = fopen(inputFile, "rb");

	if (input == NULL)
	{
		fprintf(stderr, "Error: could not open input file %s\n", inputFile);
		return 1;
	}

	/* pass 1: count how often each byte appears */
	while ((count = fread(buffer, 1, sizeof buffer, input)) > 0)
	{
		for (size_t i = 0; i < count; i++)
		{
			frequency[buffer[i]]++;
		}
	}

	if (ferror(input))
	{
		fprintf(stderr, "Error: could not read %s\n", inputFile);
		goto done;
	}

	for (int i = 0; i < 256; i++)
	{
		if (frequency[i] > 0)
		{
			distinct++;
		}

		if ((unsigned long long)frequency[i] > 0xFFFFFFFFULL)
		{
			fprintf(stderr, "Error: file is too large (limit is 4 GB).\n");
			goto done;
		}
	}

	output = fopen(outputFile, "wb");

	if (output == NULL)
	{
		fprintf(stderr, "Error: cannot create output file %s\n", outputFile);
		goto done;
	}

	created = 1;

	/* header */
	fwrite("CMP2", 1, 4, output);
	putLE(output, (unsigned long long)distinct, 2);

	for (int i = 0; i < 256; i++)
	{
		if (frequency[i] > 0)
		{
			fputc(i, output);
			putLE(output, frequency[i], 4);
		}
	}

	/* pass 2: write the codes. A file with one repeated byte needs no
	   bits at all, because the header already says byte and count. */
	if (distinct > 1)
	{
		root = treeFromFrequencies(frequency);

		if (root == NULL || assignCodes(root, path, 0, codes) != 0)
		{
			fprintf(stderr, "Error: out of memory.\n");
			goto done;
		}

		rewind(input);
		initBitWriter(&writer, output);

		while ((count = fread(buffer, 1, sizeof buffer, input)) > 0)
		{
			for (size_t i = 0; i < count; i++)
			{
				for (const char *bit = codes[buffer[i]]; *bit != '\0'; bit++)
				{
					writeBit(&writer, *bit == '1');
				}
			}
		}

		flushBits(&writer);
	}

	if (ferror(output))
	{
		fprintf(stderr, "Error: could not write %s\n", outputFile);
		goto done;
	}

	if (fclose(output) != 0)
	{
		output = NULL;
		fprintf(stderr, "Error: could not finish writing %s\n", outputFile);
		goto done;
	}

	output = NULL;
	status = 0;

done:
	if (input != NULL)
	{
		fclose(input);
	}

	if (output != NULL)
	{
		fclose(output);
	}

	if (status != 0 && created)
	{
		remove(outputFile);
	}

	for (int i = 0; i < 256; i++)
	{
		free(codes[i]);
	}

	freeTree(root);
	return status;
}
