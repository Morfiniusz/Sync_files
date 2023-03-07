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
    void enqueueTask(function<void(string)> func, string arg) {
        { // Zablokowanie sekcji krytycznej
            unique_lock<mutex> lock(queue_mutex);
            tasks.emplace(Task{ func, arg });
        } // Odblokowanie sekcji krytycznej
        condition.notify_one();
    }

    // Destruktor
    ~ThreadPool() {
        { // Zablokowanie sekcji krytycznej
            unique_lock<mutex> lock(queue_mutex);
            stop = true;
        } // Odblokowanie sekcji krytycznej
        condition.notify_all();
        for (auto& thread : threads) {
            thread.join();
        }
    }

private:
    vector<thread> threads; // Wektor wątków
    queue<Task> tasks; // Kolejka zadań
    mutex queue_mutex; // Mutex dla kolejki zadań
    condition_variable condition; // Warunek dla sekcji krytycznej
    bool stop; // Flaga zatrzymania threadpoola
};