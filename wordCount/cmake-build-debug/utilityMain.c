#include <stdio.h>
#include <string.h>
void printTheAmountOfLines(char *fileName){
    FILE *file = fopen(fileName,"r");
    char symbol;
    int linesCounter = 0;
    while ((symbol = fgetc(file)) != EOF)
        if (symbol == '\n')
            linesCounter++;
    fclose(file);
    printf("The amount of lines in the files is %d", linesCounter);
}

void printTheAmountOfWords(char *fileName){

}

void printTheFileSize(){

}


int main(int argc, char *argv[]) {
    //FILE *fileIn = fopen(argv[1], "r");
    if (argc != 3) {
        printf("not required amount of params passed");
        return 0;
    }
    char *commandName = argv[1];
    char *fileName = argv[2];
    switch (*commandName) {
        case '-l':
        case '--lines':
            printTheAmountOfLines(fileName);
            break;
        case 'c':
        case '--bytes':
            printTheFileSize(fileName);
            break;
        case 'w':
        case '--words':
            printTheAmountOfWords(fileName);
            break;

    }
}





