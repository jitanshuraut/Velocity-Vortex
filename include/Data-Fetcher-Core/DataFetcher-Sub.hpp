#ifndef DATAFETCHER_SUB_HPP
#define DATAFETCHER_SUB_HPP

#include <zmq.hpp>
#include <string>
#include <iostream>

class DataFetcherSub
{
public:
    DataFetcherSub(const std::string &address);
    void subscribe(const std::string &topic);
    void receive();

private:
    zmq::context_t context;
    zmq::socket_t subscriber;
};

#endif 
