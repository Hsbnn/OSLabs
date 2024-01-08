#include <iostream>
#include <string>
#include "calcNode.hpp"
#include "zmq_functions.hpp" 
#include "tree.hpp"

void instruction(){
    cout << "Введите команду:" << endl;;
    cout << "1) create <id>" << endl;
    // таймер
    cout << "2) ping <id>" << endl;
    cout << "3) exec <id> <start/stop/time>" << endl;
    cout << "4) kill <id>" << endl;
    cout << "5) exit - выход" << endl;
}

int main(){

    instruction();

    string command, answer, str;
    CalculationNode node(-1, -1, -1);
    Tree tree;
    int child;

    while (cout << "$> " && cin >> command){
        if (command == "create"){

            cout << "Начало работы сервера create" << endl;
            cin >> child;
            if (tree.Exist(child)) {
                cout << "error: child c таким id уже существует" << endl;
            } else {
                while(1){

                    int idParent = tree.FindID();
                    if (idParent == node.id){ //если равен идентификатор, то дочерний узел будет потомком этого узла
                        answer = node.create(child);
                        tree.AddInTree(child, idParent); //добавить узел в дерево
                        break;
                    } else {
                        string message = "create " +  to_string(child); // если он не равен, то формируем сообщение
                        answer = node.sendstring(message, idParent); //(вызывается у текущей ноды) отправка на родительский узел (поиск) сообщения о создании нового узла
                        if (answer == "Error: Parent not found") {
                            tree.AvailabilityCheck(idParent); // тогда корректируем доступность узла и продолжим цикл
                        }
                        else {
                            tree.AddInTree(child, idParent); // иначе добавим в дерево узел
                            break;
                        }
                    }

                }
                cout << answer << endl;
            }

            cout << "Конец работы сервера create" << endl;

        } else if (command == "ping"){

            cin >> child;
            try{
            if (tree.Exist(child)) {

                string message = "ping " +  to_string(child);
                answer = node.sendstring(message, child); //отправляем сообщение на узел child, если наш id не является левым или правым потомком
                cout << answer <<  endl; //потом уже получаем ответ

            } else {

                cout << "ok: 0" << endl; // узел недоступен

            }
            } catch(int){}

        } else if (command == "exec"){

            cin >> child;
            getline(cin, str);
            if (tree.Exist(child)){

                string message = "exec" + str;
                answer = node.sendstring(message, child); //формирование сообщения, отправка на вычислительный узел
                cout << answer <<  endl; // результат выводится на экран

            } else {

                cout << "error: child c таким id не существует" << endl;

            }

        } else if (command == "kill"){

            cin >> child;
            if (tree.Exist(child)){

               answer = node.sendstring("kill", child); // отправляет сообщение "kill" на указанный узел child
               if (answer != "Error: Parent not found"){
                    tree.RemoveFromRoot(child); // удаление узла из дерева
                    if (child == node.left_id){
                        node.left_id = -2; // отсутствие узла
                        unbind(node.left, node.left_port); // разрыв связи с портом, занятым соответствующим узлом, чтобы освободить его
                        answer = "ok";
                    }
                    else if (child == node.right_id) {
                        node.right_id = -2;
                        unbind(node.right, node.right_port);
                        answer = "ok";
                    }
               }
               cout << answer <<  endl;

            } else {

                cout << "error: child c таким id не существует" << endl;

            }



        } else if (command == "exit"){

            cout << "Выходим из программы :3" << endl;
            break;

        } else {

            instruction();

        }
    }

    node.kill(); //для корректного завершения работы сервера

    return 0;
}
