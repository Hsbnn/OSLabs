#include <iostream>
#include <string> 
#include <set>
#include "unistd.h"
#include "zmq_functions.hpp" 

class CalculationNode
{
private:
    zmq::context_t context; // контекст ZMQ для создания сокетов
    bool timerRunning; // переменная, которая отслеживает состояние таймера, если активен, то true
    chrono::steady_clock::time_point startTime; //время начала таймера
    std::chrono::milliseconds stopTime; // чтобы хранить время после stop таймер
public:
    zmq::socket_t left, right, parent;      // сокеты для связи с левым, правым и родительским узлами
    int left_port, right_port, parent_port; // порты соединения с левым, правым и родительским узлами
    int id, left_id = -2, right_id = -2, parent_id; // идентификаторы текущего узла, левого узла, правого узла и родительского узла

    CalculationNode(int id, int parent_port, int parent_id) : id(id),
        parent_port(parent_port),
        parent_id(parent_id),
        left(context, ZMQ_REQ), //создание сокетов с помощью контекста context и соответствующих типов (ZMQ_REQ и ZMQ_REP)
        right(context, ZMQ_REQ),
        parent(context, ZMQ_REP)
    {
        if (id != -1) // не управляющий узел
        {
            connect(parent, parent_port); // устанавливается соединение с родительским узлом через метод connect
        }
    }
    string create(int child_id)
    {
        int port;
        bool isleft = false; // создаваемый узел левый или правый

        // проверка доступности левого узла для подключения
        if (left_id == -2) // если -2, то отсутствует подключение
        {
            left_port = bind(left, child_id); // связывает сокет left с указанным идентификатором child_id и возвращает порт для соединения с левым узлом
            left_id = child_id;
            port = left_port;
            isleft = true;
        }
        else if (right_id == -2)
        {
            right_port = bind(right, child_id);
            right_id = child_id;
            port = right_port;
        }
        else
        {
            string fail = "Error: can not create the calculation node";
            return fail;
        }
        int fork_id = fork();
        if (fork_id == 0)
        {
            if (execl("./client", "client", to_string(child_id).c_str(), to_string(port).c_str(), to_string(id).c_str(), (char *)NULL) == -1)
            {
                cout << "Error: can not run the execl-command" << endl;
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            string child_pid;
            try
            {
                if (isleft)
                {
                    left.setsockopt(ZMQ_SNDTIMEO, 3000); // отправляется запрос на получение идентификатора процесса левого дочернего узла (child_id) через сокет left.
                    send_message(left, "pid");
                    child_pid = receive_message(left);
                }
                else
                {
                    right.setsockopt(ZMQ_SNDTIMEO, 3000);
                    send_message(right, "pid");
                    child_pid = receive_message(right);
                }
                return "Ok: " + child_pid;
            }
            catch (int)
            {
                string fail = "Error: can not connect to the child";
                return fail;
            }
        }
    }
    string ping(int id)

    {   
        string answer = "Ok: 0";
        try{
        
        if (this->id == id)
        {
            answer = "Ok: 1";
            return answer;
        }
        else if (left_id == id)
        {
            string message = "ping " + to_string(id);
            send_message(left, message);
            try
            {
                message = receive_message(left);
                if (message == "Ok: 1")
                {
                    answer = message;
                }
            }
            catch (int)
            {
            }
        }
        else if (right_id == id)
        {
            string message = "ping " + to_string(id);
            send_message(right, message);
            try
            {
                message = receive_message(right);
                if (message == "Ok: 1")
                {
                    answer = message;
                }
            }
            catch (int)
            {
            }
        }
        } 
        catch (int){

        }
        return answer;
    }

    string sendstring(string string, int id)
    {
        std::string answer = "Error: Parent not found";
        try{
        
        // Проверка, что нет ни левого, ни правого потомка
        if (left_id == -2 && right_id == -2)
        {
            return answer;
        }
        else if (left_id == id)
        {
            if (ping(left_id) == "Ok: 1")
            {
                send_message(left, string);
                try
                {
                    answer = receive_message(left);
                }
                catch (int)
                {
                }
            }
        }
        else if (right_id == id)
        {
            if (ping(right_id) == "Ok: 1")
            {
                send_message(right, string);
                try
                {
                    answer = receive_message(right);
                }
                catch (int)
                {
                }
            }
        }
        else
        {
            // если узел ни левый, ни правый потомок
            if (ping(left_id) == "Ok: 1") // доступен ли левый потомок
            {
                // формирование строки message, которая содержит команду "send" и идентификатор узла id, а также само сообщение string.
                std::string message = "send " + to_string(id) + " " + string;
                send_message(left, message);
                try
                {
                    message = receive_message(left);
                }
                catch (int)
                {
                    message = "Error: Parent not found";
                }
                if (message != "Error: Parent not found")
                {
                    answer = message;
                }
            }
            if (ping(right_id) == "Ok: 1")
            {
                std::string message = "send " + to_string(id) + " " + string;
                send_message(right, message);
                try
                {
                    message = receive_message(right);
                }
                catch (int)
                {
                    message = "Error: Parent not found";
                }
                if (message != "Error: Parent not found")
                {
                    answer = message;
                }
            }
        }
        }
        catch(int){}
        return answer;
    }

    string exec(const string &subcommand) // входная команда, которую нужно выполнить
    {
        if (subcommand == " start")
        {
            if (!timerRunning)
            {
                startTime = chrono::steady_clock::now();
                timerRunning = true;
                return "Ok:" + to_string(id);
            }
            else
            {
                return "Error:" + to_string(id) + ":Timer already running";
            }
        }
        else if (subcommand == " stop")
        {
            if (timerRunning)
            {
                auto endTime = chrono::steady_clock::now();
                stopTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
                timerRunning = false;
                return "Ok:" + to_string(id);
            }
            else
            {
                return "Error:" + to_string(id) + ":Timer not running";
            }
        }
        else if (subcommand == " time")
        {
            if (timerRunning)
            {
                auto currentTime = chrono::steady_clock::now();
                auto elapsedMilliseconds = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();
                return "Ok:" + to_string(id) + ":" + to_string(elapsedMilliseconds);
            }
            else
            {
                return "Ok:" + to_string(id) + ":" + to_string(stopTime.count());
            }
        }
        else
        {
            std::cout << "мда, ошибка"<< std::endl;
            return "Error:" + to_string(id) + ":" + subcommand + ": !!! Invalid subcommand";
        }
    }

    string kill()
    {
        try{
        // Завершение работы левого потомка
        if (left_id != -2)
        {
            if (ping(left_id) == "Ok: 1")
            {
                std::string message = "kill";
                send_message(left, message);
                try
                {
                    message = receive_message(left);
                }
                catch (int)
                {
                }
                unbind(left, left_port);
                left.close();
            }
        }
        if (right_id != -2)
        {
            if (ping(right_id) == "Ok: 1")
            {
                std::string message = "kill";
                send_message(right, message);
                try
                {
                    message = receive_message(right);
                }
                catch (int)
                {
                }
                unbind(right, right_port);
                right.close();
            }
        }
        }catch(int){

        }
        return to_string(parent_id);
    }
    ~CalculationNode() {
        // деструктор
    }
};