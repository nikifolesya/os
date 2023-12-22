#include <iostream>
#include <ctime>
#include <sys/wait.h>  //ожидание завершения дочернего процесса или потока
#include <cstdlib>
#include <sys/types.h>  //информация о типах данных, используемых в ос
#include <sys/utsname.h>  //для системных метрик
#include <unistd.h>  //системная информация в Linux-системах
#include <pthread.h>  //создавать и управлять потоками в программе

using namespace std;

//каждый поток будет выводить свой собственный идентификатор, идентификатор процесса, и текущее время
void* thread_function(void* arg) {
    int thread_id = *((int*)arg);
    time_t current_time;
    struct tm* time_info;
    time(&current_time);
    time_info = localtime(&current_time);
    cout << "Поток " << thread_id << ": ID=" << pthread_self() << ", PID=" << getpid() << ", PPID=" << getppid() 
            << ", Время: " << time_info->tm_hour << ":" << time_info->tm_min << ":" << time_info->tm_sec << endl;
    return NULL;
}

int main () {
    int thread_ids[2]; //идентификаторы потоков
    pthread_t threads[2];  //информация о созданных потоках

    for (int i = 0; i < 2; i++) {
        thread_ids[i] = i + 1;  //присваивается уникальный идентификатор
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]); //создается новый поток 
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL); //блокирует родительский поток до тех пор, пока соответствующий поток, не завершит свою работу
    }

    return 0;
}