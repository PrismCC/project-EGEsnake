#include "paint.h"
#include "others.h"
#include <fstream>
#include "structs.h"
#include "play.h"


ColorGroup score[16];

const color_t colorBin[16] = {
        0xfff52b01, 0xffffa442, 0xffffc09e, 0xfff5e348,
        0xff2ed97f, 0xff4fff3d, 0xff95deab, 0xffb4dec5,
        0xff2542fc, 0xff2e8ad9, 0xff3deffb, 0xffcfd5ff,
        0xffb8a3ff, 0xffc68ade, 0xfff5a5d6, 0xffe6c6a5
};

const int GREY = EGERGB(127, 127, 127);

const int box_size = 20, edge_size = 10, text_width = 200;

static int width_box;
static int height_box;
static int play_x, play_y, play_width, play_height;
static int text_x, text_y;
static int max_length, max_length_color;
static ColorGroup t_score[16];
static int history_max_length, history_max_length_color;
static int history_max_score, history_max_score_color;

enum class PrepareState {
    map, player, num, color, finish
};

bool begin () {

    const int buff_size = 30;
    char str_buffer[2 * buff_size] = "";
    size_t buff_len;

    sys_edit editBox;
    box_init(editBox);

    PIMAGE decorate_image = newimage();
    get_zoom_image(decorate_image, "resource/koishi.png", 210, 297);

    data_init();
    std::string mapName, mapPath;
    std::ifstream mapIn;
    std::string line, intro, warning;
    bool used_color[16] = {false};
    int colored_num = 0;

    std::vector<std::string> fileList;
    load_file_list(std::string("map"), fileList);

    PrepareState state = PrepareState::map;
    bool ai_colored = false;

    for (; is_run(); delay_fps(60)) {
        bool input = false;
        while (kbmsg()) {
            key_msg msg = getkey();
            if ((msg.key == key_enter) && (msg.msg) == key_msg_up)
                input = true;
        }

        if (input) {
            editBox.gettext(buff_size, str_buffer);
            editBox.settext("");
            buff_len = strlen(str_buffer);
            int option;

            switch (state) {
                case PrepareState::map:
                    mapName = std::string(str_buffer);
                    if (mapName == "quit") {
                        close_box(decorate_image);
                        exit(0);
                    }
                    mapPath = std::string("map/") + mapName + ".snakemap";
                    mapIn.open(mapPath);
                    if (mapIn.fail()) {
                        warning = "无法打开该文件, 请检查是否输入正确及文件是否存在";
                        mapIn.clear();
                        break;
                    }
                    getline(mapIn, line);
                    if (line != "MAP_FOR_SNAKE")
                        warning = "该文件格式错误, 请确认是否是地图文件";
                    else {
                        state = PrepareState::player;
                        editBox.settext("");
                        mapIn >> GlobalData::mapName >> GlobalData::map_height >> GlobalData::map_width;
                        getline(mapIn, intro);
                        getline(mapIn, intro);
                        warning = "";
                        GlobalData::snakeMap.clear();
                        int food_num = 0;
                        for (int i = 0; i < GlobalData::map_height; ++i) {
                            std::vector<int> mapLine;
                            for (int j = 0; j < GlobalData::map_width; ++j) {
                                char c;
                                do
                                    mapIn >> c;
                                while (!std::isdigit(c));
                                if (c == '2')
                                    ++food_num;
                                mapLine.push_back(c - '0');
                            }
                            GlobalData::snakeMap.push_back(mapLine);
                        }
                        GlobalData::food_num = food_num;
                    }
                    break;
                case PrepareState::player:
                    if (buff_len != 1) {
                        warning = "输入格式错误";
                        break;
                    }
                    option = str_buffer[0] - '0';
                    if (option == 0) {
                        mapIn.close();
                        state = PrepareState::map;
                        warning = "";
                    }
                    else if (option >= 1 && option <= 4) {
                        GlobalData::player_num = (option == 4 ? 0 : option);
                        state = PrepareState::num;
                        warning = "";
                    }
                    else
                        warning = "输入格式错误";
                    break;
                case PrepareState::num:
                    if (buff_len != 1) {
                        warning = "输入格式错误";
                        break;
                    }
                    option = str_buffer[0] - '0';
                    if (option == 0) {
                        state = PrepareState::player;
                        warning = "";
                    }
                    else if (option >= 1 && option <= 4) {
                        GlobalData::total_num = option * 4;
                        state = PrepareState::color;
                        warning = "";
                    }
                    else
                        warning = "输入格式错误";
                    break;
                case PrepareState::color:
                    if (buff_len == 1)
                        option = str_buffer[0] - '0';
                    else if (buff_len == 2)
                        option = (str_buffer[0] - '0') * 10 + str_buffer[1] - '0';
                    else {
                        warning = "输入格式错误";
                        break;
                    }
                    if (option >= 0 && option <= 15) {
                        if (used_color[option])
                            warning = "该颜色已被使用";
                        else {
                            used_color[option] = true;
                            warning = "";
                            GlobalData::snakes.emplace_back(option, false);
                            ++colored_num;
                            if (colored_num >= GlobalData::player_num)
                                state = PrepareState::finish;
                        }
                    }
                    else
                        warning = "输入格式错误";
                    break;
                default:
                    goto START;
            }
        }

        logo_warn(decorate_image, warning);

        int output_x, output_y;
        std::vector<int> ai_colors;
        setfont(15, 0, "楷体");
        setcolor(BLACK);

        switch (state) {
            case PrepareState::map:
                outtextxy(140, 220, "请输入地图名, 输入quit退出");
                output_y = 30;
                output_x = 240;
                outtextxy(output_x, output_y, "地图文件列表:");
                for (auto const &x: fileList) {
                    output_y += 15;
                    outtextxy(output_x, output_y, &x[0]);
                }
                break;
            case PrepareState::player:
                outtextxy(140, 220, "请输入游戏模式");
                xyprintf(240, 30, "地图名:%s  大小: %d*%d\n", &GlobalData::mapName[0], GlobalData::map_height,
                         GlobalData::map_width);
                output_x = 240;
                output_y = 80;
                outtextrect(output_x, output_y, 200, 60, &intro[0]);
                output_x += 30;
                output_y += 80;
                outtextrect(output_x, output_y, 200, 120, "[1] 单人  \n[2] 双人  \n[3] 三人  \n[4] 旁观  \n[0] 返回\n");
                break;
            case PrepareState::num:
                outtextxy(140, 220, "请输入总人数");
                xyprintf(240, 30, "地图名:%s  大小: %d*%d\n", &GlobalData::mapName[0], GlobalData::map_height,
                         GlobalData::map_width);
                output_x = 240;
                output_y = 80;
                outtextrect(output_x, output_y, 200, 60, &intro[0]);
                output_x += 30;
                output_y += 80;
                outtextrect(output_x, output_y, 200, 120, "[1] 4  \n[2] 8  \n[3] 12  \n[4] 16  \n[0] 返回\n");
                break;
            case PrepareState::color:
                if (GlobalData::player_num == 0)
                    state = PrepareState::finish;
                xyprintf(140, 220, "请输入玩家%d的颜色编号", colored_num + 1);
                xyprintf(240, 30, "地图名:%s  大小: %d*%d\n", &GlobalData::mapName[0], GlobalData::map_height,
                         GlobalData::map_width);
                output_x = 240;
                output_y = 80;
                xyprintf(output_x, output_y, "玩家人数: %d  总人数: %d", GlobalData::player_num, GlobalData::total_num);
                output_x += 30;
                output_y += 30;
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        if (used_color[4 * i + j])
                            setcolor(GREY);
                        else
                            setcolor(colorBin[4 * i + j]);
                        xyprintf(output_x, output_y, "%2d:■", 4 * i + j);
                        output_x += 50;
                    }
                    output_x -= 200;
                    output_y += 30;
                }
                setcolor(BLACK);
                break;
            default:
                output_x = 240;
                output_y = 60;
                if (!ai_colored) {
                    ai_colors = init_ai_color(used_color, GlobalData::total_num - GlobalData::player_num);
                    for (auto x: ai_colors)
                        GlobalData::snakes.emplace_back(x, true);
                    ai_colored = true;
                }
                outtextxy(240, 30, "玩家概览:");
                for (int count = 0; auto const &x: GlobalData::snakes) {
                    ++count;
                    setcolor(colorBin[x.color]);
                    xyprintf(output_x, output_y, "%2d:■", count);
                    output_x += 50;
                    if (count % 4 == 0) {
                        output_x -= 200;
                        output_y += 30;
                    }
                }
                setcolor(BLACK);
                outtextxy(140, 220, "游戏即将开始, 输入回车以继续");
                break;
        }
    }

    close_box(decorate_image);
    exit(0);

    START:
    delimage(decorate_image);
    editBox.destroy();
    board_init();
    bool ret = play();
    closegraph();
    return ret;
}

void get_zoom_image (PIMAGE img_ptr, const char *file_name, int width, int height) {
    if (img_ptr == nullptr)
        return;

    PIMAGE temp = newimage();
    getimage(temp, file_name);

    if ((getwidth(img_ptr) != width) || (getheight(img_ptr) != height))
        resize(img_ptr, width, height);

    putimage(img_ptr, 0, 0, width, height, temp, 0, 0, getwidth(temp), getheight(temp));

    delimage(temp);
}

void box_init (sys_edit &editBox) {
    initgraph(480, 320, INIT_RENDERMANUAL);
    setcaption("EGEsnake");
    setbkcolor(WHITE);
    setcolor(BLACK);
    setbkmode(TRANSPARENT);
    setfont(20, 0, "楷体");
    ege_enable_aa(true);

    editBox.create(false);
    editBox.move(120, 240);
    editBox.size(340, 40);
    editBox.setbgcolor(WHITE);
    editBox.setcolor(BLACK);
    editBox.setfont(20, 0, "仿宋");
    editBox.setmaxlen(20);
    editBox.visible(true);
    editBox.setfocus();
}

void data_init () {
    GlobalData::snakes.clear();
    GlobalData::snakeMap.clear();
}

void print_data_init () {
    width_box = GlobalData::map_width;
    height_box = GlobalData::map_height;
    play_x = edge_size, play_y = edge_size, play_width = width_box * box_size, play_height =
            height_box * box_size;
    text_x = edge_size * 2 + play_width, text_y = edge_size;
    max_length = 0, max_length_color = 0;
    for (auto &i: t_score) {
        i.length = i.living_time = i.score = 0;
        i.color = 0;
    }
    history_max_length = history_max_length_color = 0;
    history_max_score = history_max_score_color = 0;
}

void logo_warn (PIMAGE &decorate_image, std::string &warning) {
    cleardevice();
    setfont(40, 0, "黑体");
    outtextxy(20, 20, "EGE贪吃蛇");
    if (decorate_image != nullptr)
        putimage_withalpha(nullptr, decorate_image, -40, 70);

    setfont(13, 0, "楷体");
    setcolor(RED);
    outtextxy(120, 280, &warning[0]);
}

void close_box (PIMAGE &decorate_image) {
    delimage(decorate_image);
    flushkey();
    closegraph();
}

void board_init () {
    int box_width_count = GlobalData::map_width;
    int box_height_count = GlobalData::map_height;
    int board_width = box_width_count * box_size + edge_size * 3 + text_width;
    int board_height = box_height_count * box_size + edge_size * 2;
    resizewindow(board_width, board_height);
    setbkcolor(WHITE);
    setcolor(BLACK);
    cleardevice();
    flushkey();
}

void board_print (bool print_score, bool add) {


    play_print();

    if (print_score) {
        score_print(add);
    }
    else {
        setcolor(BLACK);
        rectprintf(text_x + 10, text_y + 10, text_width - 20, play_height - 20,
                   "所有玩家按下任意方向键后开始游戏\n操作方法:\n玩家一:w s a d\n玩家二:i k j l\n玩家三:↑↓←→\n按Esc退出游戏\n");
    }
}

void play_print () {

    setlinewidth(2.0f);
    setcolor(GREY);
    ege_rectangle((float) play_x, (float) play_y, (float) play_width, (float) play_height);
    ege_rectangle((float) text_x, (float) text_y, (float) text_width, (float) play_height);
    for (int y = play_y + box_size; y < play_y + play_height; y += box_size)
        ege_line((float) play_x, (float) y, (float) play_x + (float) play_width, (float) y);
    for (int x = play_x + box_size; x < play_x + play_width; x += box_size)
        ege_line((float) x, (float) play_y, (float) x, (float) play_y + (float) play_height);

    for (int i = 0; i < height_box; ++i) {
        for (int j = 0; j < width_box; ++j) {
            int x = edge_size + j * box_size + box_size / 2;
            int y = edge_size + i * box_size + box_size / 2;
            int value = GlobalData::snakeMap[i][j];
            switch (value) {
                case 0:
                    setfillcolor(WHITE);
                    floodfill(x, y, GREY);
                    break;
                case 1:
                    setfillcolor(BLACK);
                    floodfill(x, y, GREY);
                    break;
                case 2:
                    setfillcolor(WHITE);
                    floodfill(x, y, GREY);
                    setfillcolor(0xfffaaaf9);
                    ege_fillellipse((float) x - (float) box_size / 4, (float) y - (float) box_size / 4,
                                    (float) box_size / 2, (float) box_size / 2);
                    break;
                default:
                    if (value >= 100 && value < 200) {
                        setfillcolor(colorBin[value - 100]);
                        floodfill(x, y, GREY);
                    }
                    else {
                        int color = (value - 200) / 4, di = (value - 200) % 4;
                        setfillcolor(colorBin[color]);
                        floodfill(x, y, GREY);
                        bool isAI = true;
                        for (auto const &s: GlobalData::snakes) {
                            if (s.isAlive && s.head->x == j && s.head->y == i && !s.isAI) {
                                isAI = false;
                                break;
                            }
                        }
                        if (isAI)
                            setfillcolor(WHITE);
                        else
                            setfillcolor(BLACK);
                        auto ell_x = (float) x, ell_y = (float) y;
                        switch (di) {
                            case 0:
                                ell_y -= box_size / 4.0f;
                                break;
                            case 1:
                                ell_x += box_size / 4.0f;
                                break;
                            case 2:
                                ell_y += box_size / 4.0f;
                                break;
                            case 3:
                                ell_x -= box_size / 4.0f;
                                break;
                            default:
                                exit(1);
                        }
                        ege_fillellipse(ell_x - box_size / 10.0f, ell_y - box_size / 10.0f, box_size / 5.0f,
                                        box_size / 5.0f);
                    }
            }
        }
    }
}

void group_init () {
    int num = 0;
    for (const auto &i: GlobalData::snakes) {
        score[num].score = score[num].length = 0;
        score[num++].color = i.color;
    }
}

void score_print (bool add) {
    int x = text_x;
    int y = text_y;

    if (add) {
        for (int i = 0; i < GlobalData::total_num; ++i) {
            score[i].length = 0;
            for (const auto &j: GlobalData::snakes) {
                if (score[i].color == j.color) {
                    score[i].length += j.length + j.store;
                }
            }
        }
        int alive_num = 0;
        for (int i = 0; i < GlobalData::total_num; ++i)
            if (score[i].length > 0)
                ++alive_num;
        qsort(score, GlobalData::total_num, sizeof(ColorGroup), ColorGroup::l_compare);
        max_length = score[0].length;
        max_length_color = score[0].color;
        if (max_length > history_max_length) {
            history_max_length = max_length;
            history_max_length_color = max_length_color;
        }

        int score_to_add[16] = {0};
        for (int i = 0; i < GlobalData::total_num; ++i) {
            if (score[i].length > 0) {
                ++score_to_add[i];
                ++score[i].living_time;
                if (i == 0)
                    ++score_to_add[i];
                if (i < alive_num / 2)
                    ++score_to_add[i];
                int prev = i - 1;
                if (prev >= 0 && score[i].length == score[prev].length) {
                    score_to_add[i] = score_to_add[prev];
                }
            }
            score[i].score += score_to_add[i];
        }
        qsort(score, GlobalData::total_num, sizeof(ColorGroup), ColorGroup::t_compare);
        for (int i = 0; i < GlobalData::total_num; ++i)
            t_score[i] = score[i];
        if (score[0].score > history_max_score) {
            history_max_score = score[0].score;
            history_max_score_color = score[0].color;
        }
    }

    if (max_length != 0) {
        setcolor(colorBin[max_length_color]);
        xyprintf(x + 10, y + 10, "当前最长: %d ■", max_length);
        setcolor(BLACK);
        xyprintf(x + 10, y + 30, "剩余玩家: %d", GlobalData::total_num);
        outtextxy(x + 10, y + 50, "排名 玩家 分数 长度");

        for (int i = 0; i < GlobalData::total_num; ++i) {
            setcolor(colorBin[t_score[i].color]);
            xyprintf(x + 10, y + 50 + 15 * (i + 1), " %2d   ■  %4d  %3d", i + 1, t_score[i].score, t_score[i].length);
        }
    }
}

void end_print () {
    cleardevice();
    play_print();
    setfont(20, 0, "楷体");
    outtextxy(text_x + 10, text_y + 10, "游戏结束");
    int history_max_living = 0, history_max_living_color = 0;
    for (const auto &i: score) {
        if (i.living_time > history_max_living) {
            history_max_living = i.living_time;
            history_max_living_color = i.color;
        }
    }


    setfont(18, 0, "楷体");
    setcolor(colorBin[history_max_score_color]);
    xyprintf(text_x + 10, text_y + 50, "最高分数:  %4d  ■", history_max_score);
    setcolor(colorBin[history_max_length_color]);
    xyprintf(text_x + 10, text_y + 90, "最大长度:  %4d  ■", history_max_length);
    setcolor(colorBin[history_max_living_color]);
    xyprintf(text_x + 10, text_y + 130, "最久存活:  %4d  ■", history_max_living);

    qsort(score, GlobalData::total_num, sizeof(ColorGroup), ColorGroup::t_compare);
    setcolor(BLACK);
    setfont(12, 0, "楷体");
    outtextxy(text_x + 10, text_y + 180, "排名 玩家 分数");
    int y = 192 + text_y;
    for (int i = 0; i < sizeof(ColorGroup); ++i) {
        setcolor(colorBin[score[i].color]);
        xyprintf(text_x + 10, y, " %2d   ■  %4d ", i + 1, score[i].score);
        y += 12;
    }

    setcolor(BLACK);
    setfont(15, 0, "楷体");
    outtextrect(text_x + 10, y, text_width - 20, play_height - 190, "按Esc退出游戏\n或按任意其他键回到主页面\n");
    delay_ms(1000);
    flushkey();
}
