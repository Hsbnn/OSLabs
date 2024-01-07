#pragma once
#include <iostream>
#include <string> 
#include <zmq.hpp>

using namespace std;

void send_message(zmq::socket_t &socket, const string &message){

    zmq::message_t m(message.size());
    memcpy(m.data(), message.c_str(), message.size()); // копирует данные из строки message в буфер сообщения m
    socket.send(m);
    // отправляет сообщение из объекта m через заданный сокет
}

string receive_message(zmq::socket_t &socket){
    zmq::message_t message;
    int chars_read; // количество прочитанных символов из сообщения
    try {
        chars_read = (int)socket.recv(&message); // прием сообщения
    }
    catch (...) {
        chars_read = 0;
    }
    
    if (chars_read == 0) {
        throw -1;
    }
    string received_msg(static_cast<char*>(message.data()), message.size()); 

    // конструктора, который принимает указатель char* на данные сообщения (message.data()) и его размер (message.size())
    return received_msg;

}

int bind(zmq::socket_t &socket, int id){

    int port = 1024 + id;
    string address = "tcp://127.0.0.1:" + to_string(port);
    while(1){
        try{
            socket.bind(address); // попытка привязать сокет к порту
            break;
        }
        catch(...){
            port++; // попытка привязать сокет к порту на 1 больше, чем предыдущий в цикле
        }
    }
    return port;

}

void unbind(zmq::socket_t &socket, int port){

    string address = "tcp://127.0.0.1:" + to_string(port);
    socket.unbind(address);

}

void connect(zmq::socket_t &socket, int port){

    string address = "tcp://127.0.0.1:" + to_string(port); // формирование адреса (строка)
    socket.connect(address); // установка соединения

}

void disconnect(zmq::socket_t &socket, int port){

    string address = "tcp://127.0.0.1:" + to_string(port);
    socket.disconnect(address);

}
