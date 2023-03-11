#pragma once

#include "sensor_client/sensor_client.h"
#include "fmt/core.h"


void test_connection() {
    fmt::print("test_connection:\n");
    Client client("localhost", 9002);
    client.connect();
    sleep(5);
    client.disconnect();
    sleep(5);
}

void sequential_calls() {
    fmt::print("test_sequential_calls:\n");
    Client client("localhost", 9002);
    client.connect();
    client.connect();
    client.connect();
    sleep(5);
    client.disconnect();
    client.disconnect();
    client.disconnect();
    sleep(5);
}

void test_send() {
    fmt::print("test_send:\n");
    Client client("localhost", 9002);
    client.connect();
    sleep(5);
    client.subscribe("qwerty");
    client.subscribe("qwerty_2");
    sleep(2);
    client.unsubscribe("qwerty");
    sleep(3);
    client.disconnect();
    sleep(5);
}

void test_callback() {
    fmt::print("test_callback:\n");
    Client client("localhost", 9002);
    client.connect();
    sleep(3);
    std::string test_param_1("test_param");
    std::string test_param_2("another_param");
    client.registerCallback("CE7238J", [](const web::json::value& data){fmt::print(
                    "\n\tHello, im callback_1:\n\t{}\n", data.serialize());});
    client.registerCallback("CE7238J", [](const web::json::value& data, std::string test_param_1, std::string test_param_2){
        fmt::print("\n\tHello, im callback_2:\n\t{}\n\twith {} and {}\n",
                   data.serialize(), test_param_1, test_param_2);},
        test_param_1, test_param_2);
    sleep(10);
    client.removeCallback("CE7238J");
    sleep(5);
}

void run_tests() {
    test_connection();
    //sequential_calls();
    test_send();
    test_callback();
    fmt::print("tests OK\n");
}
