#include "Data-Fetcher-Core/DataFetcher-Sub.hpp"

int main()
{

    DataFetcherSub dataFetcher("tcp://localhost:5555");
    dataFetcher.subscribe("");
    dataFetcher.receive();

    return 0;
}
