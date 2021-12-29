#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

int main (){
    int pipe_AC[2];
    int pipe_AB[2];
    int pipe_CA[2];
    int pipe_CB[2];

    pipe(pipe_AC);
    pipe(pipe_AB);
    pipe(pipe_CA);
    pipe(pipe_CB);

    pid_t id_C = fork();
    if (id_C == -1){
        cout << "Fork error!" << endl;
        return -1;
    }
    else if (id_C == 0){ // program_C
            char AC[32];
            char CA[32];
            char CB[32];

            sprintf(AC, "%d", pipe_AC[0]);
            sprintf(CA, "%d", pipe_CA[1]);
            sprintf(CB, "%d", pipe_CB[1]);
            
            execl("./C", AC, CA, CB, (char*)(NULL));
    } // program_C end
    else {
        pid_t id_B = fork();
        if (id_B == -1){
            cout << "Fork error!" << endl;
            return -1;
        }
        else if (id_B == 0){ // program_B
            char AB[32];
            char CB[32];
            sprintf(AB, "%d", pipe_AB[0]);
            sprintf(CB, "%d", pipe_CB[0]);
            execl("./B", AB, CB, (char*)(NULL));
        } // program_B end
        else { // program_A
            string Str;
            while (true){
                cin >> Str;
                if(!cin.good()) break;
                size_t Sended_char_count = Str.size();
                uint8_t confirm;

                write(pipe_AB[1], &Sended_char_count, sizeof(size_t));
                write(pipe_AC[1], &Sended_char_count, sizeof(size_t));
                write(pipe_AC[1], Str.c_str(), Sended_char_count);

                read(pipe_CA[0], &confirm, sizeof(uint8_t));
                
            }
        } // program_A end 
    }
    return 0;
}