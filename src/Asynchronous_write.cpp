#include "Asynchronous_write.h"
#include <functional>
#include <SFML/System.hpp>
#include <chrono>
#include <thread>

Asynchronous_write::Asynchronous_write()
        : thread_work(true) {
    thr = std::thread(std::bind(&Asynchronous_write::Infinite_loop_function, this));
}

Asynchronous_write::~Asynchronous_write() {
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        thread_work = false;
    }
    cv.notify_all();

    thr.join();
}

void Asynchronous_write::add(message m) {
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        queue_message.push(m);
    }
    cv.notify_one();
}

void Asynchronous_write::wait_to_write() {
    while (!queue_message.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

Asynchronous_write &Asynchronous_write::getSingleton() {
    static Asynchronous_write r;

    return r;
}

void Asynchronous_write::Infinite_loop_function() {
    while (this->thread_work || !queue_message.empty()) {
        message m;
        {
            std::unique_lock<std::mutex> lock(this->mutex);

            this->cv.wait(lock, [&] { return !queue_message.empty() | !this->thread_work; });

            if (!this->thread_work && queue_message.empty()) { return; }

            m = queue_message.front();
            queue_message.pop();
        }
        fprintf(m.file, "%.*s", (int) m.str.size(), m.str.c_str());
        fflush(m.file);
    }
}
