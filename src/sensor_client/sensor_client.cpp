#include "sensor_client.h"

#include <fmt/core.h>



void Client::connect()
{
    //TODO: add Client-side message_handler here↓↓
    fmt::print("connecting( {} )", uri_);
    endpoint_.connect(uri_);
}

void Client::disconnect()
{
    endpoint_.reset();
}

bool Client::sensorConnected(const std::string &sensor_name) const
{
    fmt::print(sensor_name);
    return false;
}

void Client::subscribe(const std::string &sensor_name)
{
    fmt::print(sensor_name);
}

void Client::unsubscribe(const std::string &sensor_name)
{
    fmt::print(sensor_name);
}

void Client::removeCallback(const std::string &sensor_name)
{
    fmt::print(sensor_name);
}

