#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;

int main() {

    string filename1;
    string filename2;

    cout << "Enter enter the file names:" << endl;
    cin >> filename1;
    cin >> filename2;
    fstream fs;

    int fd1[2];
    pipe(fd1);
    int fd2[2];
    pipe (fd2);

    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        cout << "pipe error!" << endl;
        return 1;
    }

    int child_id = fork();
    if (child_id == -1) {
        cout << "Fork error!" << endl;
        return -1;
    }

    else if (child_id == 0) {
        fs.open(filename1, fstream :: in | fstream :: out | fstream :: app);
        int n;
        read (fd1[0], &n, sizeof(int));
        while (n > 0) {
            int size;
            read(fd1[0], &size, sizeof(int));
            char str[size];
            read(fd1[0], str, sizeof(char) * size);
            string s;
            for (int i = 0; i < size; ++i) {
                s.push_back(str[i]);
            }

            int j = (int)s.size() - 1;
            for (int i = 0; i < s.size() / 2; ++i) {
                char tmp = s[i];
                s[i] = s[j];
                s[j] = tmp;
                j--;
            }

            fs << s << endl;
            cout << "[CHILD1]" << s << endl;
            n--;
        }

        close(fd1[0]);
        close(fd1[1]);
    }

    else {

        int child_id2 = fork();

        if (child_id2 == -1) {
            cout << "Fork error!" << endl;
            return -1;
        }
        
        else if (child_id2 == 0) {
            fs.open(filename2, fstream :: in | fstream :: out | fstream :: app);
            int n;
            read(fd2[0], &n, sizeof(int));
            while (n > 0) {
                int size;
                read(fd2[0], &size, sizeof(int));
                char str[size];
                read(fd2[0], str, sizeof(char) * size);
                string s;
                
                for (int i = 0; i < size; ++i) {
                    s.push_back(str[i]);
                }

                int j = (int)s.size() - 1;

                for (int i = 0; i < s.size() / 2; ++i) {
                    char tmp = s[i];
                    s[i] = s[j];
                    s[j] = tmp;
                    j--;
                }
                fs << s << endl;
                cout << "[CHILD2] " << s << endl;
                n--;
            }
            
            close(fd2[0]);
            close(fd2[1]);
        }

        else {

            int n;
            cout << "[PARENT] Enter number of strings" << endl;
            cin >> n;
            write(fd1[1], &n, sizeof(int));
            write(fd2[1], &n, sizeof(int));
            cout << "[PARENT] Enter " << n << " string(s): " << endl;

            for (int i = 0; i < n; ++i) {
                string s1;
                cin >> s1;
                int k = s1.size();
                char str_ch[k];
                for (int i = 0; i < k; ++i) {
                    str_ch[i] = s1[i];
                }

                int r = rand() % 10 + 1;

                if (r == 1 || r == 2 || r == 3) {
                    write(fd1[1], &k, sizeof(int));
                    write(fd1[1], str_ch, sizeof(char) * k);
                }
                else {
                    write(fd2[1], &k, sizeof(int));
                    write(fd2[1], str_ch, sizeof(char) * k);
                }
            }  

            close(fd1[0]);
            close(fd1[1]);
            close(fd2[0]);
            close(fd2[1]);
        }
    }
    return 0;
}