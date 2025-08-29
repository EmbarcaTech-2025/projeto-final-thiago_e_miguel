# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(tracking-trilha C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(tracking-trilha main.cpp 
    src/drivers/oximeter/MAX3010X.cpp
    src/drivers/oximeter/algorithm_by_RF.cpp
    src/drivers/accelerometer/imu6050.cpp
    src/sensors/oximeter.cpp
    src/sensors/accelerometer.cpp
    src/utils/utils.cpp
    src/state/state.cpp
    src/state/state_collect.cpp
    src/analyzer/analyzer.cpp
    src/drivers/display_oled/ssd1306_i2c.cpp
    src/drivers/display_oled/display_oled.cpp
    src/display/oled.cpp
)

pico_set_program_name(tracking-trilha "tracking-trilha")
pico_set_program_version(tracking-trilha "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(tracking-trilha 0)
pico_enable_stdio_usb(tracking-trilha 1)

# Add the standard library to the build
target_link_libraries(tracking-trilha
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(tracking-trilha PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/include/drivers
        ${CMAKE_CURRENT_LIST_DIR}/include/drivers/oximeter
        ${CMAKE_CURRENT_LIST_DIR}/include/drivers/accelerometer
        ${CMAKE_CURRENT_LIST_DIR}/include/sensors
        ${CMAKE_CURRENT_LIST_DIR}/include/utils
        ${CMAKE_CURRENT_LIST_DIR}/include/state
        ${CMAKE_CURRENT_LIST_DIR}/include/analyzers
        ${CMAKE_CURRENT_LIST_DIR}/include/drivers/display_oled
        ${CMAKE_CURRENT_LIST_DIR}/include/display
)

# Add any user requested libraries 
target_link_libraries(tracking-trilha 
        hardware_i2c
        
        )

pico_add_extra_outputs(tracking-trilha)

