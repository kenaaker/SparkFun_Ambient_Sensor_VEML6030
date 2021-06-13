cmake_minimum_required(VERSION 3.5)

project(display_i2c_light_sensor LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)

set(PROJECT_SOURCES
        display_i2c_light_sensor_main.cpp
        display_i2c_light_sensor.cpp
        display_i2c_light_sensor.h
        display_i2c_light_sensor.ui
	SparkFun_VEML6030_Ambient_Light_Sensor.cpp
	SparkFun_VEML6030_Ambient_Light_Sensor.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(display_i2c_light_sensor
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(display_i2c_light_sensor SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(display_i2c_light_sensor
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(display_i2c_light_sensor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)
