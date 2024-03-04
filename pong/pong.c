// CS4-53203: Systems Programming
// Name: Dominic Ferrando
// Date: 03/04/2024
// pong.txt

#include <bits/time.h>
#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include "./pong.h"

struct ppball ball;
void ball_move(int);
int bounce_or_lose(struct ppball*);

void set_up();
void wrap_up();

int set_ticker(int);

int main(){
    int c = 0;

    set_up();

    while ( (c = getchar()) != 'Q' ){
        if (c  == 'f') ball.x_ttm--;
        else if (c  == 's') ball.x_ttm++;
        else if (c  == 'F') ball.y_ttm--;
        else if (c  == 'S') ball.y_ttm++;
    }

    wrap_up();
}

void set_up(){
    ball.y_pos = Y_INIT;
    ball.x_pos = X_INIT;
    ball.y_ttg = ball.y_ttm = Y_TTM;
    ball.x_ttg = ball.x_ttm = X_TTM;
    ball.y_dir = 1;
    ball.x_dir = 1;
    ball.symbol = DFL_SYMBOL;
    initscr();
    noecho();
    crmode();

    signal(SIGINT, SIG_IGN);
    mvaddch(ball.y_pos, ball.x_pos, ball.symbol);
    refresh();

    signal(SIGALRM, ball_move);
    set_ticker(1000 / TICKS_PER_SEC);
}

void ball_move(int signum){
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

int bounce_or_lose(struct ppball *bp){
    int return_val = 0;
    
    if (bp->y_pos == TOP_ROW){
        bp->y_dir = 1;
        return_val = 1;
    }
    else if (bp->y_pos == BOT_ROW){
        bp->y_dir = -1;
        return_val = 1;
    }
    if (bp->x_pos == LEFT_EDGE){
        bp->x_dir = 1;
        return_val = 1;
    }
    else if (bp->x_pos == RIGHT_EDGE){
        bp->x_dir = -1;
        return_val = 1;
    }
    return return_val;
}

void wrap_up(){
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
