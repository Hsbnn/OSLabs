#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

int main(){
    int inputLineLen1;
    while(1){
        read(STDIN_FILENO, &inputLineLen1, sizeof(inputLineLen1));
        char inputLine1[inputLineLen1];
        read(STDIN_FILENO, inputLine1, inputLineLen1);

        string output1;

        for (int i = 0; i < inputLineLen1; i++){
            char c1 = inputLine1[i];
            if (c1 != 'a' && c1 != 'e' && c1 != 'i' && c1 != 'o' && c1 != 'u' && c1 != 'y'){
                output1 += c1;
            }
        }

        output1 += '\n';
        write(STDOUT_FILENO, output1.c_str(), output1.length());
        //в файл просто пишем строку

    }


    return 0;
}