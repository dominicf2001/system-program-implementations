#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

enum flag_type {cflag, iflag, oflag};

struct flag {
    enum flag_type flag_type;
    const char* flag_name;
    tcflag_t flag;
};

#define FLAG_NUM 2
struct flag flags[FLAG_NUM] = {
    {cflag, "parenb", PARENB},
    {cflag, "icanon", ICANON},
};

int flag_is_enabled(struct flag*, struct termios*);
struct flag* flag_find(struct flag*, const char*);

int main() {
    int fd;
    if ((fd = open("/dev/tty1" , O_RDWR)) == -1) {
        perror("Opening tty");
        return -1;
    }

    struct termios *options = malloc(sizeof(struct termios));
    
    tcgetattr(fd, options);

    speed_t speed = cfgetospeed(options);
    printf("speed %d baud; ", speed);

    // print flags
    for (int i = 0; i < FLAG_NUM; ++i){
        struct flag flag = flags[i];
        int flag_enabled = flag_is_enabled(&flag, options);
        if (!flag_enabled)
            printf("-");
        printf("%s ", flag.flag_name);
    }
    
    printf("\n");
    
    free(options);
    close(fd);
}

int flag_is_enabled(struct flag *flag, struct termios *options){
    int flag_enabled = 1;
    
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
