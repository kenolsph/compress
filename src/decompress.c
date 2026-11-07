#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../include/decompress.h"
#include "../include/codec.h"
#include "../include/bitio.h"

/* Reads the CMP2 format described in compress.c. */

int decompressFile(const char *inputFile, const char *outputFile)
{
	int status = 1;
	int created = 0;
	int distinct = 0;
	int previous = -1;
	int lo, hi, value, bit;
	unsigned char header[4];
	unsigned char raw[4];
	unsigned long frequency[256] = {0};
	unsigned long long total = 0;
	unsigned long long done_count;
	FILE *input = NULL;
	FILE *output = NULL;
	HuffmanNode *root = NULL;
	const HuffmanNode *node;
	BitReader reader;

	input = fopen(inputFile, "rb");

	if (input == NULL)
	{
		fprintf(stderr, "Error: cannot open input file %s\n", inputFile);
		return 1;
	}

	if (fread(header, 1, 4, input) != 4 || memcmp(header, "CMP2", 4) != 0)
	{
		fprintf(stderr, "Error: not a valid compressed file (CMP2 header missing).\n");
		goto done;
	}

	lo = fgetc(input);
	hi = fgetc(input);

	if (lo == EOF || hi == EOF)
	{
		fprintf(stderr, "Error: corrupted compressed file.\n");
		goto done;
	}

	distinct = lo | (hi << 8);

	if (distinct > 256)
	{
		fprintf(stderr, "Error: corrupted compressed file.\n");
		goto done;
	}

	for (int i = 0; i < distinct; i++)
	{
		value = fgetc(input);

		if (value == EOF || fread(raw, 1, 4, input) != 4)
		{
			fprintf(stderr, "Error: corrupted compressed file.\n");
			goto done;
		}

		uint32_t count = (uint32_t)raw[0] | ((uint32_t)raw[1] << 8) |
				 ((uint32_t)raw[2] << 16) | ((uint32_t)raw[3] << 24);

		/* bytes must be strictly ascending and every count positive */
		if (value <= previous || count == 0)
		{
			fprintf(stderr, "Error: corrupted compressed file.\n");
			goto done;
		}

		previous = value;
		frequency[value] = (unsigned long)count;
		total += count;
	}

	output = fopen(outputFile, "wb");

	if (output == NULL)
	{
		fprintf(stderr, "Error: cannot create output file %s\n", outputFile);
		goto done;
	}

	created = 1;

	if (distinct > 0)
	{
		root = treeFromFrequencies(frequency);

		if (root == NULL)
		{
			fprintf(stderr, "Error: out of memory.\n");
			goto done;
		}

		if (root->left == NULL && root->right == NULL)
		{
			/* only one distinct byte: just repeat it */
			for (done_count = 0; done_count < total; done_count++)
			{
				fputc(root->byte, output);
			}
		}
		else
		{
			initBitReader(&reader, input);

			for (done_count = 0; done_count < total; done_count++)
			{
				node = root;

				while (node->left != NULL || node->right != NULL)
				{
					bit = readBit(&reader);

					if (bit < 0)
					{
						fprintf(stderr, "Error: compressed data ends early (file is truncated).\n");
						goto done;
					}

					node = bit ? node->right : node->left;
				}

				fputc(node->byte, output);
			}
		}
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

	freeTree(root);
	return status;
}
