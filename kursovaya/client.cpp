#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mutex>
#include <gtk/gtk.h>

using namespace std;

// Глобальные переменные для элементов интерфейса
GtkWidget *server1Button;
GtkWidget *server2Button;
GtkWidget *infoLabel[2]; // Массив для информации каждого сервера
GtkWidget *infoScreen[2];  // Массив окон для каждого сервера
GtkWidget *vboxMain;
GtkStack *stack;  // Объявление GtkStack глобально

std::mutex globalMutex;

// Функция для получения данных от сервера
void fetchDataFromServer(const char *serverIP, int serverPort, int serverIndex)
{
    // Создание сокета для соединения с сервером
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("Ошибка создания сокета");
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);

    // Преобразование IP-адреса из текстового в бинарный формат
    if (inet_pton(AF_INET, serverIP, &serverAddress.sin_addr) <= 0)
    {
        perror("Неверный адрес/Адрес не поддерживается");
        close(clientSocket);
        return;
    }

    // Подключение к серверу
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("Ошибка подключения к серверу");
        close(clientSocket);
        return;
    }

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    // Чтение данных от сервера
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == -1)
    {
        perror("Ошибка чтения данных от сервера");
    }
    else
    {
        buffer[bytesRead] = '\0';
        // Установка текста на метке в зависимости от сервера
        gtk_label_set_text(GTK_LABEL(infoLabel[serverIndex]), buffer);
    }

    // Закрытие сокета
    close(clientSocket);
}

// Функция обновления данных от сервера
gboolean updateDataFromServer(gpointer data)
{
    std::lock_guard<std::mutex> lock(globalMutex);
    // Выполнение опроса серверов и обновление информации для каждого сервера
    for (int i = 0; i < 2; ++i)
    {
        // Использование соответствующего IP-адреса и порта для каждого сервера
        const char *serverIP = (i == 0) ? "127.0.0.1" : "127.0.0.1";
        int serverPort = (i == 0) ? 8081 : 8082;
        fetchDataFromServer(serverIP, serverPort, i);
    }

    // Возвращение TRUE для продолжения вызовов этой функции с указанной периодичностью
    return G_SOURCE_CONTINUE;
}

// Обработчики событий кнопок

// Функция вызывается при нажатии на кнопку сервера
void onServerClicked(GtkWidget *widget, gpointer data)
{
    std::lock_guard<std::mutex> lock(globalMutex);
    int serverIndex = GPOINTER_TO_INT(data);
    // Использование соответствующего IP-адреса и порта для каждого сервера
    const char *serverIP = (serverIndex == 0) ? "127.0.0.1" : "127.0.0.1";
    int serverPort = (serverIndex == 0) ? 8081 : 8082;
    fetchDataFromServer(serverIP, serverPort, serverIndex);

    // Переключение на экран с информацией для выбранного сервера
    gtk_stack_set_visible_child_name(stack, ("infoScreen" + to_string(serverIndex)).c_str());
}

// Функция вызывается при нажатии на кнопку "Back"
void onBackClicked(GtkWidget *widget, gpointer data)
{
    // Переключение обратно на экран выбора сервера
    gtk_stack_set_visible_child_name(stack, "buttonScreen");
}

// Функция вызывается при нажатии на кнопку "Exit"
void onCloseClicked(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

int main()
{
    // Инициализация GTK
    gtk_init(nullptr, nullptr);

    // Создание основного окна
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_window_set_title(GTK_WINDOW(window), "Client");

    // Создание вертикального контейнера (vbox) для размещения элементов интерфейса
    vboxMain = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vboxMain);

    // Создание сетки (grid) для размещения кнопок
    GtkWidget *grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(vboxMain), grid, TRUE, TRUE, 0);

    // Создание кнопок "Server 1", "Server 2" и "Exit"
    server1Button = gtk_button_new_with_label("Server 1");
    server2Button = gtk_button_new_with_label("Server 2");
    GtkWidget *exitButton = gtk_button_new_with_label("Exit");

    // Подключение обработчиков событий для кнопок
    g_signal_connect(server1Button, "clicked", G_CALLBACK(onServerClicked), GINT_TO_POINTER(0));
    g_signal_connect(server2Button, "clicked", G_CALLBACK(onServerClicked), GINT_TO_POINTER(1));
    g_signal_connect(exitButton, "clicked", G_CALLBACK(onCloseClicked), nullptr);

    // Создание GtkStack для переключения между экранами
    stack = GTK_STACK(gtk_stack_new());
    gtk_box_pack_start(GTK_BOX(vboxMain), GTK_WIDGET(stack), TRUE, TRUE, 0);

    // Создание экрана с кнопками выбора сервера
    GtkWidget *buttonScreen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(buttonScreen), server1Button);
    gtk_container_add(GTK_CONTAINER(buttonScreen), server2Button);
    gtk_container_add(GTK_CONTAINER(buttonScreen), exitButton);
    gtk_stack_add_titled(stack, buttonScreen, "buttonScreen", "Buttons");

    // Создание экранов с информацией для каждого сервера
    for (int i = 0; i < 2; ++i)
    {
        infoScreen[i] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        infoLabel[i] = gtk_label_new(("Server " + to_string(i + 1) + " data will be displayed here").c_str());
        gtk_container_add(GTK_CONTAINER(infoScreen[i]), infoLabel[i]);

        // Создание кнопки "Back" и подключение обработчика событий
        GtkWidget *backButton = gtk_button_new_with_label("Back");
        g_signal_connect(backButton, "clicked", G_CALLBACK(onBackClicked), nullptr);
        gtk_container_add(GTK_CONTAINER(infoScreen[i]), backButton);

        // Добавление экранов в GtkStack
        gtk_stack_add_titled(stack, infoScreen[i], ("infoScreen" + to_string(i)).c_str(), ("Server " + to_string(i + 1)).c_str());
    }

    // Отображение всех элементов интерфейса
    gtk_widget_show_all(window);

    // Добавление таймера для опроса сервера каждую секунду (1000 миллисекунд)
    g_timeout_add(1000, reinterpret_cast<GSourceFunc>(updateDataFromServer), nullptr);
    
    // Запуск основного цикла GTK
    gtk_main();

    return 0;
}
