#include <iostream>
#include <unistd.h>   //системная информация в Linux-системах
#include <sys/utsname.h>   //для системных метрик
#include <time.h>   //системное время для Linux
#include <ncurses.h>   //системные цвета

using namespace std;

//получает и выводит имя компьютера и имя пользователя, используя функции
int sysInfo() {
    char hostname[256];
    char username[256];
    gethostname(hostname, sizeof(hostname));
    getlogin_r(username, sizeof(username));
    cout << "Имя компьютера: " << hostname << endl;
    cout << "Имя пользователя: " << username << endl;
    return 0;
}

//получает и выводит информацию о версии операционной системы с использованием структуры
int versionOS() {
    struct utsname unameInfo;
    if (uname(&unameInfo) == 0) { 
        cout << "Версия операционной системы: " << unameInfo.sysname << " " << unameInfo.release << endl;
    }
    return 0;
}

//получает и выводит некоторые системные метрики, такие как максимальное число открытых файлов, 
//максимальное число потоков и максимальная длина имени хоста
int sysMetrics() {
    long maxOpenFiles = sysconf(_SC_OPEN_MAX); //The maximum number of files that a process can have open at any time.
    long maxOpenStreams = sysconf(_SC_STREAM_MAX); //The maximum number of streams that a process can have open at any time.
    long maxHostName = sysconf(_SC_HOST_NAME_MAX); //Maximum length of a hostname.

    cout << "Максимальное число открытых файлов: " << maxOpenFiles << endl;
    cout << "Максимальное число потоков: " << maxOpenStreams << endl;
    cout << "Максимальная длина имени хоста: " << maxHostName << endl;

    return 0;
}

int getTime() {
    time_t currentTime = time(NULL);
    struct tm *date = localtime(&currentTime);

    cout << "Текущие дата и время: " << ctime(&currentTime);
    cout << "Дата: " << date->tm_mday << "." << date->tm_mon + 1 << "." << date->tm_year + 1900 << endl;
    cout << "Время: " << date->tm_hour << ":" << date->tm_min << ":" << date->tm_sec << endl;
    return 0;

}

//функция выполняет дополнительные операции, такие как получение текущего рабочего каталога с помощью getcwd(), 
//получение информации о версии операционной системы и количестве процессоров, а также размере страницы памяти.
int additionalAPIFunction () {
    char currentDir[256];
    getcwd(currentDir, sizeof(currentDir));

    struct utsname unameInfo;
    if (uname(&unameInfo) == 0) {
        cout << "Архитектура: " << unameInfo.machine << endl;
    }

    long pageSize = sysconf(_SC_PAGESIZE); // Размер страницы памяти

    long processors = sysconf(_SC_NPROCESSORS_ONLN);

    cout << "Текущий каталог: " << currentDir << endl;
    cout << "Количество процессоров: " << processors << endl;
    cout << "Размер страницы памяти: " << pageSize << " байт" << endl;
    
    return 0;
}

int main() {
    sysInfo();
    versionOS();
    sysMetrics();
    getTime();
    additionalAPIFunction();

    return 0;
}