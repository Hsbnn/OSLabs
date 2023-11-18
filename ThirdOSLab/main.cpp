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


int main(){
    string filenamep1, filenamep2;
    cout << "Введите имя файла 1 для родительского процесса (из которого будет читать child1): \n";
    cin >> filenamep1;
    cin.ignore();
    cout << "Введите имя файла 2 для родительского процесса (из которого будет читать child2): \n";
    cin >> filenamep2;
    cin.ignore();

    int f_Fdp1 = open(filenamep1.c_str(), O_CREAT | O_RDWR | O_TRUNC , 0777);
    int f_Fdp2 = open(filenamep2.c_str(), O_CREAT | O_RDWR | O_TRUNC , 0777);
    if (f_Fdp1 == -1){
        cerr << "can't open file: " << filenamep1;
        cout << "beginning\n";
        exit(EXIT_FAILURE);
    }
    if (f_Fdp2 == -1){
        cerr << "can't open file: " << filenamep2;
        exit(EXIT_FAILURE);
    }
    close(f_Fdp1);
    close(f_Fdp2);

    int shared_line1_fd;
    if((shared_line1_fd = shm_open("mapped_file1", O_CREAT | O_RDWR, 0777)) == -1) {
        cout << "....." << endl;
        exit(1);
    }
    if(ftruncate(shared_line1_fd, STRING_SIZE) == -1) {
        cout << "truncate throwed\n";
        exit(1);
    }

    //создаем отображаемую память в качестве трубы  //
    char* shared_mem1 = static_cast<char*>(mmap(NULL, STRING_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_line1_fd, 0));
    if(shared_mem1 == MAP_FAILED) {
        cout << "ploxo\n";
        exit(1);
    }


    int shared_line2_fd;
    if((shared_line2_fd = shm_open("mapped_file2", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR)) == -1) {
        cout << "....." << endl;
        exit(1);
    }
    if(ftruncate(shared_line2_fd, STRING_SIZE) == -1) {
        cout << "truncate throwed\n";
        exit(1);
    }

    //создаем отображаемую память в качестве трубы  //
    char* shared_mem2 = static_cast<char*>(mmap(NULL, STRING_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_line2_fd, 0));
    if(shared_mem2 == MAP_FAILED) {
        cout << "ploxo\n";
        exit(1);
    }


    string inputLine;
    int inputLineLen = 0;

    while (getline(cin, inputLine)){
        inputLineLen = inputLine.length();
        if(inputLine == "exit" ){ //признак завершения ввода
            break;
        } else {
            if (inputLineLen > 10){
                // write(f_Fdp2, inputLine.c_str(), inputLine.length());
                // write(f_Fdp2, "\n", 1);
                for (size_t i = 0; i < inputLine.size(); i++) {
                    shared_mem2[i] = inputLine[i];
                }

            } else {
                // write(f_Fdp1, inputLine.c_str(), inputLine.length());
                // write(f_Fdp1, "\n", 1);
                for (size_t i = 0; i < inputLine.size(); i++) {
                    shared_mem1[i] = inputLine[i];
                }
            }
        }

    }


    //struct stat statbuf;

    // struct stat statbuf1; //получаем размер файла родителя
    // int errr = fstat(f_Fdp1, &statbuf1);
    // if (errr < 0){
    //     cerr << "can't do this one fstat(f_Fdp1, &statbuf1): " << endl;
    //     exit(EXIT_FAILURE);
    // }


    // struct stat statbuf2; //получаем размер файла родителя
    // int errr2 = fstat(f_Fdp2, &statbuf2);
    // if (errr < 0){
    //     cerr << "can't do this one fstat(f_Fdp2, &statbuf2): " << endl;
    //     exit(EXIT_FAILURE);
    // }
    

    // создаем Child1 процесс
    pid_t child1 = fork();
    if (child1 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    } //проверка на создание процесса


    if (child1 == 0){ //работаем в child1 - дочерний процесс 1

        execl("child", "child", "mapped_file1", filenamep1.c_str(), nullptr);

        cout << "EXECL ERROR" << endl;
        return 1;
    
    } else { //находимся в родительском процессе

        wait(nullptr);
        //поработаем над созданием еще одного процесса
        //создаем Child2 процесс
        pid_t child2 = fork();
        if (child2 == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        } //проверка на создание процесса

        if (child2 == 0){ //находимся в child2 процессе

            execl("child", "child", "mapped_file2", filenamep2.c_str(), nullptr);

            cout << "EXECL ERROR" << endl;
            return 1;   

        } else { //в родительском процессе

            wait(nullptr);
            


            if (munmap(shared_mem1, STRING_SIZE) == -1) {
                perror("Ошибка при очистке области памяти 1");
                return 11;
            }

            if (munmap(shared_mem2, STRING_SIZE) == -1) {
                perror("Ошибка при очистке области памяти 2");
                return 22;
            }
            shm_unlink("mapped_file1");
            shm_unlink("mapped_file2");

        }

    }

    return 0;
}