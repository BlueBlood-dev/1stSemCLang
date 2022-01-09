#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int DIV = 1000000000;

struct uint1024_t {
    unsigned int array[35];
    int blocks;
};

void make_inited(struct uint1024_t *x) {
    for (int i = 0; i < 35; i++)
        x->array[i] = 0;
    x->blocks = 0;
}

struct uint1024_t from_uint(unsigned int x) {
    struct uint1024_t number;
    for (int i = 0; i < 35; ++i)
        number.array[i] = 0;
    number.array[0] = x % DIV;
    if (x >= DIV)
        number.blocks = 2;
    else
        number.blocks = 1;
    if (number.blocks > 1)
        number.array[1] = (unsigned int) (x / DIV);
    return number;
}

struct uint1024_t add_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t result;
    int additive = 0;
    for (int i = 0; i < 35; ++i)
        result.array[i] = 0;

    for (int i = 0; i < 35; i++) {
        result.array[i] = x.array[i] + additive + y.array[i];
        if (result.array[i] >= DIV) {
            additive = result.array[i];
            result.array[i] -= DIV;
        }
    }

    for (int j = 34; j >= 0; --j) {
        if (result.array[j] != 0) {
            result.blocks = j + 1;
            break;
        }
    }
    return result;
}

struct uint1024_t subtr_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t result;
    int temp = 0;
    make_inited(&result);
    for (int i = 0; i < x.blocks; ++i) {
        result.array[i] = x.array[i] - temp;
        if (i < y.blocks)
            result.array[i] -= y.array[i];
        if (result.array[i] < 0) {
            temp = x.array[i];
            result.array[i] += DIV;
        }
    }
    for (int j = 34; j >= 0; --j) {
        if (result.array[j] != 0) {
            result.blocks = j + 1;
            break;
        }
    }
    return result;
}

void printf_value(struct uint1024_t x) {
    if (x.blocks) printf("%u", x.array[x.blocks - 1]); else printf("0\n");
    for (int i = x.blocks - 2; i > -1; --i)
        printf("%09u", x.array[i]);
    printf("\n");
}

struct uint1024_t mult_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t result;
    for (int i = 0; i < 35; ++i)
        result.array[i] = 0;
    for (int i = 0; i < 35; ++i)
        for (int j = 0, flag = 0; 35 > i + j; ++j) {
            long long current = result.array[i + j] + x.array[i] * 1ll * y.array[j] + flag;
            result.array[i + j] = (int) (current % DIV);
            flag = (int) (current / DIV);
            if (flag && i == 34)
                break;
        }
    for (int k = 34; k >-1; --k) {
        if (result.array[k] != 0) {
            result.blocks = k + 1;
            break;
        }
    }
    return result;
}

void init_number_of_blocks(struct uint1024_t *x, char *string) {
    int numberOfBlocks = strlen(string);
    for (int k = numberOfBlocks; k > 0; k -= 9) {
        string[k] = '\0';
        if (k >= 9)
            x->array[x->blocks] = (unsigned int) (atoi(string + k - 9));
        else
            x->array[x->blocks] = (unsigned int) (atoi(string));
        x->blocks += 1;
    }
}

struct uint1024_t pow_op(struct uint1024_t x, int power) {
    struct uint1024_t result;
    make_inited(&result);
    result.array[0] = 1;
    result.blocks = 1;
    struct uint1024_t temp = x;
    power++;
    while (power) {
        if (power % 2 == 0) {
            power /= 2;
            temp = mult_op(temp, x);
        } else {
            power--;
            result = mult_op(result, temp);
        }
    }
    return result;
}

void scanf_value(struct uint1024_t *x) {
    make_inited(x);
    char *input = (char *) (calloc(310, sizeof(char)));
    scanf("%s", input);
    init_number_of_blocks(x, input);
}

int main() {
    struct uint1024_t x, y;
    scanf_value(&x);
    scanf_value(&y);
    printf("mult_op:\n");
    printf_value(mult_op(x, y));
    printf("\npow_op:\n");
    printf_value(pow_op(x, 5));
    return 0;
}


