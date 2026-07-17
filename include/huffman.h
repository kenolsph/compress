#ifndef HUFFMAN_H
#define HUFFMAN_H

typedef struct HuffmanNode
{
	unsigned char byte;
	unsigned long frequency;

	struct HuffmanNode *left;
	struct HuffmanNode *right;

} HuffmanNode;

typedef struct
{
	HuffmanNode *nodes[256];
	int size;

} PriorityQueue;

void calculateFrequency(const char *filename, unsigned long frequency[256]);
void buildFrequencyTable(const char *filename);
HuffmanNode *createNode(unsigned char byte, unsigned long frequency);

void initializeQueue(PriorityQueue *queue);
void enqueue (PriorityQueue *queue, HuffmanNode *node);
HuffmanNode *dequeue(PriorityQueue *queue);

PriorityQueue *buildPriorityQueue(const char *filename);
HuffmanNode *buildHuffmanTree(PriorityQueue *queue);

void printHuffmanTree(HuffmanNode *root, int depth);
void generateCodes(HuffmanNode *root, char code[], int depth);
void testHuffman(const char *filename);

#endif
