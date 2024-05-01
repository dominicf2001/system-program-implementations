#pragma once

#define MIN_X_TTM 3
#define MAX_X_TTM 4
#define MIN_Y_TTM 1
#define MAX_Y_TTM 3
#define BLANK ' '
#define DFL_SYMBOL 'o'
#define TOP_ROW 5
#define BOT_ROW 20
#define LEFT_EDGE 10
#define RIGHT_EDGE 70

enum SESSION_STATE { INTRODUCTION, PLAYBALL };
enum PLAY_STATE { PLAY, WAIT };

enum HOST_TYPE { SERVER, CLIENT };

#define TICKS_PER_SEC 75

struct ppball {
    int y_pos, x_pos,
        y_ttm, x_ttm,
        y_ttg, x_ttg,
        y_dir, x_dir;
    char symbol;
};
