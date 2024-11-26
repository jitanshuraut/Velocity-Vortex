#include "Data-Fetcher-Core/DataFetcher-Sub.hpp"

DataFetcherSub::DataFetcherSub(const std::string& address)
    : context(1), subscriber(context, ZMQ_SUB)
{
    subscriber.connect(address);
}

void DataFetcherSub::subscribe(const std::string& topic)
{
    subscriber.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
}

void DataFetcherSub::receive()
{
    zmq::message_t message;
    while (true)
    {
        subscriber.recv(message, zmq::recv_flags::none);
        std::string receivedMessage(static_cast<char*>(message.data()), message.size());
        std::cout << "Received: " << receivedMessage << std::endl;
    }
}
