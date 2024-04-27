# **AlgorithmLab1** 
**Цель:** Сравнение Алгоритмов Перебора, Сжатой матрицы, Персистентного дерева отрезков.  
**Выполнил:** Трясков Алексей 22ПИ-2  
**Язык:** C++
___
# Реализация 3х алгоритмов
**Алгоритм перебора**  
Перебираем для каждой точки прямоугольник.

```C++
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
```

**Сложность подготовки:** O(1)  
**Сложность поиска:** O(N)  
___
**Алгоритм сжатой матрицы**  
Сначала строим сжатую матрицу.

```C++
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
```

Далее в ней ищем бинарным поиском наш промежуток и возвращаем результат из матрицы.

```C++
vector<int> search_2(vector<Rectangle>& arr, vector<Point>& req) {
    if (req.size() == 0) {
        return vector<int>(arr.size(), 0);
    }
    vector<int> x, y;
    auto matrix = search_2_preporation(x, y, arr);
    vector<int> res;
    for (auto& point : req) {
        auto res_x = binSearch(x, point.x);
        auto res_y = binSearch(y, point.y);
        if ((res_x < 0 || res_x >= x.size() - 1) || (res_y < 0 || res_y >= y.size() - 1)) {
            res.push_back(0);
        }
        else {
            res.push_back(matrix[res_x][res_y]);
        }
    }
    return res;
}
```

**Сложность подготовки:** O(N^3)  
**Сложность поиска:** O(log(N))  
___
**Алгоритм персистентного дерева отрезков**  
Для начала подготавливаем все линии по координате Х. Затем строим персистентное дерево и в нем ищем наши промежутки X и Y. 

```C++
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
            persistent.push_back(new Node{ root });
            prev = line.x;
        }
        if (line.isStart) {
            insert(root, line.y1, line.y2, 1, kids);
        }
        else {
            insert(root, line.y1, line.y2, -1, kids);
        }
    }
    persistent.push_back(new Node{ root });

    for (int i = 0; i < req.size(); i++) {
        auto ind = binSearch(x, req[i].x);

        if (ind >= 0 && ind < persistent.size()) {
            result[i] = find_in_tree(persistent[ind], req[i].y, kids);
        }

    }
    return result;
}
```

**Сложность подготовки:** O(N*log(N))  
**Сложность поиска:** O(log(N))    

___  


# Что нужно для запуска


Для запуска необходимо скачать файл с кодом и расширением **.cpp** и запустить его с помощью заранее установленного **C++** компилятора. 

Переходим в папку с файлом

```cmd
cd "путь к папке"
```

Создаём **.exe** файл

```cmd
g++ AlgorithmLab2.cpp
```

Запускаем созданный файл

```cmd
a.exe
```
Также возможно просто клонировать репозиторий и запустить через Visual Studio.
# Результаты запусков
Результат, который выдаст программа:

```
1st generation
M Ladder Binary Exponential
1 0.0428 0.0933 0.0428
2 6.6187 0.1418 0.1162
4 10.1926 0.2448 0.4267
8 11.7969 0.4652 0.5381
16 12.8675 0.9101 1.0333
32 13.5365 1.8045 1.9086
64 14.1405 5.391 3.4862
128 15.1912 11.8577 6.0936
256 14.2993 28.8846 10.7637
512 21.7151 63.0767 18.8999
1024 23.0342 129.026 34.9802
2048 38.698 296.073 82.079
4096 58.5792 695.741 146.681
8192 92.0101 1854.81 267.26
2nd generation
M Ladder Binary Exponential
1 0.042 0.0925 0.0422
2 13.2776 0.1405 0.1213
4 13.2545 0.2411 0.1756
8 13.3031 0.4509 0.2622
16 13.3561 0.8864 0.4225
32 13.4153 1.7363 0.6629
64 13.6863 3.786 1.0299
128 13.9736 8.7135 1.6877
256 14.8889 17.9281 2.7401
512 15.3397 43.0584 4.6194
1024 18.1005 87.5711 8.2467
2048 33.029 176.661 26.3559
4096 49.5447 502.355 46.8514
8192 85.6321 1504.96 85.7827
```


Здесь можно увидеть время работы алгоритмов при 2х генерациях и всех значениях **M**.

Результат представлен в микросекундах.

# Визуализация

**1ая генерация**

<img src="images/graph1.png">

Используя первый способ генерации, самым эффективным алгоритмом оказывается лесенка. При M <= 256 алгоритмы работают примерно за одинаковое время, однако на больших данных лучший - лесенка, худший - бинарный поиск. При M <= 1024 лесенка и экспонента работают примерно одинаково.

**2ая генерация**

<img src="images/graph2.png">

При второй генерации худшим также оказался бинарный поиск. Однако точно определить лучший нельзя. Лесенка и экспонента работают примерно одинаково при любом M.

# Выводы
Подводя итог, можно сказать, что худшим по времени оказался бинарный поиск. Это связано с тем что он всегда работает за **M*log(N)**, и из-за этого различия в генерациях никак на нём не отражаются. При M = 8192 он всегда работает приблизительно за 1500-1800 микросекунд.

Самым же стабильным и универсальным оказался алгоритм лесенки, ведь в отличии от бинарного, он в худшем случае пройдет всего лишь **M+N**. Из-за его простоты, разница генераций на него не сильно повлияла.

Говоря про экспоненциальный поиск, нужно отметить, что он сильно зависит от исходных данных. Он не такой стабильный как лесенка, но при некоторых генерациях, может наравне с ней конкурировать. Такой пример мы видим при 2ой генерации.

*Все графики можно найти в файле graph.xlsx*