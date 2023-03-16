#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <chrono>
#include <iomanip>

#define LOG_ENABLED true

// Struktura Task reprezentująca pojedyncze zadanie
struct Task {
    std::function<void(std::string)> func; // Funkcja do wykonania
    std::string arg; // Argument dla funkcji
};

// Klasa ThreadPool
class ThreadPool {
public:
    // Konstruktor tutaj deklaruje ile wątków ma być w puli
    ThreadPool(int numThreads);

    // Dodanie zadania do kolejki
    void enqueueTask(std::function<void(std::string)> func, std::string arg);

    void executeTasks();

    // Destruktor
    ~ThreadPool();

    void logWithThreadIdAndTime(const std::string &where, const std::string &message);

private:
    std::vector<std::thread> threads; // Wektor wątków
    std::queue<Task> tasks; // Kolejka zadań
    std::mutex queue_mutex; // Mutex dla kolejki zadań
    std::condition_variable condition; // Warunek dla sekcji krytycznej

    bool stop; // Flaga zatrzymania threadpoola
    std::time_t startTime;
};
