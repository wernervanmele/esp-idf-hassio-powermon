cmake_minimum_required(VERSION 3.16.0)

# extra components for mdns component
set(EXTRA_COMPONENT_DIRS "/Users/wvanmele/Documents/PlatformIO/ESP-IDF/esp/esp-protocols/components/mdns")

# when you use this method you have to specify ALL component sused in your projects
# set(COMPONENTS main mqtt mdns)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(hassio_powermon_espidf)

# in case you wanrt some extra compile output
# idf_build_set_property(COMPILE_OPTIONS -Wall -Wextra -Werror APPEND)