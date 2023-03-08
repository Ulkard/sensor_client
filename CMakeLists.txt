cmake_minimum_required(VERSION 3.5)

project(sensor_client_test)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -Wno-error=unused-variable \
    -Wcast-align -Wcast-qual -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual \
    -Wredundant-decls -Wconversion -Wno-sign-conversion -Wswitch-default -Wundef")
        
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op -Wnoexcept -Wstrict-null-sentinel")

    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10 AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
        message(WARNING "Disabling noexcept warning error in g++-9 due to compiler bug.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=noexcept")
    endif()
endif()


add_subdirectory(src/sensor_client)


set(HEADER_FILES
    src/tests.h
)

set(SOURCE_FILES
    src/main.cpp)

add_executable(
    ${PROJECT_NAME}
    ${HEADER_FILES} ${SOURCE_FILES}
)


target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE fmt
    PRIVATE sensor_client_lib
)

