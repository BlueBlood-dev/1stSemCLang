#include <stdio.h>
#include <string.h>
#include <math.h>

#define  MAX_LENGTH 309
#define DIV 1000000000

struct uint1024_t {
    int array[35];
};

void make_inited(struct uint1024_t *x) {
    for (int i = 0; i < 35; i++)
        x->array[i] = 0;
}

struct uint1024_t add_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t sum;
    make_inited(&sum);
    int add = 0;
    for (int i = 34; i > -1; --i) {
        unsigned long long int sm = x.array[i] + y.array[i] + add;
        sum.array[i] = sm % DIV;
        add = sm / DIV;
    }
    return sum;
}

struct uint1024_t subtr_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t subtr;
    int debt = 0;
    for (int i = 34; i > -1; --i) {
        if (x.array[i] >= y.array[i] + debt) {
            subtr.array[i] = x.array[i] - y.array[i] - debt;
            debt = 0;
        } else {
            subtr.array[i] = x.array[i] + DIV - y.array[i] - debt;
            debt = 1;
        }
    }
    if (debt) {
        make_inited(&subtr);
        printf("WRONG ANSWER!!! THE RESULT OF SUBSTRACTION IS BELOW ZERO\n");
        return subtr;
    } else {
        return subtr;
    }

}

struct uint1024_t from_uint(unsigned int x) {
    struct uint1024_t uintStructure;
    make_inited(&uintStructure);
    if (x / DIV != 0) {
        uintStructure.array[33] = x / DIV;
        uintStructure.array[34] = x % DIV;
    } else {
        uintStructure.array[34] = x;
    }
    return uintStructure;
}

int was_multiplicified(struct uint1024_t x) {
    for (int i = 0; i < 35; ++i) {
        if (x.array[i] != 0)
            return 0;
    }
    return 1;
}

struct uint1024_t mult_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t multiplification = from_uint(0);
    make_inited(&multiplification);
    struct uint1024_t decrement = from_uint(1);
    while (!was_multiplicified(y)) {
        y = subtr_op(y, decrement);
        multiplification = add_op(multiplification, x);
    }
    return multiplification;
}

void printf_value(struct uint1024_t x) {
    int flag = -1;
    for (int i = 0; i < 35; ++i) {
        if (x.array[i] != 0) {
            flag = i;
            break;
        }
    }
    if (flag == -1) {
        printf("0\n");
    } else {
        printf("%d", x.array[flag]);
        if (flag + 1 < 35)
            for (int i = flag + 1; i < 35; ++i)
                printf("%09d", x.array[i]);
        printf("\n");
    }
}

int convert_block_to_int(char array[], int i1) {
    int number = 0;
    int pw = 0;
    if (i1 != 9) {
        pw = 0;
        for (int i = 0; pw != i1; ++i) {
            number += (array[i] - '0') * pow(10, pw++);
        }
        return number;
    }
    for (int i = 0; pw != strlen(array); ++i) {
        number += (array[i] - '0') * pow(10, pw++);
    }
    return number;
}

void scanf_value(struct uint1024_t *x) {
    make_inited(x);
    char tempArray[MAX_LENGTH];
    while (1) {
        scanf("%s", &tempArray);
        if (tempArray[0] == '0') printf("THE ETERED VALUE IS INVALID\n"); else break;
    }
    int length = strlen(tempArray);
    int blocksToFill;
    if (length % 9 == 0) blocksToFill = length / 9; else blocksToFill = length / 9 + 1;
    char blockToconvert[10];
    int filler = 34;
    int digits = 0;
    for (int i = length - 1; i > -1; --i) {
        if (digits == 9) {
            blockToconvert[9] = '\0';
            x->array[filler--] = convert_block_to_int(blockToconvert, digits);
            blocksToFill--;
            digits = 0;
        }
        blockToconvert[digits++] = tempArray[i];
    }
    if (blocksToFill != 0) {
        x->array[filler] = convert_block_to_int(blockToconvert, digits);
    }
}

int main() {
    struct uint1024_t x, y;
    scanf_value(&x);
    scanf_value(&y);
    printf_value(mult_op(x, y));
    printf_value(add_op(x, y));
    printf_value(subtr_op(x, y));
    unsigned int number;
    scanf("%u", &number);
    printf_value(from_uint(number));
    return 0;
}
