#include "IO-Broker-Core/RedisDBManager.hpp"

RedisDBManager::RedisDBManager(const std::string &host, int port)
    : host(host), port(port), context(nullptr) {}

RedisDBManager::~RedisDBManager()
{
    if (context)
    {
        redisFree(context);
    }
}

bool RedisDBManager::connect()
{
    context = redisConnect(host.c_str(), port);
    if (context == nullptr || context->err)
    {
        if (context)
        {
            std::cout << "Error: " << context->errstr << std::endl;
            redisFree(context);
        }
        else
        {
            std::cout << "Can't allocate redis context" << std::endl;
        }
        return false;
    }
    std::cout << "Connected to Redis server" << std::endl;
    return true;
}

void RedisDBManager::publishCreateMessage(const std::string &dbFileName, const std::string &sqlStructure, const std::vector<std::string> &channels)
{
    for (const auto &channel : channels)
    {
        std::string createMessage = "CREATE " + dbFileName + " \"" + sqlStructure + "\"";
        if (publishMessage(channel, createMessage))
        {
            std::cout << "Published CREATE message to " << channel << std::endl;
        }
    }
}

void RedisDBManager::publishInsertMessage(const std::string &dbFileName, const std::string &dataToInsert, const std::vector<std::string> &channels)
{
    for (const auto &channel : channels)
    {
        std::string insertMessage = "INSERT " + dbFileName + " \"" + dataToInsert + "\"";
        if (publishMessage(channel, insertMessage))
        {
            std::cout << "Published INSERT message to " << channel << std::endl;
        }
    }
}

void RedisDBManager::publishLogMessage(const std::string &dataToInsert, const std::vector<std::string> &channels)
{
    for (const auto &channel : channels)
    {
        std::string logMessage = "LOG " + dataToInsert;
        if (publishMessage(channel, logMessage))
        {
            std::cout << "Published LOG message to " << channel << std::endl;
        }
    }
}

bool RedisDBManager::publishMessage(const std::string &channel, const std::string &message)
{
    redisReply *reply = (redisReply *)redisCommand(context, "PUBLISH %s %s", channel.c_str(), message.c_str());
    if (reply == nullptr)
    {
        std::cout << "Error publishing message to " << channel << std::endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}
