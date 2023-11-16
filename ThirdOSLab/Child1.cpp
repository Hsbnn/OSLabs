#include <unistd.h>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <iostream> 
#include <unistd.h> //fork, exec, pipe, read, write
#include <fcntl.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Необходимо передать имя MMF в аргументах командной строки.\n");
        return 1;
    }


    string filename = argv[1];
    int sharedFileDescriptor = open(filename.c_str(), O_RDWR, 0777);


    int sharedFileSize = atoi(argv[2]);
    char buffer[sharedFileSize];
    ssize_t bytesRead;
    bytesRead = read(sharedFileDescriptor, &buffer, sizeof(buffer));

    if (bytesRead == -1) {
        std::cerr << "Ошибка при чтении из файла." << std::endl;
        return 1;
    }

    close(sharedFileDescriptor);

    string output1;
    for (int i = 0; i < sharedFileSize; i++){
        char c1 = buffer[i];
        if (c1 != 'a' && c1 != 'e' && c1 != 'i' && c1 != 'o' && c1 != 'u' && c1 != 'y'){
            output1 += c1;
        }
    }
    //output1 += '\n';

    sharedFileDescriptor = open(filename.c_str(), O_WRONLY | O_TRUNC, 0777);

    write(sharedFileDescriptor, output1.c_str(), output1.length());
    //в файл просто пишем строки
    close(sharedFileDescriptor);

    return 0;
}