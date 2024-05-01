// CS4-53203: Systems Programming
// Name: Dominic Ferrando
// Date: 03/04/2024
// pong.txt

#include <bits/time.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "./paddle.h"
#include "netpong.h"

#define BUFFER_SIZE 100

enum HOST_TYPE host_type = SERVER;

enum PLAY_STATE play_state = WAIT;

struct ppball ball;
struct pppaddle paddle;

int balls_left = 3;

void ball_move(int);
int bounce_or_lose(struct ppball*);

void draw_walls();

void set_up();
int execute(char*);
void serve();
void wrap_up();
char** split(char*);

int set_ticker(int);

int main(int argc, char** argv) {
    if (argc != 2){
        printf("Invalid arguments");
        return 1;
    }
    
    if (strcmp(argv[1], "-c") == 0){
        host_type = CLIENT;
    }
    else if (strcmp(argv[1], "-s") == 0){
        host_type = SERVER;
    }

    // networking setup
    // ----

    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        perror("Failed to open socket\n");
        exit(1);   
    }

    // setup server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1234);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int connfd;
    if (host_type == SERVER){        
        if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
            perror("Failed to bind socket\n");
            exit(1);
        }

        if (listen(sockfd, 5) == - 1){
            perror("Failed to listen on socket\n");
            exit(1);
        };

        connfd = accept(sockfd, NULL, NULL);
        if (sockfd == -1){
            perror("Failed to accept socket\n");
            exit(1);   
        }

        char cmd_welcome[BUFFER_SIZE] = { 0 };
        sprintf(cmd_welcome, "HELO 1.0 %d %d server\n", TICKS_PER_SEC, TOP_ROW - BOT_ROW);
        write(connfd, cmd_welcome, BUFFER_SIZE);

        char res[BUFFER_SIZE] = { 0 };
        read(connfd, res, BUFFER_SIZE - 1);
        char** cmd_name_arr = split(res);

        if (strcmp(cmd_name_arr[0], "NAME") != 0){
            printf("SPPBTP introduction failed: Unexpected command\n");
            exit(1);
        }
        free(cmd_name_arr);

        play_state = WAIT;
    }
    else if (host_type == CLIENT){        
        if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))){
            perror("Failed to connect to socket\n");
            exit(1);
        };

        connfd = sockfd;

        char res[BUFFER_SIZE] = { 0 };
        read(connfd, res, BUFFER_SIZE - 1);

        char** cmd_welcome_arr = split(res);

        if (strcmp(cmd_welcome_arr[0], "HELO") != 0){
            printf("SPPBTP introduction failed: Unexpected command\n");
            exit(1);
        }

        char cmd_name[BUFFER_SIZE] = "NAME 1.0 client\n";
        write(connfd, cmd_name, BUFFER_SIZE);
        free(cmd_welcome_arr);

        play_state = PLAY;
    }
    // ----
    
    int c = 0;

    set_up();

    if (host_type == CLIENT){
        serve();
    }

    while ( balls_left && (c = getchar()) != 'Q' ){        
        switch (play_state) {
        case WAIT:
            {
                char cmd[BUFFER_SIZE] = { 0 };
                read(connfd, cmd, BUFFER_SIZE - 1);

                execute(cmd);
            }
            break;

        case PLAY:
            {
                if (c  == 'k') {
                    paddle_up(&paddle);
                    paddle_draw(&paddle);
                }
                else if (c  == 'j') {
                    paddle_down(&paddle);
                    paddle_draw(&paddle);
                };
            }
            break;
        }
    }

    wrap_up();
    close(sockfd);
}

int execute(char* cmd) {
    int status = -1;
    char** cmd_arr = split(cmd);
    char* cmd_name = cmd_arr[0];

    free(cmd_arr);
    return status;
}

void serve() {
    ball.y_pos = 10;
    ball.x_pos = host_type == CLIENT ?
        15 :
        55;
    ball.y_ttg = ball.y_ttm = MIN_X_TTM + (rand() % MAX_X_TTM);
    ball.x_ttg = ball.x_ttm = MIN_Y_TTM + (rand() % MAX_Y_TTM);
    ball.y_dir = 1;
    ball.x_dir = host_type == CLIENT ?
        1 :
        -1;
    ball.symbol = DFL_SYMBOL;
}

void set_up() {
    srand(getpid());
    
    initscr();
    noecho();
    crmode();

    mvaddch(5, 5, '0' + balls_left);

    paddle = paddle_init(host_type);
    paddle_draw(&paddle);
    
    draw_walls();
    
    signal(SIGINT, SIG_IGN);
    mvaddch(ball.y_pos, ball.x_pos, ball.symbol);
    refresh();

    signal(SIGALRM, ball_move);
    set_ticker(1000 / TICKS_PER_SEC);
}

void ball_move(int signum) {
    int y_cur, x_cur, moved;
    signal(SIGALRM, SIG_IGN);
    y_cur = ball.y_pos;
    x_cur = ball.x_pos;
    moved = 0;

    if (ball.y_ttm > 0 && ball.y_ttg-- == 1){
        ball.y_pos += ball.y_dir;
        ball.y_ttg = ball.y_ttm;
        moved = 1;
    }

    if (ball.x_ttm > 0 && ball.x_ttg-- == 1){
        ball.x_pos += ball.x_dir;
        ball.x_ttg = ball.x_ttm;
        moved = 1;
    }

    if (moved){
        mvaddch(y_cur, x_cur, BLANK);
        mvaddch(y_cur, x_cur, BLANK);
        mvaddch(ball.y_pos, ball.x_pos, ball.symbol);
        bounce_or_lose(&ball);
        move(LINES-1, COLS-1);
        refresh();
    }
    signal(SIGALRM, ball_move);
}

int bounce_or_lose(struct ppball *bp) {
    int return_val = 0;
    const int losing_edge = host_type == CLIENT ?
        RIGHT_EDGE + 2 : 
        LEFT_EDGE - 2;
    const int non_losing_edge = host_type == CLIENT ?
        LEFT_EDGE + 2: 
        RIGHT_EDGE - 2;

    if (paddle_contact(&paddle, bp->y_pos, bp->x_pos)){
        ball.y_ttg = ball.y_ttm = MIN_X_TTM + (rand() % MAX_X_TTM);
        ball.x_ttg = ball.x_ttm = MIN_Y_TTM + (rand() % MAX_Y_TTM);
        bp->x_dir = -bp->x_dir;
        return_val = 1; 
    }
    
    if (bp->y_pos == TOP_ROW){
        bp->y_dir = 1;
        return_val = 1;
    }
    else if (bp->y_pos == BOT_ROW){
        bp->y_dir = -1;
        return_val = 1;
    }
    if (bp->x_pos == non_losing_edge){
        bp->x_dir = -bp->x_dir;
        return_val = 1;
    }
    else if (bp->x_pos == losing_edge){
        return_val = -1;
        mvaddch(bp->y_pos, bp->x_pos, BLANK);
        --balls_left;
        mvaddch(5, 5, '0' + balls_left);
        serve();
    }
    return return_val;
}

void wrap_up() {
    set_ticker(0);
    endwin();
}

int set_ticker(int ms) {
    struct itimerval new_timeset;
    long sec, usecs;

    sec = ms / 1000;
    usecs = (ms % 1000) * 1000L;

    new_timeset.it_interval.tv_sec = sec;
    new_timeset.it_interval.tv_usec = usecs;
    new_timeset.it_value.tv_sec = sec;
    new_timeset.it_value.tv_usec = usecs;
    
    return setitimer(ITIMER_REAL, &new_timeset, NULL);
}

void draw_walls() {
    if (host_type == CLIENT){
        // draw left edge
        for (int y = TOP_ROW; y < BOT_ROW + 1; ++y){
            mvaddch(y, LEFT_EDGE - 1 , '|');
        }
    }
    else if (host_type == SERVER) {
        // draw right edge
        for (int y = TOP_ROW; y < BOT_ROW + 1; ++y){
            mvaddch(y, RIGHT_EDGE - 1 , '|');
        }
    }
    // draw top
    for (int x = LEFT_EDGE - 1; x < RIGHT_EDGE; ++x){
        mvaddch(TOP_ROW - 1, x , '-');
    }
    // draw bottom
    for (int x = LEFT_EDGE - 1; x < RIGHT_EDGE; ++x){
        mvaddch(BOT_ROW + 1, x , '-');
    }
}

char** split(char* str) {
    char** split_arr = malloc(sizeof(char*) * BUFFER_SIZE);
    int i = 0; 
    char* token = strtok(str, " ");
    while (token != NULL && i < BUFFER_SIZE - 1){
        split_arr[i++] = strdup(token);
        token = strtok(NULL, " ");
    }
    split_arr[i] = NULL;
    return split_arr;
}
