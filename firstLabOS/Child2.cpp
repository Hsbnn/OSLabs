#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

int main(){
    int inputLineLen2;
    while(1){
        read(STDIN_FILENO, &inputLineLen2, sizeof(inputLineLen2));
        char inputLine2[inputLineLen2];
        read(STDIN_FILENO, inputLine2, inputLineLen2);

        string output2;

        for (int j = 0; j < inputLineLen2; j++){
            char c2 = inputLine2[j];
            if (c2 != 'a' && c2 != 'e' && c2 != 'i' && c2 != 'o' && c2 != 'u' && c2 != 'y'){
                output2 += c2;
            }
        }

        output2 += '\n';
        write(STDOUT_FILENO, output2.c_str(), output2.length());
        //в файл просто пишем строку


    }

    return 0;
}