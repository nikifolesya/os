#include <stdio.h>
#include <stdlib.h>

// Функция для отображения содержимого файла с учетом групп строк
void displayFile(FILE *file, int groupSize) {
    int currentChar;
    int currentLine = 0;

    // Чтение файла посимвольно
    while ((currentChar = fgetc(file)) != EOF) {
        // Вывод символа
        putchar(currentChar);

        // Если достигнут конец строки, увеличиваем счетчик текущей строки
        if (currentChar == '\n') {
            currentLine++;

            // Если задан размер группы и достигнут предел строк в группе, ожидаем нажатия клавиши
            if (groupSize > 0 && currentLine % groupSize == 0) {
                printf("\nНажмите любую клавишу для продолжения...");
                getchar(); // Ожидание нажатия клавиши
                printf("\n");
            }
        }
    }

    // Вывод сообщения при достижении конца файла
    if (feof(file)) {
        printf("\nЧтение файла закончено\n");
    } else {
        // Обработка ошибок, если они возникнут
        perror("Ошибка при чтении файла");
    }
}

int main(int argc, char *argv[]) {
    // Проверка количества аргументов
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <имя_файла> <N>\n", argv[0]);
        return 1;
    }

    // Открытие файла для чтения
    FILE *file = fopen(argv[1], "r");

    if (file == NULL) {
        perror("Ошибка открытия файла");
        return 1;
    }

    // Преобразование второго аргумента в число
    int groupSize = atoi(argv[2]);

    // Вывод содержимого файла
    displayFile(file, groupSize);

    // Закрытие файла
    if (fclose(file) != 0) {
        perror("Ошибка закрытия файла");
        return 1;
    }

    return 0;
}


