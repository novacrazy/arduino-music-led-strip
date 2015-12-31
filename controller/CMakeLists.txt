cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME controller)
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("C:/Program Files (x86)/Arduino/libraries/FastLED")
include_directories("C:/Program Files (x86)/Arduino/libraries/LiquidCrystal")

set(${CMAKE_PROJECT_NAME}_SKETCH controller.ino)
set(${CMAKE_PROJECT_NAME}_BOARD mega)
set(${CMAKE_PROJECT_NAME}_PORT COM5)
set(mega.build.mcu atmega2560)
set(mega.upload.protocol wiring)
set(mega.upload.speed 115200)

generate_arduino_firmware(${CMAKE_PROJECT_NAME})
