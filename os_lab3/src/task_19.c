#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

char* sieve;
int num;

void* SieveStep(void* i_void){
    int i = *(int*)i_void;
    for (int j = i + i; j <= num; j += i) {
        sieve[j] = 1; 
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc!= 3) {
        printf("Sitax: ./a.out Threads_num Num_for_test\n");
        return 0;
    }

    int threads_num = atoi(argv[1]);
    num = atoi(argv[2]);

    pthread_t* threads = (pthread_t*)calloc(threads_num, sizeof(pthread_t));

    // массив для аргументов, которые будут переданы в функцию потока
    if (threads == NULL) {
        printf("Can't create an array for arguments for threads\n");
        exit(EXIT_FAILURE);
    }

    // массив для аргументов, которые будут переданы в функцию потока
    int *args = (int*)malloc(threads_num * sizeof(int));

    // создание массива, заполненного 0 для сита
    // 0 - простое число, 1 - не простое число
    sieve = (char*)calloc((num + 1), sizeof(char));
    if (sieve == NULL) {
        printf("Can't create an array for sieve\n");
        exit(EXIT_FAILURE);
    }

    // маркировка чисел, которые не являются простыми по определению
    sieve[0] = 1;
    sieve[1] = 1;

    int cur_thread = 0; // id текущего потока
    for (int i = 2; i * i <= num; ++i) {
        if (sieve[i] == 1) continue; // пропуск не простых чисел
        if (cur_thread >= threads_num) { 
            pthread_join(threads[cur_thread % threads_num] ,NULL);
        }

        args[cur_thread % threads_num] = i; // копирование аргумента для функции потока в специальный массив
        pthread_create(&threads[cur_thread % threads_num], NULL, SieveStep, &args[cur_thread % threads_num]);
        ++cur_thread;
    }

    for (int i = 0; i < threads_num; ++i) {
        pthread_join(threads[i], NULL);
    }

    if (sieve[num] == 1) 
        printf("%d is not a prime number\n", num);
    else
        printf("%d is a prime number\n", num);

    free(sieve);
    free(threads);
    free(args);
}