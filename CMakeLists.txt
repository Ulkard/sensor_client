set(PROJECT_LIB_NAME sensor_client_lib)


find_package(Boost COMPONENTS system)
find_package(cpprestsdk REQUIRED)
find_package(fmt REQUIRED)

set(HEADER_FILES
    src/sensor_client.h
)

set(SOURCE_FILES
    src/sensor_client.cpp)

add_library(
    ${PROJECT_LIB_NAME} STATIC
    ${HEADER_FILES} ${SOURCE_FILES}
)

# Specify libraries to link a library or executable target against
target_link_libraries(
    ${PROJECT_LIB_NAME}
    PRIVATE Boost::system
    PRIVATE cpprestsdk::cpprest
    PRIVATE fmt
)

