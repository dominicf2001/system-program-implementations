/* CS4-53203: Systems Programming
Name: Dominic Ferrando
Date: 02/03/2024
watch.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <utmp.h>
#include "./utmplib.h"

const char** online_lognames;
const char** watched_lognames;

void lognames_initialize(const char* lognames[], int size) {
    for (int i = 0; i < size; ++i) {
        lognames[i] = NULL;
    }
}

void lognames_print(const char* lognames[], const int size) {
    for (int i = 0; i < size; ++i){
        if (lognames[i])
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

int lognames_get_online() {
    if (utmp_open(NULL) == -1) {
        perror("Could not open utmp file");
    }

    size_t size = 10;
    online_lognames = malloc(sizeof(char*) * size);
    
    struct utmp* cur_rec;
    int count = 0;
    while ((cur_rec = utmp_next()) != (struct utmp*) NULL){
        if (count == size){
            size *= 2;
            const char** temp = realloc(online_lognames, sizeof(char*) * size);
            if (temp == NULL) {
                perror("Failed to allocate more memory of online lognames.");
                free(online_lognames);
                exit(EXIT_FAILURE);
            }
            online_lognames = temp;
        }
        online_lognames[count++] = cur_rec->ut_user;
    }

    return count;
}

int main (int argc, char* argv[]) {
    if (argc == 1) {
        printf("Not enough arguments\n");
        return -1;
    }
    
    // Initialize constants
    const int INTERVAL = is_number(argv[1]) ?
        atoi(argv[1]) :
        300;

    const int SIZE = is_number(argv[1]) ?
        argc - 2 :
        argc - 1;

    // Initialize lognames and do first print
    watched_lognames = malloc(sizeof(char*) * SIZE);
    int offset = is_number(argv[1]) ? 2 : 1;
    for (int i = offset, j = 0; i < argc; ++i, ++j) {
        watched_lognames[j] = argv[i];
    }
    int count = lognames_get_online();
    lognames_print(online_lognames, count);
    lognames_print(watched_lognames, SIZE);

    free(watched_lognames);
}
