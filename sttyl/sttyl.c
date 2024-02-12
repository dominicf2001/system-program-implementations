#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

enum flag_type {cflag, iflag, oflag, lflag};

struct flag {
    enum flag_type flag_type;
    const char* flag_name;
    tcflag_t flag;
};

#define FLAG_NUM 12
struct flag flags[FLAG_NUM] = {
    {cflag, "parenb", PARENB},
    {cflag, "icanon", ICANON},
    {cflag, "hupcl", HUPCL},
    {iflag, "brkint", BRKINT},
    {iflag, "inpck", INPCK},
    {iflag, "icnrl", ICRNL},
    {iflag, "ixany", IXANY},
    {oflag, "onlcr", ONLCR},
    {lflag, "iexten", IEXTEN},
    {lflag, "echo", ECHO},
    {lflag, "echoe", ECHOE},
    {lflag, "echok", ECHOK}
};

int flag_is_enabled(struct flag*, struct termios*);
struct flag *flag_find(struct flag*, const char *);
void flags_print(struct flag*, enum flag_type, struct termios*);

int main(int argc, char *argv[]) {
    struct termios *options = malloc(sizeof(struct termios));

    if (argc == 1) {
        tcgetattr(STDIN_FILENO, options);
    
        speed_t speed = cfgetospeed(options);
        printf("speed %d baud; ", speed);

        // print cflags
        flags_print(flags, cflag, options);
        printf("\n");

        // print iflags
        flags_print(flags, iflag, options);
        // print oflags
        flags_print(flags, oflag, options);
        printf("\n");

        // print lflags
        flags_print(flags, lflag, options);    
        printf("\n");   
    }
    else {
        for (int i = 1; i < argc; ++i){
            int flag_disabled = argv[i][0] == '-';
            
            char* flag_name = malloc(sizeof(char) * (strlen(argv[i]) - flag_disabled));
            strcpy(flag_name, argv[i] + flag_disabled);

            printf("name: %s\n", flag_name);
            struct flag* flag = NULL;
            if ((flag = flag_find(flags, flag_name)) == NULL){
                printf("Unknown mode\n");
                return -1;
            }

            tcsetattr(STDIN_FILENO, TCSANOW, options);int main(int argc, char *argv[]) {
    struct termios *options = malloc(sizeof(struct termios));

    if (argc == 1) {
        tcgetattr(STDIN_FILENO, options);
    
        speed_t speed = cfgetospeed(options);
        printf("speed %d baud; ", speed);

        // print cflags
        flags_print(flags, cflag, options);
        printf("\n");

        // print iflags
        flags_print(flags, iflag, options);
        // print oflags
        flags_print(flags, oflag, options);
        printf("\n");

        // print lflags
        flags_print(flags, lflag, options);    
        printf("\n");   
    }
    else {
        for (int i = 1; i < argc; ++i){
            int flag_disabled = argv[i][0] == '-';
            
            char* flag_name = malloc(sizeof(char) * (sizeof(argv[i]) - flag_disabled));
            strcpy(flag_name, argv[i] + flag_disabled);
            
            struct flag* flag = NULL;
            if ((flag = flag_find(flags, flag_name)) == NULL){
                printf("Unknown mode\n");
                return -1;
            }

            tcsetattr(STDIN_FILENO, TCSANOW, options);
            free(flag_name);
        }
    }
    
    free(options);
}
        }
    }
    
    free(options);
}

int flag_is_enabled(struct flag *flag, struct termios *options){
    int flag_enabled = 0;
    
    switch (flag->flag_type){
    case cflag:
        flag_enabled = options->c_cflag & flag->flag;
        break;
    case iflag:
        flag_enabled = options->c_iflag & flag->flag;
        break;
    case oflag:
        flag_enabled = options->c_oflag & flag->flag;
        break;
    case lflag:
        flag_enabled = options->c_lflag & flag->flag;
        break;
    default:
        return -1;
    }
    return flag_enabled;
}

struct flag* flag_find(struct flag* flags, const char* flag_name) {
    for (int i = 0; i < FLAG_NUM; ++i){
        if (strcmp(flags[i].flag_name, flag_name) == 0){
            return &flags[i];
        }
    }
    return NULL;
}

void flags_print(struct flag* flags, enum flag_type flag_type, struct termios* options){ 
    for (int i = 0; i < FLAG_NUM; ++i){
        if (flags[i].flag_type != flag_type)
            continue;
        
        struct flag flag = flags[i];
        int flag_enabled = flag_is_enabled(&flag, options);
        if (!flag_enabled)
            printf("-");
        printf("%s ", flag.flag_name);
    }
}
