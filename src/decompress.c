#include <stdio.h>
#include <stdlib.h>

#include "../include/decompress.h"

void decompressFile(const char *inputFile, const char *outputFile)
{
	FILE *input = fopen(inputFile, "r");
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

	char letter;
	int count;

	while (fscanf(input, "%c%d", &letter, &count) == 2)
	{
		for (int i = 0; i < count; i++)
		{
			fputc(letter, output);
		}	
	}

	fclose(input);
	fclose(output);
}
