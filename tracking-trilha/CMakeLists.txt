# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICOTOOL_FORCE_FETCH_FROM_GIT ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.12)

# Set any variables required for importing libraries
if (DEFINED ENV{FREERTOS_PATH})
  SET(FREERTOS_PATH $ENV{FREERTOS_PATH})
else()
  SET(FREERTOS_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS)
endif()

message("FreeRTOS Kernel located in ${FREERTOS_PATH}")

# Set FreeRTOS port for RP2040
set(FREERTOS_PORT GCC_RP2040 CACHE STRING "FreeRTOS port name")

# Create freertos_config target
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM
  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include) # The config file directory
target_compile_definitions(freertos_config
  INTERFACE
    projCOVERAGE_TEST=0)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include(${FREERTOS_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(tracking-trilha C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(tracking-trilha main.cpp 
    src/drivers/oximeter/MAX3010X.cpp
    src/drivers/oximeter/algorithm_by_RF.cpp
    src/drivers/accelerometer/imu6050.cpp
    src/drivers/gps/GPS_GY-NEO6MV2.cpp
    src/sensors/oximeter.cpp
    src/sensors/accelerometer.cpp
    src/utils/utils.cpp
    src/state/state.cpp
    src/state/state_collect.cpp
    src/analyzer/analyzer.cpp
    src/drivers/display_oled/ssd1306_i2c.cpp
    src/sensors/gps.cpp
    src/drivers/display_oled/display_oled.cpp
    src/display/oled.cpp
    src/drivers/sd_card/hw_config.c
    src/drivers/sd_card/SD.cpp
    src/buzzer/buzzer.cpp
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
        ${CMAKE_CURRENT_LIST_DIR}/include/drivers/gps
        ${CMAKE_CURRENT_LIST_DIR}/include/sensors
        ${CMAKE_CURRENT_LIST_DIR}/include/utils
        ${CMAKE_CURRENT_LIST_DIR}/include/state
        ${CMAKE_CURRENT_LIST_DIR}/include/analyzers
        ${CMAKE_CURRENT_LIST_DIR}/include/drivers/display_oled
        ${CMAKE_CURRENT_LIST_DIR}/include/display
        ${CMAKE_CURRENT_LIST_DIR}/lib/sd/src/FreeRTOS+FAT+CLI/include
        ${CMAKE_CURRENT_LIST_DIR}/lib/sd/src/Lab-Project-FreeRTOS-FAT/include
        ${CMAKE_CURRENT_LIST_DIR}/lib/sd/src/FreeRTOS+FAT+CLI/portable/RP2040
        ${CMAKE_CURRENT_LIST_DIR}/include/drivers/sd_card
        ${CMAKE_CURRENT_LIST_DIR}/include/buzzer
)

# Add any user requested libraries 
target_link_libraries(tracking-trilha 
        pico_stdlib
        hardware_i2c
        hardware_spi
        hardware_rtc
        hardware_pio
        pico_time
        FreeRTOS-Kernel-Heap4
        FreeRTOS+FAT+CLI
        hardware_uart
        )

        target_compile_options(tracking-trilha PUBLIC 
        -Wall 
        -Wextra 
        -Wshadow 
        -Wstack-usage=2048 
        -fanalyzer 
      )
      target_compile_definitions(tracking-trilha PUBLIC
          # pico-sdk
          #PARAM_ASSERTIONS_ENABLE_ALL=1
          PICO_USE_STACK_GUARDS=1
          PICO_MALLOC_PANIC=1
          PICO_MAX_SHARED_IRQ_HANDLERS=6
      )
      target_compile_definitions(tracking-trilha PRIVATE
          USE_PRINTF
      )
      
      set_property(TARGET tracking-trilha APPEND_STRING PROPERTY LINK_FLAGS
          "-Wl,--print-memory-usage"
      )
      

pico_add_extra_outputs(tracking-trilha)

# if you have anything in "lib" folder then uncomment below - remember to add a CMakeLists.txt
# file to the "lib" directory
add_subdirectory(lib/)

