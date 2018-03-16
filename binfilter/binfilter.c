/**
 * Simple filter that filters out all changes from Saleae binary output that do
 * not happen in the command line specific list of channels
 *
 * Mainly useful to avoid python wasting too much time parsing the binary input
 * when running scorpy parser stage
 *
 * Build with: gcc -Wall -Os binfilter.c -o binfilter
 *    Or with: make binfilter
 * 
 * SPDX-License-Identifier: GPL-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

typedef struct {
    uint64_t ts;
    uint16_t data;
} __attribute__((packed)) Change16;

int main(int argc, char** argv) {

    if (argc != 4) {
        fprintf(stderr, "USAGE: binfilter input.bin 0xmask output.bin\n");
        exit(EXIT_FAILURE);
    }
    FILE* in = fopen(argv[1], "rb");
    assert(in != NULL);

    FILE* out = fopen(argv[3], "wb");
    assert(out != NULL);

    Change16 buffer;
    size_t readCount = fread(&buffer, sizeof(buffer), 1, in);
    unsigned counter = 0;
    // uint16_t mask = (1 << 1);
    // uint16_t mask = 0xFFFFU; // pass-all testing

    long parsedMask = strtol(argv[2], NULL, 0);
    assert(parsedMask >= 0 && parsedMask <= 0xFFFFU);
    uint16_t mask = parsedMask;

    uint16_t prevData = 0;
    if (readCount == 1) {
        // guarantee change on all channels at first iteration
        prevData = (buffer.data ^ 0xFFFFU) & mask;
    }
    // track whether prev entry was filtered or not
    uint8_t wasFiltered = 0;
    while (readCount == 1) {
        buffer.data = buffer.data & mask;
        if (buffer.data != prevData) {
            // printf("%4u: %llu: 0x%04x\n", counter, (long long unsigned)buffer.ts, buffer.data);
            prevData = buffer.data;

            // writeout
            size_t writeCount = fwrite(&buffer, sizeof(buffer), 1, out);
            assert(writeCount == 1);

            wasFiltered = 0;
        } else {
            wasFiltered = 1;
        }
        // attempt to read the next one
        readCount = fread(&buffer, sizeof(buffer), 1, in);
        counter += 1;
    }
    // if last entry was filtered, we need to emit the last entry to guarantee
    // the proper total length of the data (we keep the data length)
    if (wasFiltered) {
        // printf("%4u: %llu: 0x%04x\n", counter, (long long unsigned)buffer.ts, buffer.data);
        size_t writeCount = fwrite(&buffer, sizeof(buffer), 1, out);
        assert(writeCount == 1);
    }

    fclose(in);
    fclose(out);
    exit(EXIT_SUCCESS);
}
