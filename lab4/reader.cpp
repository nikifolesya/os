#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdlib>
#include <dispatch/dispatch.h>

using namespace std;

int main() {
    // Имя объекта разделяемой памяти
    const char* shm_name = "/myshared";
    // Размер разделяемой памяти (в байтах)
    const size_t shm_size = sizeof(char) * 256;

    // Открываем существующий объект разделяемой памяти
    int fd_shm = shm_open(shm_name, O_RDONLY, 0666);
    if (fd_shm == -1) {
        perror("shm_open"); // Выводим сообщение об ошибке, если не удалось открыть разделяемую память
        return 1;
    }

    // Отображаем разделяемую память в адресное пространство процесса
    char* shared_data = static_cast<char*>(mmap(nullptr, shm_size, PROT_READ, MAP_SHARED, fd_shm, 0));
    if (shared_data == MAP_FAILED) {
        perror("mmap"); // Выводим сообщение об ошибке, если не удалось отобразить разделяемую память
        return 1;
    }


    while (true) {

        // Читаем случайное число из разделяемой памяти и выводим его
        cout << "Reader: Reading data from shared memory: " << shared_data << endl;

        sleep(1); // Пауза на 1 секунду
    }

    // Освобождаем ресурсы
    if (munmap(shared_data, shm_size) == -1) {
        perror("munmap"); // Выводим сообщение об ошибке, если не удалось освободить отображенную память
    }
    if (close(fd_shm) == -1) {
        perror("close"); // Выводим сообщение об ошибке, если не удалось закрыть файл разделяемой памяти
    }

    return 0;
}
