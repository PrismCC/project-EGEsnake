#include "play.h"
#include "others.h"
#include "paint.h"

const int fps = 40;
const int frame_per_move = 10;

bool play () {

    set_seed();
    print_data_init();
    init_pos_di();
    group_init();

    int frame_count = 0;

    for (; is_run(); delay_fps(fps)) {
        bool add = frame_count >= fps * 3 && frame_count % frame_per_move == 0;

        cleardevice();
        board_print(true, add);

        int key = input_di();
        if (key == key_esc)
            return false;
        if (key == 'R')
            return true;
        if (frame_count % frame_per_move == 0) {
            for (auto &x: GlobalData::snakes) {
                if (x.is_alive())
                    x.move();
            }

            setFood();
        }
        ++frame_count;

        if (isEnd())
            break;
    }
    end_print();
    return getch() != key_esc;
}

void init_pos_di () {

    std::vector<int> coordinates;
    for (int i = 0; i < GlobalData::map_height; ++i) {
        for (int j = 0; j < GlobalData::map_width; ++j) {
            if (GlobalData::snakeMap[i][j] == 0)
                coordinates.push_back(i * GlobalData::map_width + j);
        }
    }
    std::vector<int> result = random_select(coordinates, GlobalData::total_num);

    for (int i = 0; i < GlobalData::total_num; ++i) {
        node *newNode = new node;
        newNode->y = result[i] / GlobalData::map_width;
        newNode->x = result[i] % GlobalData::map_width;
        newNode->next = nullptr;
        GlobalData::snakes[i].tail = GlobalData::snakes[i].head = newNode;
    }

    for (auto &x: GlobalData::snakes) {
        if (x.isAI) {
            x.change_direction(random_int(1, 4));
            x.ori_di = x.di;
        }
        else {
            x.ori_di = x.di = Direction::null;
            GlobalData::snakeMap[x.tail->y][x.tail->x] = 100 + x.color;
        }
    }

    int playerNum = GlobalData::player_num;
    bool player1Flag = false, player2Flag = false, player3Flag = false;
    if (playerNum < 1)
        player1Flag = true;
    if (playerNum < 2)
        player2Flag = true;
    if (playerNum < 3)
        player3Flag = true;

    while (!(player1Flag && player2Flag && player3Flag)) {
        cleardevice();
        board_print(false, false);
        while (kbmsg()) {
            key_msg msg = getkey();
            if ((msg.msg) == key_msg_down) {
                int key = msg.key;
                auto pair = trans_key(key);
                int player = pair.first;
                int di = pair.second;
                if (player == 1) {
                    GlobalData::snakes[0].change_direction(di);
                    player1Flag = true;
                }
                else if (player == 2 && GlobalData::player_num >= 2) {
                    GlobalData::snakes[1].change_direction(di);
                    player2Flag = true;
                }
                else if (player == 3 && GlobalData::player_num >= 3) {
                    GlobalData::snakes[2].change_direction(di);
                    player3Flag = true;
                }
            }
        }
        delay_fps(60);
    }
}

void setFood () {
    std::vector<int> coordinates;
    int height = GlobalData::map_height, width = GlobalData::map_width;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (GlobalData::snakeMap[i][j] == 0)
                coordinates.push_back(i * width + j);
        }
    }

    if (GlobalData::food_num < GlobalData::map_height * GlobalData::map_width / 3) {
        std::vector<int> result = random_select(coordinates, 1);
        for (auto x: result) {
            GlobalData::snakeMap[x / width][x % width] = 2;
            ++GlobalData::food_num;
        }
    }


}

std::pair<int, int> trans_key (int key) {
    int player;
    int di;
    switch (key) {
        case 'W':
            player = 1;
            di = 1;
            break;
        case 'D':
            player = 1;
            di = 2;
            break;
        case 'S':
            player = 1;
            di = 3;
            break;
        case 'A':
            player = 1;
            di = 4;
            break;
        case 'I':
            player = 2;
            di = 1;
            break;
        case 'L':
            player = 2;
            di = 2;
            break;
        case 'K':
            player = 2;
            di = 3;
            break;
        case 'J':
            player = 2;
            di = 4;
            break;
        case key_up:
            player = 3;
            di = 1;
            break;
        case key_right:
            player = 3;
            di = 2;
            break;
        case key_down:
            player = 3;
            di = 3;
            break;
        case key_left:
            player = 3;
            di = 4;
            break;
        default:
            break;
    }
    return {player, di};
}

int input_di () {
    while (kbmsg()) {
        key_msg msg = getkey();
        if ((msg.msg) == key_msg_down) {
            int key = msg.key;
            if (key == key_esc || key == 'R')
                return key;
            auto pair = trans_key(key);
            int player = pair.first;
            int di = pair.second;
            if (GlobalData::snakes[0].is_alive() && player == 1)
                GlobalData::snakes[0].change_direction(di);
            else if (GlobalData::snakes[1].is_alive() && player == 2 && GlobalData::player_num >= 2)
                GlobalData::snakes[1].change_direction(di);
            else if (GlobalData::snakes[2].is_alive() && player == 3 && GlobalData::player_num >= 3)
                GlobalData::snakes[2].change_direction(di);
        }
    }
    return 0;
}

bool isEnd () {
    bool result = true;
    for (auto const &x: GlobalData::snakes) {
        if (x.is_alive())
            result = false;
    }
    return result;
}