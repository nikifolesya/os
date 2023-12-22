#include <iostream>
#include <thread>
#include <chrono>
#include <dispatch/dispatch.h>
#include <vector>

using namespace std;

const int numHouses = 3;
dispatch_semaphore_t waterSemaphore;

// Вектор для буферизации вывода каждого потока
vector<string> outputBuffer;

// Функция, представляющая подачу воды в водопровод
void waterSupply() {
    outputBuffer.push_back("Водопровод: начал подачу воды");

    // Имитируем подачу воды в течение некоторого времени
    this_thread::sleep_for(chrono::seconds(5));

    outputBuffer.push_back("Водопровод: завершил подачу воды");

    // Уведомляем, что подача воды завершена
    for (int i = 0; i < numHouses; ++i) {
        dispatch_semaphore_signal(waterSemaphore);
    }
}

// Функция, представляющая потребление воды в доме
void waterConsumption(int id) {
    outputBuffer.push_back("Дом " + to_string(id) + ": начал потребление воды");

    // Ждем, пока водопровод завершит подачу воды
    dispatch_semaphore_wait(waterSemaphore, DISPATCH_TIME_FOREVER);

    // Имитируем потребление воды в доме
    this_thread::sleep_for(chrono::seconds(3));

    outputBuffer.push_back("Дом " + to_string(id) + ": завершил потребление воды");
}

int main() {
    // Инициализируем семафор с начальным значением 0
    waterSemaphore = dispatch_semaphore_create(0);

    thread waterSupplyThread(waterSupply);

    // Запускаем потоки-потребители воды (дома)
    thread houseThreads[numHouses];
    for (int i = 0; i < numHouses; ++i) {
        houseThreads[i] = thread(waterConsumption, i + 1);
    }

    // Ожидаем завершения потока подачи воды
    waterSupplyThread.join();

    // Ожидаем завершения потоков-потребителей (домов)
    for (int i = 0; i < numHouses; ++i) {
        houseThreads[i].join();
    }

    // Выводим буферизированные данные
    for (const auto& message : outputBuffer) {
        cout << message << endl;
    }

    cout << "Программа завершена!" << endl;

    // Уничтожаем семафор
    dispatch_release(waterSemaphore);

    return 0;
}

