#include "curses.h"
#include "./paddle.h"
#include "./pong.h"
#include <stdio.h>

struct pppaddle paddle_init() {
    struct pppaddle paddle = {
        .pad_bot = BOT_ROW - 4,
        .pad_top = TOP_ROW + 6,
        .pad_char = ']',
        .pad_col = RIGHT_EDGE - 2
    };
    
    return paddle;
}

void paddle_up(struct pppaddle* paddle){
    mvaddch(paddle->pad_top, paddle->pad_col, BLANK);
    paddle->pad_top -= 1;
    mvaddch(paddle->pad_bot, paddle->pad_col, BLANK);
    paddle->pad_bot -= 1;
}

void paddle_down(struct pppaddle* paddle){
    mvaddch(paddle->pad_top, paddle->pad_col, BLANK);
    paddle->pad_top += 1;
    mvaddch(paddle->pad_bot, paddle->pad_col, BLANK);
    paddle->pad_bot += 1;
}

void paddle_draw(struct pppaddle* paddle) {
    for (int y = paddle->pad_top; y <= paddle->pad_bot; ++y){
        mvaddch(y, paddle->pad_col, paddle->pad_char);
    }
}