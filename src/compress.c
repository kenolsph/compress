#include <stdio.h>
#include <stdlib.h>

#include "../include/compress.h"
void compressFile(const char *inputFile, const char *outputFile)
{
	FILE *input = fopen(inputFile, "rb");

	if (input == NULL)
	{
		printf("Error: could not open file. %s\n", inputFile);
		return;
	}

	FILE *output = fopen(outputFile, "wb");

	if	(output == NULL)
	{
		printf("Error: Cannot create output file %s\n", outputFile);
		fclose(input);
		return;
	}

	/* File Header */ 
	fwrite("CMP1", 1, 4, output);

	int previous = fgetc(input);

	if (previous == EOF)
	{
		fclose(input);
		fclose(output);
		return;
	}

	int current;
	int count = 1;

	while	((current = fgetc(input)) != EOF)
	{
		if (current == previous && count < 255)
		{
			count++;
		}
		else
		{
			unsigned char letter = (unsigned char)previous;
			unsigned char run = (unsigned char)count;

			fwrite(&letter, 1, 1, output);
			fwrite(&run, 1, 1, output);

			previous = current;
			count = 1;
		}
	}
	unsigned char letter = (unsigned char)previous;
	unsigned char run = (unsigned char)count;

	fwrite(&letter, 1, 1, output);
	fwrite(&run, 1, 1, output);

	fclose(input);
	fclose(output);
}
