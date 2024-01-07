#pragma once
#include <iostream>
#include <string> 
class Tree {
        class TreeNode {
            public:
                int id; //идентификатор узла
                TreeNode* left; //указатель на левый дочерний узел
                TreeNode* right;
                int height; //высота узла в дереве
                bool available; //флаг доступности узла

                TreeNode (int id) { //init узла
                    this->id = id;
                    available = true;
                    left = NULL;
                    right = NULL;
                }

                void CheckAvailability (int id) {
                    if (this->id == id){
                        available = false; // если id совпадает с узлом, на котором проходит проверка, то это узел недоступен
                    }
                    else {
                        if (left != NULL) {
                            left->CheckAvailability(id);
                        }
                        if (right != NULL) {
                            right->CheckAvailability(id);
                        }
                    }
                }
                void Remove (int id, set<int> &ids) { // узла; множество идентификаторов всех узлов
                    if (left != NULL && left->id == id) {
                        left->RecursionRemove(ids); //удаление всех потомков левого потомка
                        ids.erase(left->id); // удаляется идентификатор левого потомка
                        delete left; //освобождается память, выделенная для узла левого потомка
                        left = NULL;
                    }
                    else if (right != NULL && right->id == id) {
                        right->RecursionRemove(ids);
                        ids.erase(right->id);
                        delete right;
                        right = NULL;
                    }
                    else { // если не совпали ранее
                        if (left != NULL) {
                            left->Remove(id, ids);
                        }
                        if (right != NULL) {
                            right->Remove(id, ids);
                        }
                    }
                }
                void RecursionRemove (set<int> &ids) {
                    if (left != NULL) {
                        left->RecursionRemove(ids); // удаляем всех левых
                        ids.erase(left->id);
                        delete left;
                        left = NULL;
                    }
                    if (right != NULL) {
                        right->RecursionRemove(ids); // удаляем всех правых
                        ids.erase(right->id);
                        delete right;
                        right = NULL;
                    }
                }
                void AddInNode (int id, int parent_id, set<int> &ids) { // id - кого добавляем, parent_id - куда добавляем, set - все узлы
                    if (this->id == parent_id) {
                        if (left == NULL){
                            left = new TreeNode(id);
                        }
                        else {
                            right = new TreeNode(id);
                        }
                        ids.insert(id); // добавляем в id множество всех
                    }
                    else {
                        if (left != NULL) {
                            left->AddInNode(id, parent_id, ids); // ведем поиск parent_id
                        }
                        if (right != nullptr) {
                            right->AddInNode(id, parent_id, ids);
                        }
                    }
                }
                int MinimalHeight() {
                    // Проверяется, существуют ли левый и правый потомки.
                    // Если хотя бы один из них отсутствует (равен NULL), то это значит, что текущий узел является листом и его высота равна 0. 
                    // В этом случае метод возвращает 0.
                    if (left == NULL || right == NULL) { //
                        return 0;
                    }
                    int left_height = -1;
                    int right_height = -1;
                    if (left != NULL && left->available == true) {
                        left_height = left->MinimalHeight(); // если узел существует и доступен, то считаем высоту
                    }
                    if (right != NULL && right->available == true) {
                        right_height = right->MinimalHeight(); 
                    }
                    if (right_height == -1 && left_height == -1) {
                        available = false; // если высота не вычислена, то узел является недоступным
                        return -1;
                    }
                    else if (right_height  == -1) {
                        return left_height + 1;
                    }
                    else if (left_height == -1) {
                        return right_height + 1;
                    }
                    else {
                        return min(left_height, right_height) + 1;
                    }
                }

                int IDMinimalHeight(int height, int current_height) { //поиск идентификатора узла с заданной высотой
                    if (height < current_height) {
                        return -2;
                    }
                    else if (height > current_height) {
                        int current_id = -2;
                        if (left != NULL && left->available == true) {
                            current_id = left->IDMinimalHeight(height, (current_height + 1));
                        }
                        if (right != NULL && right->available == true && current_id == -2){
                            current_id = right->IDMinimalHeight(height, (current_height + 1));
                        }
                        return current_id;
                    }
                    else {
                        if (left == NULL || right == NULL){
                            return id;
                        }
                        return -2;
                    }
                }
                ~TreeNode() {}
        };
    private:
            TreeNode* root; // корневой узел дерева
    public:
        set<int> ids;
        Tree() {
            root = new TreeNode(-1); // корневой узел становися -1, init
        } 
        bool Exist(int id) {
            if (ids.find(id) != ids.end()) { // поиск во множестве
                return true;
            }
            return false;
        }
        void AvailabilityCheck(int id) { // проверка доступности узла
            root->CheckAvailability(id);
        }
        int FindID() {
            int h = root->MinimalHeight(); // сюда будем добавлять
            return root->IDMinimalHeight(h, 0); // находит идентификатор узла с минимальной высотой в дереве
        }
        void AddInTree(int id, int parent) { // добавляет новый узел с заданным идентификатором
            root->AddInNode(id, parent, ids);
        }
        void RemoveFromRoot(int idElem) { //удаляет узел
            root->Remove(idElem, ids);
        }
        ~Tree() {
            root->RecursionRemove(ids); //деструктор, рекурсивно удаляет все
            delete root;
        }
};