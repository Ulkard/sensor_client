#include "sensor_client.h"

#include <fmt/core.h>

#define MAX_SENSOR_NAME_SIZE 40


using Json = web::json::value;

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
    //TODO: endpoint seems broken here
    //send(QUERY::CLIENT_CONNECT, "");
}

void Client::disconnect()
{
    send(QUERY::CLIENT_DISCONNECT, "");
    endpoint_.disconnect();
}

bool Client::sensorConnected(const std::string &sensor_name)
{
    //TODO: request, then wait until response?
    send(QUERY::SENSOR_CONNECTION_STATUS, sensor_name);
    return connected_sensors.count(sensor_name);
}

void Client::subscribe(const std::string &sensor_name)
{
    if (!isValid(sensor_name)) {
        throw std::invalid_argument("too long sensor_name");
    }
    send(QUERY::SUBSCRIBE_SENSOR, sensor_name);
}

void Client::unsubscribe(const std::string &sensor_name)
{
    send(QUERY::UNSUBSCRIBE_SENSOR, sensor_name);
}

void Client::removeCallback(const std::string &sensor_name)
{
    if (subscriptions_.count(sensor_name)) {
        subscriptions_[sensor_name].clear();
    }
}

void Client::onMessage(websocketpp::connection_hdl, WSClient::message_ptr msg)
{
    if (msg->get_opcode() != websocketpp::frame::opcode::text) {
        fmt::print("non-text msg received: {}", websocketpp::utility::to_hex(msg->get_payload()));
        return;
    }

    const Json json_msg = Json::parse(msg->get_payload());

    const std::string& event = json_msg.at("event").as_string();
    const Json& data = json_msg.at("data");
    const std::string& sensor_id = data.at("sensor_id").as_string();

    if (event == "new_sensor_data") {
        const std::string& sensor_id = json_msg.at("data").at("sensor_id").as_string();
        if (subscriptions_.count(sensor_id)) {
            for (auto callback : subscriptions_[sensor_id]) {
                callback(data.at("sensor_readings"));
            }
        }

    } else if (event == "sensor_connection_status") {
        if (data.at("connected").as_bool()) {
            connected_sensors.insert(sensor_id);
        } else {
            connected_sensors.erase(sensor_id);
        }
    }
    fmt::print("sensors connected: {}\n", connected_sensors.size());
}

bool Client::isValid(const std::string &sensor_name)
{
    return sensor_name.size() > 0 && sensor_name.size() < MAX_SENSOR_NAME_SIZE;
}

void Client::send(Client::QUERY type, const std::string &content)
{
    constexpr auto connect = R"({{"event": "client_connect", "data": "{}" }})";
    constexpr auto disconnect = R"({{"event": "client_disconnect", "data": "{}" }})";
    constexpr auto sensor_connection_status = R"({{"event": "sensor_connection_status", "data": {{ "sensor_id": "{}" }} }})";
    constexpr auto subscribe = R"({{"event": "subscribe_sensor", "data": {{ "sensor_id": "{}" }} }})";
    constexpr auto unsubscribe = R"({{"event": "unsubscribe_sensor", "data": {{ "sensor_id": "{}" }} }})";

    switch (type) {
    case QUERY::CLIENT_CONNECT:
        endpoint_.send(fmt::format(connect, content));
        return;
    case QUERY::CLIENT_DISCONNECT:
        endpoint_.send(fmt::format(disconnect, content));
        return;
    case QUERY::SENSOR_CONNECTION_STATUS:
        endpoint_.send(fmt::format(sensor_connection_status, content));
        return;
    case QUERY::SUBSCRIBE_SENSOR:
        endpoint_.send(fmt::format(subscribe, content));
        return;
    case QUERY::UNSUBSCRIBE_SENSOR:
        endpoint_.send(fmt::format(unsubscribe, content));
        return;
    default:
        throw std::invalid_argument("unknown query type");
    }

}
