#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

#define SOCK_NAME "/tmp/mysocket3" //определяем сокет

using namespace std;

int main() {
    // Создаем дейтаграммный сокет
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    //инициализация структуры sockaddr_un для указания адреса сервера
    sockaddr_un srvr_name;
    srvr_name.sun_family = AF_UNIX;
    strcpy(srvr_name.sun_path, SOCK_NAME);

    cout << "Client: Enter 'exit' to end the conversation." << endl;

    while (true) {
        // Запрашиваем у пользователя ввод сообщения
        cout << "Client: Enter a message: ";
        string message;
        getline(cin, message);

        // Отправляем сообщение серверу
        sendto(sock, message.c_str(), message.size(), 0, reinterpret_cast<sockaddr*>(&srvr_name), sizeof(srvr_name));

        // Проверяем, введена ли команда 'exit' для завершения цикла
        if (message == "exit") {
            break;
        }
    }

    // Закрываем сокет
    close(sock);
    unlink(SOCK_NAME);

    return 0;
}


