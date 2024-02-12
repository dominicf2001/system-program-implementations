#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

//int flags[3][3] = {
//    {} // cflags
//};

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

    // c flags
    int is_parenb = options->c_cflag & PARENB;
    if (!is_parenb){
        printf("-");
    }
    printf("parenb ");

    int is_icanon = options->c_cflag & ICANON;
    if (!is_icanon){
        printf("-");
    }
    printf("icanon ");


    
    printf("\n");

    close(fd);
}
