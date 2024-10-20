#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <hiredis/hiredis.h>
#include <sqlite3.h>
#include <filesystem>

void executeSQL(const std::string& dbPath, const std::string& sqlCommand) {
    sqlite3* db;
    char* errorMessage = nullptr;

    if (sqlite3_open(dbPath.c_str(), &db) == SQLITE_OK) {
        if (sqlite3_exec(db, sqlCommand.c_str(), nullptr, nullptr, &errorMessage) != SQLITE_OK) {
            std::cout << "SQL error: " << errorMessage << std::endl;
            sqlite3_free(errorMessage);
        } else {
            std::cout << "SQL command executed successfully." << std::endl;
        }
        sqlite3_close(db);
    } else {
        std::cout << "Cannot open database: " << dbPath << std::endl;
    }
}

int main() {
    redisContext* c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            std::cout << "Error: " << c->errstr << std::endl;
            redisFree(c);
        } else {
            std::cout << "Can't allocate redis context" << std::endl;
        }
        return 1;
    }

    std::cout << "Connected to Redis server" << std::endl;

    std::vector<std::string> channels = {"AAPL"};
    
    for (const auto& channel : channels) {
        redisReply* reply = (redisReply*)redisCommand(c, "SUBSCRIBE %s", channel.c_str());
        if (reply == NULL) {
            std::cout << "Error subscribing to " << channel << std::endl;
            redisFree(c);
            return 1;
        }
        freeReplyObject(reply);
        std::cout << "Subscribed to " << channel << std::endl;
    }

    std::cout << "Waiting for messages..." << std::endl;

    while (true) {
        redisReply* reply;
        if (redisGetReply(c, (void**)&reply) == REDIS_OK) {
            if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 3) {
                if (std::string(reply->element[0]->str) == "message") {
                    std::string channel = reply->element[1]->str;
                    std::string message = reply->element[2]->str;

                    std::istringstream iss(message);
                    std::string operation, dbFileName, sqlCommand;
                    iss >> operation >> dbFileName;

                    std::getline(iss, sqlCommand);
                    sqlCommand = sqlCommand.substr(2, sqlCommand.length() - 3);

                    std::string dbPath = "./database/" + dbFileName;

                    if (operation == "CREATE") {
                        std::cout << "Creating database: " << dbPath << std::endl;
                        std::filesystem::create_directory("./database");
                        executeSQL(dbPath, sqlCommand);

                    } else if (operation == "INSERT") {
                        std::cout << "Inserting data into database: " << dbPath << std::endl;
                        std::string insertSQL = "INSERT INTO my_table VALUES(" + sqlCommand + ");";
                        executeSQL(dbPath, insertSQL);
                    }

                    std::cout << "Received message from " << channel << ": " << message << std::endl;
                }
            }
            freeReplyObject(reply);
        }
    }

    redisFree(c);
    return 0;
}