#include <stdio.h>
#include <dlfcn.h>
#include "dynamic_lib.h"

int main() {
    void* handle = dlopen("libdynn.so", RTLD_LAZY); 
    // загружаем динамическую библиотеку libdynn.so, и возвращаем прямой указатель на начало динамической библиотеки
    // RTLD_LAZY подразумевает разрешение неопределенных символов в виде кода, содержащегося в исполняемой динамической библиотеке
    
    if(handle == NULL) {
		printf("%s\n",dlerror());
		return 1;
	}

	float (*Square1) (float a, float b) = dlsym(handle, "Square1"); // Вызываем функцию Square1
	float (*Square2) (float a, float b) = dlsym(handle, "Square2");

    int c;
    float a, b;

    while(scanf("%d", &c)) {
        if (c == 1) {
            scanf("%f %f", &a, &b);
            printf("[1]: Square = %f\n", Square1(a, b));
        }
        else if (c == 2) {
            scanf("%f %f", &a, &b);
            printf("[2]: Square = %f\n", Square2(a, b));
        }
        else {
            printf("ERROR\n");
            break;
        }
    }    
    
    dlclose(handle);
    return 0;
}