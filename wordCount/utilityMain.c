#include <stdio.h>
#include <string.h>
#include <ctype.h>

void printTheAmountOfWords(char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("file not found ");
    } else {
        int symbol;
        char previousChar = 5;//isgraph == 0 when NULL
        int words = 0;
        while ((symbol = fgetc(file)) != EOF) {
            if (isgraph(symbol) != 0) {
                if (isgraph(previousChar) == 0)
                    words++;
            }
            previousChar = symbol;
        }
        printf("the amount of words is: %d", words);
    }
    fclose(file);
}

void printTheFileSize(char *fileName) {
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("file not found");
    } else {
        int bytes;
        for (bytes = 0; getc(file) != EOF; bytes++);
        printf("The file size is %d bytes", bytes);
    }
    fclose(file);
}

void printTheAmountOfLines(char *fileName) {
    FILE *file = fopen(fileName, "r");
    int symbol;
    char lastSymbol;
    int counter = 0;
    if (file == NULL) {
        printf("file not found");
    } else {
        while ((symbol = fgetc(file)) != EOF) {
            if(symbol == '\n')
                counter++;
            lastSymbol = symbol;
        }
        if(counter == 0){
            printf("The amount of lines in file is %d", isgraph(lastSymbol)==0?0:1);
        } else {
            printf("The amount of lines in file is: %d", lastSymbol == '\n' ? counter : counter + 1);
        }
    }
    fclose(file);
}

char removeChar(const char *command, char symbol) {
    char erasedCommand;
    for (int i = 0;; i++) {
        if (command[i] != symbol) {
            erasedCommand = command[i];
            break;
        }
    }
    return erasedCommand;
}

void defTheCommandAndComplete(char *command, char *filein) {
    switch (removeChar(command, '-')) {
        case 'l':
            printTheAmountOfLines(filein);
            break;
        case 'c':
        case 'b':
            printTheFileSize(filein);
            break;
        case 'w':
            printTheAmountOfWords(filein);
            break;
    }
}

int checkIfCommand(char *command) {
    char *listOfCommands[] = {"-l", "--lines", "-c", "--bytes", "-w", "--words"};
    for (int i = 0; i < sizeof listOfCommands / sizeof *listOfCommands; i++) {
        if (strcmp(command, listOfCommands[i]) == 0)
            return 1;
    }
    printf("You have entered the wrong or non-existent command");
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("not required amount of params passed");
        return 0;
    }
    char *command = argv[1];
    char *fileName = argv[2];
    if (checkIfCommand(command) != -1)
        defTheCommandAndComplete(command, fileName);
}