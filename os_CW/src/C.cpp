#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

int main (int argc, char* argv[]){
    int pipe_AC = atoi(argv[0]);
    int pipe_CA = atoi(argv[1]);
    int pipe_CB = atoi(argv[2]);

    size_t Sended_char_count;

    while (read(pipe_AC, &Sended_char_count, sizeof(size_t)) > 0){
        char char_str[Sended_char_count];
        read(pipe_AC, char_str, Sended_char_count);
        string Str(char_str, Sended_char_count);
        cout << "C: string from program A: " << Str << endl;

        size_t Received_char_count = Str.size();
        write(pipe_CB, &Received_char_count, sizeof(size_t));

        uint8_t confirm = 1;
        write(pipe_CA, &confirm, sizeof(uint8_t));
    }
}