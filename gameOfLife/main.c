#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    int height, width;
} fieldProperties;

typedef struct{
    int red, green, blue;
} pixelInfo;

typedef struct{
    char buff_[18];
    char width[4];
    char height[4];
    char buff__[28];
} header;

int cast(unsigned char *bytes) {
    return (int) ((bytes[3] << 24) + (bytes[2] << 16) + (bytes[1] << 8) + bytes[0]);
}

void gameOfLife(int **firstGen, fieldProperties bitmap) {
    int **newGen = (int **) malloc(sizeof(int *) * bitmap.height);
    for (int i = 0; i < bitmap.height; i++)
        newGen[i] = (int *) malloc(bitmap.width * sizeof(int));
    int nbrsCounter;
    for (int y = 0; y < bitmap.height; y++)
        for (int x = 0; x < bitmap.width; x++) {
            nbrsCounter = 0;
            for (int i = x - 1; i <= x + 1; i++) {
                for (int j = y - 1; j <= y + 1; j++) {
                    if (i == x && j == y)
                        continue;
                    nbrsCounter += firstGen[(j + bitmap.height) % bitmap.height][(i + bitmap.width) % bitmap.width];
                }
            }
            if (firstGen[y][x] == 1) {
                if (nbrsCounter < 2 || nbrsCounter > 3)
                    newGen[y][x] = 0;
                else
                    newGen[y][x] = 1;
            } else {
                if (nbrsCounter == 3)
                    newGen[y][x] = 1;
                else
                    newGen[y][x] = 0;
            }
        }
    for (int y = 0; y < bitmap.height; y++)
        for (int x = 0; x < bitmap.width; x++)
            firstGen[y][x] = newGen[y][x];
    for (int i = 0; i < bitmap.height; i++)
        free(newGen[i]);
    free(newGen);
}


int main(int argc, char *argv[]) {
    int max_iter = 100, dump_freq = 1;
    char *inputFile;
    char *outputFile;
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "--input") == 0)
            inputFile = argv[i + 1];

        if (strcmp(argv[i], "--output") == 0)
            outputFile = argv[i + 1];

        if (strcmp(argv[i], "--max_iter") == 0)
            sscanf(argv[i + 1], "%d", &max_iter);

        if (strcmp(argv[i], "--dump_freq") == 0)
            sscanf(argv[i + 1], "%d", &dump_freq);
    }

    FILE *input = fopen(inputFile,"rb");

    header infoFromFile;
    fread(&infoFromFile, 1, 54, input);
    fieldProperties bitmap;
    bitmap.height = cast((unsigned char *) infoFromFile.height);
    bitmap.width = cast((unsigned char *) infoFromFile.width);
    pixelInfo **pixels = (pixelInfo **) malloc(bitmap.height * sizeof(pixelInfo *));
    for (int i = 0; i < bitmap.height; i++)
        pixels[i] = (pixelInfo *) malloc(bitmap.width * sizeof(pixelInfo));
    for (int i = bitmap.height - 1; i >= 0; i--)
        for (int j = 0; j < bitmap.width; j++) {
            pixels[i][j].blue = getc(input);
            pixels[i][j].green = getc(input);
            pixels[i][j].red = getc(input);
        }
    int **firstGen = (int **) malloc(bitmap.height * sizeof(int *));
    for (int i = 0; i < bitmap.height; i++) {
        firstGen[i] = (int *) malloc(bitmap.width * sizeof(int));
    }
    for (int i = 0; i < bitmap.height; i++)
        for (int j = 0; j < bitmap.width; j++) {
            firstGen[i][j] = (pixels[i][j].red == 0 && pixels[i][j].blue == 0 && pixels[i][j].green == 0)?1:0;
        }
    for (int i = 0; i < bitmap.height; i++)
        free(pixels[i]);
    free(pixels);

    for (int i = 1; i <= max_iter; i++) {
        gameOfLife(firstGen, bitmap);
        if (i % dump_freq != 0)
            continue;
        char filePath[256];
        char fileName[32];
        sprintf(fileName, "%d", i);
        strcpy(filePath, outputFile);
        strcat(filePath, "/");
        strcat(filePath, fileName);
        strcat(filePath, ".bmp");
        FILE *output = fopen(filePath, "wb");
        fwrite(&infoFromFile, 1, 54, output);
        int n = 0;
        unsigned char *newGen = (unsigned char *) malloc(bitmap.width * bitmap.height * 3);
        for (int tmp_iterator = bitmap.height - 1; tmp_iterator >= 0; tmp_iterator--)
            for (int j = 0; j < bitmap.width; j++)
                for (int k = 0; k < 3; k++) {
                    if (firstGen[tmp_iterator][j] == 0)
                        newGen[n] = 255;
                    else
                        newGen[n] = 0;
                    n++;
                }
        fwrite(newGen, 1, bitmap.width * bitmap.height * 3, output);
        free(newGen);
        fclose(output);
    }
    for (int i = 0; i < bitmap.height; i++)
        free(firstGen[i]);
    free(firstGen);
    return 0;
}