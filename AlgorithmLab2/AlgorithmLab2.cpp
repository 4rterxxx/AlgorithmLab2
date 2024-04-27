#include <iostream>
#include <chrono>
#include<vector>
#include<algorithm>
#include<unordered_set>
#include<unordered_map>

using namespace std;

int N = 10; // Колличество прямоугольников
int M = 1000000; // Колличество точек

int Num_Of_Tests = 1; // Колличество тестов

// Вспомогательные структуры

class Rectangle {
public:
    int x1, y1, x2, y2;
    Rectangle() : x1{ 0 }, y1{ 0 }, x2{ 0 }, y2{ 0 } {}
    Rectangle(int x1, int y1, int x2, int y2) : x1{ x1 }, y1{ y1 }, x2{ x2 }, y2{ y2 } {}
};

class Point {
public:
    int x, y;
    Point() : x{ 0 }, y{ 0 } {}
    Point(int x, int y) : x{ x }, y{ y } {}
    bool operator < (const Point& str) const
    {
        return (x < str.x);
    }
};

class Line {
public:
    int x, y1, y2;
    bool isStart;
    Line() : x{ 0 }, y1{ 0 }, y2{ 0 }, isStart{ true } {}
    Line(int x, int y1, int y2, bool isStart) : x{ x }, y1{ y1 }, y2{ y2 }, isStart{ isStart } {}
    bool operator < (const Line& str) const
    {
        return (x < str.x);
    }
};

struct Node {
    int val;
    int marker;
    Node* left;
    Node* right;
    pair<int, int> border;
    Node() : val{ 0 }, left{ nullptr }, right{ nullptr }, border{ 0,0 }, marker{ 0 } {}
    Node(vector<int>& y, pair<int, int> border) : val{ 0 }, left{ nullptr }, right{ nullptr }, marker{ 0 } {
        this->border.first = y[border.first];
        this->border.second = y[border.second];
        if (border.second - border.first > 1) {
            this->left = new Node(y, { border.first, border.first + (border.second - border.first) / 2 });
            this->right = new Node(y, { border.first + (border.second - border.first) / 2, border.second });
        }
    }
    Node(const Node* other) {
        this->val = other->val;
        this->border = other->border;
        this->marker = other->marker;
        this->left = other->left;
        this->right = other->right;
        
    }
};

// Функции генерации прямоугольников и точек

void generate_rectangles(vector<Rectangle>& vec) {
    for (int i = 0; i < vec.size(); i++) {
        vec[i] = Rectangle{ 10 * i, 10 * i, 10 * (2 * N - i),10 * (2 * N - i) };
    }
}
void generate_requests(vector<Point>& vec) {
    for (int i = 0; i < vec.size(); i++) {
        vec[i] = Point{ i * 20 + 1, i * 20 + 1};
    }
}

// Бинарный поиск

int binSearch(vector<int> arr, int target) {
    int left = 0, right = arr.size() - 1;
    while (left <= right) {
        int mid = (right - left) / 2 + left;
        if (arr[mid] == target) {
            return mid;
        }
        else if (arr[mid] < target) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }
    return right;
}

// Вспомогательные функции 

void create_ordered_array_x_y(vector<int>& x, vector<int>& y, vector<Rectangle>& arr) {
    unordered_set<int> set_x;
    unordered_set<int> set_y;
    for (auto& rec : arr) {
        if (set_x.find(rec.x1) == set_x.end()) {
            x.push_back(rec.x1);
            set_x.emplace(rec.x1);
        }
        if (set_x.find(rec.x2) == set_x.end()) {
            x.push_back(rec.x2);
            set_x.emplace(rec.x2);
        }
        if (set_y.find(rec.y1) == set_y.end()) {
            y.push_back(rec.y1);
            set_y.emplace(rec.y1);
        }
        if (set_y.find(rec.y2) == set_y.end()) {
            y.push_back(rec.y2);
            set_y.emplace(rec.y2);
        }
    }
    sort(x.begin(), x.end());
    sort(y.begin(), y.end());
}

// Вспомогательные функции для работы с деревом отрезков

void create_hash_of_kids(vector<int>& y, unordered_map<int, int>& kids) {
    for (int i = 0; i < y.size(); ++i) {
        kids[y[i]] = i;
    }
}
Node* insert(Node* root, int left, int right, int op, unordered_map<int, int>& kids) {
    if (left >= right) return root;
    if (root->marker != 0) {
        root->val += root->marker * (kids[root->border.second] - kids[root->border.first]);
        if (root->left != nullptr) {
            root->left = new Node{ root->left };
            root->left->marker += root->marker;
        }
        if (root->right != nullptr) {
            root->right = new Node{ root->right };
            root->right->marker += root->marker;
        }
        root->marker = 0;
    }
    if (root->border.first == left && root->border.second == right) {
        root = new Node{ root };
        root->val += op * (kids[right] - kids[left]);
        if (root->left != nullptr) {
            root->left = new Node{ root->left };
            root->left->marker += op;
        }
        if (root->right != nullptr) {
            root->right = new Node{ root->right };
            root->right->marker += op;
        }
        return root;
    }
    else if (root->border.first <= left && root->border.second >= right) {
        root = new Node{ root };
        if (root->left == nullptr || root->right == nullptr) {
            root->val += op * (kids[right] - kids[left]);
            return root;
        }
        else {
            root->val += op * (kids[right] - kids[left]);
            root->left = insert(root->left, left, min(right, root->left->border.second), op, kids);
            root->right = insert(root->right, max(left, root->right->border.first), right, op, kids);
        }
        return root;
    }

}
int find_in_tree(Node* root, int point, int modifier, unordered_map<int, int>& kids) {
    if (root->border.first <= point && root->border.second > point) {
        if (root->left == nullptr || root->right == nullptr) {
            return root->val + modifier + root->marker;
        }
        else {
            return max(find_in_tree(root->left, point, modifier + root->marker, kids), find_in_tree(root->right, point, modifier + root->marker, kids));
        }
    }
    else {
        return 0;
    }
}

// Алгоритм перебора

vector<int> search_1(vector<Rectangle>& arr, vector<Point>& req) {
    int counter;
    vector<int> res;
    for (auto& point : req) {
        counter = 0;
        for (auto& rec : arr) {
            if (point.x >= rec.x1 && point.x < rec.x2 && point.y >= rec.y1 && point.y < rec.y2) counter++;
        }
        res.push_back(counter);
    }
    return res;
}

// Алгоритм на карте

vector<vector<int>> search_2_preporation(vector<int>& x, vector<int>& y, vector<Rectangle>& arr) {
    create_ordered_array_x_y(x, y, arr);
    vector<vector<int>> matrix(x.size() - 1, vector<int>(y.size() - 1, 0));
    for (auto& rec : arr) {
        int left_x = binSearch(x, rec.x1);
        int left_y = binSearch(y, rec.y1);
        for (int i = left_x; x[i] < rec.x2; i++) {
            for (int j = left_y; y[j] < rec.y2; j++) {
                matrix[i][j]++;
            }
        }
    }
    return matrix;
}

vector<int> search_2(vector<Rectangle>& arr, vector<Point>& req) {
    if (req.size() == 0) {
        return vector<int>(arr.size(), 0);
    }
    vector<int> x, y;
    auto matrix = search_2_preporation(x, y, arr);
    vector<int> res(req.size());
    int res_id = 0;
    for (auto& point : req) {
        auto res_x = binSearch(x, point.x);
        auto res_y = binSearch(y, point.y);
        if ((res_x < 0 || res_x >= x.size() - 1) || (res_y < 0 || res_y >= y.size() - 1)) {
            res[res_id++] = 0;
        }
        else {
            res[res_id++] = matrix[res_x][res_y];
        }
    }
    return res;
}

// Алгоритм на дереве

void search_3_preporation(vector<Rectangle>& arr, vector<Line>& lines, vector<int>& x, vector<int>& y, unordered_map<int, int>& kids) {
    create_ordered_array_x_y(x, y, arr);
    for (auto& rec : arr) {
        lines.push_back(Line{ rec.x1, rec.y1, rec.y2, true });
        lines.push_back(Line{ rec.x2, rec.y1, rec.y2, false });
    }
    sort(lines.begin(), lines.end());
    create_hash_of_kids(y, kids);
}

vector<int> search_3(vector<Rectangle>& arr, vector<Point>& req) {
    vector<int> result(req.size(), 0);
    if (arr.size() == 0) {
        return result;
    }
    vector<int> y;
    vector<int> x;
    vector<Line> lines;
    unordered_map<int, int> kids;
    search_3_preporation(arr, lines, x, y, kids);

    vector<Node*> persistent;
    Node* root = new Node{ y, {0, y.size() - 1} };
    int prev = x[0];
    for (auto& line : lines) {
        if (prev != line.x) {
            persistent.push_back(root);
            prev = line.x;
        }
        if (line.isStart) {
            root = insert(root, line.y1, line.y2, 1, kids);
        }
        else {
            root = insert(root, line.y1, line.y2, -1, kids);
        }
    }
    persistent.push_back(root);

    for (int i = 0; i < req.size(); i++) {
        auto ind = binSearch(x, req[i].x);
        if (ind >= 0 && ind < persistent.size()) {
            result[i] = find_in_tree(persistent[ind], req[i].y, 0, kids);
        }

    }
    return result;
}

// Функция замера времени

void check_time(vector<Rectangle>& arr, vector<Point>& req, vector<int>(*Search)(vector<Rectangle>& arr, vector<Point>& req)) {
    long double res = 0;
    for (int i = 0; i < Num_Of_Tests; i++) {
        auto start_time = chrono::steady_clock::now();
        Search(arr, req);
        auto end_time = chrono::steady_clock::now();
        auto result = end_time - start_time;
        res += chrono::duration<double, milli>(result).count();
    }

    res /= Num_Of_Tests;
    cout << res << " ";
}

int main()
{
    setlocale(LC_ALL, "ru");

    vector<Rectangle> arr(N);
    vector<Point> requests(M);


    generate_rectangles(arr);
    generate_requests(requests);
    check_time(arr, requests, search_1);
    //check_time(arr, requests, search_2);
    check_time(arr, requests, search_3);
    auto res_1 = search_1(arr, requests);
    auto res_3 = search_3(arr, requests);
    bool otvet = true;
    for (int i = 0; i < res_1.size(); i++) {
        if (res_1[i] != res_3[i]) {
            otvet = false;
        }
    }
    cout << "\n";
    cout << otvet;


}