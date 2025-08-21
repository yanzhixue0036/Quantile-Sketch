cmake_minimum_required(VERSION 3.15)

# set the project name
project(DDsketch VERSION 3.0.0)

# add the executable
set(SRC_LIST main.cpp DDsketch/DDsketch.cpp)
add_executable(${PROJECT_NAME} ${SRC_LIST})
find_package(Threads REQUIRED)
target_link_libraries(DDsketch PRIVATE Threads::Threads)

configure_file(FMSConfig.h.in FMSConfig.h)
string(TIMESTAMP COMPILE_TIME %Y%m%d-%H%M%S)

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${PROJECT_BINARY_DIR}
                           )

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-g")