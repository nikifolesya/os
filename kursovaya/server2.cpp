#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <mach/mach.h>
#include <sys/resource.h>

using namespace std;

mutex serverMutex;  // Мьютекс для обеспечения безопасности работы с ресурсами

// Функция для получения процента свободной физической памяти
float getFreeMemoryPercentage() {
    mach_port_t hostPort;
    mach_msg_type_number_t hostSize;
    vm_size_t pageSize;

    // Инициализация переменных для получения информации о памяти
    hostPort = mach_host_self();
    hostSize = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    host_page_size(hostPort, &pageSize);

    vm_statistics_data_t vmStats;
    if (host_statistics(hostPort, HOST_VM_INFO, (host_info_t)&vmStats, &hostSize) != KERN_SUCCESS) {
        return -1; // Ошибка при получении информации о памяти
    }

    natural_t freeMemory = vmStats.free_count * pageSize;
    natural_t totalMemory = (vmStats.active_count + vmStats.inactive_count + vmStats.wire_count + vmStats.free_count) * pageSize;

    if (totalMemory > 0) {
        return (freeMemory / static_cast<float>(totalMemory)) * 100.0;
    }

    return -1; // Ошибка, totalMemory не больше 0
}

// Функция для обработки клиентского запроса
void handleClient(int clientSocket) {
    // Получение информации о проценте свободной физической памяти
    string freeMemoryPercentage;
    if (getFreeMemoryPercentage() >= 0) {
        freeMemoryPercentage = to_string(getFreeMemoryPercentage()) + "%";
    } else {
        freeMemoryPercentage = "Error getting memory information.";
    }

    // Получение информации о времени работы серверного процесса в пользовательском режиме
    struct rusage resourceUsage;

    string modeTime;
    if (getrusage(RUSAGE_SELF, &resourceUsage) == 0) {
        // Время работы в пользовательском режиме в секундах и микросекундах
        long seconds = resourceUsage.ru_utime.tv_sec;
        long microseconds = resourceUsage.ru_utime.tv_usec;

        modeTime = to_string(seconds) + " seconds, " + to_string(microseconds) + " microseconds";
    } else {
        modeTime = "Error getting resource usage.";
    }

    // Формирование ответа для клиента
    string response = "Free memory percentage: " + freeMemoryPercentage + "\nUser time: " + modeTime;

    {
        lock_guard<mutex> lock(serverMutex); // Защита доступа к ресурсам
        // Отправка ответа клиенту
        send(clientSocket, response.c_str(), response.length(), 0);
    }

    // Освобождение ресурсов
    close(clientSocket);
}

int main() {
    // Создание сокета для сервера
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        return -1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8082);  // Порт для сервера 2

    // Привязка сокета к адресу и порту
    if (::bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error binding socket");
        close(serverSocket);
        return -1;
    }

    // Ожидание подключений
    if (listen(serverSocket, 10) == -1) {
        perror("Error listening on socket");
        close(serverSocket);
        return -1;
    }

    cout << "Server2 is listening on port 8082...\n";

    // Бесконечный цикл для обработки клиентских запросов
    while (true) {
        sockaddr_in clientAddress{};
        socklen_t clientAddressLen = sizeof(clientAddress);

        // Принятие подключения от клиента
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLen);
        if (clientSocket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Запуск нового потока для обработки клиентского запроса
        thread(handleClient, clientSocket).detach();
    }

    // Закрытие серверного сокета
    close(serverSocket);
    return 0;
}
