#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace std;

int main() {
    // Имя объекта разделяемой памяти
    const char* shm_name = "/myshared";
    // Размер разделяемой памяти (в байтах)
    const size_t shm_size = sizeof(char) * 256;

    // Создаем или открываем объект разделяемой памяти
    int fd_shm = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (fd_shm == -1) {
        perror("shm_open"); // Выводим сообщение об ошибке, если не удалось создать или открыть разделяемую память
        return 1;
    }

    // Устанавливаем размер разделяемой памяти
    ftruncate(fd_shm, shm_size);

    // Отображаем разделяемую память в адресное пространство процесса
    char* shared_data = static_cast<char*>(mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0));
    if (shared_data == MAP_FAILED) {
        perror("mmap"); // Выводим сообщение об ошибке, если не удалось отобразить разделяемую память
        return 1;
    }


    while (true) {
        char buffer[256];
        cin.getline(buffer, sizeof(buffer)); //вводим текст

        // Проверяем, если пользователь ввел "exit", то выходим из цикла
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Записываем его в разделяемую память
        strncpy(shared_data, buffer, shm_size);

        cout << "Writer: Writing message to shared memory: " << buffer << endl;

        sleep(1); // Ждем 1 секунду
    }

    // Освобождаем ресурсы
    munmap(shared_data, shm_size); // Освобождаем отображенную память
    close(fd_shm); // Закрываем файл разделяемой памяти
    shm_unlink(shm_name); // Удаляем объект разделяемой памяти из системы

    return 0;
}

