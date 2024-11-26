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
public:
    /**
     * @brief Construct a new WebSocket object
     *
     * @param ioc The io_context to use for asynchronous operations.
     * @param ctx The SSL context to use for SSL connections.
     * @param provider_name The name of the provider.
     */
    WebSocket(net::io_context &ioc, ssl::context &ctx, const std::string &provider_name);

    /**
     * @brief Destroy the WebSocket object
     */
    ~WebSocket();

    /**
     * @brief Run the WebSocket connection
     *
     * @param host The host to connect to.
     * @param port The port to connect to.
     * @param text The initial text to send.
     * @param stock_ The stock symbol to subscribe to.
     * @param provider_name The name of the provider.
     */
    void run(const char *host, const char *port, const char *text, std::string stock_, const std::string &provider_name);

protected:
    /**
     * @brief Process a received message
     *
     * @param message The JSON message to process.
     */
    void processMessage(const Json::Value &message);

private:
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
    std::string stock_;
    std::string provider_name_;

    /**
     * @brief Handle the resolve result
     *
     * @param ec The error code.
     * @param results The resolved endpoints.
     */
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);

    /**
     * @brief Handle the connect result
     *
     * @param ec The error code.
     * @param ep The connected endpoint.
     */
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);

    /**
     * @brief Handle the SSL handshake result
     *
     * @param ec The error code.
     */
    void on_ssl_handshake(beast::error_code ec);

    /**
     * @brief Handle the WebSocket handshake result
     *
     * @param ec The error code.
     */
    void on_handshake(beast::error_code ec);

    /**
     * @brief Handle the result of writing the authentication message
     *
     * @param ec The error code.
     * @param bytes_transferred The number of bytes transferred.
     */
    void on_auth_write(beast::error_code ec, std::size_t bytes_transferred);

    /**
     * @brief Handle the result of writing a message
     *
     * @param ec The error code.
     * @param bytes_transferred The number of bytes transferred.
     */
    void on_write(beast::error_code ec, std::size_t bytes_transferred);

    /**
     * @brief Handle the result of reading a message
     *
     * @param ec The error code.
     * @param bytes_transferred The number of bytes transferred.
     */
    void on_read(beast::error_code ec, std::size_t bytes_transferred);

    /**
     * @brief Send the authentication message
     */
    void sendAuthMessage();

    /**
     * @brief Subscribe to the necessary channels
     */
    void subscribeToChannels();

    /**
     * @brief Start reading messages
     */
    void doRead();
};
