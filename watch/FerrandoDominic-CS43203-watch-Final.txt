/* CS4-53203: Systems Programming
Name: Dominic Ferrando
Date: 02/03/2024
watch.txt
*/

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <utmp.h>
#include <string.h>
#include <pwd.h>
#include "./utmplib.h"

const char **online_lognames;
const char **watched_lognames;
      int   *online_watched_lognames;

int lognames_includes(const char** lognames, const char* logname, const size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (strcmp(lognames[i], logname) == 0){
            return 1;
        }
    }
    return 0;
}

void lognames_initialize(const char* lognames[], const size_t size) {
    for (int i = 0; i < size; ++i) {
        lognames[i] = NULL;
    }
}

void lognames_print(const char* lognames[], const size_t size) {
    for (int i = 0; i < size; ++i){
        printf("%s ", lognames[i]);
    }
    printf("Are currently logged in\n");
}

int is_number(const char* str){
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
        free(watched_lognames);
        free(online_lognames);
        free(online_watched_lognames); 
        exit(EXIT_FAILURE);
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
                free(watched_lognames);
                free(online_lognames);
                free(online_watched_lognames); 
                exit(EXIT_FAILURE);
            }
            online_lognames = temp;
        }
        online_lognames[count++] = cur_rec->ut_user;
    }

    utmp_close();

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

    const size_t SIZE = is_number(argv[1]) ?
        argc - 2 :
        argc - 1;

    // Initialize watched lognames
    if ((watched_lognames = malloc(sizeof(char*) * SIZE)) == NULL){
        perror("Error allocating memory for watched_lognames");
        free(watched_lognames);
        free(online_lognames);
        free(online_watched_lognames);
        return -1;
    }
    int offset = is_number(argv[1]) ? 2 : 1;
    for (size_t i = offset, j = 0; i < argc; ++i, ++j) {
        watched_lognames[j] = argv[i];
    }

    // initialize online watched lognames
    if ((online_watched_lognames = malloc(sizeof(int) * SIZE)) == NULL){
        perror("Error allocating memory for online_watched_lognames");
        free(watched_lognames);
        free(online_lognames);
        free(online_watched_lognames);
        return -1;
    }

    int online_count = lognames_get_online();
    for (int i = 0; i < SIZE; ++i) {
        // check if logname is online
        const char* watched_logname = watched_lognames[i];
        int is_online = 0;
        if (lognames_includes(online_lognames, watched_logname, online_count)){
            is_online = 1;
            printf("%s ", watched_logname);
        }
        online_watched_lognames[i] = is_online;
    }
    printf("are currently logged in\n");

    /* for (int i = 0; i < SIZE; ++i) { */
    /*     printf("%s ", watched_lognames[i]); */
    /*     printf("%d\n", online_watched_lognames[i]); */
    /* } */

    uid_t uid = getuid();
    struct passwd* passwd = getpwuid(uid);
    const char* caller_logname = passwd->pw_name;

    while (1){
        printf("Checking...\n");
        int online_count = lognames_get_online();
        
        for (int i = 0; i < SIZE; ++i) {
            // check if logname is online
            const char* watched_logname = watched_lognames[i];
            int is_online = 0;
            if (lognames_includes(online_lognames, watched_logname, online_count)){
                is_online = 1;
            }
            const int was_online = online_watched_lognames[i];
            if (was_online == 1 && is_online == 0){
                printf("%s logged out\n", watched_logname);
                if (strcmp(watched_logname, caller_logname) == 0){
                    free(watched_lognames);
                    free(online_lognames);
                    free(online_watched_lognames); 
                    exit(EXIT_SUCCESS);
                }
            }
            if (was_online == 0 && is_online == 1){
                printf("%s logged in\n", watched_logname);                
            }
            online_watched_lognames[i] = is_online;
        }

        sleep(INTERVAL);
    }

    free(watched_lognames);
    free(online_lognames);
    free(online_watched_lognames);
}
