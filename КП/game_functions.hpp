#pragma once
#include <ctime>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

bool realkill(vector<vector<char>> &field, int n, char c){

    bool real = true;

    if (field[n][int(c) - int('A')] == 'W'){

        if (c == 'B' || c == 'A' ){
            real = true;
        } else if (field[n][int(c) - int('A') - 1] == 'X'){

            return false;

        } else if (field[n][int(c) - int('A') - 1] == 'W'){
            if (c == 'C'){
                real = true;
            } else if (field[n][int(c) - int('A') - 2] == 'X'){
                return false;
            }
        }

    } 
    if (field[n - 1][int(c) - int('A') + 1] == 'W'){

        if (n == 2){
            real = true;
        } else if (field[n - 2][int(c) - int('A') + 1] == 'X'){
            return false;
        } else if (field[n - 2][int(c) - int('A') + 1] == 'W'){
            if (n == 3){
                real = true;
            } else if (field[n - 3][int(c) - int('A') + 1] == 'X'){
                return false;
            }
        }

    }
    if (field[n][int(c) - int('A') + 2] == 'W'){

        if (c == 'I'){
            real = true;
        } else if (field[n][int(c) - int('A') + 3] == 'X'){
            return false;
        } else if (field[n][int(c) - int('A') + 3] == 'W'){
            if (c == 'H'){
                real = true;
            } else if (field[n][int(c) - int('A') + 4] == 'X'){

                return false;

            }
        }

    }
    if (field[n + 1][int(c) - int('A') + 1] == 'W'){

        if (n == 9){
            real = true;
        } else if (field[n + 2][int(c) - int('A') + 1] == 'X'){
            return false;
        } else if (field[n + 2][int(c) - int('A') + 1] == 'W'){
            if (n == 8){
                real = true;
            } else if  (field[n + 3][int(c) - int('A') + 1] == 'X'){

                return false;

            }
        }

    }

    return real;
}

bool ifkill(vector<vector<char>> &field, int n, char c){
    bool k = false;

    if (field[n][int(c) - int('A') + 1] == 'X'){

        if (field[n][int(c) - int('A')] != 'X' &&
            field[n - 1][int(c) - int('A')] != 'X' &&
            field[n - 1][int(c) - int('A') + 1] != 'X' &&
            field[n - 1][int(c) - int('A') + 2] != 'X' &&
            field[n][int(c) - int('A') + 2] != 'X' &&
            field[n + 1][int(c) - int('A') + 2] != 'X' &&
            field[n + 1][int(c) - int('A') + 1] != 'X' &&
            field[n + 1][int(c) - int('A')] != 'X'){
            
            if (realkill(field, n, c)){
                k = true;
            }
        }
    }
    return k;
}

bool ifhit(vector<vector<char>> &field, int n, char c){
    bool h = false;

    if ((field[n][int(c) - int('A') + 1] == 'X') && (!ifkill(field, n, c))){
        h = true;
    }

    return h;
}

bool ifmiss(vector<vector<char>> &field, int n, char c){
    bool m = false;

    if (field[n][int(c) - int('A') + 1] == '.'){
        m = true;
    }

    return m;
}

bool ifwinner(vector< vector<char>> &field) {
    for (int i = 1; i < 11; ++i) {
        for (int j = 1; j < 11; ++j) {
            if (field[i][j] == 'X') {
                return false;
            }
        }
    }
    return true;
}

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
                    if (newRow >= 0 && newRow < 12 && newCol >= 0 && newCol < 12 && field[newRow][newCol] != '.') {
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
                    if (newRow >= 0 && newRow < 12 && newCol >= 0 && newCol < 12 && field[newRow][newCol] != '.') {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

void new_field(std::vector<std::vector<char>>& field) {

    for (int i = 0; i < field.size(); ++i) {
        for (int j = 0; j < field[i].size(); ++j) {
            field[i][j] = '.';
        }
    }

    srand(static_cast<unsigned int>(std::time(0))); // Инициализация генератора случайных чисел

    // Генерируем корабли случайным образом
    std::vector<int> shipLengths = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1}; // Длины кораблей
    for (int i = 0; i < shipLengths.size(); ++i) {
        int shipLength = shipLengths[i];
        int row, col;
        char direction;

        // Генерируем случайные координаты и направление для корабля
        do {
            row = 1 + rand() % 10;
            col = 1 + rand() % 10; 
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


void print( vector< vector<char>> &field) {
    cout << "  ";
    for (int i = 0; i < 10; ++i){
        cout << char(int('A')+i) << " ";
    }
    cout << endl;

    for (int i = 0; i < field.size(); ++i) {
        cout << i+1 << " ";
        for (int j = 0; j < field[i].size(); ++j) {
            std::cout << field[i][j] << ' ';
        }
        std::cout << std::endl;
    }
}
