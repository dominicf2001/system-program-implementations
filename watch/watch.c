/* CS4-53203: Systems Programming
Name: Dominic Ferrando
Date: 02/03/2024
watch.txt
*/

#include <stdio.h>
#include <stdlib.h>

void lognames_initialize(const char* lognames[], int size) {
    for (int i = 0; i < size; ++i) {
        lognames[i] = NULL;
    }
}

void lognames_print(const char* lognames[], const int size) {
    for (int i = 0; i < size; ++i){
        printf("%s ", lognames[i]);
    }
    printf("Are currently logged in\n");
}

int is_number(char* str){
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] < '0' || '9' < str[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}

int main (int argc, char* argv[]) {
    if (argc == 1) {
        printf("Not enough arguments\n");
        return -1;
    }

    const int INTERVAL = is_number(argv[1]) ?
        atoi(argv[1]) :
        300;

    const int SIZE = is_number(argv[1]) ?
        argc - 2 :
        argc - 1;
    
    const char* lognames[SIZE];
    
    int offset = is_number(argv[1]) ? 2 : 1;
    for (int i = offset, j = 0; i < argc; ++i, ++j) {
        lognames[j] = argv[i];
    }

    lognames_print(lognames, SIZE);
    printf("interval: %d\n", INTERVAL);
}
