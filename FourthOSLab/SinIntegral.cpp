#include <iostream>
#include <cmath>
#include "SinIntegral.hpp"
extern "C" {
float sinintegralFirst(float A, float B, float e) { //методом прямоугольников
    float integral = 0.0f;
    float n = (B - A) / e; //задаем число разбиений n
    
    for(int i = 1; i <= n; ++i){
        integral = integral + e * std::sin(A + e * (i - 0.5));
    }
    
    // float x = A;
    
    // while (x <= B) {
    //     integral += std::sin(x) * e;
    //     x += e;
    // }
    
    return integral;
}

float sinintegralSecond(float A, float B, float e) { //методом трапеций

    float n = (B - A) / e; //задаем число разбиений n
    float integral = e * (std::sin(A)+std::sin(B)) / 2.0;
    for (int i = 1; i <= n - 1; ++i){
        integral = integral + e * std::sin(A + e * i);
    }


    // float integral = 0.0f;
    // float x = A;
    
    // while (x <= B) {
    //     integral += (std::sin(x) + std::sin(x + e)) * e / 2.0f;
    //     x += e;
    // }
    
    return integral;

}
}