#include "sensor_client.h"

#include <fmt/core.h>

#define MAX_SENSOR_NAME_SIZE 40



void Client::connect()
{
    fmt::print("connecting uri( {} )", uri_);
    endpoint_.connect(uri_);
    endpoint_.setMessageCallback(websocketpp::lib::bind(
                           &Client::onMessage,
                           this,
                           websocketpp::lib::placeholders::_1,
                           websocketpp::lib::placeholders::_2
                           ));

}

void Client::disconnect()
{
    endpoint_.disconnect();
}

bool Client::sensorConnected(const std::string &sensor_name) const
{
    return connected_sensors.count(sensor_name);
}

void Client::subscribe(const std::string &sensor_name)
{
    if (isValid(sensor_name)) {
        throw std::invalid_argument("too long sensor_name");
    }
    //TODO: subscription code here

}

void Client::unsubscribe(const std::string &sensor_name)
{
    fmt::print(sensor_name);
}

void Client::removeCallback(const std::string &sensor_name)
{
    fmt::print(sensor_name);
}

void Client::onMessage(websocketpp::connection_hdl, WSClient::message_ptr msg)
{
    if (msg->get_opcode() != websocketpp::frame::opcode::text) {
        fmt::print("non-text msg received: {}", websocketpp::utility::to_hex(msg->get_payload()));
        return;
    }

    const Json json_msg = Json::parse(msg->get_payload());
    if (!isValid(json_msg)) {
        fmt::print("bad format msg received: {}", msg->get_payload());
        return;
    }
    fmt::print("---\n");
    //fmt::print("event: {} \ndata: {}", json_msg.at("event").as_string(), json_msg.at("data").serialize());
}

bool Client::isValid(const std::string &sensor_name)
{
    return sensor_name.size() > 0 && sensor_name.size() < MAX_SENSOR_NAME_SIZE;
}

bool Client::isValid(const Json &json_msg)
{
    //TODO: check req fields
    return !json_msg.is_null();
}

