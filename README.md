# compression_tool

A command-line file compressor written in C.

## Build

    make

Needs only `gcc` (or `clang`) and `make`.

## Usage

    ./compression_tool compress <input_file> <output_file>
    ./compression_tool decompress <input_file> <output_file>

## How it works

The current version uses run-length encoding (RLE). Each run of identical bytes
is stored as the byte followed by a count (up to 255), after a 4-byte `CMP1`
header. Decompression reads the header and expands each run back out.

RLE only helps on data with long runs of repeated bytes. On ordinary text or
already-compressed files such as JPEG, the output can be larger than the input.

## Roadmap

- Huffman coding: frequency counting and tree construction are written
  (`frequency` and `huffman` test commands); wiring them into `compress` and
  `decompress` is in progress.

## Project layout

    src/        main, compress, decompress, fileio, huffman, bitio
    include/    headers for each module
    test-files/ sample inputs
