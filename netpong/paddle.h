#pragma once

#include "netpong.h"

struct pppaddle {
    int edge_side;
    int pad_top, pad_bot, pad_col;
    char pad_char;
};

struct pppaddle paddle_init(enum HOST_TYPE);

void paddle_draw(struct pppaddle*);

void paddle_up(struct pppaddle*);

void paddle_down(struct pppaddle*);

int paddle_contact(struct pppaddle*, int, int);

