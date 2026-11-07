#ifndef DECOMPRESS_H
#define DECOMPRESS_H

/* Restore a file made by compressFile. Returns 0 on success, 1 on error. */
int decompressFile(const char *inputFile, const char *outputFile);

#endif
