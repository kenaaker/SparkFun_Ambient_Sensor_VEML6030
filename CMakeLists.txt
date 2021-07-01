cmake_minimum_required(VERSION 3.5)

project(cli_light_sensor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cli_light_sensor SparkFun_VEML6030_Ambient_Light_Sensor.cpp cli_light_sensor.cpp)
