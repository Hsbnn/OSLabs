#include <pthread.h>
#include <sys/mman.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <cassert>
#include <fstream>
#include <mutex>
#include "game_functions.hpp"

using namespace std;

struct pirat {
    bool turn;
    std::string name;
    std::vector<std::vector<char>> field;
};

void initpirat(pirat &prt){
    prt.name = "";
    prt.turn = false;
    prt.field = vector<vector<char>>(12, vector<char>(12, '.'));
}

struct gamestruct{
    bool connected, created;
    string name, password;
};

void initgame(gamestruct &g) {
    g.connected = false;
    g.created = false;
    g.password = "";
    g.name = "";
}

int main() {
    std::mutex mtx;
    pirat pirat1, pirat2;
    initpirat(pirat1);
    initpirat(pirat2);
    gamestruct game;
    initgame(game);
    string mclient = "";
    int mmapfd = shm_open("buffer", O_RDWR | O_CREAT, 0777);
    if (mmapfd == -1) {
        perror("error shm_open (26)");
        return -1;
    }
    char *mmapdata = (char *)mmap(NULL, 10000, PROT_READ | PROT_WRITE, MAP_SHARED, mmapfd, 0);
    if (mmapdata == MAP_FAILED) {
        perror("error (char *)mmap");
        return -1;
    }
    memset(mmapdata, '\0', 10000);

    cout << "Сервер начал свою работу... Вызови пирата на бой!" << endl;

    while (1) {

        if (mmapdata[0] == '\0') {
            continue;
        }
        mclient = mmapdata;
        vector<string> message_from_client;
        string strings = ""; 
        for (int i = 0; i < mclient.size(); ++i) {
            if (mclient[i] == ':') {
                message_from_client.push_back(strings);
                strings = "";
            } 
            else {
                strings.push_back(mclient[i]);
            }
        }
        if (message_from_client[0] != "server") {
            continue;
        }
        mtx.lock();
        cout << "mutex lock (87)" << endl;

        cout << "Получено пиратское сообщение: " << mclient << endl;
        memset(mmapdata, '\0', 10000);
        
        if (message_from_client[2] == "creategame") {

            if (game.created) {
                
                string message_to_client = string("client:") + message_from_client[1] + ":c_error:";
                sprintf(mmapdata, "%s", message_to_client.c_str());
                cout << "Отправлено сообщение: " << message_to_client << endl;

            } else {

                game.created = true;
                pirat1.turn = true;
                pirat2.turn = false;

                pirat1.name = message_from_client[1];
                game.name = message_from_client[3];
                game.password = message_from_client[4];

                new_field(pirat1.field);
                string message_to_client = string("client:") + message_from_client[1] + ":createdgame:";
                sprintf(mmapdata, "%s", message_to_client.c_str());
                cout << "Отправлено сообщение: " << message_to_client << endl;

            }
        } else if (message_from_client[2] == "newname"){
            if (game.created){
                if (pirat1.name == message_from_client[1]){
                    pirat1.name = message_from_client[3];

                    string message_to_client = string("client:") + message_from_client[1] + ":changedname:" + message_from_client[3] + ":";
                    sprintf(mmapdata, "%s", message_to_client.c_str());
                    cout << "Отправлено сообщение: " << message_to_client << endl;

                } else if (pirat2.name == message_from_client[1]){
                    pirat2.name = message_from_client[3];

                    string message_to_client = string("client:") + message_from_client[1] + ":changedname:" + message_from_client[3] + ":";
                    sprintf(mmapdata, "%s", message_to_client.c_str());
                    cout << "Отправлено сообщение: " << message_to_client << endl;
                }
            } else {
                string message_to_client = string("client:") + message_from_client[1] + ":notchangedname:";
                sprintf(mmapdata, "%s", message_to_client.c_str());
                cout << "Отправлено сообщение: " << message_to_client << endl;
            }

        }  else if (message_from_client[2] == "connection"){

            if (game.connected) {
                string message_to_client = string("client:") + message_from_client[1] + ":c_error:";
                sprintf(mmapdata, "%s", message_to_client.c_str());
                cout << "Отправлено сообщение: " << message_to_client << endl;

            } else {

                if (game.name == message_from_client[3]) {

                    if (game.password == message_from_client[4]) {

                        pirat2.turn = false;
                        pirat1.turn = true;
                        game.connected = true;

                        pirat2.name = message_from_client[1];
                        new_field(pirat2.field);
                        string message_to_client = string("client:") + message_from_client[1] + ":connected:";
                        sprintf(mmapdata, "%s", message_to_client.c_str());
                        cout << "Отправлено сообщение: " << message_to_client << endl;

                    } else {

                        game.connected = false;
                        string message_to_client = string("client:") + message_from_client[1] + ":p_error:";
                        sprintf(mmapdata, "%s", message_to_client.c_str());
                        cout << "Отправлено сообщение: " << message_to_client << endl;

                    }

                } else {

                    game.connected = false;
                    string message_to_client = string("client:") + message_from_client[1] + ":gamenotexists:";
                    sprintf(mmapdata, "%s", message_to_client.c_str());
                    cout << "Отправлено сообщение:" << message_to_client << endl;

                }
            }

        } else if (message_from_client[2] == "invite"){

            if (game.connected) {

                string message_to_client = string("client:") + message_from_client[1] + ":c_error:";
                sprintf(mmapdata, "%s", message_to_client.c_str());
                cout << "Отправлено сообщение: " << message_to_client << endl;
            }
            else{

                //pirat2.invite = true;

                game.name = message_from_client[3];
                game.password = message_from_client[4];
                game.connected = true;
                string message_to_client = string("client:") + message_from_client[1] + ":invited:";
                sprintf(mmapdata, "%s", message_to_client.c_str());
                cout << "Отправлено сообщение: " << message_to_client << endl;

            }

        } else if(message_from_client[2] == "checking"){

            if(game.connected){

                pirat2.turn = false;
                pirat1.turn = true;
                //game.connected = true;

                pirat2.name = message_from_client[1];
                new_field(pirat2.field);
                string message_to_client = string("client:") + message_from_client[1] + ":checked:" + game.name + ':' + game.password + ':';
                sprintf(mmapdata, "%s", message_to_client.c_str());
                cout << "Отправлено сообщение: " << message_to_client << endl;

            }
            else{

                string message_to_client = string("client:") + message_from_client[1] + ":check_error:" + game.name + ':' + game.password + ':';
                sprintf(mmapdata, "%s", message_to_client.c_str());
                cout << "Отправлено сообщение: " << message_to_client << endl;

            }

        } else if (message_from_client[2] == "hit") {

            if (!game.connected) {
                string message_to_client = string("client:") + message_from_client[1] + ":game_not_connected:";
                sprintf(mmapdata, "%s", message_to_client.c_str());
                cout << "Отправлено сообщение: " << message_to_client << endl;
            }
            if (message_from_client[1] == pirat2.name) {
                if (pirat2.turn && !pirat1.turn) {  // check try
                    if (game.name == message_from_client[3]) {
                        int number = stoi(message_from_client[5]);
                        string l = message_from_client[4];
                        char letter = l[0];
                        if (ifkill(pirat1.field, number, letter)) {
                            pirat1.field[number][int(letter) - int('A') + 1] = 'K';
                            pirat2.turn = true;
                            pirat1.turn = false;
                            if (ifwinner(pirat1.field)) {
                                string message_to_client = string("client:") + message_from_client[1] + ":WIN:";
                                sprintf(mmapdata, "%s", message_to_client.c_str());
                                cout << "Sending to pirat2 next message:" << message_to_client << endl;
                                initpirat(pirat1);
                                initpirat(pirat2);
                                initgame(game);
                                
                            } else {
                                string message_to_client = string("client:") + message_from_client[1] + ":kill:";
                                sprintf(mmapdata, "%s", message_to_client.c_str());
                                cout << "Отправлено сообщение:" << message_to_client << endl;
                            }

                        } else if (pirat1.field[number][int(letter) - int('A') + 1] == 'W' || pirat1.field[number][int(letter) - int('A') + 1] == 'K' || pirat1.field[number][int(letter) - int('A') + 1] == 'M') {
                            pirat2.turn = true;
                            pirat1.turn = false;
                            string message_to_client = string("client:") + message_from_client[1] + ":onemore:";
                            sprintf(mmapdata, "%s", message_to_client.c_str());
                            cout << "Отправлено сообщение:" << message_to_client << endl;  

                        } else if (ifhit(pirat1.field, number, letter)) {
                            pirat1.field[number][int(letter) - int('A') + 1] = 'W';
                            pirat2.turn = true;
                            pirat1.turn = false;
                            string message_to_client = string("client:") + message_from_client[1] + ":hit:";
                            sprintf(mmapdata, "%s", message_to_client.c_str());
                            cout << "Отправлено сообщение: " << message_to_client << endl;
                        } else if (ifmiss(pirat1.field, number, letter)) {
                            pirat2.turn = false;
                            pirat1.turn = true;
                            pirat1.field[number][int(letter) - int('A') + 1] = 'M';
                            string message_to_client = string("client:") + message_from_client[1] + ":miss:";
                            sprintf(mmapdata, "%s", message_to_client.c_str());
                            cout << "Отправлено сообщение: " << message_to_client << endl;
                        }
                        cout << "Current state of " << pirat1.name << "'s field is: " << endl;
                        print(pirat1.field);

                    } else {

                        string message_to_client = string("client:") + message_from_client[1] + ":game_name_error:";
                        sprintf(mmapdata, "%s", message_to_client.c_str());
                        cout << "Отправлено сообщение: " << message_to_client << endl;
                    }

                } else {
                    string message_to_client = string("client:") + message_from_client[1] + ":turn_error:";
                    sprintf(mmapdata, "%s", message_to_client.c_str());
                    cout << "Отправлено сообщение: " << message_to_client << endl;
                }
            } else if (message_from_client[1] == pirat1.name) {

                if (pirat1.turn && !pirat2.turn) {
                    if (game.name == message_from_client[3]) {
                        int number = stoi(message_from_client[5]);
                        string l = message_from_client[4];
                        char letter = l[0];
                        // wounded
                        if (ifkill(pirat2.field, number, letter)) {
                            pirat2.field[number][int(letter) - int('A') + 1] = 'K';
                            pirat1.turn = true;
                            pirat2.turn = false;
                            if (ifwinner(pirat2.field)) {
                                string message_to_client = string("client:") + message_from_client[1] + ":WIN:";
                                sprintf(mmapdata, "%s", message_to_client.c_str());
                                cout << "Sending to pirat1 next message: " << message_to_client << endl;
                                initpirat(pirat1);
                                initpirat(pirat2);
                                initgame(game);
                            }
                            else {
                                string message_to_client = string("client:") + message_from_client[1] + ":kill:";
                                sprintf(mmapdata, "%s", message_to_client.c_str());
                                cout << "Отправлено сообщение: " << message_to_client << endl;
                            }
                        } else if (pirat2.field[number][int(letter) - int('A') + 1] == 'W' || pirat2.field[number][int(letter) - int('A') + 1] == 'K' || pirat2.field[number][int(letter) - int('A') + 1] == 'M') {
                            pirat1.turn = true;
                            pirat2.turn = false;
                            string message_to_client = string("client:") + message_from_client[1] + ":onemore:";
                            sprintf(mmapdata, "%s", message_to_client.c_str());
                            cout << "Отправлено сообщение: " << message_to_client << endl;
                        } else if (ifhit(pirat2.field, number, letter)) {
                            pirat2.field[number][int(letter) - int('A') + 1] = 'W';
                            pirat1.turn = true;
                            pirat2.turn = false;
                            string message_to_client = string("client:") + message_from_client[1] + ":hit:";
                            sprintf(mmapdata, "%s", message_to_client.c_str());
                            cout << "Отправлено сообщение: " << message_to_client << endl;
                        } else if (ifmiss(pirat2.field, number, letter)) {
                            pirat1.turn = false;
                            pirat2.turn = true;
                            pirat2.field[number][int(letter) - int('A') + 1] = 'M';
                            string message_to_client = string("client:") + message_from_client[1] + ":miss:";
                            sprintf(mmapdata, "%s", message_to_client.c_str());
                            cout << "Отправлено сообщение: " << message_to_client << endl;
                        }
                        cout << "Current state of " << pirat2.name << "'s field is: " << endl;
                        print(pirat2.field);
                    }
                    else {
                        string message_to_client = string("client:") + message_from_client[1] + ":game_name_error:";
                        sprintf(mmapdata, "%s", message_to_client.c_str());
                        cout << "Отправлено сообщение: " << message_to_client << endl;
                    }
                }
                else {
                    pirat1.turn = false;
                    string message_to_client = string("client:") + message_from_client[1] + ":turn_error:";
                    sprintf(mmapdata, "%s", message_to_client.c_str());
                    cout << "Отправлено сообщение: " << message_to_client << endl;
                }
            }

        } else if(message_from_client[2]=="print"){

            vector< vector<char>> field1, field2;
            if (message_from_client[1] == pirat2.name) {
                field1 = pirat2.field;
                field2 = pirat1.field;   
            } else {
                field1 = pirat1.field;
                field2 = pirat2.field;
            }
            string message_to_client = string("client:") + ":print:";
            message_to_client = message_to_client + "   ";    
            for (int i = 0; i<10; ++i){
                message_to_client = message_to_client + char(int('A')+i) + " ";
            }
            message_to_client = message_to_client + ':';
            for (int i = 1; i < 11; ++i) {
                if (i == 10){
                    message_to_client = message_to_client + to_string(i) + " ";
                } else {
                    message_to_client = message_to_client + to_string(i)  + "  ";
                }
                for (int j = 1; j < 11; ++j) {
                    message_to_client = message_to_client + field1[i][j] + " ";
                }
                message_to_client = message_to_client + ':';
            }
            message_to_client = message_to_client + "   ";
            for (int i = 0; i<10; ++i){
                message_to_client = message_to_client + char(int('A')+i) + " ";
            }
            message_to_client = message_to_client + ':';
            for (int i = 1; i < 11; ++i) {
                if (i == 10){
                    message_to_client = message_to_client + to_string(i) + " ";
                } else {
                    message_to_client = message_to_client + to_string(i)  + "  ";
                }
                for (int j = 1; j < 11; ++j) {
                    if(field2[i][j] != 'X') {
                        message_to_client = message_to_client + field2[i][j] + " ";
                    } else {
                        message_to_client = message_to_client + "." + " ";
                    }
                }
                message_to_client = message_to_client + ':';
            }
            sprintf(mmapdata, "%s", message_to_client.c_str());
            cout << "Напечатаны: " << "field1, field2" << endl;
        }
else if (message_from_client[2] == "disconnect") {
            if (message_from_client[1] == pirat1.name) {
                pirat1.turn = false;
                pirat2.turn = true;
                game.connected = false;
                string message_to_client = string("client") + ':' + message_from_client[1] + ':' + "disconnected" + ':';
                sprintf(mmapdata, "%s", message_to_client.c_str());
                cout << "Отправлено сообщение: " << message_to_client << std::endl;
            }
            else {
                pirat1.turn = true;
                pirat2.turn = false;
                game.connected = false;
                string message_to_client = string("client") + ':' + pirat2.name + ':' + "disconnected" + ':';
                sprintf(mmapdata, "%s", message_to_client.c_str());
                cout << "Отправлено сообщение: " << message_to_client << endl;
            }
        }
        mtx.unlock();
        cout << "mutex unlock" << endl;       
    }
    shm_unlink("buffer");
    return 0;
}