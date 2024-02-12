#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int main() {
    int fd;
    if ((fd = open("/dev/tty1" , O_RDWR)) == -1) {
        perror("Opening tty");
        return -1;
    }

    struct termios *options = malloc(sizeof(struct termios));
    
    tcgetattr(fd, options);

    printf("ospeed: %d\n", options->c_ospeed);

    close(fd);
}
