#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <atomic>
#include <mutex>
#include <string>
#include <iostream>
#include <json/json.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

class WebSocket : public std::enable_shared_from_this<WebSocket>
{
    tcp::resolver resolver_;
    websocket::stream<ssl::stream<beast::tcp_stream>> ws_;
    beast::flat_buffer buffer_;
    std::string host_;
    std::string text_;
    bool connected_;
    bool authenticated_;
    std::atomic<bool> reading_;
    std::mutex websocket_mutex_;
    std::atomic<bool> stop_reading_;
    std::string Stock;

protected:
    virtual void processMessage(const Json::Value &message) = 0;

public:
    explicit WebSocket(net::io_context &ioc, ssl::context &ctx);
    ~WebSocket();

    void run(const char *host, const char *port, const char *text, std::string stock_);

private:
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void on_ssl_handshake(beast::error_code ec);
    void on_handshake(beast::error_code ec);
    void on_auth_write(beast::error_code ec, std::size_t bytes_transferred);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void sendAuthMessage();
    void subscribeToChannels();
    void doRead();
};
