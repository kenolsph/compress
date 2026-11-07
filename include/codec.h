#ifndef CODEC_H
#define CODEC_H

#include "huffman.h"

/*
 * Build a Huffman tree straight from byte frequencies (no file needed).
 * Returns NULL if every frequency is zero, or if memory runs out.
 * The same frequencies always give the same tree, which is what lets
 * decompression rebuild exactly the tree compression used.
 */
HuffmanNode *treeFromFrequencies(const unsigned long frequency[256]);

/* Free a whole tree. Safe to call with NULL. */
void freeTree(HuffmanNode *root);

#endif
