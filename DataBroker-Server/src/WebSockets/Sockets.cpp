#include "WebSockets/Sockets.hpp"

// Constructor for WebSocket class
WebSocket::WebSocket(net::io_context &ioc, ssl::context &ctx, const std::string &provider_name)
    : resolver_(net::make_strand(ioc)),
      ws_(net::make_strand(ioc), ctx),
      connected_(false),
      authenticated_(false),
      reading_(false),
      stop_reading_(false),
      provider_name(provider_name) {}

// Destructor for WebSocket class
WebSocket::~WebSocket()
{
    stop_reading_ = true;
    if (connected_)
    {
        beast::error_code ec;
        ws_.close(websocket::close_code::normal, ec);
    }
}

// Initiates the WebSocket connection
// Inputs: host - server host, port - server port, text - message text, stock_ - stock symbol, provider_name - name of the provider
void WebSocket::run(const char *host, const char *port, const char *text, std::string stock_, const std::string &provider_name)
{
    host_ = host;
    text_ = text;
    Stock = stock_;
    this->provider_name = provider_name;

    resolver_.async_resolve(
        host, port,
        beast::bind_front_handler(&WebSocket::on_resolve, shared_from_this()));
}

// Handles the resolution of the host and port
// Inputs: ec - error code, results - resolved endpoints
void WebSocket::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
    if (ec)
    {
        std::cerr << "Resolve error: " << ec.message() << std::endl;
        return;
    }

    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    beast::get_lowest_layer(ws_).async_connect(
        results,
        beast::bind_front_handler(&WebSocket::on_connect, shared_from_this()));
}

// Handles the connection to the server
// Inputs: ec - error code, ep - endpoint type
void WebSocket::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
    if (ec)
    {
        std::cerr << "Connect error: " << ec.message() << std::endl;
        return;
    }

    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str()))
    {
        ec = beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
        std::cerr << "SSL hostname error: " << ec.message() << std::endl;
        return;
    }

    host_ += ':' + std::to_string(ep.port());

    ws_.next_layer().async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(&WebSocket::on_ssl_handshake, shared_from_this()));
}

// Handles the SSL handshake
// Inputs: ec - error code
void WebSocket::on_ssl_handshake(beast::error_code ec)
{
    if (ec)
    {
        std::cerr << "SSL handshake error: " << ec.message() << std::endl;
        return;
    }

    beast::get_lowest_layer(ws_).expires_never();

    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

    ws_.set_option(websocket::stream_base::decorator(
        [](websocket::request_type &req)
        {
            req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-async-ssl");
        }));

    ws_.async_handshake(host_, "/v2/iex",
                        beast::bind_front_handler(&WebSocket::on_handshake, shared_from_this()));
}

// Handles the WebSocket handshake
// Inputs: ec - error code
void WebSocket::on_handshake(beast::error_code ec)
{
    if (ec)
    {
        std::cerr << "Handshake error: " << ec.message() << std::endl;
        return;
    }

    connected_ = true;
    sendAuthMessage();
}

// Sends the authentication message
void WebSocket::sendAuthMessage()
{
    const char *apiKey = std::getenv("APCA_API_KEY_ID");
    const char *secretKey = std::getenv("APCA_API_SECRET_KEY");

    if (!apiKey || !secretKey)
    {
        std::cerr << "Error: API credentials not found in environment variables" << std::endl;
        return;
    }

    Json::Value auth_msg;
    auth_msg["action"] = "auth";
    auth_msg["key"] = apiKey;
    auth_msg["secret"] = secretKey;

    Json::StreamWriterBuilder writer;
    std::string jsonString = Json::writeString(writer, auth_msg);

    std::lock_guard<std::mutex> lock(websocket_mutex_);
    ws_.async_write(
        net::buffer(jsonString),
        beast::bind_front_handler(&WebSocket::on_auth_write, shared_from_this()));
}

// Handles the write operation for authentication
// Inputs: ec - error code, bytes_transferred - number of bytes transferred
void WebSocket::on_auth_write(beast::error_code ec, std::size_t bytes_transferred)
{
    if (ec)
    {
        std::cerr << "Auth write error: " << ec.message() << std::endl;
        return;
    }

    doRead();
}

// Subscribes to the necessary channels
void WebSocket::subscribeToChannels()
{
    if (!authenticated_)
    {
        std::cerr << "Cannot subscribe: not authenticated" << std::endl;
        return;
    }

    Json::Value subscribe_msg;
    subscribe_msg["action"] = "subscribe";
    subscribe_msg["quotes"].append(Stock);
    subscribe_msg["bars"].append("*");
    subscribe_msg["dailyBars"].append("VOO");
    subscribe_msg["statuses"].append("*");

    Json::StreamWriterBuilder writer;
    std::string jsonString = Json::writeString(writer, subscribe_msg);

    std::lock_guard<std::mutex> lock(websocket_mutex_);
    ws_.async_write(
        net::buffer(jsonString),
        beast::bind_front_handler(&WebSocket::on_write, shared_from_this()));
}

// Initiates the read operation
void WebSocket::doRead()
{
    if (stop_reading_ || reading_.exchange(true))
        return;

    std::lock_guard<std::mutex> lock(websocket_mutex_);
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(&WebSocket::on_read, shared_from_this()));
}

// Handles the write operation
// Inputs: ec - error code, bytes_transferred - number of bytes transferred
void WebSocket::on_write(beast::error_code ec, std::size_t bytes_transferred)
{
    if (ec)
    {
        std::cerr << "Write error: " << ec.message() << std::endl;
        return;
    }
}

// Handles the read operation
// Inputs: ec - error code, bytes_transferred - number of bytes transferred
void WebSocket::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
    reading_ = false;

    if (ec)
    {
        std::cerr << "Read error: " << ec.message() << std::endl;
        return;
    }
    std::string msg = beast::buffers_to_string(buffer_.data());
    buffer_.consume(buffer_.size());
    Json::Value root;
    Json::Reader reader;

    if (reader.parse(msg, root))
    {
        if (root.isArray() && root.size() > 0)
        {
            const Json::Value &firstMessage = root[0];

            if (!authenticated_ && firstMessage.isMember("msg") && firstMessage["msg"] == "authenticated")
            {
                authenticated_ = true;
                std::cout << "Successfully authenticated" << std::endl;
                subscribeToChannels();
            }
            else if (authenticated_)
            {
                processMessage(firstMessage);
            }
        }
        else
        {
            std::cerr << "Unexpected message format: not an array." << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to parse JSON: " << reader.getFormattedErrorMessages() << std::endl;
    }

    if (!stop_reading_)
    {
        doRead();
    }
}

// Processes the received message
// Inputs: message - JSON message
void WebSocket::processMessage(const Json::Value &message)
{
    if (!message.isObject())
    {
        std::cerr << "Unexpected message format: not an object." << std::endl;
        return;
    }

    if (message.isMember("T"))
    {
        std::string msgType = message["T"].asString();
        if (msgType == "t")
        {
            std::cout << "Trade: " << message["S"].asString()
                      << " Price: " << message["p"].asDouble()
                      << " Size: " << message["s"].asInt() << std::endl;
        }
        else if (msgType == "q")
        {
            std::cout << "Quote: " << message["S"].asString()
                      << " Bid: " << message["b"].asDouble()
                      << " Ask: " << message["a"].asDouble() << std::endl;
        }
        else
        {
            std::cerr << "Unknown message type: " << msgType << std::endl;
        }
    }
}