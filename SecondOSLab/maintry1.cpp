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

void find_largest_triangle(vector<Point>& points, size_t start, size_t end, vector<Point>& largest_points, int a) {
    //cout << " find_largest_triangle " << a << endl;

    //this_thread::sleep_for(chrono::milliseconds(4000));

    Point min_x = points[start], max_x = points[start];
    Point min_y = points[start], max_y = points[start];
    Point min_z = points[start], max_z = points[start];

    Point mix,miy,miz;
    Point max,may,maz;

    for (int i = start; i < end; i++) {
        if (points[i].x <= min_x.x) {
            min_x.x = points[i].x;
            mix = points[i];
        }
        if (points[i].y <= min_y.y) {
            min_y.y = points[i].y;
            miy = points[i];
        }
        if (points[i].z <= min_z.z) {
            min_z.z = points[i].z;
            miz = points[i];
        }
        if (points[i].x >= max_x.x) {
            max_x.x= points[i].x;
            max = points[i];
        }
        if (points[i].y >= max_y.y) {
            max_y.y= points[i].y;
            may = points[i];
        }
        if (points[i].z >= max_z.z) {
            max_z.z= points[i].z;
            maz = points[i];
        }

    }

    //cout << " find_largest_triangle " << a << endl;

    largest_points[0] = mix;
    largest_points[1] = miy;
    largest_points[2] = miz;
    largest_points[3] = max;
    largest_points[4] = may;
    largest_points[5] = maz;

    //cout << " find_largest_triangle " << a << endl;

}

int main(int argc, char** argv) {
   auto startclock = chrono::high_resolution_clock::now();

    if (argc < 2) {
        cout << "Usage: ./main(or try2) n" << endl;
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
    //vector<double> largest_areas(num_threads);
    vector<vector<Point>> largest_points(num_threads, vector<Point>(6));

    size_t block_size = num_points / num_threads;
    size_t remainder = num_points % num_threads;

    // Создание потоков для обработки блоков
    size_t start = 0;
    for (int i = 0; i < num_threads; i++) {
        size_t end = start + block_size;
        if (i < remainder) {
            end++;
        }
        threads[i] = thread(find_largest_triangle, ref(points), start, end, ref(largest_points[i]), i); //ref - передача по ссылке
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


    Point minn_x = largest_points[0][0], maxx_x = largest_points[0][0];
    Point minn_y = largest_points[0][0], maxx_y = largest_points[0][0];
    Point minn_z = largest_points[0][0], maxx_z = largest_points[0][0];

    Point mixx,miyy,mizz;
    Point maxx,mayy,mazz;

    //this_thread::sleep_for(chrono::milliseconds(1000));



    for (int i = 0; i < num_threads; ++i){
        for (int j = 0; j < 6; ++j){
            if (largest_points[i][j].x <= minn_x.x) {
                minn_x.x = largest_points[i][j].x;
                mixx = largest_points[i][j];
            }
            if (largest_points[i][j].y <= minn_y.y) {
                minn_y.y = largest_points[i][j].y;
                miyy = largest_points[i][j];
            }
            if (largest_points[i][j].z <= minn_z.z) {
                minn_z.z = largest_points[i][j].z;
                mizz = largest_points[i][j];
            }
            if (largest_points[i][j].x >= maxx_x.x) {
                maxx_x.x= largest_points[i][j].x;
                maxx = largest_points[i][j];
            }
            if (largest_points[i][j].y >= maxx_y.y) {
                maxx_y.y= largest_points[i][j].y;
                mayy = largest_points[i][j];
            }
            if (largest_points[i][j].z >= maxx_z.z) {
                maxx_z.z= largest_points[i][j].z;
                mazz = largest_points[i][j];
            }

        }
    }

    Point my[6] = {mixx,miyy,mizz,maxx,mayy,mazz};

    double largest_area = 0.0;
    vector<Point> largest_point_set(3);


    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 5; j++) {
            for (int k = j + 1; k < 6; k++) {
                double area = triangle_area(my[i], my[j], my[k]);

                if (area > largest_area) {
                    largest_area = area;
                    largest_point_set[0] = my[i];
                    largest_point_set[1] = my[j];
                    largest_point_set[2] = my[k];
                }
            }
        }
    }

    cout << "Максимальная площадь: " << largest_area << endl;

    cout << "Координаты максимальной площади: " << endl;
    cout << "X: " << largest_point_set[0].x << " Y: " << largest_point_set[0].y << " Z: " << largest_point_set[0].z << endl;
    cout << "X: " << largest_point_set[1].x << " Y: " << largest_point_set[1].y << " Z: " << largest_point_set[1].z << endl;
    cout << "X: " << largest_point_set[2].x << " Y: " << largest_point_set[2].y << " Z: " << largest_point_set[2].z << endl;

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - startclock;
    
    cout << "Время выполнения программы: " << elapsed.count() << " секунд" << endl;


    return 0;
}
