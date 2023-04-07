#include "structs.h"
#include "others.h"

const int oppo_di[5] = {0, 3, 4, 1, 2};

void Snake::change_direction (int d) {
    if ((int) ori_di == oppo_di[d])
        return;
    switch (d) {
        case 1:
            di = Direction::up;
            break;
        case 2:
            di = Direction::right;
            break;
        case 3:
            di = Direction::down;
            break;
        case 4:
            di = Direction::left;
            break;
        default:
            break;
    }
    GlobalData::snakeMap[head->y][head->x] = 200 + color * 4 + (int) di - 1;
}

void Snake::step_into (int gy, int gx) {
    GlobalData::snakeMap[head->y][head->x] = 100 + color;
    GlobalData::snakeMap[gy][gx] = 200 + color * 4 + (int) di - 1;

    node *newNode = new node;
    newNode->y = gy;
    newNode->x = gx;
    newNode->next = nullptr;
    head->next = newNode;
    head = newNode;
}

void Snake::move () {
    if (isAI)
        AI_direction();

    int gx = head->x, gy = head->y;
    int height = (int) GlobalData::snakeMap.size();
    int width = (int) GlobalData::snakeMap[0].size();
    if (di == Direction::up)
        gy = (gy - 1 + height) % height;
    else if (di == Direction::down)
        gy = (gy + 1) % height;
    else if (di == Direction::right)
        gx = (gx + 1) % width;
    else if (di == Direction::left)
        gx = (gx - 1 + width) % width;
    int block = GlobalData::snakeMap[gy][gx];

    if (block == 2) {
        ++store;
        --GlobalData::food_num;
    }

    if (store > 0) {
        --store;
        ++length;
    }
    else {
        node *p = tail->next;
        GlobalData::snakeMap[tail->y][tail->x] = 0;
        free(tail);
        tail = p;
    }


    switch (block) {
        case 2:
        case 0:
            step_into(gy, gx);
            break;
        case 1:
            die();
            return;
        default:
            if (block > 100 && block <= 200)
                die();
            else {
                for (auto &x: GlobalData::snakes) {
                    if (!x.is_alive() || x.color == color)
                        continue;
                    if (x.head->y == gy && x.head->x == gx) {
                        x.die();
                        break;
                    }
                }
                die();
            }
            return;
    }
    ori_di = di;
}

void Snake::die () {
    isAlive = false;
    node *p = tail->next;
    while (tail != head) {
        int r = random_int(0, 3);
        if (r == 0) {
            GlobalData::snakeMap[tail->y][tail->x] = 2;
            ++GlobalData::food_num;
        }
        else
            GlobalData::snakeMap[tail->y][tail->x] = 0;
        free(tail);
        tail = p;
        p = p->next;
    }
    GlobalData::snakeMap[tail->y][tail->x] = 2;
    ++GlobalData::food_num;
    free(tail);
    tail = head = nullptr;
    length = 0;
    store = 0;
}

void Snake::AI_direction () {
    int height = GlobalData::map_height, width = GlobalData::map_width;

    if (!is_alive())
        exit(3);
    int way[3] = {1, 2, 1};
    int headX = head->x, headY = head->y;
    int straightX, straightY, turnLeftX, turnLeftY, turnRightX, turnRightY;
    switch (ori_di) {
        case Direction::up:
            straightX = headX;
            straightY = (headY - 1 + height) % height;
            turnLeftX = (headX - 1 + width) % width;
            turnRightX = (headX + 1) % width;
            turnLeftY = turnRightY = headY;
            break;
        case Direction::down:
            straightX = headX;
            straightY = (headY + 1) % height;
            turnRightX = (headX - 1 + width) % width;
            turnLeftX = (headX + 1) % width;
            turnLeftY = turnRightY = headY;
            break;
        case Direction::right:
            straightX = (headX + 1) % width;
            straightY = headY;
            turnRightY = (headY + 1) % height;
            turnLeftY = (headY - 1 + height) % height;
            turnLeftX = turnRightX = headX;
            break;
        case Direction::left:
            straightX = (headX - 1 + width) % width;
            straightY = headY;
            turnLeftY = (headY + 1) % height;
            turnRightY = (headY - 1 + height) % height;
            turnLeftX = turnRightX = headX;
            break;
        default:
            exit(2);
    }
    int block[3] = {GlobalData::snakeMap[turnLeftY][turnLeftX], GlobalData::snakeMap[straightY][straightX],
                    GlobalData::snakeMap[turnRightY][turnRightX]};
    int judge_table[3][2] = {{turnLeftY,  turnLeftX},
                             {straightY,  straightX},
                             {turnRightY, turnRightX}};

    std::vector<std::vector<int>> temp_map=GlobalData::snakeMap;

    for (int j = 0; j < 3; ++j) {
        if (block[j] == 0 || block[j] == 2) {
            int ori = temp_map[judge_table[j][0]][judge_table[j][1]];
            temp_map[judge_table[j][0]][judge_table[j][1]] = 1;
            way[j] *= (block[j] == 2 ? 5 : 1)* judge(temp_map,judge_table[j][1],judge_table[j][0]);
            temp_map[judge_table[j][0]][judge_table[j][1]] = ori;
        }
        else
            way[j] = 0;
    }
    int di_table[4][3] = {
            {4, 1, 2},
            {1, 2, 3},
            {2, 3, 4},
            {3, 4, 1}};
    int choice = random_direction(way);
    change_direction(di_table[(int) ori_di - 1][choice]);
}

Snake::~Snake () {
    if (tail == nullptr || head == nullptr)
        return;
    node *p = tail->next;
    while (tail != head) {
        free(tail);
        tail = p;
        p = p->next;
    }
    free(tail);
}

int Snake::judge (std::vector<std::vector<int>> map, int x, int y, int deep) {
    if (deep < 0)
        return 1;
    int w = GlobalData::map_width, h = GlobalData::map_height;
    int score = 0;
    int up_y = (y + h - 1) % h, down_y = (y + 1) % h;
    int left_x = (x + w - 1) % w, right_x = (x + 1) % w;
    if (map[up_y][x] == 0 || map[up_y][x] == 2) {
        int ori = map[up_y][x];
        map[up_y][x] = 1;
        score += judge(map, x, up_y, deep - 1) * (ori == 2 ? 10 : 1);
        map[up_y][x] = ori;
    }
    if (map[down_y][x] == 0 || map[down_y][x] == 2) {
        int ori = map[down_y][x];
        map[down_y][x] = 1;
        score += judge(map, x, down_y, deep - 1) * (ori == 2 ? 10 : 1);
        map[down_y][x] = ori;
    }
    if (map[y][left_x] == 0 || map[y][left_x] == 2) {
        int ori = map[y][left_x];
        map[y][left_x] = 1;
        score += judge(map, left_x, y, deep - 1) * (ori == 2 ? 10 : 1);
        map[y][left_x] = ori;
    }
    if (map[y][right_x] == 0 || map[y][right_x] == 2) {
        int ori = map[y][right_x];
        map[y][right_x] = 1;
        score += judge(map, right_x, y, deep - 1) * (ori == 2 ? 10 : 1);
        map[y][right_x] = ori;
    }
    return score;
}


std::string GlobalData::mapName;
int GlobalData::map_width, GlobalData::map_height;
int GlobalData::player_num, GlobalData::total_num;
std::vector<Snake> GlobalData::snakes;
std::vector<std::vector<int>> GlobalData::snakeMap;
int GlobalData::food_num;


int ColorGroup::t_compare (const void *a, const void *b) {
    int score_sub = -((ColorGroup *) a)->score + ((ColorGroup *) b)->score;
    int length_sub = -((ColorGroup *) a)->length + ((ColorGroup *) b)->length;
    int color_sub = ((ColorGroup *) a)->color - ((ColorGroup *) b)->color;
    if (score_sub != 0)
        return score_sub;
    else if (length_sub != 0)
        return length_sub;
    return color_sub;
}

int ColorGroup::l_compare (const void *a, const void *b) {
    return -((ColorGroup *) a)->length + ((ColorGroup *) b)->length;
}
