#pragma once

#define MIN_X_TTM 2
#define MAX_X_TTM 3
#define MIN_Y_TTM 1
#define MAX_Y_TTM 4
#define BLANK ' '
#define DFL_SYMBOL 'o'
#define TOP_ROW 5
#define BOT_ROW 20
#define LEFT_EDGE 10
#define RIGHT_EDGE 70

#define X_INIT 10
#define Y_INIT 10
#define TICKS_PER_SEC 50

struct ppball {
    int y_pos, x_pos,
        y_ttm, x_ttm,
        y_ttg, x_ttg,
        y_dir, x_dir;
    char symbol;
};
