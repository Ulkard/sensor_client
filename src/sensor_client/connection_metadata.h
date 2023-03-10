#pragma once

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>

#include <fmt/core.h>


typedef websocketpp::client<websocketpp::config::asio_client> WSClient;
enum class ConnectionState { CONNECTING, CONNECTED, FAILED, CLOSING, CLOSED };

class connection_metadata {
public:
    typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;


    connection_metadata(websocketpp::connection_hdl hdl, std::string uri)
      : m_hdl(hdl)
      , m_status(ConnectionState::CONNECTING)
      , m_uri(uri)
      , m_server("N/A")
    {}

    void on_open(WSClient * c, websocketpp::connection_hdl hdl) {
        m_status = ConnectionState::CONNECTED;

        WSClient::connection_ptr con = c->get_con_from_hdl(hdl);
        m_server = con->get_response_header("Server");
    }

    void on_fail(WSClient * c, websocketpp::connection_hdl hdl) {
        m_status = ConnectionState::FAILED;

        WSClient::connection_ptr con = c->get_con_from_hdl(hdl);
        m_server = con->get_response_header("Server");
        m_error_reason = con->get_ec().message();
    }

    void on_close(WSClient * c, websocketpp::connection_hdl hdl) {
        m_status = ConnectionState::CLOSED;
        WSClient::connection_ptr con = c->get_con_from_hdl(hdl);
        std::stringstream s;
        s << "close code: " << con->get_remote_close_code() << " ("
          << websocketpp::close::status::get_string(con->get_remote_close_code())
          << "), close reason: " << con->get_remote_close_reason();
        m_error_reason = s.str();
    }

    websocketpp::connection_hdl get_hdl() const {
        return m_hdl;
    }

    ConnectionState get_status() const {
        return m_status;
    }

    friend std::ostream & operator<< (std::ostream & out, connection_metadata const & data){
        out << "> URI: " << data.m_uri << "\n"
            << "> Status: " << static_cast<int>(data.m_status) << "\n"
            << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
            << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason) << "\n";
        return out;
    }
private:
    websocketpp::connection_hdl m_hdl;
    ConnectionState m_status;
    std::string m_uri;
    std::string m_server;
    std::string m_error_reason;
};

