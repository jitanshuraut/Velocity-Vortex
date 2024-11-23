#include "Scheduler/scheduler.hpp"
#include "Executor-Manager-Core/executor_manager.hpp"
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <thread>

int main()
{
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);

    std::string topic = "";
    subscriber.connect("tcp://localhost:5555");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());

    ExecutorManager Executor_1;
    ExecutorManager Executor_2;
    ExecutorManager Executor_3;

    std::thread thread1(&ExecutorManager::processOrderQueue, &Executor_1);
    std::thread thread2(&ExecutorManager::processOrderQueue, &Executor_2);
    std::thread thread3(&ExecutorManager::processOrderQueue, &Executor_3);

    thread1.detach();
    thread2.detach();
    thread3.detach();

    Scheduler scheduler;
    scheduler.Register_Provider("Alpaca", Executor_1.getOrderQueuePointer());
    scheduler.Register_Provider("Polygon", Executor_2.getOrderQueuePointer());
    scheduler.Register_Provider("TwelveData", Executor_3.getOrderQueuePointer());

    zmq::message_t message;
    while (true)
    {
        subscriber.recv(message, zmq::recv_flags::none);
        std::string receivedMessage(static_cast<char *>(message.data()), message.size());
        std::cout << "Received: " << receivedMessage << std::endl;
        scheduler.RoundRobin(receivedMessage);
    }

    return 0;
}