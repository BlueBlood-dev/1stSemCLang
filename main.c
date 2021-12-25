#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int compressFlag = 0;

FILE* createTemplate(char* nameOfFile, int numberOfFiles){
    
    FILE* archive = fopen(nameOfFile, "wb");
    fwrite("ARC", 1, 3, archive);
    fwrite(&compressFlag, 1, 1, archive);
    fwrite(&numberOfFiles, sizeof(int), 1, archive);
    return archive;
}

int readFileAndWriteToArchive(FILE* archive, char* nameOfFile) {
    FILE* readFile = fopen(nameOfFile, "rb");
    if (readFile == NULL) {
        printf("Error when opening file \"%s\", skipping\n", nameOfFile);
        return 1;
    }

    for (unsigned long i = 0; i < strlen(nameOfFile); ++i) {
        char buffer = *(nameOfFile + i);
        fwrite(&buffer, 1, 1, archive);
    }

    char zero = 0x0;
    unsigned char maxChar = 0xFF;
    fwrite(&maxChar, 1, 1, archive);

    unsigned long long bytesWritten = 0;
    long pointInFileForSize = ftell(archive);
    fwrite(&zero, sizeof(unsigned long long), 1, archive);

    char buffer;
    while (fread(&buffer, 1, 1, readFile) != 0) {
        fwrite(&buffer, 1, 1, archive);
        bytesWritten += 1;
    }

    long lastPointInFile = ftell(archive);
    fseek(archive, pointInFileForSize, SEEK_SET);
    fwrite(&bytesWritten, sizeof(unsigned long long), 1, archive);
    fseek(archive, lastPointInFile, SEEK_SET);

    return 0;
}

void extractArchive(char* nameOfArchive){
    FILE* archive = fopen(nameOfArchive, "rb");
    char* buffer = calloc(1, 3);
    fread(buffer, 1, 3, archive);
    if (strcmp(buffer, "ARC") != 0) {
        printf("This is not supportable file\n");
        return;
    }
    free(buffer);
    buffer = calloc(1, 1);
    fread(buffer, 1, 1, archive); //compression
    int numberOfFiles = 0;
    fread(&numberOfFiles, sizeof(int), 1, archive);
    unsigned long long k;
    char* bufferName;
    unsigned char bufferByte;
    FILE* writingFile;
    unsigned long long sizeOfFile;
    for (int i = 0; i < numberOfFiles; ++i) {
        k = 1;
        bufferName = calloc(1, 1);
        *bufferName = '\0';
        bufferByte = 0;
        fread(&bufferByte, 1, 1, archive);
        while (bufferByte != 0xFF) {
            *(bufferName + k - 1) = bufferByte;
            fread(&bufferByte, 1, 1, archive);
            k++;
        }
        *(bufferName + k - 1) = '\0';
        writingFile = fopen((const char*)bufferName, "wb");
        free(bufferName);
        sizeOfFile = 0;
        fread(&sizeOfFile, sizeof(unsigned long long), 1, archive);
        for (unsigned long long j = 0; j < sizeOfFile; ++j) {
            fread(&bufferByte, 1, 1, archive);
            fwrite(&bufferByte, 1, 1, writingFile);
        }
        fclose(writingFile);
    }
}

void listFiles(char* nameOfArchive){
    FILE* archive = fopen(nameOfArchive, "rb");
    char* buffer = calloc(1, 3);
    fread(buffer, 1, 3, archive);
    if (strcmp(buffer, "ARC") != 0) {
        printf("This is not supportable file\n");
        return;
    }
    free(buffer);
    buffer = calloc(1, 1);
    fread(buffer, 1, 1, archive);

    printf("List of files:\n");

    int numberOfFiles = 0;
    fread(&numberOfFiles, sizeof(int), 1, archive);
    unsigned long long k;
    char* bufferName;
    unsigned char bufferByte;
    unsigned long long sizeOfFile;

    for (int i = 0; i < numberOfFiles; ++i) {
        k = 1;
        bufferName = calloc(1, 1);
        *bufferName = '\0';
        bufferByte = 0;
        fread(&bufferByte, 1, 1, archive);
        while (bufferByte != 0xFF) {
            *(bufferName + k - 1) = bufferByte;
            fread(&bufferByte, 1, 1, archive);
            k++;
        }
        *(bufferName + k - 1) = '\0';
        printf("%s\n", bufferName);
        free(bufferName);
        sizeOfFile = 0;
        fread(&sizeOfFile, sizeof(unsigned long long), 1, archive);
        fseek(archive, sizeOfFile, SEEK_CUR);
    }
    printf("Amount of files: %d\n", numberOfFiles);
}


int main(int argc, char *argv[]) {

    char* nameOfFile = "";
    unsigned char create = 0;
    int indexOfFirstFile = 0;
    unsigned char extract = 0;
    unsigned char list = 0;
    unsigned char flag = 0;

    for (int i = 1; i < argc; ++i) {
        if (flag == 2) {
            if (strlen(argv[i]) >= 2 && strncmp(argv[i], "--", 2) == 0) {
                printf("After --create key only names of files are acceptable\n");
                return 0;
            }
            continue;
        }
        else if (flag) {
            flag = 0;
            continue;
        }
        if (strlen(argv[i]) > 2 && strncmp(argv[i], "--", 2) == 0) {
            if (strlen(argv[i]) == 6 && strncmp(argv[i], "--file", 6) == 0) {
                nameOfFile = argv[i + 1];
                flag = 1;
            } else if (strlen(argv[i]) == 8 && strncmp(argv[i], "--create", 8) == 0) {
                create = 1;
                indexOfFirstFile = i + 1;
                flag = 2;
            } else if (strlen(argv[i]) == 9 && strncmp(argv[i], "--extract", 9) == 0) {
                extract = 1;
            } else if (strlen(argv[i]) == 6 && strncmp(argv[i], "--list", 6) == 0) {
                list = 1;
            } else {
                printf("Invalid argument: %s\n", argv[i]);
                return 0;
            }
        }

    }
    if (create) {
        FILE* archive = createTemplate(nameOfFile, argc - indexOfFirstFile);
        for (int i = indexOfFirstFile; i < argc; ++i) {
            readFileAndWriteToArchive(archive, argv[i]);
        }
        fclose(archive);
    }
    else if (list) {
        listFiles(nameOfFile);
    }
    else if (extract) {
        extractArchive(nameOfFile);
    }
    return 0;
}

