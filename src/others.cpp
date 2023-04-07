#include <ctime>
#include "others.h"

void load_file_list (const std::string &path, std::vector<std::string> &list) {
    intptr_t hFile;
    struct _finddata_t fileinfo{};
    std::string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            if (!(fileinfo.attrib & _A_SUBDIR)) {
                std::string name{fileinfo.name};
                auto pos = name.rfind(".snakemap");
                if (pos != std::string::npos)
                    list.push_back(name.substr(0, pos));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }

}

std::vector<int> init_ai_color (const bool *used_color, int num) {
    std::vector<int> colors;
    for (int i = 0; i < 16; ++i) {
        if (!used_color[i])
            colors.push_back(i);
    }
    return random_select(colors, num);
}

std::vector<int> random_select (std::vector<int> &nums, int m) {
    std::vector<int> result; // 存储结果
    int n = (int) nums.size(); // 获取n的值
    if (m > n) return result; // 如果m大于n，返回空结果
    static std::default_random_engine engine(std::random_device{}()); // 创建默认的随机数引擎
    static std::uniform_int_distribution<int> dist(0, 1000000); // 创建均匀分布
    for (int i = 0; i < m; i++) {
        int index = dist(engine) % n; // 随机生成一个索引
        result.push_back(nums[index]); // 将对应的数放入结果中
        std::swap(nums[index], nums[n - 1]); // 将选中的数与最后一个数交换
        n--; // 将n减一，避免重复选择
    }
    return result;
}

void set_seed ()//设置随机数种子
{
    clockid_t a = 0;
    struct timespec p = {0, 0};
    clock_gettime(a, &p);
    srand((unsigned) p.tv_nsec);
}

int random_int (int from, int to) {
    return rand() % (to - from + 1) + from;
}

int random_direction (int way[3]) {
    int sum = 0;
    for (int i = 0; i < 3; ++i)
        sum += way[i];
    if (sum < 1)
        return random_int(0, 2);
    for (int i = 1; i < 3; ++i) {
        way[i] += way[i - 1];
    }
    int r = random_int(0, sum - 1);
    for (int i = 0; i < 3; ++i)
        if (r < way[i])
            return i;
    return 1;
}