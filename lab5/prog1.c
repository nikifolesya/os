#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

// Функция для сохранения карты памяти в файл
void saveMemoryMapToFile(pid_t processId, const char* fileName) {
    // Строка для хранения команды
    char command[50];

    // Составление команды для выполнения внешней программы "vmmap"
    snprintf(command, sizeof(command), "vmmap %d > %s", processId, fileName);

    // Выполнение команды
    system(command);
}

int main(int argc, char *argv[], char *envp[]) {
    // Проверка количества аргументов
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <идентификатор_процесса> <имя_файла>\n", argv[0]);
        return 1;
    }

    // Парсинг идентификатора процесса из аргумента командной строки
    pid_t processId = atoi(argv[1]);

    // Проверка существования выходного файла
    const char* fileName = argv[2];
    struct stat fileStat;

    // Получение информации о файле
    int fileExists = stat(fileName, &fileStat);

    if (fileExists == 0) {
        // Файл существует, спросить пользователя, заменить или переименовать
        char choice;
        printf("Файл %s уже существует. Вы хотите заменить его (1) или переименовать новый файл (2)? ", fileName);

        // Считывание выбора пользователя
        scanf(" %c", &choice);

        if (choice == '1') {
            // Заменить существующий файл
            FILE *output_file = fopen(fileName, "w");
            if (output_file == NULL) {
                perror("Failed to open output file");
                return 1;
            }

            // Сохранение карты памяти в файл
            saveMemoryMapToFile(processId, fileName);

            // Закрыть файл после использования
            if (fclose(output_file) != 0) {
                perror("Failed to close output file");
                return 1;
            }

            printf("Карта памяти сохранена в %s\n", fileName);
        } else if (choice == '2') {
            // Переименовать новый файл
            char newName[50];
            printf("Введите новое имя файла: ");
            
            // Считывание нового имени файла от пользователя
            scanf("%s", newName);

            FILE *output_file = fopen(newName, "w");
            if (output_file == NULL) {
                perror("Failed to open output file");
                return 1;
            }

            // Сохранение карты памяти в файл с новым именем
            saveMemoryMapToFile(processId, newName);

            // Закрыть файл после использования
            if (fclose(output_file) != 0) {
                perror("Failed to close output file");
                return 1;
            }

            printf("Карта памяти сохранена в %s\n", newName);
        } else {
            fprintf(stderr, "Неверный выбор. Завершение без сохранения.\n");
        }
    } else {
        // Файл не существует, сохранить карту памяти
        FILE *output_file = fopen(fileName, "w");
        if (output_file == NULL) {
            perror("Failed to open output file");
            return 1;
        }

        // Сохранение карты памяти в новый файл
        saveMemoryMapToFile(processId, fileName);

        // Закрыть файл после использования
        if (fclose(output_file) != 0) {
            perror("Failed to close output file");
            return 1;
        }

        printf("Карта памяти сохранена в %s\n", fileName);
    }

    return 0;
}



