#ifndef EGESNAKE_OTHERS_H
#define EGESNAKE_OTHERS_H

#include "head.h"
#include <random>
#include <io.h>

void set_seed();

void load_file_list (const std::string &path, std::vector<std::string> &result);

std::vector<int> init_ai_color (const bool *used_color, int num);

std::vector<int> random_select (std::vector<int> &nums, int m);

int random_int (int from, int to);

int random_direction (int way[3]);

#endif //EGESNAKE_OTHERS_H
