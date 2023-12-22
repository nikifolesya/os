#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

#define SOCK_NAME "/tmp/mysocket3"

using namespace std;

int main() {
    // Создаем дейтаграммный сокет
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    sockaddr_un srvr_name;
    srvr_name.sun_family = AF_UNIX;
    strcpy(srvr_name.sun_path, SOCK_NAME);

    //привязка сокета к адресу. В случае ошибки, выводится сообщение об ошибке и закрытие сокета
    if (::bind(sock, reinterpret_cast<struct sockaddr*>(&srvr_name), sizeof(srvr_name)) == -1) {
        perror("bind");
        close(sock);
        return 1;
    }

    cout << "Server: Waiting for data from client...\n";

    char buf[256];
    sockaddr_un rcvr_name;
    sockaddr_un client_name;
    socklen_t namelen = sizeof(client_name);

    while (true) {
         // Проверяем наличие данных в канале
        if (access(SOCK_NAME, F_OK) == -1) {
            cout << "Server: Client closed the channel. Exiting.\n";
            break;
        }

        // Читаем данные из сокета
        ssize_t bytes = recvfrom(sock, buf, sizeof(buf), 0, reinterpret_cast<struct sockaddr*>(&client_name), &namelen);
        if (bytes == -1) {
            perror("recvfrom");
            break;
        }

        // Выводим данные
        cout << "Server: Received data from client: " << buf << endl;

        // Опционально: отправляем ответ обратно клиенту
        sendto(sock, buf, bytes, 0, reinterpret_cast<struct sockaddr*>(&rcvr_name), namelen);
    }

    // Закрываем сокет и удаляем файл сокета
    close(sock);
    unlink(SOCK_NAME);

    return 0;
}
