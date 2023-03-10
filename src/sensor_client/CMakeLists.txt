set(PROJECT_LIB_NAME sensor_client_lib)


find_package(Boost COMPONENTS system thread)
find_package(cpprestsdk REQUIRED)
find_package(fmt REQUIRED)

set(HEADER_FILES
    sensor_client.h
    connection_metadata.h
    websocket_endpoint.h
)

set(SOURCE_FILES
    sensor_client.cpp)

add_library(
    ${PROJECT_LIB_NAME} STATIC
    ${HEADER_FILES} ${SOURCE_FILES}
)


target_compile_definitions(${PROJECT_LIB_NAME} PUBLIC _WEBSOCKETPP_CPP11_STL_)

# Specify libraries to link a library or executable target against
target_link_libraries(
    ${PROJECT_LIB_NAME}
    PRIVATE Boost::system
    PRIVATE Boost::thread
    PRIVATE cpprestsdk::cpprest
    PRIVATE fmt
)

