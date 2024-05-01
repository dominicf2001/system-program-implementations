#include "curses.h"
#include "./paddle.h"
#include "./netpong.h"

struct pppaddle paddle_init(enum HOST_TYPE host_type) {
    struct pppaddle paddle = {
        .pad_bot = BOT_ROW - 4,
        .pad_top = TOP_ROW + 6,
        .pad_char = host_type == CLIENT ? 
            ']' :
            '[',
        .edge_side = host_type == CLIENT ?
            RIGHT_EDGE :
            LEFT_EDGE,
        .pad_col = host_type == CLIENT ? 
            RIGHT_EDGE - 2 :
            LEFT_EDGE
    };
    
    return paddle;
}

void paddle_up(struct pppaddle* paddle){
    if (paddle->pad_top == TOP_ROW)
        return;
    
    mvaddch(paddle->pad_top, paddle->pad_col, BLANK);
    paddle->pad_top -= 1;
    mvaddch(paddle->pad_bot, paddle->pad_col, BLANK);
    paddle->pad_bot -= 1;
}

void paddle_down(struct pppaddle* paddle){
    if (paddle->pad_bot == BOT_ROW)
        return;
    
    mvaddch(paddle->pad_top, paddle->pad_col, BLANK);
    paddle->pad_top += 1;
    mvaddch(paddle->pad_bot, paddle->pad_col, BLANK);
    paddle->pad_bot += 1;
}

int paddle_contact(struct pppaddle* paddle, int y, int x) {
    int offset = paddle->edge_side == RIGHT_EDGE ?
        -2 :
        2;
    
    return x == (paddle->pad_col + offset) && paddle->pad_top <= y && y <= paddle->pad_bot;
}

void paddle_draw(struct pppaddle* paddle) {
    for (int y = paddle->pad_top; y <= paddle->pad_bot; ++y){
        mvaddch(y, paddle->pad_col, paddle->pad_char);
    }
}
