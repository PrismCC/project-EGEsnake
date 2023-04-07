#ifndef EGESNAKE_PAINT_H
#define EGESNAKE_PAINT_H

#include "graphics.h"
#include "head.h"
#include "ege/sys_edit.h"


bool begin ();

void data_init();

void print_data_init();

void box_init (sys_edit &editBox);

void logo_warn (PIMAGE &decorate_image, std::string &warning);

void close_box(PIMAGE &decorate_image);

void get_zoom_image (PIMAGE img_ptr, const char *file_name, int width, int height);

void board_init();

void board_print (bool print_score, bool add);

void play_print();

void group_init ();

void score_print (bool add);

void end_print();


#endif //EGESNAKE_PAINT_H