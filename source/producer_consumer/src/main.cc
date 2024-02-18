#include <iostream>
#include <atomic>
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
    std::atomic<bool> completed;
};

void Producer(int numItems, std::queue<Item> &sharedQueue, std::mutex &m, std::atomic<bool> &completed)
{
    int counter = 0;
    while(--numItems)
    {
        auto item = Item{.i = counter++};
        std::cout << "producer: " << item << std::endl;
        {
            std::lock_guard<std::mutex> lock(m);
            sharedQueue.push(item);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }

    completed = true;
}

void Consumer(std::queue<Item> &sharedQueue, std::mutex &m, std::atomic<bool> &completed)
{
    while(completed == false or !sharedQueue.empty())
    {
        std::optional<Item> item;
        {
            std::lock_guard<std::mutex> lock(m);
            item = sharedQueue.front();
            sharedQueue.pop();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{50});
        std::cout << "consumer: " << item.value() << std::endl;
    }
}


int main(int argc, char** argv)
{
    Context context;
    auto producerThread = std::thread(Producer, 10, std::ref(context.sharedQueue), std::ref(context.m), std::ref(context.completed));
    auto consumerThread = std::thread(Consumer, std::ref(context.sharedQueue), std::ref(context.m), std::ref(context.completed));

    return 0;
}
