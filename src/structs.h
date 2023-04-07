#ifndef EGESNAKE_STRUCTS_H
#define EGESNAKE_STRUCTS_H

#include "head.h"
#include <memory>

typedef struct node_t {
    int x;
    int y;
    node_t *next;
} node, *list;

struct Snake {
    int color;
    int length;
    Direction di;
    Direction ori_di;
    bool isAlive = true;
    bool isAI;
    list tail = nullptr;
    node *head = nullptr;
    int store;

    Snake (int _color, bool _isAI) : color(_color), length(1), di(Direction::null), ori_di(Direction::null),
                                     isAI(_isAI), store(2) {}

    [[nodiscard]] bool is_alive () const { return isAlive; }

    static int judge (std::vector<std::vector<int>> map, int x, int y, int deep = 5);

    void AI_direction ();

    void change_direction (int d);

    void step_into (int gy, int gx);

    void move ();

    void die ();

    ~Snake ();
};


struct GlobalData {
    static std::string mapName;
    static int map_width, map_height;
    static int player_num, total_num;
    static std::vector<Snake> snakes;
    static std::vector<std::vector<int>> snakeMap;
    static int food_num;
};

struct ColorGroup {
    int color = -1;
    int score = 0;
    int length = 0;
    int living_time = 0;

    static int t_compare (const void *a, const void *b);

    static int l_compare (const void *a, const void *b);
};

#endif //EGESNAKE_STRUCTS_H
