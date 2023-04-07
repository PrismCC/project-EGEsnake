#ifndef EGESNAKE_PLAY_H
#define EGESNAKE_PLAY_H

#include "head.h"
#include "structs.h"

bool play();

void init_pos_di();

void setFood();

std::pair<int,int> trans_key(int key);

int input_di();

bool isEnd();


#endif //EGESNAKE_PLAY_H
