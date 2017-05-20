/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t BYTE;

typedef struct { 
    BYTE a;
    BYTE b;
    BYTE c;
    BYTE d;
} __attribute__((__packed__)) 
head_t;

const int BLOCK_SIZE_BYTES = 512;

bool jpeg_match(head_t *head);

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[1];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    char *fileName = malloc(8 * sizeof(char));
    FILE *outptr = NULL;
    
    // read single block
    int buf_size = BLOCK_SIZE_BYTES / sizeof(head_t);
    head_t *buffer = malloc(BLOCK_SIZE_BYTES);

    int readCount = 0, fileNum = 0;
    while ((readCount = fread(buffer, sizeof(head_t), buf_size, inptr)) > 0) {
        head_t head = buffer[0];
        if (jpeg_match(&head)) {
            sprintf(fileName, "%03d.jpg", fileNum);
            if (outptr != NULL)
                fclose(outptr);
                
            outptr = fopen(fileName, "w");
            
            //printf("%s: %#02x %#02x %#02x %#02x\n", fileName, head.a, head.b, head.c, head.d);
            fileNum += 1;
        }
        
        if (outptr != NULL)
            fwrite(buffer, sizeof(head_t), buf_size, outptr);
    }

    // free memory
    free(buffer);
    free(fileName);
    
    // close files
    fclose(inptr);
    if (outptr != NULL)
        fclose(outptr);  
        
    // success
    return 0;
}

bool jpeg_match(head_t *head) {
    head_t value = *head;
    return value.a == 0xff 
        && value.b == 0xd8 
        && value.c == 0xff 
        && value.d >> 4 == 0x0e;
}