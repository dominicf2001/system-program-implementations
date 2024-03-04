#pragma once

struct pppaddle {
    int pad_top, pad_bot, pad_col;
    char pad_char;
};

struct pppaddle paddle_init();

void paddle_draw(struct pppaddle*);

void paddle_up(struct pppaddle*);

void paddle_down(struct pppaddle*);

void paddle_contact(struct pppaddle*, int, int);
