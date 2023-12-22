#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

int main() {
    const char* fifo_path = "/tmp/myfifo";  // Имя именованного канала

    // Создаем именованный канал (если он не существует)
    mkfifo(fifo_path, 0666);

    cout << "Server: Waiting for data from client...\n";

    // Открываем канал для чтения
    int fifo_fd = open(fifo_path, O_RDONLY);
    if (fifo_fd == -1) {
        perror("open");  // Выводит сообщение об ошибке, если открытие канала не удалось
        return 1;
    }

    while (true) {
         // Проверяем наличие данных в канале
        if (access(fifo_path, F_OK) == -1) {
            cout << "Server: Client closed the channel. Exiting.\n";
            break;
        }

        // Читаем данные из канала
        char buffer[256];
        ssize_t bytes_read = read(fifo_fd, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("read");  // Выводит сообщение об ошибке, если чтение данных не удалось
            return 1;
        }

        // Выводим данные из буфера (без новой строки)
        for (ssize_t i = 0; i < bytes_read; ++i) {
            if (buffer[i] == '\n') {
                cout << endl;  // Переход на новую строку при обнаружении символа '\n'
            } else {
                cout << buffer[i];  // Выводим символ из буфера
            }
        }

    }

    // Закрываем и удаляем именованный канал
    close(fifo_fd);
    unlink(fifo_path);  // Удаляет именованный канал из файловой системы

    return 0;
}




