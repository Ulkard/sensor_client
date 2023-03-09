#pragma once

#include "sensor_client/sensor_client.h"
#include "fmt/core.h"


void test_connection(){
    Client client("localhost", 9002);
    client.connect();
    sleep(10);
    client.disconnect();
}

void run_tests() {
    test_connection();
    fmt::print("tests OK\n");
}
