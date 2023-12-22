#include <iostream>
#include <csignal>
#include <unistd.h>

using namespace std;

bool shouldExit = false;

void sig_handler(int sig) {
    if (sig == SIGQUIT) {
        cout << "\nПринят сигнал SIGQUIT (CTRL+\\)\nПрекращение подачи воды" << endl;
        shouldExit = true;
    }
}

int main() {
    signal(SIGQUIT, sig_handler);

    cout << "Начало подачи воды\nПерекрытие воды по CTRL+\\" << endl;

    while (!shouldExit) {
        // Основной рабочий цикл программы
        sleep(1);
    }

    cout << "Подача воды остановлена" << endl;

    return 0;
}



