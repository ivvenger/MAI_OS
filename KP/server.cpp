#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <ctime>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <random> 

const int MAX_PLAYERS = 4; // Максимальное количество игроков в одной игре

struct Game {
    std::string name;
    int players;
    std::string word;
    std::vector<int> clients;
};

std::vector<Game> games;
std::mutex gamesMutex;

// Функция для проверки наличия буквы в строке
bool contains(const std::string& s, char lit) {
    return std::find(s.begin(), s.end(), lit) != s.end();
}

// Функция для генерации случайного слова
std::string generateRandomWord() {
    std::string letters = "qwertyuiopasdfghjklzxcvbnm";
    std::random_device rd;
    std::default_random_engine rng(rd());
    std::shuffle(letters.begin(), letters.end(), rng); // Перемешивает случайным образом
    std::string word;
    for (int i = 0; i < 4; ++i) {
        word += letters[i];
    }
    return word;
}

void createGame(const std::string& name, int clientSocket) {
    std::lock_guard<std::mutex> lock(gamesMutex);
    Game game;
    game.name = name;
    game.players = 1;
    game.word = generateRandomWord();
    game.clients.push_back(clientSocket);
    games.push_back(game);

    // Отправляем подтверждение клиенту
    std::string response = "Игра \"" + name + "\" создана. Цель игры: угадать слово. Игроков: 1\n";
    write(clientSocket, response.c_str(), response.length());

    std::cout << "Игра \"" << name << "\" создана. Цель игры: угадать слов. Игроков: 1" << std::endl;
}

void joinGame(std::string& name, int clientSocket) {
    std::lock_guard<std::mutex> lock(gamesMutex);
    for (Game& game : games) {
        if (game.name == name) {
            if (game.players < MAX_PLAYERS) {
                game.players++;
                game.clients.push_back(clientSocket);

                // Отправляем подтверждение клиенту
                std::string response = "Игрок присоединился к игре \"" + name + "\". Игроков: " + std::to_string(game.players) + "\n";
                write(clientSocket, response.c_str(), response.length());

                std::cout << "Игрок присоединился к игре \"" << name << "\". Игроков: " << game.players << std::endl;
            } else {
                // Отправляем сообщение о том, что игра полная
                std::string response = "Игра \"" + name + "\" уже полная.\n";
                write(clientSocket, response.c_str(), response.length());

                std::cout << "Игра \"" << name << "\" уже полная." << std::endl;
            }
            return;
        }
    }

    // Отправляем сообщение о том, что игра не найдена
    std::string response = "Игра \"" + name + "\" не найдена.\n";
    write(clientSocket, response.c_str(), response.length());

    std::cout << "Игра \"" << name << "\" не найдена." << std::endl;
}

void findGame(int clientSocket) {
    std::lock_guard<std::mutex> lock(gamesMutex);
    for (Game& game : games) {
        if (game.players < MAX_PLAYERS) {
            game.players++;
            game.clients.push_back(clientSocket);

            // Отправляем подтверждение клиенту
            std::string response = "Игрок присоединился к игре \"" + game.name + "\". Игроков: " + std::to_string(game.players) + "\n";
            write(clientSocket, response.c_str(), response.length());

            std::cout << "Игрок присоединился к игре \"" << game.name << "\". Игроков: " << game.players << std::endl;
            return;
        }
    }

    // Отправляем сообщение о том, что свободные игры не найдены
    std::string response = "Свободные игры не найдены.\n";
    write(clientSocket, response.c_str(), response.length());

    std::cout << "Свободные игры не найдены." << std::endl;
}

void playGame(std::string guessWord, int clientSocket) {
    std::lock_guard<std::mutex> lock(gamesMutex);
    for (Game& game : games) {
        if (std::find(game.clients.begin(), game.clients.end(), clientSocket) != game.clients.end()) {
            std::string word = game.word;
            int bulls = 0;
            int cows = 0;

            for (int i = 0; i < 4; ++i) {
                if (word[i] == guessWord[i]) {
                    bulls++;
                } else if (contains(word, guessWord[i])) {
                    cows++;
                }
            }

            // Отправляем ответ клиенту
            std::string response = "Bulls: " + std::to_string(bulls) + ", Cows: " + std::to_string(cows) + "\n";
            if (bulls == 4) {
                response += "You won!\n";
            }
            write(clientSocket, response.c_str(), response.length());

            std::cout << "Bulls: " << bulls << ", Cows: " << cows << std::endl;

            if (bulls == 4) {
                std::cout << "You won!" << std::endl;
            }
            return;
        }
    }

    // Если игрок не найден в играх
    std::string response = "Игрок не найден в играх.\n";
    write(clientSocket, response.c_str(), response.length());

    std::cout << "Игрок не найден в играх." << std::endl;
}

void handleClient(int clientSocket) {
    char buffer[256];
    while (true) {
        int bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            break;
        }
        buffer[bytesRead] = '\0';
        std::string command(buffer);

        if (command.substr(0, 6) == "create") {
            std::string name = command.substr(7);
            createGame(name, clientSocket);
        } else if (command.substr(0, 4) == "join") {
            std::string name = command.substr(5);
            joinGame(name, clientSocket);
        } else if (command.substr(0, 4) == "find") {
            findGame(clientSocket);
        } else if (command.substr(0, 4) == "play") {
            std::string guessWord = command.substr(5);
            playGame(guessWord, clientSocket);
        } else if (command == "exit") {
            break;
        } else {
            std::string response = "Неверная команда.\n";
            write(clientSocket, response.c_str(), response.length());
            std::cout << "Неверная команда." << std::endl;
        }
    }
    close(clientSocket);
}

int main() {
    std::srand(std::time(nullptr)); // Инициализация генератора случайных чисел

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Ошибка при создании сокета." << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Порт сервера
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Ошибка при привязке сокета." << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Ошибка при прослушивании сокета." << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Сервер запущен." << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            std::cerr << "Ошибка при принятии соединения." << std::endl;
            continue;
        }

        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    close(serverSocket);
    return 0;
}