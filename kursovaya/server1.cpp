#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>
#include <sys/sysctl.h>

using namespace std;

mutex serverMutex; // Мьютекс для обеспечения безопасности работы с ресурсами

// Функция для получения времени работы компьютера в секундах
time_t getSystemUptime() {
    struct timeval boottime;
    size_t size = sizeof(boottime);
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};

    // Получение времени загрузки системы
    if (sysctl(mib, 2, &boottime, &size, nullptr, 0) != 0) {
        perror("Error getting system uptime");
        return -1;
    }

    return time(nullptr) - boottime.tv_sec;
}

// Функция для обработки клиентского запроса
void handleClient(int clientSocket) {
    // Получение времени работы компьютера в секундах
    time_t uptime = getSystemUptime();
    
    // Преобразование времени в дни, часы, минуты и секунды
    int days = static_cast<int>(uptime / (60 * 60 * 24));
    int hours = static_cast<int>((uptime % (60 * 60 * 24)) / (60 * 60));
    int minutes = static_cast<int>((uptime % (60 * 60)) / 60);
    int seconds = static_cast<int>(uptime % 60);

    // Формирование строки с продолжительностью сеанса работы
    string sessionDuration = to_string(days) + " days, " + to_string(hours) + " hours, " + to_string(minutes) + " minutes, " + to_string(seconds) + " seconds";
    
    // Получение текущего времени с учетом часового пояса
    auto now = chrono::system_clock::now();

    time_t currentTime = chrono::system_clock::to_time_t(now);
    tm* localTime = localtime(&currentTime);

    char timeZone[256];
    strftime(timeZone, sizeof(timeZone), "%c %Z", localtime(&currentTime));

    // Формирование ответа для клиента
    string response = "Session Duration: " + sessionDuration + "\nCurrent time with timezone: " + timeZone;

    {
        lock_guard<mutex> lock(serverMutex); // Защита доступа к ресурсам
        // Отправка ответа клиенту
        send(clientSocket, response.c_str(), response.length(), 0);
    }

    // Закрытие сокета после отправки ответа
    close(clientSocket);
}

// Главная функция
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
    serverAddress.sin_port = htons(8081);  // Порт для сервера 1

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

    cout << "Server1 is listening on port 8081...\n";

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
