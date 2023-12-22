#include <iostream>
#include <ctime>
#include <sys/wait.h>  //ожидание завершения дочернего процесса или потока
#include <cstdlib>
#include <sys/types.h>  //информация о типах данных, используемых в ос
#include <sys/utsname.h>  //для системных метрик
#include <unistd.h>  //системная информация в Linux-системах
#include <pthread.h>  //создавать и управлять потоками в программе

using namespace std;


//Созданный процесс называется дочерним процессом, а процесс, осуществивший вызов fork(), называется родительским.
//после вызова родительский процесс и его вновь созданный потомок выполняются одновременно
int main() {
    pid_t pid1, pid2;

    pid1 = fork(); // Создание первого дочернего процесса
    
    /*
    Если создание процесса не удалось, выводится ошибка, 
    в противном случае внутри дочернего процесса выполняется код, который выводит информацию
    */
    if (pid1 < 0) {
        perror("Ошибка при создании первого дочернего процесса");
        exit(1);
    }


    if (pid1 == 0) {
        // Код первого дочернего процесса
        time_t current_time;
        struct tm* time_info;
        time(&current_time);
        time_info = localtime(&current_time);
        cout << "Дочерний процесс 1: PID=" << getpid() << ", PPID=" << getppid() 
                << ", Время: " << time_info->tm_hour << ":" << time_info->tm_min << ":" << time_info->tm_sec << endl;

        // Вывод списка смонтированных файловых систем
        cout << "Список смонтированных файловых систем:" << endl;
        system("mount");
        exit(0);
    }

    pid2 = fork(); // Создание второго дочернего процесса

    if (pid2 < 0) {
        perror("Ошибка при создании второго дочернего процесса");
        exit(1);
    }

    //создает два дочерних потока
    //каждый поток выполняет функцию thread_function, и после завершения потоков дочерний процесс завершает свою работу

    if (pid2 == 0) {
        // Код второго дочернего процесса
        time_t current_time;
        struct tm* time_info;
        time(&current_time);
        time_info = localtime(&current_time);
        cout << "Дочерний процесс 2: PID=" << getpid() << ", PPID=" << getppid() 
                << ", Время: " << time_info->tm_hour << ":" << time_info->tm_min << ":" << time_info->tm_sec << endl;
        
        // Замещение второго дочернего процесса задачей, создающей два дочерних потока
        
        execl("/Users/nikiforova.olesya/Desktop/ОС/output/threads", "threads", (char*)NULL);
        perror("execl");

        exit(0);
    }
    // Код родительского процесса
    time_t current_time;
    struct tm* time_info;
    time(&current_time);
    time_info = localtime(&current_time);
    cout << "Родительский процесс: PID=" << getpid() 
            << ", Время: " << time_info->tm_hour << ":" << time_info->tm_min << ":" << time_info->tm_sec << endl;

    //Если pid1 и pid2 не равны 0, то код выполняется в родительском процессе
    //в этом случае он выводит информацию о родительском процессе и использует system("ps -x"), чтобы вывести список процессов

    // Использование system() для выполнения команды ps -x
    cout << "Список процессов с помощью 'ps -x':" << endl;
    system("ps -x");

    // Ожидание завершения обоих дочерних процессов
    wait(NULL);
    wait(NULL);

    return 0;
}




