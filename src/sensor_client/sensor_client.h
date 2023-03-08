#pragma once

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <string>


class BadHttpCodeException : public std::runtime_error {
public:
    explicit BadHttpCodeException(const std::string& str)
        : std::runtime_error(str)
    {
    }
};
