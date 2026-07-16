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

	FILE *output = fopen(outputFile, "w");

	if	(output == NULL)
	{
		printf("Error: Cannot create output file %s\n", outputFile);
		fclose(input);
		return;
	}

	int previous = fgetc(input);

	if (previous == EOF)
	{
		printf("Input file is empty.\n");
		fclose(input);
		fclose(output);
		return;
	}

	int current;
	int count = 1;

	while	(fread(&current, 1, 1, input) == 1)
	{
		if	(current == previous)
		{
			count++;
		}
		else
		{
			fprintf(output, "%c%d", previous, count);

			previous = current;
			count = 1;
		}
	}
	fprintf(output, "%c%d", previous, count);

	fclose(input);
	fclose(output);
}
