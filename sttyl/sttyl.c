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

struct cc {
    const char* c_name;
    cc_t c_cc;
};

#define FLAG_NUM 11
struct flag flags[FLAG_NUM] = {
    {cflag, "icanon", ICANON},
    {cflag, "hupcl", HUPCL},   {iflag, "brkint", BRKINT},
    {iflag, "inpck", INPCK},   {iflag, "icnrl", ICRNL},
    {iflag, "ixany", IXANY},   {oflag, "onlcr", ONLCR},
    {lflag, "iexten", IEXTEN}, {lflag, "echo", ECHO},
    {lflag, "echoe", ECHOE},   {lflag, "echok", ECHOK}};

#define CC_NUM 3
struct cc ccs[CC_NUM] = {
    {"intr", VINTR},
    {"erase", VERASE},
    {"kill", VKILL}
};

int flag_is_enabled(struct flag*, struct termios*);
struct flag *flag_find(struct flag *, const char *);
struct cc *cc_find(struct cc*, const char *);
void flags_print(struct flag *, enum flag_type, struct termios *);
void ccs_print(struct cc *, struct termios *);
void flag_set(struct flag*, int, struct termios*);

int main(int argc, char *argv[]) {
    struct termios *options = malloc(sizeof(struct termios));

    if (!isatty(STDIN_FILENO)){
        fprintf(stderr, "STDIN is not a terminal\n");
        return -1;
    }

    tcgetattr(STDIN_FILENO, options);
    
    if (argc == 1) {
        speed_t speed = cfgetospeed(options);
        printf("speed %d baud; ", speed);

        // print cflags
        flags_print(flags, cflag, options);
        printf("\n");

        ccs_print(ccs, options);

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
        while (--argc){
            ++argv;
            
            int option_disabled = (*argv[0] == '-');
            
            char* option_name = malloc(sizeof(char) * (strlen(*argv) - option_disabled));
            strcpy(option_name, *argv + option_disabled);

            struct flag* flag = flag_find(flags, option_name);
            struct cc* cc = cc_find(ccs, option_name);
            
            int is_cc = !flag && cc;
            int is_flag = flag && !cc;
            
            if (!is_cc && !is_flag){
                printf("Unknown mode\n");
                return -1;
            }

            if (is_flag){
                flag_set(flag, option_disabled, options);
            }
            
            if (tcsetattr(STDIN_FILENO, TCSANOW, options) == -1){
                perror("tcsetattr");
                return -1;
            }

            free(option_name);
        }
    }
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

struct cc *cc_find(struct cc* ccs, const char * c_name) {
    for (int i = 0; i < CC_NUM; ++i){
        struct cc cc = ccs[i];
        if (strcmp(cc.c_name, c_name) == 0){
            return &ccs[i];
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

void ccs_print(struct cc * ccs, struct termios * options) {
    for (int i = 0; i < CC_NUM; ++i){
        struct cc cc = ccs[i];
        cc_t code = options->c_cc[cc.c_cc];
        printf("%s = ", cc.c_name);
        if (code >= 0 && code < 32){
            printf("^%c ", code + '@');
        }
        else if (code == 127){
            printf("^? ");
        }
        else if (code >= 32 && code < 127){
            printf("%c ", code);
        }
        else {
            printf("0x%X ", code);
        }
    }
}

void flag_set(struct flag* flag, int disable, struct termios* options){
    switch (flag->flag_type){
    case cflag:
        if (disable)
            options->c_cflag &= ~flag->flag;
        else 
            options->c_cflag |= flag->flag;
        break;
    case iflag:
        if (disable)
            options->c_iflag &= ~flag->flag;
        else
            options->c_iflag |= flag->flag;
        break;
    case oflag:
        if (disable)
            options->c_oflag &= ~flag->flag;
        else
            options->c_oflag |= flag->flag;
        break;
    case lflag:
        if (disable)
            options->c_lflag &= ~flag->flag;
        else
            options->c_lflag |= flag->flag;
        break;
    }
    
}
