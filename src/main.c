#include <stdio.h>
#include <string.h>

#include "../include/compress.h"
#include "../include/decompress.h"
#include "../include/huffman.h"

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("=======================================\n");
		printf("         Compression Tool\n");
		printf("=======================================\n");
		
		printf("Usage:\n");
		printf("./compression_tool compress <input_file> <output_file>\n");
		printf("./compression_tool decompress <input_file> <output_file>\n");
		return 1;
	}
	
	if (strcmp(argv[1], "compress") == 0)
	{
		printf("Compressing \"%s\"...\n", argv[2]);

		if (compressFile(argv[2], argv[3]) != 0)
			return 1;

		printf("Compression complete.\n");
	}
	else if (strcmp(argv[1], "decompress") == 0)
	{
		printf("Decompressing \"%s\"...\n", argv[2]);

		if (decompressFile(argv[2], argv[3]) != 0)
			return 1;

		printf("Decompression complete.\n");
	}
	else if (strcmp(argv[1], "frequency") == 0)
	{
		buildFrequencyTable(argv[2]);
	}
	else if (strcmp(argv[1], "huffman") == 0)
	{
		testHuffman(argv[2]);
	}
	else
	{
		printf("Error: Unknown command \"%s\"\n", argv[1]);
		printf("Use either 'compress' or 'decompress'.\n");
		return 1;
	}
	return 0;
}
