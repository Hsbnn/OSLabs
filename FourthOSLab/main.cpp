#include <iostream>
#include "SinIntegral.hpp"
#include "Pi.hpp"
#include <dlfcn.h>

using namespace std;

int main(){

    cout << "программа join на этапе линковки" << endl << endl;

    unsigned int switch_ = 1;

    while(1){
        cout << "Введите, какую библиотеку Вы хотите подключить - 1 (итнеграл sin(x)) или 2 (число pi)" << endl;
        cout << "Если необходимо поменять реализацию программы, введите 0 (по умолчанию реализация - первая из таблицы)" << endl;
        cout << "Для выхода из программы можете ввести exit" << endl;
        int indicator;
        if(!(cin >> indicator)){
            cout << "корректное завершение" << endl;
            break;
        } else if (indicator == 0){
            if (switch_ == 1){
                switch_ = 2;
            } else {
                switch_ = 1;
            }

            cout << "Реализация изменена на " << switch_ << endl;

        } else if (indicator == 1) {
            
            float A, B, e;

            if (switch_ == 1) {
                cout << "(первый интеграл) Введите три числа - A, B - границы отрезка интегрирования, e - шаг " << endl;;
                cin >> A >> B >> e;

                float integral = sinintegralFirst(A, B, e); 
                cout << "(первый интеграл) Ответ: " << integral << endl;

            } else {
                cout << "(второй интеграл) Введите три числа - A, B - границы отрезка интегрирования, e - шаг " << endl;;
                cin >> A >> B >> e;

                float integral = sinintegralSecond(A, B, e); 
                cout << "(второй интеграл) Ответ: " << integral << endl;
            }


        } else if (indicator == 2) {

            int K;
            if (switch_ == 1) {
                cout << "(первое число пи) Введите число K - длина ряда " << endl;;
                cin >> K;

                float pi = piFirst(K); 
                cout << "(первое число пи) Ответ: " << pi << endl;

            } else {
                cout << "(второе число пи) Введите число K - длина ряда " << endl;;
                cin >> K;

                float pi = piSecond(K); 
                cout << "(второе число пи) Ответ: " << pi << endl;
            }


        }

    }


    return 0;
}
