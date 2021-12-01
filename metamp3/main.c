#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

unsigned int reverseBytes(unsigned int n) {
    return ((n >> 24) & 0x000000ff) |
           ((n >> 8) & 0x0000ff00) |
           ((n << 8) & 0x00ff0000) |
           ((n << 24) & 0xff000000);
}

struct headerFrame {
    char id[4];
    int size;
    unsigned short flags;
};
union fHeader {
    char buffer[10];
    struct headerFrame header_;
};
enum command {
    get,
    show,
    set
};

int readFrame(FILE *input, char tag[4], size_t position, enum command type) {
    struct headerFrame headerFr;
    fseek(input, position, SEEK_SET);
    fread(&headerFr.id, 4, 1, input);
    fread(&headerFr.size, 4, 1, input);
    fread(&headerFr.flags, 2, 1, input);
    int size = reverseBytes(headerFr.size);
    switch (type) {
        case get: {
            if (!strcmp(headerFr.id, tag)) {
                char info[size];
                fread(&info, size, 1, input);
                CharToOem(info, info);
                printf("%s:\t", tag);
                for (int i = 0; i < size; i++) {

                    printf("%c", info[i]);
                }
                return -1;
            } else if (size > 0) {
                return position + 10 + size;
            } else
                return -1;
        }
        case show: {
            char info[size];
            fread(&info, 1, size, input);
            CharToOem(info, info);
            if (size > 0) {
                printf("ID: %s\tValue: ", headerFr.id);
                for (int i = 0; i < size; i++) {
                    printf("%c", info[i]);
                }
                printf("\n");
            } else
                return -1;
        }
        case set: {
            if (!strcmp(headerFr.id, tag)) {
                char info[size];
                fread(&info, size, 1, input);
                CharToOem(info, info);
                for (int i = 0; i < size; i++) {

                    printf("%c", info[i]);
                }
                return -1;
            } else if (size > 0) {
                return position + 10 + size;
            } else
                return -1;
        }
    }
    return position + 10 + size;
}

void setValue(FILE *input, char tag[4], char *info, char *argv[]) {
    size_t position = 10;
    size_t i_size = strlen(info);
    fseek(input, 0, SEEK_END);
    FILE *temporaryOutput = fopen("temp", "wb");
    size_t length = ftell(input);
    fseek(input, 10, SEEK_SET);
    int size = 0;
    struct headerFrame headFr;
    while (position != -1 && position < length - 10) {
        fseek(input, position, SEEK_SET);
        fread(&headFr.id, 4, 1, input);
        fread(&headFr.size, 4, 1, input);
        fread(&headFr.flags, 2, 1, input);
        size = reverseBytes(headFr.size);
        if (!strcmp(headFr.id, tag)) {
            position = position + 10 + size;
            break;
        } else if (size != 0) {
            position = position + 10 + size;
        } else {
            position = -1;
            break;
        }
    }
    if (position == -1) {
        fseek(input, 0, SEEK_SET);
        char header[10];
        fread(&header, 10, 1, input);
        fwrite(header, 10, 1, temporaryOutput);
        union fHeader frameHeader;
        for (int i = 0; i < 4; i++)
            frameHeader.header_.id[i] = tag[i];
        frameHeader.header_.size = reverseBytes(i_size);
        frameHeader.header_.flags = 0;
        fwrite(frameHeader.buffer, 1, 10, temporaryOutput);
        fwrite(info, 1, i_size, temporaryOutput);
        fseek(input, 10, SEEK_SET);
        for (int i = 0; i < length; i++) {
            char tmp;
            tmp = getc(input);
            putc(tmp, temporaryOutput);
        }
    } else {
        fseek(input, 0, SEEK_SET);
        char header[position - 10 - size];
        fread(&header, position - 10 - size, 1, input);
        fwrite(header, position - 10 - size, 1, temporaryOutput);
        union fHeader frameHeader;
        for (int i = 0; i < 4; i++)
            frameHeader.header_.id[i] = tag[i];
        frameHeader.header_.size = reverseBytes(i_size);
        fwrite(frameHeader.buffer, 1, 10, temporaryOutput);
        fwrite(info, 1, i_size, temporaryOutput);
        fseek(input, position, SEEK_SET);
        for (int i = position; i < length; i++) {
            char tmp;
            tmp = getc(input);
            putc(tmp, temporaryOutput);
        }
    }
    CopyFile("temp", strpbrk(argv[1], "=") + 1, 0);
    fclose(temporaryOutput);
    remove("temp");
}

int main(int argc, char *argv[]) {
    FILE *input = fopen(strpbrk(argv[1], "=") + 1, "rb");
    if (!input) {
        printf("File wasn't opened");
        return 1;
    }
    if (argv[2][2] == 'g') {
        size_t pos = 10;
        fseek(input, 0, SEEK_END);
        size_t length = ftell(input);
        while (pos != -1 && pos < length - 10) {
            pos = readFrame(input, strpbrk(argv[2], "=") + 1, pos, get);
        }
    } else if (!strcmp(argv[2], "--show")) {
        size_t position = 10;
        fseek(input, 0, SEEK_END);
        size_t length = ftell(input);
        while (position != -1 && position < length - 10) {
            position = readFrame(input, "", position, show);
        }
    } else if (argv[2][2] == 's') {
        setValue(input, strpbrk(argv[2], "=") + 1, strpbrk(argv[3], "=") + 1, argv);
    }
    return 0;
}