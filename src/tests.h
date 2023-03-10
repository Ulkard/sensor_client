#pragma once

#include "sensor_client/sensor_client.h"
#include "fmt/core.h"


void test_connection(){
    Client client("localhost", 9002);
    client.connect();
    sleep(5);
    client.disconnect();
    sleep(5);
}

void sequential_calls(){
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

void run_tests() {
    test_connection();
    //sequential_calls();
    fmt::print("tests OK\n");
}
