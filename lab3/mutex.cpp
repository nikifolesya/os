#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mutex>

using namespace std;

pthread_mutex_t relayMutex = PTHREAD_MUTEX_INITIALIZER;
int runner = 1; //Переменная, представляющая номер текущего человека в эстафете.

void* relayRace(void* arg) {
    pthread_mutex_lock(&relayMutex); //Блокирует мьютекс перед доступом к общим данным.

    int runnerNumber = *((int*)arg);

    cout << "Дом " << runnerNumber << ": начал потребление воды" << endl;
    sleep(1);
    cout << "Дом " << runnerNumber << " завершил потребление воды" << endl;
    runner++;

    pthread_mutex_unlock(&relayMutex); //Освобождает мьютекс после обработки данных.

    return NULL;
}

int main() {
    pthread_t thread1, thread2, thread3;
    int runners[] = {1, 2, 3};

    cout << "Водопровод: начал подачу воды" << endl;

    //Создает потоки для каждого участника эстафеты.
    pthread_create(&thread1, NULL, relayRace, &runners[0]);
    sleep(1);

    pthread_create(&thread2, NULL, relayRace, &runners[1]);
    sleep(1);

    pthread_create(&thread3, NULL, relayRace, &runners[2]);

    //Ожидает завершения выполнения каждого потока.
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    cout << "Водопровод: закончил подачу воды" << endl;

    return 0;
}

