#include <iostream> 
#include <unistd.h> //fork, exec, pipe, read, write
#include <fcntl.h> //работа с файловыми дескрипторами
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/mman.h>

using namespace std;

bool isVowel(char c1) {
    if (c1 != 'a' && c1 != 'e' && c1 != 'i' && c1 != 'o' && c1 != 'u' && c1 != 'y'){
            return true;
    }
}



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
        exit(EXIT_FAILURE);
    }
    if (f_Fdp2 == -1){
        cerr << "can't open file: " << filenamep2;
        exit(EXIT_FAILURE);
    }

    string inputLine;
    int inputLineLen = 0;

    while (getline(cin, inputLine)){
        inputLineLen = inputLine.length();
        if(inputLine == "exit" ){ //признак завершения ввода
            break;
        } else {
            if (inputLineLen > 10){
                write(f_Fdp2, inputLine.c_str(), inputLine.length());
                write(f_Fdp2, "\n", 1);
            } else {
                write(f_Fdp1, inputLine.c_str(), inputLine.length());
                write(f_Fdp1, "\n", 1);
            }
        }

    }


    struct stat statbuf;

    struct stat statbuf1; //получаем размер файла родителя
    int errr = fstat(f_Fdp1, &statbuf1);
    if (errr < 0){
        cerr << "can't do this one fstat(f_Fdp1, &statbuf1): " << endl;
        exit(EXIT_FAILURE);
    }


    struct stat statbuf2; //получаем размер файла родителя
    int errr2 = fstat(f_Fdp2, &statbuf2);
    if (errr < 0){
        cerr << "can't do this one fstat(f_Fdp2, &statbuf2): " << endl;
        exit(EXIT_FAILURE);
    }

    //создаем отображаемую память //
    char* ptr1 = static_cast<char*>(mmap(NULL, statbuf1.st_size, PROT_READ, MAP_SHARED, f_Fdp1, 0));
    if (ptr1 == MAP_FAILED){
        cerr << "can't do this one *ptr1 = mmap(...): " << endl;
        exit(EXIT_FAILURE);
    }

    char* ptr2 = static_cast<char*>(mmap(NULL, statbuf2.st_size, PROT_READ, MAP_SHARED, f_Fdp2, 0));
    if (ptr2 == MAP_FAILED){
        cerr << "can't do this one *ptr2 = mmap(...): " << endl;
        exit(EXIT_FAILURE);
    }

    close(f_Fdp1);
    close(f_Fdp2);

    // создаем Child1 процесс
    pid_t child1 = fork();
    if (child1 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    } //проверка на создание процесса


    if (child1 == 0){ //работаем в child1 - дочерний процесс 1

        for (int i = 0; i < statbuf1.st_size; i++) {
            cout << " " << ptr1[i] << " ";
        }

        for (int i = 0; i < statbuf1.st_size; i++) {
            if (isVowel(ptr1[i])) {
                // Замена гласной
                ptr1[i] = ' ';
            }
        }  

        exit(0);

    } else { //находимся в родительском процессе

        //поработаем над созданием еще одного процесса
        //создаем Child2 процесс
        pid_t child2 = fork();
        if (child2 == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        } //проверка на создание процесса

        if (child2 == 0){ //находимся в child2 процессе

            for (int i = 0; i < statbuf2.st_size; i++){
                cout << " " << ptr2[i] << " ";
            }
            cout << endl;

            for (int i = 0; i < statbuf2.st_size; i++) {
                if (isVowel(ptr2[i])) {
                    // Замена гласной
                    ptr2[i] = ' ';
                }

            } 

            exit(0);

        } else { //в родительском процессе


            //Ожидание завершения child1
            int status1, status2;
            pid_t terminated_child1 = waitpid(child1, &status1, 0);
            // Ожидание завершения child2
            pid_t terminated_child2 = waitpid(child2, &status2, 0);

            if (terminated_child1 == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

            if (WIFEXITED(status1)) {
                //int exit_status = WEXITSTATUS(status);
                cout << "Normal termination child1 with exit status = " << WIFEXITED (status1) << endl;
                // Обработка статуса завершения child1
            }

            
            if (terminated_child2 == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

            if (WIFEXITED(status2)) {
                //int exit_status = WEXITSTATUS(status);
                cout << "Normal termination child2 with exit status = " << WIFEXITED (status2)  << endl;
                // Обработка статуса завершения child2
            }


            if (munmap(ptr1, statbuf1.st_size) == -1) {
                perror("Ошибка при очистке области памяти 1");
                return 11;
            }

            if (munmap(ptr2, statbuf2.st_size) == -1) {
                perror("Ошибка при очистке области памяти 2");
                return 22;
            }

        }

    }

    return 0;
}
