#pragma once

#include "websocket_endpoint.h"

#include <string>
#include <unordered_map>


//TODO: add ClientInterface and inherit Client from it?
class Client {
public:
    Client() = delete;
    Client(const std::string& host, const std::string& port) :
        uri_("ws://" + host + ":" + port)
    {}
    Client(const std::string& host, int port) : Client(host, std::to_string(port)) {}

    void connect();
    void disconnect();
    bool sensorConnected(const std::string& sensor_name) const;
    void subscribe(const std::string& sensor_name);
    void unsubscribe(const std::string& sensor_name);

    //template<typename CBType, typename... ArgTypes>
    //void registerCallback(const std::string& sensor_name, CBType callback, ArgTypes... args);
    void removeCallback(const std::string& sensor_name);

private:
    websocket_endpoint endpoint_;
    std::string uri_;

    std::unordered_map<std::string, int> dummy_subscriptions_;

};


/*template<typename CBType, typename ArgTypes>
void Client::registerCallback(const std::string &sensor_name, Client::CBType callback, Client::ArgTypes args)
{

}
*/
