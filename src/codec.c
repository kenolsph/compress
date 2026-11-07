#include <stdlib.h>

#include "../include/codec.h"

void freeTree(HuffmanNode *root)
{
	if (root == NULL)
	{
		return;
	}

	freeTree(root->left);
	freeTree(root->right);
	free(root);
}

HuffmanNode *treeFromFrequencies(const unsigned long frequency[256])
{
	PriorityQueue queue;

	initializeQueue(&queue);

	for (int i = 0; i < 256; i++)
	{
		if (frequency[i] == 0)
		{
			continue;
		}

		HuffmanNode *node = createNode((unsigned char)i, frequency[i]);

		if (node == NULL)
		{
			while (queue.size > 0)
			{
				freeTree(dequeue(&queue));
			}
			return NULL;
		}

		enqueue(&queue, node);
	}

	if (queue.size == 0)
	{
		return NULL;
	}

	return buildHuffmanTree(&queue);
}
