#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;
#define INF 10000
#define maxn 30
#define maxm 40
#define my_id 2023200716  // 这里对应你的学号，请修改

int map[maxn][maxm];
int rtime, object_num, player_num, op_num;

struct Head {
    int x;
    int y;
    int id;
} myhead;

struct Object {
    int x;
    int y;
    int value;
} *o;

struct Player {
    int id;
    int len;
    struct Head head;
    int score;
    int direct;
    int shield_cd;
    int shield_time;
} *p, me;

vector<pair<int, int>> wall_openings = {{9, 20}, {21, 20}, {15, 14}, {15, 26}};

void get_info() { // 获取输入信息
    int xx, yy;
    scanf("%d", &rtime); // 游戏剩余时间
    scanf("%d", &object_num); // 地图上物品的个数
    o = new struct Object[object_num];
    for (int i = 0; i < object_num; i++) {
        scanf("%d %d %d", &o[i].x, &o[i].y, &o[i].value);  // 地图上物品的位置和信息
        if(o[i].value==-1){
            o[i].value = 1;
        }
        map[o[i].x][o[i].y] = o[i].value;


    }

    scanf("%d", &player_num); // 存活的玩家数
    p = new struct Player[player_num];
    for (int i = 0; i < player_num; i++) {
        scanf("%d %d %d %d %d %d", &p[i].id, &p[i].len, &p[i].score, &p[i].direct, &p[i].shield_cd, &p[i].shield_time); // 存活的玩家信息
        if (p[i].id != my_id) { // 不是自己
            scanf("%d %d", &p[i].head.x, &p[i].head.y); //头的位置
            map[p[i].head.x][p[i].head.y] = -3;
            for (int j = 1; j < p[i].len; j++) {
                scanf("%d %d", &xx, &yy); //身子的位置
                map[xx][yy] = -3;
            }
        } else { // 是自己
            me = p[i];
            scanf("%d %d", &myhead.x, &myhead.y); //头的位置
            for (int j = 1; j < p[i].len; j++) {
                scanf("%d %d", &xx, &yy); //身子的位置
                map[xx][yy] = 0; // 标记自己身体部分为 0
            }
        }
    }
}

// 打印地图状态
void print_map() {
    printf("Map state:\n");
    for (int i = 0; i < maxn; ++i) {
        for (int j = 0; j < maxm; ++j) {
            printf("%3d ", map[i][j]);
        }
        printf("\n");
    }
}

// 判断是否在地图内
bool is_inside(int x, int y) {
    return x >= 0 && x < maxn && y >= 0 && y < maxm;
}

// 判断是否安全
bool is_safe(int x, int y) {
    if (me.shield_time > 0) {
        if (!is_inside(x, y)|| map[x][y] == -4 || map[x][y] == -2) {
            return false;
        }
    } else {
        if (!is_inside(x, y) || map[x][y] == -3 || map[x][y] == -4 || map[x][y] == -2) {
            return false;
        }
    }

    // 检查移动后是否会陷入死路
    int dx[4] = {0, -1, 0, 1};
    int dy[4] = {-1, 0, 1, 0};
    int block_count = 0;
    for (int i = 0; i < 4; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (map[nx][ny] == -3 || map[nx][ny] == -4 || map[nx][ny] == -2) {
            block_count++;
        }
    }
    if (block_count >= 3) {
        return false;
    }

    return true;
}

// 判断是否直接折返
bool is_opposite(int dir1, int dir2) {
    return (dir1 + 2) % 4 == dir2;
}

// 判断是否有其他蛇头可能相撞
bool will_collide_with_other_snake(int nx, int ny) {
    for (int i = 0; i < player_num; ++i) {
        if (p[i].id != my_id) {
            int opp_head_x = p[i].head.x;
            int opp_head_y = p[i].head.y;
            if (abs(opp_head_x - nx) + abs(opp_head_y - ny) == 1) {
                return true;
            }
        }
    }
    return false;
}

// 寻找所有安全的移动方式
vector<int> find_safe_moves() {
    vector<int> safe_moves;
    int dx[4] = {0, -1, 0, 1};
    int dy[4] = {-1, 0, 1, 0};
    for (int i = 0; i < 4; ++i) {
        int nx = myhead.x + dx[i];
        int ny = myhead.y + dy[i];
        if (is_safe(nx, ny) && !is_opposite(me.direct, i) && !will_collide_with_other_snake(nx, ny)) {
            safe_moves.push_back(i);
        }
    }
    return safe_moves;
}

// 贪心算法选择目标食物，保留前3个
vector<pair<int, int>> choose_best_foods() {
    vector<pair<int, int>> best_foods;
    vector<pair<double, pair<int, int>>> food_list;
    int dx[4] = {0, -1, 0, 1};
    int dy[4] = {-1, 0, 1, 0};

    for (int i = 0; i < object_num; ++i) {
        int x = o[i].x;
        int y = o[i].y;
        int value = o[i].value;
        if (value > 0) { // 普通豆子
            int distance = abs(myhead.x - x) + abs(myhead.y - y);
            double value_per_distance = double(value) / distance;
            food_list.push_back({value_per_distance, {x, y}});
        }
    }

    // 按照价值/距离排序
    sort(food_list.rbegin(), food_list.rend());

    // 保留前3个
    for (int i = 0; i < min(3, int(food_list.size())); ++i) {
        best_foods.push_back(food_list[i].second);
    }

    // 检查是否有可以直接吃到且是安全的食物
    for (int i = 0; i < 3; ++i) {
        int x = best_foods[i].first;
        int y = best_foods[i].second;
        for (int j = 0; j < 4; ++j) {
            int nx = myhead.x + dx[j];
            int ny = myhead.y + dy[j];
            if (nx == x && ny == y && is_safe(nx, ny)) {
                // 如果下一步可以吃到且是安全的，优先吃这个
                return {{x, y}};
            }
        }
    }

    return best_foods;
}

// 输出目标食物信息
void print_target_food(int x, int y) {
    cout << "Target food: (" << x << ", " << y << ")" << endl;
}

// 输出安全移动路径
void print_safe_moves(vector<int>& safe_moves) {
    cout << "Safe moves: ";
    for (int move : safe_moves) {
        cout << move << " ";
    }
    cout << endl;
}

// 寻找所有安全的撞陷阱方式
vector<int> find_safe_traps() {
    vector<int> safe_traps;
    int dx[4] = {0, -1, 0, 1};
    int dy[4] = {-1, 0, 1, 0};
    for (int i = 0; i < 4; ++i) {
        int nx = myhead.x + dx[i];
        int ny = myhead.y + dy[i];
        if (is_inside(nx, ny) && map[nx][ny] == -2) { // -2表示陷阱
            safe_traps.push_back(i);
        }
    }
    return safe_traps;
}

// 检查是否有其他蛇头更接近目标食物
bool is_other_snake_closer(int target_x, int target_y) {
    for (int i = 0; i < player_num; ++i) {
        if (p[i].id != my_id) {
            int opp_head_x = p[i].head.x;
            int opp_head_y = p[i].head.y;
            int opp_distance = abs(opp_head_x - target_x) + abs(opp_head_y - target_y);
            int my_distance = abs(myhead.x - target_x) + abs(myhead.y - target_y);
            if (opp_distance < my_distance) {
                return true;
            }
        }
    }
    return false;
}

// 判断路径上是否有墙
bool is_wall_in_path(int x1, int y1, int x2, int y2) {
    if (x1 == x2) {
        int min_y = min(y1, y2);
        int max_y = max(y1, y2);
        for (int y = min_y; y <= max_y; ++y) {
            if (map[x1][y] == -4) {
                return true;
            }
        }
    } else if (y1 == y2) {
        int min_x = min(x1, x2);
        int max_x = max(x1, x2);
        for (int x = min_x; x <= max_x; ++x) {
            if (map[x][y1] == -4) {
                return true;
            }
        }
    }
    return false;
}

// 找到离目标食物最近的开口
pair<int, int> find_closest_opening(int target_x, int target_y) {
    int min_distance = INF;
    pair<int, int> closest_opening = wall_openings[0];
    for (auto& opening : wall_openings) {
        int distance = abs(opening.first - myhead.x) + abs(opening.second - myhead.y)
                     + abs(opening.first - target_x) + abs(opening.second - target_y);
        if (distance < min_distance) {
            min_distance = distance;
            closest_opening = opening;
        }
    }
    return closest_opening;
}

// 进行下一步决策
// 0为左，1为上，2为右，3为下，4为开盾
int judge() {
    // 找到所有安全的移动方式
    vector<int> safe_moves = find_safe_moves();

    // 如果没有安全的移动方式，先尝试撞陷阱
    if (safe_moves.empty()) {
        for (int move : find_safe_traps()) {
            return move;
        }
    }

    if (safe_moves.empty()) {
            if (me.shield_cd == 0 && me.shield_time == 0 && me.score >= 20) {
                return 4; // 主动使用护盾
            } else {
                // 找到最近的蛇头
                int min_distance = INF;
                int target_x = -1, target_y = -1;
                for (int i = 0; i < player_num; ++i) {
                    if (p[i].id != my_id) {
                        int opp_head_x = p[i].head.x;
                        int opp_head_y = p[i].head.y;
                        int distance = abs(myhead.x - opp_head_x) + abs(myhead.y - opp_head_y);
                        if (distance < min_distance) {
                            min_distance = distance;
                            target_x = opp_head_x;
                            target_y = opp_head_y;
                        }
                    }
                }

                // 向最近的蛇头移动
                int dx[4] = {0, -1, 0, 1};
                int dy[4] = {-1, 0, 1, 0};
                int best_move = -1;
                min_distance = INF;
                for (int i = 0; i < 4; ++i) {
                    int nx = myhead.x + dx[i];
                    int ny = myhead.y + dy[i];
                    int distance = abs(nx - target_x) + abs(ny - target_y);
                    if (distance < min_distance) {
                        min_distance = distance;
                        best_move = i;
                    }
                }
                return best_move;
            }
        }

    // 选择前3个最佳食物
    vector<pair<int, int>> best_foods = choose_best_foods();

    // 在安全的移动方式中选择一种移动后与目标食物距离最小的，且没有其他蛇头更接近的
    int chosen_target = 0;
    for (int i = 0; i < min(2, int(best_foods.size())); ++i) {
        int target_x = best_foods[i].first;
        int target_y = best_foods[i].second;
        if (!is_other_snake_closer(target_x, target_y)) {
            chosen_target = i;
            break;
        }
    }

    int target_x = best_foods[chosen_target].first;
    int target_y = best_foods[chosen_target].second;
    int min_distance = INF;
    int best_move = safe_moves[0];
    int dx[4] = {0, -1, 0, 1};
    int dy[4] = {-1, 0, 1, 0};

    // 如果路径上有墙，则找到离目标食物最近的开口
    if (is_wall_in_path(myhead.x, myhead.y, target_x, target_y)) {
        pair<int, int> closest_opening = find_closest_opening(target_x, target_y);
        int opening_x = closest_opening.first;
        int opening_y = closest_opening.second;
        min_distance = INF;
        for (int move : safe_moves) {
            int nx = myhead.x + dx[move];
            int ny = myhead.y + dy[move];
            int distance = abs(nx - opening_x) + abs(ny - opening_y);
            if (distance < min_distance) {
                min_distance = distance;
                best_move = move;
            }
        }
    } else {
        // 否则，选择距离目标食物最近的安全移动方式
        for (int move : safe_moves) {
            int nx = myhead.x + dx[move];
            int ny = myhead.y + dy[move];
            int distance = abs(nx - target_x) + abs(ny - target_y);
            if (distance < min_distance) {
                min_distance = distance;
                best_move = move;
            }
        }
    }

    return best_move;
}


int main() {
    memset(map, 0, sizeof(map)); // 初始化地图
    get_info();
    //print_map();
    int res = judge();
    printf("%d", res);
    return 0;
}
