#ifndef ASYNCHRONOUS_WRITE_H
#define ASYNCHRONOUS_WRITE_H

#include <mutex>
#include <thread>
#include <condition_variable>
#include <cstdarg>
#include <queue>

class Asynchronous_write {
private:
    Asynchronous_write();

    ~Asynchronous_write();

public:
    struct message {
        FILE *file;
        std::string str;
    };

    void add(message m);

    /// Wait for end of writing
    void wait_to_write();

    static Asynchronous_write &getSingleton();

private:
    std::mutex mutex;
    std::thread thr;
    std::condition_variable cv;

    bool thread_work;
    std::queue<message> queue_message;

    void Infinite_loop_function();
};

#endif // ASYNCHRONOUS_WRITE_H
