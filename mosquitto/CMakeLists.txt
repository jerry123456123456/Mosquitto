cmake_minimum_required(VERSION 2.8)

project(mqtt-cjson)
# 设置编译参数
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
include_directories(/usr/include/jsoncpp)

include_directories(/usr/local/include/)
add_executable(pub pub.cpp)
target_link_libraries(pub mosquitto jsoncpp)

add_executable(sub sub.cpp)
target_link_libraries(sub mosquitto cjson)

add_executable(01-will-set 01-will-set.cpp)
target_link_libraries(01-will-set mosquitto)


add_executable(emqx_pub emqx_pub.cpp)
target_link_libraries(emqx_pub mosquitto jsoncpp)


add_executable(emqx_sub emqx_sub.cpp)
target_link_libraries(emqx_sub mosquitto cjson)