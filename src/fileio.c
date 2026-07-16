#include <stdio.h>
#include <stdlib.h>

FILE *openInputFile(const char *filename)
{
	FILE *file = fopen(filename, "rb");

	if (file ==NULL)
	{
		printf("cannot open %s\n", filename);
		exit(1);
	}

	return file;
}

	FILE *createOutputFile(const char *filename)
	{
		FILE *file = fopen(filename, "wb");

		if (file == NULL)
		{
			printf("cannot create %s\n", filename);
			exit(1);
		}
		
		return file;
	}
