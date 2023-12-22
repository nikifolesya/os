#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

int main() {
    const char* fifo_path = "/tmp/myfifo";  // Имя именованного канала

    // Открываем или создаем именованный канал для записи
    int fifo_fd = open(fifo_path, O_WRONLY);
    if (fifo_fd == -1) {
        perror("open");  // Выводит сообщение об ошибке, если открытие канала не удалось
        return 1;
    }

    cout << "Client: Enter data to send to server (enter 'exit' to quit):\n";

    while (true) {
        // Читаем данные с консоли (стандартного ввода)
        char buffer[256];
        cin.getline(buffer, sizeof(buffer));

        // Проверяем, если пользователь ввел "exit", то выходим из цикла
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Записываем данные в именованный канал, включая новую строку в конце
        ssize_t bytes_written = write(fifo_fd, buffer, strlen(buffer));
        if (bytes_written == -1) {
            perror("write");  // Выводит сообщение об ошибке, если запись в канал не удалась
            return 1;
        }

        // Добавляем новую строку в качестве разделителя сообщений
        write(fifo_fd, "\n", 1);

        cout << "Client: Data sent to server.\n";
    }

    // Закрываем именованный канал
    close(fifo_fd);
    unlink(fifo_path);  // Удаляет именованный канал из файловой системы

    return 0;
}



