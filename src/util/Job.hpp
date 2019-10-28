#pragma once

#include "optick.h"
#include "readerwriterqueue.h"
using namespace moodycamel;

#include <thread>
#include <functional>

class Job {
private:
    BlockingReaderWriterQueue<std::function<void()>> queue;
    BlockingReaderWriterQueue<bool> returnQueue;
    std::thread *thread;

public:
    void threadLoop();

    Job();

    ~Job();

    void startJob(std::function<void()> func);

    void waitForJob();
};