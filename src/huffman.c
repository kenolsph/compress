#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/huffman.h"

char *huffmanCodes[256] = {0};

void calculateFrequency(const char *filename, unsigned long frequency[256])
{
	FILE *input = fopen(filename, "rb");

	if (input == NULL)
	{
		printf("Unable to open file.\n");
		return;
	}

	int byte;

	while ((byte = fgetc(input)) != EOF)
	{
		frequency[(unsigned char)byte]++;
	}

	fclose(input);
}

void buildFrequencyTable(const char *filename)
{
	unsigned long frequency[256] = {0};

	calculateFrequency(filename, frequency);
	
	printf("\nFrequency Table\n");
	printf("---------------------------\n");

	for (int i = 0; i < 256; i++)
	{
		if (frequency[i] > 0)
		{
			if (i >= 32 && i <= 126)
				printf("'%c' : %lu\n", i, frequency[i]);
			else
				printf("0x%02X : %lu\n", i, frequency[i]);
		}
	}
}

HuffmanNode *createNode(unsigned char byte, unsigned long frequency)
{
	HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));

	if (node == NULL)
	{
		printf("Memory allocation failed.\n");
		return NULL;
	}

	node->byte = byte;
	node->frequency = frequency;

	node->left = NULL;
	node->right = NULL;

	return node;
}

void initializeQueue(PriorityQueue *queue)
{
	queue->size = 0;
}

void enqueue(PriorityQueue *queue, HuffmanNode *node)
{
	int i = queue->size - 1;

	while (i >= 0 && queue->nodes[i]->frequency > node->frequency)
	{
		queue->nodes[i + 1] = queue->nodes[i];
		i--;
	}

	queue->nodes[i + 1] = node;
	queue->size++;
}

HuffmanNode *dequeue(PriorityQueue *queue)
{
	if (queue->size == 0)
	{
		return NULL;
	}

	HuffmanNode *node = queue->nodes[0];

	for (int i = 1; i < queue->size; i++)
	{
		queue->nodes[i - 1] = queue->nodes[i];
	}
	queue->size--;

	return node;
}

PriorityQueue *buildPriorityQueue(const char *filename)
{
	unsigned long frequency[256] = {0};

	calculateFrequency(filename, frequency);

	PriorityQueue *queue = (PriorityQueue *)malloc(sizeof(PriorityQueue));

	if (queue == NULL)
	{
		printf("Memory allocation failed.\n");
		return NULL;
	}

	initializeQueue(queue);

	for (int i = 0; i < 256; i++)
	{
		if (frequency[i] > 0 )
		{
			HuffmanNode *node = createNode((unsigned char)i, frequency[i]);
			enqueue(queue, node);
		}
	}

	return queue;
}

HuffmanNode *buildHuffmanTree(PriorityQueue *queue)
{
	while (queue->size >1)
	{
		HuffmanNode *left = dequeue(queue);
		HuffmanNode *right = dequeue(queue);

		HuffmanNode *parent = createNode(0, left->frequency + right->frequency);

		if (parent == NULL)
		{
			return NULL;
		}

		parent->left = left;
		parent->right = right;

		enqueue(queue, parent);
	}

	return dequeue(queue);
}

void printHuffmanTree(HuffmanNode *root, int depth)
{
	if (root == NULL)
	{
		return;
	}

	for (int i = 0; i < depth; i++)
	{
		printf("   ");
	}

	if (root->left == NULL && root->right == NULL)
	{
		if (isprint(root->byte))
		{
			printf("'%c' (%lu)\n", root->byte, root->frequency);
		}
		else
		{
			printf("0x%02x (%lu)\n", root->byte, root->frequency);
		}
	}
	else
	{
		printf("* (%lu)\n", root->frequency);
	}

	printHuffmanTree(root->left, depth + 1);
	printHuffmanTree(root->right, depth +1);
}

void generateCodes(HuffmanNode *root, char code[], int depth)
{
	if (root == NULL)
	{
		return;
	}

	if (root->left == NULL && root->right == NULL)
	{
		code[depth] = '\0';

		huffmanCodes[root->byte] = malloc(strlen(code) + 1);

		if (huffmanCodes[root->byte] != NULL)
		{
			strcpy(huffmanCodes[root->byte], code);
		}

		if (root->byte >= 32 && root->byte <= 126)
		{
			printf("'%c' : %s\n", root->byte, code);
		}
		else
		{
			printf("0x%02x : %s\n", root->byte, code);
		}

		return;
	}

	code[depth] = '0';
	generateCodes(root->left, code, depth + 1);

	code[depth] = '1';
	generateCodes(root->right, code, depth + 1);
}

void testHuffman(const char *filename)
{
	PriorityQueue *queue = buildPriorityQueue(filename);

	if (queue == NULL)
	{
		return;
	}

	HuffmanNode *root = buildHuffmanTree(queue);

	if (root == NULL)
	{
		printf("Failed to build Huffman tree.\n");
		return;
	}

	char code[256];

	printf("\n========================\n");
	printf("       Huffman Codes\n");
	printf("=======================\n");

	generateCodes(root, code, 0);

	printf("==========================\n");
}

void buildCodes(const char *filename)
{
	PriorityQueue *queue = buildPriorityQueue(filename);

	if (queue == NULL)
	{
		return;
	}

	HuffmanNode *root = buildHuffmanTree(queue);

	char code[256];

	generateCodes(root, code, 0);
}
