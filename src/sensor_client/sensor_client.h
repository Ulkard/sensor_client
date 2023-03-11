#pragma once

#include <functional>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include <cpprest/ws_client.h>
#include <cpprest/json.h>

namespace ws_client = web::websockets::client;

//TODO: add ClientInterface and inherit Client from it?
class Client {
public:
    Client() = delete;
    Client(const std::string& host, const std::string& port) :
        uri_("ws://" + host + ":" + port)
    {
        ws_client_.set_message_handler([=](const ws_client::websocket_incoming_message& msg) {
            this->onMessage(msg);
        });
        ws_client_.set_close_handler([=](ws_client::websocket_close_status close_status,
                                     const utility::string_t& reason,
                                     const std::error_code& error){
            this->onClose(close_status, reason, error);
        });
    }
    Client(const std::string& host, int port) : Client(host, std::to_string(port)) {}

    void connect();
    void disconnect();
    bool sensorConnected(const std::string& sensor_name);
    void subscribe(const std::string& sensor_name);
    void unsubscribe(const std::string& sensor_name);

    template<typename CBType, typename... ArgTypes>
    void registerCallback(const std::string& sensor_name, CBType callback, ArgTypes... args);
    void removeCallback(const std::string& sensor_name);
    void onMessage(const ws_client::websocket_incoming_message& msg);
    void onClose(ws_client::websocket_close_status close_status,
                 const utility::string_t& reason,
                 const std::error_code& error);

private:
    enum class QUERY { CLIENT_CONNECT=0, CLIENT_DISCONNECT=1, SENSOR_CONNECTION_STATUS=2,
                       SUBSCRIBE_SENSOR=3, UNSUBSCRIBE_SENSOR=4 };
    bool isValid(const std::string& sensor_name);
    void send(QUERY type, const std::string& content);

    web::websockets::client::websocket_callback_client ws_client_;
    std::string uri_;

    std::unordered_set<std::string> connected_sensors;
    std::unordered_map<std::string, std::vector<std::function<void(const web::json::value&)>>> subscriptions_;

};


template<typename CBType, typename... ArgTypes>
void Client::registerCallback(const std::string &sensor_name, CBType callback, ArgTypes... args)
{
    std::function<void(const web::json::value&)> CB_lambda = [&](const web::json::value& sensor_readings) { callback(sensor_readings, args...); };
    subscriptions_[sensor_name].push_back(CB_lambda);
    subscribe(sensor_name);
}

