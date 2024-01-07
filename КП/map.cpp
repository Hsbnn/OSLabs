#include <iostream>
#include <vector>
#include <ctime>
#include <unistd.h>

bool isValidPosition(const std::vector<std::vector<char>>& field, int row, int col, int shipLength, char direction) {
    // Проверяем, попадает ли корабль в границы поля
    if (direction == 'h') {
        if (col + shipLength > 10) {
            return false;
        }
    } else {
        if (row + shipLength > 10) {
            return false;
        }
    }

    // Проверяем, свободны ли клетки для размещения корабля
    if (direction == 'h') {
        for (int i = 0; i < shipLength; ++i) {
            // Проверка клеток корабля
            if (field[row][col + i] != '.') {
                return false;
            }
            // Проверка соседних клеток
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    int newRow = row + dx;
                    int newCol = col + i + dy;
                    if (newRow >= 0 && newRow < 10 && newCol >= 0 && newCol < 10 && field[newRow][newCol] != '.') {
                        return false;
                    }
                }
            }
        }
    } else {
        for (int i = 0; i < shipLength; ++i) {
            // Проверка клеток корабля
            if (field[row + i][col] != '.') {
                return false;
            }
            // Проверка соседних клеток
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    int newRow = row + i + dx;
                    int newCol = col + dy;
                    if (newRow >= 0 && newRow < 10 && newCol >= 0 && newCol < 10 && field[newRow][newCol] != '.') {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

void generateField(std::vector<std::vector<char>>& field) {

    srand(static_cast<unsigned int>(std::time(0))); // Инициализация генератора случайных чисел

    // Заполняем поле точками, по идее уже заполнено
    for (int i = 0; i < field.size(); ++i) {
        for (int j = 0; j < field[i].size(); ++j) {
            field[i][j] = '.';
        }
    }

    // Генерируем корабли случайным образом
    std::vector<int> shipLengths = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1}; // Длины кораблей
    for (int i = 0; i < shipLengths.size(); ++i) {
        int shipLength = shipLengths[i];
        int row, col;
        char direction;

        // Генерируем случайные координаты и направление для корабля
        do {
            row = rand() % 10;
            col = rand() % 10;
            direction = rand() % 2 == 0 ? 'h' : 'v';
        } while (!isValidPosition(field, row, col, shipLength, direction));

        // Размещаем корабль на поле
        if (direction == 'h') {
            for (int j = 0; j < shipLength; ++j) {
                field[row][col + j] = 'X';
            }
        } else {
            for (int j = 0; j < shipLength; ++j) {
                field[row + j][col] = 'X';
            }
        }
    }
}


int main() {
    std::vector<std::vector<char>> field(10, std::vector<char>(10));
    std::vector<std::vector<char>> field2(10, std::vector<char>(10));

    // Генерируем поле кораблей
    generateField(field);
    
    sleep(2);

    std::cout << "  ";
    for (int i = 0; i < 10; ++i){
       std:: cout << char(int('A')+i) << " ";
    }
    std::cout << std::endl;
    // Выводим поле на экран
    for (int i = 0; i < field.size(); ++i) {
        for (int j = 0; j < field[i].size(); ++j) {
            std::cout << field[i][j] << ' ';
        }
        std::cout << std::endl;
    }
    
    generateField(field2);

    std::cout << std::endl;

    std::cout << "  ";
    for (int i = 0; i < 10; ++i){
       std:: cout << char(int('A')+i) << " ";
    }
    std::cout << std::endl;
    
    for (int i = 0; i < field2.size(); ++i) {
        for (int j = 0; j < field2[i].size(); ++j) {
            std::cout << field2[i][j] << ' ';
        }
        std::cout << std::endl;
    }

    return 0;
}