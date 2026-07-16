#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/decompress.h"

void decompressFile(const char *inputFile, const char *outputFile)
{
	FILE *input = fopen(inputFile, "rb");
	if (input == NULL)
	{
		printf("Error: Cannot open input file %s\n", inputFile);
		return ;
	}

	FILE *output = fopen(outputFile, "w");

	if (output == NULL)
	{
		printf("Error: Cannot open output file %s\n", outputFile);
		fclose(input);
		return ;
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

	if (strcmp(header, "CMP1") != 0)
	{
		printf("Invalid compressed file.\n");
		fclose(input);
		fclose(output);
		return;
	}

	unsigned char letter;
	unsigned char count;

	while (fread(&letter, 1, 1, input)  == 1)
	{
		if (fread(&count, 1, 1, input) != 1)
		{
			printf("Error: Corrupted compressed file.\n");
		       	break;
		}

		for (int i = 0; i < count; i++)
		{
			fputc(letter, output);
		}	
	}

	fclose(input);
	fclose(output);
}
