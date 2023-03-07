#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>

using namespace std;

// Struktura Task reprezentująca pojedyncze zadanie
struct Task {
    function<void(string)> func; // Funkcja do wykonania
    string arg; // Argument dla funkcji
};

// Klasa ThreadPool
class ThreadPool {
public:
    ThreadPool(int numThreads);

    // Dodanie zadania do kolejki
    void enqueueTask(function<void(string)> func, string arg);

    // Destruktor
    ~ThreadPool();

private:
    vector<thread> threads; // Wektor wątków
    queue<Task> tasks; // Kolejka zadań
    mutex queue_mutex; // Mutex dla kolejki zadań
    condition_variable condition; // Warunek dla sekcji krytycznej
    bool stop; // Flaga zatrzymania threadpoola
};