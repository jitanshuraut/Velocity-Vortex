#include "Utilities/Logger.hpp"

std::shared_ptr<RedisDBManager> Logger::logger = nullptr;

void Logger::initialize()
{
    logger = std::make_shared<RedisDBManager>();
    logger->connect();
}

std::shared_ptr<RedisDBManager>& Logger::getInstance()
{
    if (!logger)
    {
        initialize();
    }
    return logger;
}