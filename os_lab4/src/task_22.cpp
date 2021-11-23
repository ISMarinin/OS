#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <sstream>

const int MAX_LENGTH = 50;
const int NUMBER_OF_BYTES = MAX_LENGTH * sizeof(char);

using namespace std;

int main() {

    int two_path = 0;
    int one_path = 0;
    int first_pos = 0;
    int second_pos = 0;
    int first_length = 0;
    int second_length = 0;
    int fd1;
    int fd2;
    fstream fs;
    string path_child1, path_child2;

    cout << "Enter the file names: " << endl;
    cin >> path_child1 >> path_child2;

    string str;

    if ((fd1 = open("f1.txt", O_RDWR| O_CREAT, S_IRWXU)) == -1) { 
        cout << "Error: can not open the f1.txt. Try again later." << endl;
        exit(EXIT_FAILURE);
    }

    if ((fd2 = open("f2.txt", O_RDWR| O_CREAT, S_IRWXU)) == -1) {
        cout << "Error: can not open the f2.txt. Try again later." << endl;
        exit(EXIT_FAILURE);
    }

    char *mapped_file1 = (char *)mmap(0, NUMBER_OF_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0); 
    char *mapped_file2 = (char *)mmap(0, NUMBER_OF_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
    /*
        Первый аргумент — желаемый адрес начала участка отбраженной памяти
        Второй -  количество байт, которое нужно отобразить в память
        Третий - число, определяющее степень защищённости отображенного участка памяти
        Четвертый - описывает атрибуты области
        Пятый -  дескриптор файла, который нужно отобразить
        Шестой - смещение отображенного участка от начала файла
    */

    if (mapped_file1 == MAP_FAILED ||mapped_file2 == MAP_FAILED) {
        cout << "An error with mmap function one has been detected" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Enter your strings: " << endl;

    while (cin >> str) {

        str = str + "\n";

        int r = rand() % 10 + 1;
        if (r >= 2) {
            one_path++;
            first_length += str.size();
            if (ftruncate(fd1, first_length)) { // устанавливаем длину fd1 в first_length байт. При успешной работе функции возвращаемое значение равно нулю; При ошибке возвращается -1
                cout << "Error during ftrancate with mf1 has been detected" << endl;
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < str.size(); ++i) {
                mapped_file1[first_pos++] = str[i];
            };
        }

        else {
            two_path++;
            second_length += str.size();
            if (ftruncate(fd2, second_length)) {
                cout << "Error during ftrancate with mf2 has been detected" << endl;
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < str.size(); ++i) {
                mapped_file2[second_pos++] = str[i];
            }
        }
    }

    int first_identificator = fork();
    if (first_identificator == -1) {
        cout << "Fork error!" << endl;
        exit(EXIT_FAILURE);
    }

    else if (first_identificator == 0) {
        ofstream ofs (path_child1, ios::out | ios::trunc);
        fs.open(path_child1, fstream::in | fstream::out | fstream::app);
        if (!fs.is_open()) {
            exit(EXIT_FAILURE);
        }

        int i = 0;
        while (one_path > 0) {
            string str;
            while (mapped_file1[i] != '\n') {
                str += mapped_file1[i];
                i++;
            }
            if (mapped_file1[i] == '\n')
                i++;

            int j = (int)str.size() - 1;

            for (int i = 0; i < str.size() / 2; ++i) {
                char tmp = str[i];
                str[i] = str[j];
                str[j] = tmp;
                j--;
            }

            fs << str << endl;
            one_path--;
        }
    }
    else {
        int second_identificator = fork();
        if (second_identificator == -1) {
            cout << "Fork error!" << endl;
            return 4;
        }
        else if (second_identificator == 0) {
            ofstream ofs (path_child2, ios::out | ios::trunc);
            fs.open(path_child2, fstream::in | fstream::out | fstream::app);
            if (!fs.is_open()) {
                exit(EXIT_FAILURE);
            }

            int i = 0;
            while (two_path > 0) {
                string str;
                while (mapped_file2[i] != '\n') {
                    str += mapped_file2[i];
                    i++;
                }
                if (mapped_file2[i] == '\n')
                    i++;

                int j = (int)str.size() - 1;

                for (int i = 0; i < str.size() / 2; ++i) {
                    char tmp = str[i];
                    str[i] = str[j];
                    str[j] = tmp;
                    j--;
                }

                fs << str << endl;
                two_path--;
            }
        }
        else
        {
            if (munmap(mapped_file1, NUMBER_OF_BYTES) == -1) { // Проверяем отражается ли NUMBER_OF_BYTES байт, определенного файловым описателем, в память
                cout << "Munmap1 error has been dected!" << endl;
                exit(EXIT_FAILURE);
            }
            if (munmap(mapped_file2, NUMBER_OF_BYTES) == -1) {
                cout << "Munmap2 error has been dected!" << endl;
                exit(EXIT_FAILURE);
            }
            close(fd1);
            close(fd2);
            remove("f1.txt");
            remove("f2.txt");
            return 0;
        }
    }
}
