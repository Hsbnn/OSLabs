#include <iostream>
#include <string> 
#include "calcNode.hpp"
#include "zmq_functions.hpp" 
#include "tree.hpp"

using namespace std;

int main(int argc, char *argv[]) {

    CalculationNode node(atoi(argv[1]), atoi(argv[2]), atoi(argv[3])); //int
    std::cout << "Работает Client.cpp" << std::endl;
    string message, command, answer, str;
    int child;

    while (1){

        message = receive_message(node.parent); //получаем сообщение от родительского узла
        istringstream request(message); //поток ввода, как cin, только для строки message
        request >> command; //из потока извлекается первое слово

        if (command == "create"){
            cout << "Creating..." << endl;
            request >> child;
            answer = node.create(child); //вернет child_pid созданного процесса
            send_message(node.parent, answer);
            cout << "Done..." << endl;


        } else if (command == "ping"){

            request >> child; //считываем после команды число - идентификатор процесса
            answer = node.ping(child); //пингуем дочерний процесс с эти идентификатором
            send_message(node.parent, answer);

        } else if (command == "exec"){

            getline(request, str); // считываем всю оставшуюся строку после exec до '\n'
            answer = node.exec(str); // вернет id
            send_message(node.parent, answer);

        } else if (command == "kill"){

            answer = node.kill(); // возвращает parent_id
            send_message(node.parent, answer); 
            disconnect(node.parent, node.parent_port); // отключение от родительского узла
            node.parent.close(); // закрытие соединения
            break;

        } else if (command == "pid"){
            answer = to_string(getpid()); //просто получаем идентификатор текущего процесса
            send_message(node.parent, answer); //отправляем ответ родительскому узлу
        }

    }

    return 0;
}