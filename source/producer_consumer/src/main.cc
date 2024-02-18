#include <iostream>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

struct Item
{
    int i;
};

std::ostream &operator << (std::ostream &os, const Item &item)
{
    os << "i: " << item.i;
    return os;
}

struct Context
{
    std::mutex m;
    std::queue<Item> sharedQueue;
    std::condition_variable cv;
    std::atomic<bool> completed;
};

void Producer(int numItems, std::queue<Item> &sharedQueue, std::mutex &m, std::condition_variable &cv, std::atomic<bool> &completed)
{
    int counter = 0;
    while(numItems--)
    {
        auto item = Item{.i = counter++};
        std::cout << "producer: " << item << std::endl;
        {
            std::lock_guard<std::mutex> lock(m);
            sharedQueue.push(item);
        }
        cv.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }

    completed = true;
    cv.notify_one();
    std::cout << "producer completed? " << (completed ? "yes" : "no") << std::endl;
}

void Consumer(std::queue<Item> &sharedQueue, std::mutex &m, std::condition_variable &cv, std::atomic<bool> &completed)
{
    std::cout << "completed? " << (completed ? "yes" : "no") << std::endl;
    while(not completed or not sharedQueue.empty())
    {
        std::optional<Item> item;
        if(sharedQueue.empty() and completed == false)
        {
            std::unique_lock lk(m);
            cv.wait(lk, [&]{return !sharedQueue.empty() or completed;});
        }

        if(!sharedQueue.empty())
        {
            std::lock_guard<std::mutex> lock(m);
            item = sharedQueue.front();
            sharedQueue.pop();
        }

        if(item)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{150});
            std::cout << "consumer: " << item.value() << std::endl;
            std::cout << "consumer completed? " << (completed ? "yes" : "no") << std::endl;
        }
    }
}


int main(int argc, char** argv)
{
    Context context;
    context.completed = false;

    {
        auto producerThread = std::thread(Producer, 10, std::ref(context.sharedQueue), std::ref(context.m), std::ref(context.cv), std::ref(context.completed));
        auto consumerThread = std::thread(Consumer, std::ref(context.sharedQueue), std::ref(context.m), std::ref(context.cv), std::ref(context.completed));

        producerThread.join();
        consumerThread.join();
    }

    return 0;
}
