# compression_tool

A command-line file compressor written in C, using Huffman coding.

## Build 

    make

Needs only `gcc` (or `clang`) and `make`.

## Usage

    ./compression_tool compress <input_file> <output_file>
    ./compression_tool decompress <input_file> <output_file>

## Example

    ./compression_tool compress notes.txt notes.cmp
    ./compression_tool decompress notes.cmp notes.restored

On a 17,507-byte sample of this project's own source code, the output was
11,976 bytes (about 32% smaller), and decompressing gave back an identical file.

## How it works

1. Count how often each byte value appears in the input.
2. Build a Huffman tree from those counts, so common bytes get short bit codes.
3. Write a small header (`CMP2`, then each byte value with its count), followed
   by the encoded bits.
4. Decompression reads the header, rebuilds the same tree from the counts, and
   decodes the bits back into bytes.

Already-compressed data (JPEG, ZIP, PNG) has no pattern left to remove, so it
will not shrink and can grow slightly because of the header.

## Project layout

    src/        main, compress, decompress, codec, fileio, huffman, bitio
    include/    headers for each module
    test-files/ sample inputs
