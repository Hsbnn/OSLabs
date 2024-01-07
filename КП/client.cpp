#include <cassert>
#include <cstring>
#include <vector>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <algorithm>
#include <sys/stat.h>
#include <fstream>
#include <mutex>
#include "game_functions.hpp"

using namespace std;


//std::set<string> characters;
std::mutex mtx;
string name, password, name2, gamename = ""; // ник игрока и пароль, ник второго игрока, название игры

bool check_vovel(string c){
    bool fl = false;

    if (c == "A" || c == "B" || c == "C" || c == "D" || c == "E" || c == "F" || c == "G" || c == "H" || c == "I" || c == "J") {
        fl = true;
    }
    // if (c == "a" || c == "b" || c == "c" || c == "d" || c == "e" || c == "f" || c == "g" || c == "h" || c == "i" || c == "j") {
    //     fl = true;
    // }

    return fl;
}

void instruction() {
    cout << endl;
    cout << "Хэй, пират! Возьми вражеский корабль на абордаж!" << endl << endl;
    cout << "Инструкция для успешной игры:" << endl;
    cout << "--- create <название игры> - приготовься к сражению, начни игру" << endl;
    cout << "--- connection <название игры> - присоединение к серверу" << endl;
    cout << "--- newname <new_name> - поменять ник, у тебя новое прозвище? Для начала вступи в бой!" << endl;
    cout << "--- invite <ник второго пирата> - карамба! что за осьминога океан принёс? вызови другого игрока на бой" << endl;
    cout << "--- checking the invitation - если тебя вызвал противник на бой, проверь, успешло ли ты присоединился к игре" << endl;
    cout << "--- <буква - горизонталь> <цифра - вертикаль> - пали изо всех пушек!" << endl;
    cout << "--- print - твой корабль затопили? Проверь, напечатай карту своего флота и карту пирата-разведчика" << endl;
    cout << "--- disconnect - отключиться от сервера" << endl;
    cout << "--- exit - выход из программы, ты сдаёшься? Не так быстро! Для начала отключись от сервера." << endl;
    cout << "--- instruction - прочитай инструкции к игре... первое правило - у настоящих пиратов нет правил!" << endl;
    cout << endl;
}

int mmapfd = shm_open("game_size", O_RDWR, 0777); // открытие области разделяемой памяти
char* mmapdata = (char*)mmap(0, 10000, PROT_READ | PROT_WRITE, MAP_SHARED, mmapfd, 0); // отображение области разделяемой памяти на адресное пространство процесса (возвращает указатель на начало отображ памяти)

void message_send(const string &m) {
    mtx.lock(); // блокировка мьютекса
    memset(mmapdata, '\0', 10000); // установка \0 в разделяемую память, чтобы очистить предыдущее сообщение (если оно было)
    sprintf(mmapdata, "%s", m.c_str()); // запись сообщения message в область отображенной памяти
    mtx.unlock(); // разблокировка мьютекса после записи сообщения
}

bool play = false;
bool unlocking(int p){
    if (p == 1){
        play = true;
    } else {
        play = false;
    }
    mtx.unlock();
    return true;
}

bool message_get() { // получение ответа от сервера
    if (mmapdata[0] != 'c') { 
        return false; // проверка на то, что сообщение записано для клиента
    }
    string message = mmapdata; // преобразование данных из отображенной памяти в строку
    vector<string> message_from_server; // вектор команд сервера
    string strings = "";

    cout << "строка из памяти   " << message << endl;

    for (int i = 0; i < message.size(); i++) { // по каждому символу в сообщении
        if (message[i] == ':') {
            message_from_server.push_back(strings); // заполняем вектор команд
            strings = "";
        } 
        else {
            strings.push_back(message[i]); // заполняем команду
        }
    }

    

    if(message_from_server[1] == "print"){

        cout << "Сообщение о принт получено" << endl;

        for (int i=2; i < message_from_server.size(); ++i){ // вывод поля (может сделать void?)
            cout << message_from_server[i] << endl;
        }
        return true;

    } else if (message_from_server[1] == name){

        mtx.lock();
        memset(mmapdata, '\0', 10000);
        if (message_from_server[2] == "createdgame" || message_from_server[2] == "connected") {
            cout << "Гром и молния! Ты начинаешь бой! Игра успешно создана." << endl;
            cout << "Посмотри на расположение своих кораблей, введи команду print." << endl;
            return unlocking(1);
        } else if (message_from_server[2] == "checked") {
            cout << "Пришло время сойтись якорями! Тебя вызвали на бой." << endl;
            gamename = message_from_server[3];
            password = message_from_server[4];
            return unlocking(1);
        } else if (message_from_server[2] == "check_error") {
            cout << "Ты дрейфуешь в океане, бой остался за горизонтом. Подключиться к серверу не удалось." << endl;
            mtx.unlock();
            return true;
        } else if (message_from_server[2] == "changedname"){

            cout << "Имя успешно изменено на " << message_from_server[3] << endl;
            name = message_from_server[3];
            mtx.unlock();
            return true;

        } else if (message_from_server[2] == "notchangedname"){

            cout << "Имя не изменено, для доступа к имени вступи в игру." << endl;
            mtx.unlock();
            return true;

        } else if (message_from_server[2] == "game_not_connected") {
            cout << "Ждем появление вражеского корабля на горизонте, пират не борется сам с собой." << endl;
            return unlocking(1);
        } else if (message_from_server[2] == "game_name_error") {
            cout << "Проклятье медузы! Что за битва? Такой еще нет!" << endl;
            gamename = "";
            return unlocking(0);
        } else if (message_from_server[2] == "p_error") {
            cout << "Тайный код неверный! Где-то бегает портовая крыса!" << endl;
            gamename = ""; 
            return unlocking(0);
        } else if (message_from_server[2] == "turn_error") {
            cout << "Хорошая попытка, но будем мутить шторм позже, сейчас не твоя очередь!" << endl;
            return unlocking(1);
        } else if (message_from_server[2] == "kill") {
            cout << "УБИЛ - Ты отправил на дно вражеский корабль! Пли еще раз!" << endl;
            return unlocking(1);
        } else if (message_from_server[2] == "hit") {
            cout << "ПОПАЛ - Корма вражеского корабля повреждена! Пли еще раз!" << endl;
            return unlocking(1);
        } else if (message_from_server[2] == "miss") {
            cout << "МИМО - Салют для вражеских кораблей! Пустая трата пушечных ядер, ты промахнулся!" << endl;
            return unlocking(1);
        } else if (message_from_server[2] == "WIN") {
            cout << "ТЫСЯЧА ЧЕРТЕЙ! ЭТО ПОБЕДА!" << endl;
            cout << "Есть только один флаг и на нём черная метка!" << endl;
            return unlocking(0);
        } else if (message_from_server[2] == "c_error") {
            cout << "Бой уже идет, рубите мачты, корабль остаётся у причала." << endl;
            return unlocking(0);
        } else if (message_from_server[2] == "onemore") {
            cout << "Не трать пушечные ядра просто так, введи новые координаты." << endl;
            return unlocking(1);
        } else if (message_from_server[2] == "disconnected") {
            cout << "Ты отключился от сервера." << endl;
            return unlocking(0);
        } else {
            cout  << "Это какой-то шифр, пираты его не знают. Пушки заряжены!" << endl;
            return unlocking(0);
        }

        mtx.unlock();
        return true;
        
    }
    else if (message_from_server[1] == name2)
    {
        if(message_from_server[2] == "invited"){
            cout << "Ты вызвал другого пирата на бой!" << endl;
            mtx.unlock();
            return true;
        }
    }
    
    return false;
}


bool waiting_answer(string message_to_server){

    bool wait = true;
    message_send(message_to_server); //отправка сообщения на сервер
    while (wait) {
        wait = !message_get(); //отслеживание получения ответа
    }
    return wait;

}

int main() {

    
    // // Добавление маленьких букв от a до j
    // for (char ch = 'a'; ch <= 'j'; ++ch) {
    //     characters.insert(to_string(ch));
    // }

    // // Добавление заглавных букв от A до J
    // for (char ch = 'A'; ch <= 'J'; ++ch) {
    //     characters.insert(to_string(ch));
    // }

    instruction();
    cout << "Тысяча чертей! Пора в бой, пират!" << endl;
    cout << "Как тебя будет звать противник? Введи своё имя" << endl;
    cout << "пират$ ";
    cin >> name;
    cout << name << ", мы начинаем..." << endl;

    if (mmapfd == -1) {
        perror("error shm_open (214)\n");
        return 1;
    }
    if (mmapdata == MAP_FAILED) {
        perror("error (char*)mmap (224)\n");
        return 2;
    }

    string game, command;
    while ((cout << name << "$ ") && cin >> command) {

        if (command == "create") {

            cin >> game;
            cout << "Введите тайный код доступа к игре" << endl;
            cout << name << "$ " ;
            cin >> password;

            if (play == false){

                gamename = game;
                string message_to_server = "server:" + name + ":creategame:" + game + ":" + password + ":";
                //std::cout << message_to_server << std::endl;
                waiting_answer(message_to_server);

            } else {

                cout << "Еще один бой!? Сил флота недостаточно для второй игры." << endl;
                continue;

            }

        }  else if (command == "connection"){

            cin >> game;
            cout << "Введите тайный код доступа к игре" << endl;
            cout << name << "$ " ;
            cin >> password;

            if (play == false){

                gamename = game; // можно сразу записать
                string message_to_server = "server:" + name + ":connection:" + game + ":"+ password + ":";
                waiting_answer(message_to_server);

            } else {

                cout << "Фок-грот-брамсель в левое ухо! Ты уже присоединился к серверу." << endl;
                continue;

            }

        } else if (command == "newname"){

            string newn;
            cin >> newn;
            string message_to_server = "server:" + name + ":newname:" + newn + ":";
            waiting_answer(message_to_server);


        } else if (command == "invite"){

            if (play){
            
                cin >> name2; 
                string message_to_server = "server:" + name2 + ":invite:" + game + ":"+ password + ":";
                waiting_answer(message_to_server);

            } else {

                cout << "Ты должен быть в игре." << endl;
                continue;

            }

        } else if (command == "checking"){

            string the, invitation;
            cin >> the >> invitation;

            if (play == false){

                string message_to_server = "server:" + name + ":checking:";
                waiting_answer(message_to_server);

            } else {

                cout << "Ты уже в бою." << endl;
                continue;

            }

        } else if (check_vovel(command)){

            int n;
            cin >> n;

            cout << command << " " << n << endl;

            if (play){

                if ((n <= 0) || (n >= 11)) {
                    cout << "Опечатка? число (1...10)" << endl;
                    continue;
                }
                else {
                    
                    string message_to_server = "server:" + name + ":ABC:" + gamename + ":" + command + ":" + to_string(n) + ":";
                    waiting_answer(message_to_server);

                }

            } else {
        
                cout << "В мирное время пираты никого не бьют! Конечно, если это не заклятый враг и не обидчик прекрасной дамы... " << endl;
                cout << "Начни бой с вражеским флотом, чтобы пальнуть из пушки!" << endl;
                continue;

            }

        } else if (command == "print"){

            if (play){

                // cout << "Твой флот" << endl;
                string message_to_server = "server:" + name + ":print:";
                waiting_answer(message_to_server);

            } else {

                cout << "Ты не играешь, у тебя еще нет карты флота." << endl;
                continue;

            }

        } else if (command == "disconnect"){

            if (play){

                string message_to_server = "server:" + name + ":disconnect:" + gamename + ":";
                waiting_answer(message_to_server);

            } else {

                cout << "Ты не присоединен к серверу." << endl;
                continue;

            }

        } else if (command == "instruction"){

            instruction();

        } else if (!play && command == "exit"){

            break;

        } else{
            cout << "Гром и молния! 50 отжиманий за неверную команду." << endl;

        }
    }

    return 0;
}