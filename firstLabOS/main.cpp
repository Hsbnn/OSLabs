#include <iostream> 
#include <unistd.h> //fork, exec, pipe, read, write
#include <fcntl.h> //работа с файловыми дескрипторами
#include <sys/types.h>
#include <cstring>

using namespace std;

int main(){
    string filename1, filename2;
    cout << "Введите имя файла для Child1: \n";
    cin >> filename1;
    cin.ignore();
    cout << "Введите имя файла для Child2: \n";
    cin >> filename2;
    cin.ignore();

    //откроем файлы для записи
    int f_Fd1 = open(filename1.c_str(), O_CREAT | O_WRONLY | O_TRUNC , 0777); //0777 - все права, но открываем только для записи
    //c_str() - преобразование в char из string
    int f_Fd2 = open(filename2.c_str(), O_CREAT | O_WRONLY | O_TRUNC , 0777);

    if (f_Fd1 == -1){
        cerr << "can't open file: " << filename1;
        exit(EXIT_FAILURE);
    }
    if (f_Fd2 == -1){
        cerr << "can't open file: " << filename2;
        exit(EXIT_FAILURE);
    }


    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1){
        cerr << "fail pipe()";
        exit(EXIT_FAILURE);
    } //проверили на правильное создание

    // создаем Child1 процесс
    pid_t child1 = fork();
    if (child1 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    } //проверка на создание процесса (вилка)

    if (child1 == 0){ //работаем в child1 - дочерний процесс 1
        close(pipe1[1]); //закрыли pipe1 на запись

        dup2(pipe1[0], STDIN_FILENO); //читаем в IN из pipe1
        dup2(f_Fd1, STDOUT_FILENO); //записываем результат работы в f_Fd1 (OUT)

        execl("./child1", "./child1", NULL);

        //если не смогли запустить child1
        close(pipe1[0]);
        close(f_Fd1);
        cout << "can't exec child1 process";
        exit(-1);

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
            close(pipe2[1]); //закрыли pipe1 на запись
            
            dup2(pipe2[0], STDIN_FILENO); //читаем в IN из pipe2
            dup2(f_Fd2, STDOUT_FILENO); //записываем результат работы в f_Fd2 (OUT)

            execl("./child2", "./child2", NULL);

            //если не смогли запустить child2
            close(pipe2[0]);
            close(f_Fd2);
            cout << "can't exec child2 process";
            exit(-1);
        } else { //в родительском процессе

            close(pipe1[0]);
            close(pipe2[0]);

            string inputLine;
            int inputLineLen = 0;

            while (getline(cin, inputLine)){
                inputLineLen = inputLine.length();
                if (inputLineLen > 10){
                    write(pipe2[1], &inputLineLen, sizeof(inputLineLen));
                    write(pipe2[1], inputLine.c_str(), inputLine.length());
                } else {
                    write(pipe1[1], &inputLineLen, sizeof(inputLineLen));
                    write(pipe1[1], inputLine.c_str(), inputLine.length());
                }

            }

            close(pipe1[1]);
            close(pipe2[1]);

        }


    }

    return 0;
}
