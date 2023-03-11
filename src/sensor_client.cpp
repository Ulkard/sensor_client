#include "sensor_client.h"

#include <fmt/core.h>

#define MAX_SENSOR_NAME_SIZE 40


using Json = web::json::value;
using namespace web::web_sockets::client;

void Client::connect()
{
    fmt::print("connecting uri( {} )", uri_);
    ws_client_.connect(uri_).wait();
    send(QUERY::CLIENT_CONNECT, "");
}

void Client::disconnect()
{
    send(QUERY::CLIENT_DISCONNECT, "");
    ws_client_.close().wait();
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

void Client::onMessage(const ws_client::websocket_incoming_message& msg)
{
    if (msg.message_type() != websocket_message_type::text_message) {
        return;
    }

    const Json json_msg = Json::parse(msg.extract_string().get());

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
}

void Client::onClose(websocket_close_status close_status, const utility::string_t &reason, const std::error_code &error)
{
    fmt::print("connection closed with status {},\n\t reason: {}, \n\t error: {}\n", close_status, reason, error.message());
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

    websocket_outgoing_message out_msg;
    switch (type) {
    case QUERY::CLIENT_CONNECT:
        out_msg.set_utf8_message(fmt::format(connect, content));
        return;
    case QUERY::CLIENT_DISCONNECT:
        out_msg.set_utf8_message(fmt::format(disconnect, content));
        return;
    case QUERY::SENSOR_CONNECTION_STATUS:
        out_msg.set_utf8_message(fmt::format(sensor_connection_status, content));
        return;
    case QUERY::SUBSCRIBE_SENSOR:
        out_msg.set_utf8_message(fmt::format(subscribe, content));
        return;
    case QUERY::UNSUBSCRIBE_SENSOR:
        out_msg.set_utf8_message(fmt::format(unsubscribe, content));
        return;
    default:
        throw std::invalid_argument("unknown query type");
    }

    ws_client_.send(out_msg).wait();
}
