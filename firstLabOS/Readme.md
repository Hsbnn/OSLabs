## Первая лабораторная работа

![image](https://github.com/Hsbnn/OSLabs/assets/116022974/541c341f-48e7-44bc-bc49-698fb404b072)

### Задание. Вариант 17

Родительский процесс создает два дочерних процесса. Первой строкой пользователь в консоль родительского процесса вводит имя файла, которое будет использовано для открытия File с таким
именем на запись для child1. Аналогично для второй строки и процесса child2. Родительский процесс принимает от пользователя строки произвольной длины и пересылает их в pipe1 или в pipe2 в зависимости от правила фильтрации. Процесс child1 и child2 производят работу над строками. Процессы пишут результаты своей работы в стандартный вывод.

Правило фильтрации: строки длины больше 10 символов отправляются в pipe2, иначе
в pipe1. Дочерние процессы удаляют все гласные из строк