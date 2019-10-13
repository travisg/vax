#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("not enough args");
usage:
        printf("usage %s <infile> <outfile>\n", argv[0]);
        return 1;
    }

    FILE *infp = fopen(argv[1], "rb");
    if (!infp) {
        printf("error opening file %s\n", argv[1]);
        goto usage;
    }

    FILE *outfp = fopen(argv[2], "wb");
    if (!infp) {
        printf("error opening file %s\n", argv[2]);
        goto usage;
    }

    // get length of the input file
    fseek(infp, 0, SEEK_END);
    long int len = ftell(infp);
    printf("input file length %#lx\n", len);
    rewind(infp);

    // round up to next 512 byte boundary
    len = (len + 511) / 512 * 512;
    printf("rounded %#lx\n", len);

    // generate the header
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));

    // main header
    uint16_t isd = 0xd4;
    uint16_t iha = 0x30;
    uint8_t hbcnt = 1;
    uint16_t image_type = 0xffff;
    memcpy(buffer + 0, &isd, sizeof(isd));
    memcpy(buffer + 2, &iha, sizeof(iha));
    memcpy(buffer + 16, &hbcnt, sizeof(hbcnt));
    memcpy(buffer + 510, &image_type, sizeof(image_type));

    // image header (at offset isd)
    uint16_t isize = len / 512;
    uint16_t load_addr = 0 / 512;

    memcpy(buffer + isd + 2, &isize, sizeof(isize));
    memcpy(buffer + isd + 4, &load_addr, sizeof(load_addr));

    // something else at iha
    uint32_t xfer_addr = 0;
    memcpy(buffer + iha, &xfer_addr, sizeof(xfer_addr));

    // write the header out
    fseek(outfp, 0, SEEK_SET);
    fwrite(buffer, sizeof(buffer), 1, outfp);

    // copy the input file to the output file
    while (!feof(infp)) {
        char c;
        fread(&c, 1, 1, infp);
        fwrite(&c, 1, 1, outfp);
    }

    fclose(infp);
    fclose(outfp);

    return 0;
}
