#pragma once

#include "connection_metadata.h"

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <cstdlib>

#include <map>
#include <string>
#include <sstream>

class websocket_endpoint {
public:
    websocket_endpoint () {
        //m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
        //m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

        m_endpoint.init_asio();
        m_endpoint.start_perpetual();

        //TODO: check possibility to go out from boost to std
        m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&WSClient::run, &m_endpoint);
    }

    ~websocket_endpoint() {
        m_endpoint.stop_perpetual();
        close(websocketpp::close::status::going_away, "endpoint destruction");
        m_thread->join();
    }

    void connect(std::string const & uri) {
        if (metadata_ptr_ && metadata_ptr_->get_status() == ConnectionState::CONNECTED) {
            return;
        }

        // error_code-like overload only?
        websocketpp::lib::error_code ec;
        WSClient::connection_ptr con = m_endpoint.get_connection(uri, ec);
        if (ec) {
            throw websocketpp::exception("init connection failed: " + ec.message());
        }

        metadata_ptr_ = websocketpp::lib::make_shared<connection_metadata>(con->get_handle(), uri);

        con->set_open_handler(websocketpp::lib::bind(
                                  &connection_metadata::on_open,
                                  metadata_ptr_,
                                  &m_endpoint,
                                  websocketpp::lib::placeholders::_1
                                  ));
        con->set_fail_handler(websocketpp::lib::bind(
                                  &connection_metadata::on_fail,
                                  metadata_ptr_,
                                  &m_endpoint,
                                  websocketpp::lib::placeholders::_1
                                  ));
        con->set_close_handler(websocketpp::lib::bind(
                                   &connection_metadata::on_close,
                                   metadata_ptr_,
                                   &m_endpoint,
                                   websocketpp::lib::placeholders::_1
                                   ));

        m_endpoint.connect(con);
    }

    void disconnect() {
        close(websocketpp::close::status::going_away, "user request");
    }

    template<typename F>
    void setMessageCallback(F msg_callback) {
        WSClient::connection_ptr con = m_endpoint.get_con_from_hdl(metadata_ptr_->get_hdl());
        con->set_message_handler(msg_callback);
    }

    void close(websocketpp::close::status::value code, std::string reason) {
        //TODO: try open/close connection in closening state
        std::cout << "> Closing connection : " << reason << std::endl;
        if (!metadata_ptr_ || metadata_ptr_->get_status() != ConnectionState::CONNECTED) {
            // Only close open connections
            return;
        }

        m_endpoint.close(metadata_ptr_->get_hdl(), code, reason);
    }

    void send(std::string message) {
        if (!metadata_ptr_) {
            std::cout << "> No connection found " << std::endl;
            return;
        }

        m_endpoint.send(metadata_ptr_->get_hdl(), message, websocketpp::frame::opcode::text);
    }

    connection_metadata::ptr get_metadata() const {
        return metadata_ptr_;
    }

private:
    WSClient m_endpoint;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

    connection_metadata::ptr metadata_ptr_;
};
