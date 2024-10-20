#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <json/json.h>

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <map>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

class WebSocket
{
public:
    WebSocket() : m_done(false)
    {
        m_client.set_access_channels(websocketpp::log::alevel::all);
        m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
        m_client.init_asio();
        m_client.set_tls_init_handler(bind(&AlpacaWebSocket::on_tls_init, this, ::_1));
        m_client.set_message_handler(bind(&AlpacaWebSocket::on_message, this, ::_1, ::_2));
    }

    void run(const std::string &uri)
    {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);
        if (ec)
        {
            std::cout << "Could not create connection: " << ec.message() << std::endl;
            return;
        }

        m_client.connect(con);
        m_client.run();
    }

    void send(const std::string &message)
    {
        m_client.send(m_hdl, message, websocketpp::frame::opcode::text);
    }

    void close()
    {
        m_client.close(m_hdl, websocketpp::close::status::normal, "");
    }

    bool is_done() const
    {
        return m_done;
    }

private:
    context_ptr on_tls_init(websocketpp::connection_hdl)
    {
        context_ptr ctx = websocketpp::lib::make_shared<websocketpp::lib::asio::ssl::context>(
            websocketpp::lib::asio::ssl::context::sslv23);
        try
        {
            ctx->set_options(
                websocketpp::lib::asio::ssl::context::default_workarounds |
                websocketpp::lib::asio::ssl::context::no_sslv2 |
                websocketpp::lib::asio::ssl::context::no_sslv3 |
                websocketpp::lib::asio::ssl::context::single_dh_use);
        }
        catch (std::exception &e)
        {
            std::cout << "Error in context pointer: " << e.what() << std::endl;
        }
        return ctx;
    }

    std::map<std::string, std::string> on_message(websocketpp::connection_hdl hdl, client::message_ptr msg)
    {
        std::map<std::string, std::string> result;
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(msg->get_payload(), root);
        if (!parsingSuccessful)
        {
            result["error"] = "Failed to parse JSON";
            return result;
        }

        if (root.isArray() && !root.empty())
        {
            const Json::Value &first_item = root[0];
            if (first_item.isMember("T"))
            {
                const std::string &msg_type = first_item["T"].asString();
                result["type"] = msg_type;

                if (msg_type == "success")
                {
                    result["message"] = first_item["msg"].asString();
                }
                else if (msg_type == "subscription")
                {
                    result["subscription_data"] = first_item.toStyledString();
                }
                else if (msg_type == "q")
                {
                    result["symbol"] = first_item["S"].asString();
                    result["bid_price"] = first_item["bp"].asString();
                    result["bid_size"] = first_item["bs"].asString();
                    result["ask_price"] = first_item["ap"].asString();
                    result["ask_size"] = first_item["as"].asString();
                }
                else if (msg_type == "b")
                {
                    result["symbol"] = first_item["S"].asString();
                    result["open"] = first_item["o"].asString();
                    result["high"] = first_item["h"].asString();
                    result["low"] = first_item["l"].asString();
                    result["close"] = first_item["c"].asString();
                    result["volume"] = first_item["v"].asString();
                    result["timestamp"] = first_item["t"].asString();
                    result["trades"] = first_item["n"].asString();
                    result["vwap"] = first_item["vw"].asString();
                }
            }
        }

        m_hdl = hdl;
        return result;
    }

    client m_client;
    websocketpp::connection_hdl m_hdl;
    bool m_done;
};
