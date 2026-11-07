# compression_tool

A command-line file compressor written in C, using Huffman coding.

## Build

    make

This produces the `compression_tool` binary. It needs only `gcc` (or `clang`) and `make`.

## Usage

    ./compression_tool compress <input_file> <output_file>
    ./compression_tool decompress <input_file> <output_file>

## Example

    ./compression_tool compress notes.txt notes.cmp
    ./compression_tool decompress notes.cmp notes.restored

On a sample text file, 11133 bytes compressed to 20724 bytes (-86% smaller), and decompressing gave back an identical file.

## How it works

1. Count how often each byte value appears in the input.
2. Build a Huffman tree from those frequencies, so common bytes get short codes.
3. Write a small binary header holding the frequency data, then the encoded bits.
4. Decompression reads the header, rebuilds the same tree, and decodes the bits back into bytes.

## Project layout

    src/       main, compress, decompress, fileio, huffman, bitio
    include/   headers for each module
    test-files sample inputs

## Install (optional)

    sudo make install

Installs to `/usr/local/bin`. Change the location with `make install PREFIX=$HOME/.local`.
