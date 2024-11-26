#ifndef REDIS_DB_MANAGER_HPP
#define REDIS_DB_MANAGER_HPP

#include <iostream>
#include <vector>
#include <hiredis/hiredis.h>
#include <string>

class RedisDBManager
{
public:
    RedisDBManager(const std::string &host = "127.0.0.1", int port = 6379);
    ~RedisDBManager();

    bool connect();
    void publishCreateMessage(const std::string &dbFileName, const std::string &sqlStructure, const std::vector<std::string> &channels);
    void publishInsertMessage(const std::string &dbFileName, const std::string &dataToInsert, const std::vector<std::string> &channels);
    void publishLogMessage(const std::string &dataToInsert, const std::vector<std::string> &channels);
    
    private : redisContext *context;
    std::string host;
    int port;

    bool publishMessage(const std::string &channel, const std::string &message);
};

#endif
