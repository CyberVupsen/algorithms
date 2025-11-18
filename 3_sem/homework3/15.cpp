#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include <string>
#include <cmath>
#include <algorithm>
#include "graph.h"

using namespace std;

typedef long long StNum;

StNum state_to_number(const vector<int>& state) {
    StNum num = 0; // num - это десятичное число, которое кодирует 16-тиричное состояние
    for (int tile : state) {
        num = num * 16 + tile;
    }
    return num;
}

vector<int> number_to_state(StNum num) {
    vector<int> state;
    for (int i = 0; i < 16; ++i) {
        state.push_back(num % 16);
        num /= 16;
    }
    reverse(state.begin(), state.end());
    return state;
}

int manhattan_dist(const vector<int>& state) {
    int distance = 0;
    for (int i = 0; i < 16; ++i) {
        int tile = state[i];
        if (tile != 0) {
            int target_x = (tile - 1) % 4;
            int target_y = (tile - 1) / 4;
            int current_x = i % 4;
            int current_y = i / 4;
            distance += abs(current_x - target_x) + abs(current_y - target_y);
        }
    }
    return distance;
}

vector<vector<int>> generate_neighbors(const vector<int>& state) {
    int empty_index = find(state.begin(), state.end(), 0) - state.begin();
    int empty_x = empty_index % 4;
    int empty_y = empty_index / 4;
    vector<vector<int>> neighbors;
    int dx[] = {0, 0, -1, 1};
    int dy[] = {1, -1, 0, 0};
    for (int dir = 0; dir < 4; ++dir) {
        int new_x = empty_x + dx[dir];
        int new_y = empty_y + dy[dir];
        if (0 <= new_x && new_x < 4 && 0 <= new_y && new_y < 4) {
            int swap_index = new_y * 4 + new_x;
            vector<int> new_state = state;
            swap(new_state[empty_index], new_state[swap_index]);
            neighbors.push_back(new_state);
        }
    }
    return neighbors;
}

bool is_solvable(const vector<int>& state) {
    int inv_count = 0;
    for (int i = 0; i < 16; ++i) {
        for (int j = i + 1; j < 16; ++j) {
            if (state[i] && state[j] && state[i] > state[j]) {
                inv_count++;
            }
        }
    }
    int empty_row = find(state.begin(), state.end(), 0) - state.begin();
    empty_row /= 4;
    return (inv_count + (3 - empty_row)) % 2 == 0;
}

vector<StNum> a_star(StNum initial_num) {
    StNum goal_num = state_to_number({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0});
    
    priority_queue<pair<int, StNum>, vector<pair<int, StNum>>, greater<pair<int, StNum>>> open_set;
    unordered_map<StNum, int> g_score;
    unordered_map<StNum, StNum> came_from;
    
    open_set.push({0, initial_num});
    g_score[initial_num] = 0;

    while (!open_set.empty()) {
        StNum current_num = open_set.top().second;
        open_set.pop();
        
        vector<int> current_state = number_to_state(current_num);

        if (current_num == goal_num) {
            vector<StNum> path;
            path.push_back(current_num);
            while (came_from.count(current_num)) {
                current_num = came_from[current_num];
                path.push_back(current_num);
            }
            reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& next_state : generate_neighbors(current_state)) {
            StNum next_num = state_to_number(next_state);
            int tentative_g = g_score[current_num] + 1;

            if (!g_score.count(next_num) || tentative_g < g_score[next_num]) {
                came_from[next_num] = current_num;
                g_score[next_num] = tentative_g;
                int f = tentative_g + manhattan_dist(next_state);
                open_set.push({f, next_num});
            }
        }
    }
    return {};
}

string get_move(StNum prev_num, StNum next_num) {
    vector<int> prev_state = number_to_state(prev_num);
    vector<int> next_state = number_to_state(next_num);
    int prev_empty = find(prev_state.begin(), prev_state.end(), 0) - prev_state.begin();
    int next_empty = find(next_state.begin(), next_state.end(), 0) - next_state.begin();
    int dx = (next_empty % 4) - (prev_empty % 4);
    int dy = (next_empty / 4) - (prev_empty / 4);
    
    if (dx == 1) return "R";
    if (dx == -1) return "L";
    if (dy == 1) return "D";
    if (dy == -1) return "U";
    return "?";
}

vector<int> parse_input() {
    cout << "Enter the initial state (4 lines of 4 numbers):" << endl;
    vector<int> state;
    for (int i = 0; i < 4; ++i) {
        while (true) {
            string line;
            getline(cin, line);
            int a, b, c, d;
            if (sscanf(line.c_str(), "%d %d %d %d", &a, &b, &c, &d) == 4) {
                state.push_back(a);
                state.push_back(b);
                state.push_back(c);
                state.push_back(d);
                break;
            } else {
                cout << "Error: please enter exactly 4 numbers in the line." << endl;
            }
        }
    }
    return state;
}

int main() {
    vector<int> initial_state = parse_input();
    cout << "Success parse_input" << endl;
    
    if (!is_solvable(initial_state)) {
        cout << "This position is unsolvable." << endl;
        return 0;
    }
    
    StNum initial_num = state_to_number(initial_state);
    vector<StNum> path_nums = a_star(initial_num);
    
    if (path_nums.empty()) {
        cout << "No solution found." << endl;
        return 0;
    }
    
    vector<string> moves;
    for (size_t i = 1; i < path_nums.size(); ++i) {
        moves.push_back(get_move(path_nums[i-1], path_nums[i]));
    }
    
    cout << "Sequence of tile [00] moves:" << endl;
    for (const auto& move : moves) {
        cout << move << " ";
    }
    cout << endl;
    
    return 0;
}
