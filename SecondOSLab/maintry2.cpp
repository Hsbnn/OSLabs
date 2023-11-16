#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <fstream>
#include <chrono>

using namespace std;

struct Point {
    int x;
    int y;
    int z;
};

double triangle_area(Point a, Point b, Point c) {
    //return abs(0.5 * ((p1.x * (p2.y - p3.y)) + (p2.x * (p3.y - p1.y)) + (p3.x * (p1.y - p2.y))));
    int abx = b.x - a.x;
    int aby = b.y - a.y;
    int abz = b.z - a.z;

    int acx = c.x - a.x;
    int acy = c.y - a.y;
    int acz = c.z - a.z;

    int area = 0.5 * fabs(abx * acy - aby * acx + abx * acz - abz * acx + aby * acz - acy * abz);
    return area;
}

void find_largest_triangle(vector<Point>& points, size_t start, size_t end, double &largest_area, int a) {
    //cout << " find_largest_triangle " << a << endl;

    //this_thread::sleep_for(chrono::milliseconds(4000));

    int maxArea = 0;
    Point maxPoints[3];

    for (int i = start; i < end - 2; i++) {
        for (int j = i + 1; j < end - 1; j++) {
            for (int k = j + 1; k < end; k++) {
                int area = triangle_area(points[i], points[j], points[k]);

                if (area > maxArea) {
                    maxArea = area;
                    maxPoints[0] = points[i];
                    maxPoints[1] = points[j];
                    maxPoints[2] = points[k];
                }
            }
        }
    }

    //cout << " find_largest_triangle " << a << endl;
    largest_area = maxArea;

    //cout << " find_largest_triangle " << a << endl;

}

int main(int argc, char** argv) {
   auto startclock = chrono::high_resolution_clock::now();

    if (argc < 2) {
        cout << "Usage: ./main n" << endl;
        return 1;
    }

    int num_threads = atoi(argv[1]);
    //cout << "num_threads: " << num_threads << endl;

    //cout << "Введите координаты: " << endl;
    vector<Point> points;
    Point temp_point;
    //int x, y, z;
    size_t num_points = 0;

    std::ifstream data("input.txt");

    if (!data.is_open()) {
        throw std::runtime_error("File bad!");
    }

    Point p;
    int x, y, z;

    while (!data.eof()) {
        data >> x >> y >> z;
        p.x = x;
        p.y = y;
        p.z = z;
        points.push_back(p);
        num_points++;
    }

    //cout << "после ввода координат " << endl;

    vector<thread> threads(num_threads);
    vector<double> largest_areas(num_threads);
    //vector<Point> largest_points(num_threads);

    size_t block_size = num_points / num_threads;
    size_t remainder = num_points % num_threads;

    // Создание потоков для обработки блоков
    size_t start = 0;
    for (int i = 0; i < num_threads; i++) {
        size_t end = start + block_size;
        if (i < remainder) {
            end++;
        }
        threads[i] = thread(find_largest_triangle, ref(points), start, end,  ref(largest_areas[i]) , i); //ref - передача по ссылке
        //ref(points) - массив координат, Начало, Конец, первому потоку первый вектор из массива векторов с координатами largest_points.
        start = end;
    }

    //cout << "После создания потоков " << endl;

    //на выходе из функции потока получаем элемент массива largest_points - массив из 6 элементов

    // Ожидание завершения потоков
    for (int i = 0; i < num_threads; i++) {
        threads[i].join();
        //cout << "Поток " << i << endl;
    }

    //largest_points содержит num_threads массивов из 6 элементов


    double largest_Area = 0.0;


    for (int i = 0; i < num_threads; i++) {
        if (largest_areas[i] > largest_Area){
            largest_Area = largest_areas[i];
        }
    }

    cout << "Максимальная площадь: " << largest_Area << endl;

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - startclock;
    
    cout << "Время выполнения программы: " << elapsed.count() << " секунд" << endl;


    return 0;
}
