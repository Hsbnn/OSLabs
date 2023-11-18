#include <iostream> 
#include <unistd.h> //fork, exec, pipe, read, write
#include <fcntl.h> //работа с файловыми дескрипторами
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <sys/stat.h>
#include <cstdio>
#include <errno.h>
#include <sys/mman.h>
#include <stdlib.h>

using namespace std;

const int STRING_SIZE = 1024; 


void isVowel(char c1[STRING_SIZE], int fd) {
    for(size_t i = 0; i < STRING_SIZE; ++i) {
        if (c1[i] != 'a' && c1[i] != 'e' && c1[i] != 'i' && c1[i] != 'o' && c1[i] != 'u' && c1[i] != 'y'){
            write(fd, &c1[i], sizeof(char)); 
        }     
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Необходимо передать имя MMF в аргументах командной строки.\n");
        return 1;
    }


    char* filename = argv[1];
    int shared_line1_fd;
    if((shared_line1_fd = shm_open(filename, O_CREAT | O_RDWR, 0777)) == -1) {
        cout << "....." << endl;
        exit(1);
    }

    char* shared_mem = static_cast<char*>(mmap(NULL, STRING_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_line1_fd, 0));
    if(shared_mem == MAP_FAILED) {
        cout << "ploxo\n";
        exit(1);
    }
    
    int f_Fdp = open(argv[2],  O_RDWR | O_TRUNC , 0777);
    if (f_Fdp == -1){
        cerr << "can't open file: " << argv[2];
        exit(EXIT_FAILURE);
    }

    isVowel(shared_mem, f_Fdp);
    
    close(f_Fdp);
    

    return 0;
}