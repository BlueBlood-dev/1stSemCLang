#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


struct Node {
    int time;
    int counter;
    struct Node *previous;
    struct Node *next;
};

struct List {
    struct Node *first;
    struct Node *last;
};

struct timeManager {
    int end;
    int max;
    int begin;
};

void Push(struct List *list, int time, int counter) {
    struct Node *element = (struct Node *) malloc(sizeof(struct Node));
    element->time = time;
    element->counter = counter;
    element->previous = NULL;
    element->next = NULL;
    if (list->first == NULL)
        list->last = element;
    else {
        element->next = list->first;
        list->first->previous = element;
    }
    list->first = element;
}

void Pop(struct List *list) {
    struct Node *element = list->last;
    if (element != NULL) {
        if (element->previous != NULL) {
            list->last = element->previous;
            list->last->next = NULL;
        } else {
            list->first = NULL;
            list->last = NULL;
        }
        free(element);
    }
}

char *getLine(FILE *in, int counter, int *isEnd) {
    int symbol = getc(in);
    char *line;
    if (symbol == '\n' || symbol == EOF) {
        line = (char *) malloc(counter + 1);
        line[counter] = '\0';
        if (symbol == EOF)
            *isEnd = 1;
    } else {
        line = getLine(in, counter + 1, isEnd);
        line[counter] = symbol;
    }
    return line;
}

void parse(char *line, char **requests, char **times, char **statuses) {
    strtok(line, "[");
    *times = strtok(NULL, "]");
    strtok(NULL, "\"");
    *requests = strtok(NULL, "\"");
    *statuses = strtok(NULL, " ");
}

int countTheSecondsInterpreter(char *timings) {
    if (strcmp(timings, "seconds") == 0) return 1;
    else if (strcmp(timings, "minutes") == 0) return 60;
    else if (strcmp(timings, "hours") == 0) return 3600;
    else if (strcmp(timings, "days") == 0) return 86400;
    else return 0;
}

int inputPeriod() {
    printf("Enter the time period which you prefer\n");
    char *userInput[100];
    scanf("%99[^\n]", &userInput);
    char *tmp;
    int period = 0;
    int num;
    for (int i = 0; i > -1; ++i) {
        tmp = strtok((char *) (i == 0 ? userInput : NULL), " ");
        if (tmp == NULL) break;
        num = atoi(tmp);
        if (num == 0) return 0;
        tmp = strtok(NULL, " ");
        if (tmp == NULL) return 0;
        if (countTheSecondsInterpreter(tmp) == 0) return 0;
        period += num * countTheSecondsInterpreter(tmp);
    }
    return period;
}

int getTime(char *times) {
    struct tm time;
    char *tmp = strtok(times, "/: ");
    // printf("%s\n",tmp);
    time.tm_mday = atoi(tmp);
    tmp = strtok(NULL, "/: ");
    if (strcmp(tmp, "Jan") == 0) time.tm_mon = 0;
    else if (strcmp(tmp, "Feb") == 0)
        time.tm_mon = 1;
    else if (strcmp(tmp, "Mar") == 0)
        time.tm_mon = 2;
    else if (strcmp(tmp, "Apr") == 0)
        time.tm_mon = 3;
    else if (strcmp(tmp, "May") == 0)
        time.tm_mon = 4;
    else if (strcmp(tmp, "Jun") == 0)
        time.tm_mon = 5;
    else if (strcmp(tmp, "Jul") == 0)
        time.tm_mon = 6;
    else if (strcmp(tmp, "Aug") == 0)
        time.tm_mon = 7;
    else if (strcmp(tmp, "Sep") == 0)
        time.tm_mon = 8;
    else if (strcmp(tmp, "Oct") == 0)
        time.tm_mon = 9;
    else if (strcmp(tmp, "Nov") == 0)
        time.tm_mon = 10;
    else if (strcmp(tmp, "Dec") == 0)
        time.tm_mon = 11;
    tmp = strtok(NULL, "/: ");
    time.tm_year = atoi(tmp) - 1900;
    tmp = strtok(NULL, "/: ");
    time.tm_hour = atoi(tmp);
    tmp = strtok(NULL, "/: ");
    time.tm_min = atoi(tmp);
    tmp = strtok(NULL, "/: ");
    time.tm_sec = atoi(tmp);
    return mktime(&time);
}

void getTheFailedRequestsAndPeriod(int period) {
    FILE *file = fopen("access_log_Jul95", "rb");
    if (file == NULL) {
        printf("file wasn't opened");
    } else {
        char *requests;
        char *statuses;
        char *times;
        int counter = 0;
        int index = 0;
        int isEnd = 0;
        struct List list = {NULL, NULL};
        struct timeManager manager = {0, 0, 0};
        printf("failed requests: ...\n");
        while (!isEnd) {
            char *line = getLine(file, 0, &isEnd);
            parse(line, &requests, &times, &statuses);
            if (times != NULL) {
                int time = getTime(times);
                if (atoi(statuses) >= 500 && atoi(statuses) <= 599) {
                    printf("%s\n", requests);
                    counter++;
                }
                Push(&list, time, index);
                while (time - list.last->time > period)
                    Pop(&list);
                int amount = index - list.last->counter + 1;
                if (amount > manager.max) {
                    manager.max = amount;
                    manager.begin = list.last->time;
                    manager.end = time;
                }
            }
            free(line);
            index++;
        }
        printf("---------------------------------\n");
        printf("The amount of failed requests: %d\n", counter);
        time_t temp = manager.begin;
        struct tm *time;
        time = localtime(&temp);
        printf("The biggest amount of request started at %s", asctime(time));
        temp = manager.end;
        time = localtime(&temp);
        printf(" and finished at %s\n", asctime(time));
        printf("The amount of requests was %d",manager.max);
        fclose(file);
    }
}

int main() {
    int period = inputPeriod();
    if (period <= 0) {
        printf("user input is invalid");
        return -1;
    }
    getTheFailedRequestsAndPeriod(period);
}