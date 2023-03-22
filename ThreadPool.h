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

// Structure Task represents a task to be done
struct Task {
    std::function<void(size_t idx)> func; // Functor to do
     size_t arg; // Argument to do
};

class ThreadPool {
public:
    // The constructor here declares how many threads should be in the pool.
    ThreadPool(int numThreads);

    // Adding a task to the queue
    void enqueueTask(std::function<void(size_t idx)> func, size_t  arg);

    void executeTasks();

    // Destruktor
    ~ThreadPool();

    void threadLogger(const std::string &where, const std::string &message);

private:
    std::vector<std::thread> threads; // Wektor wątków
    std::queue<Task> tasks; // Kolejka zadań
    std::mutex queue_mutex; // Mutex dla kolejki zadań
    std::condition_variable condition; // Warunek dla sekcji krytycznej

    bool stop; // Flaga zatrzymania threadpoola
    std::time_t startTime;
};
