#include <iostream>
#include "Pi.hpp"
extern "C" {
float piFirst(int K) { //ряд Лейбница

    float pi = 0.0;
    float sign = 1.0;

    for (int i = 0; i < K; i++) {
        pi += sign / (2 * i + 1);
        sign *= -1;
    }

    return 4 * pi; //умножается на 4, так как ряд Лейбница сходится к π/4.

}

float piSecond(int K) { //формула Валлиса

    float pi = 1.0;

    for (int i = 1; i <= K; i++) {
        float numerator = 4 * i * i;
        float denominator = 4 * i * i - 1;
        pi *= numerator / denominator;        
    }
    
    return 2 * pi;

}
}