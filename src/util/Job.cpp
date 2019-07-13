#include "Job.hpp"

void Job::threadLoop() {
    OPTICK_THREAD("Worker Thread");

    while (true) {
        std::function<void()> j;
        queue.wait_dequeue(j);

        j();

        returnQueue.enqueue(true);
    }
}

Job::Job() {
    thread = new std::thread([&]() {
        threadLoop();
    });
}

Job::~Job() {
    delete thread;
}

void Job::startJob(std::function<void()> func) {
    queue.enqueue(func);
} 

void Job::waitForJob() {
    bool j;
    returnQueue.wait_dequeue(j);
}