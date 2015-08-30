# set the arm toolchain
set(CMAKE_TOOLCHAIN_FILE mbed-cmake/mbed-cmake/arm_toolchain.cmake)

cmake_minimum_required(VERSION 3.0.1)

project (WifiLedController C CXX ASM)

# ------------------------------------------------------------------------------
# set the target processor and include settings for cmake
set(MBED_TARGET NUCLEO_F401RE)

# ------------------------------------------------------------------------------
# will add the include dirs and link the lib
#set(USE_RTOS true)
#set(USE_NET true)
#set(USE_USB true)
#set(USE_DSP true)

# ------------------------------------------------------------------------------
# build the executable
set(BLINKLED_SRC
  src/Main.cpp
  src/CurrentSensor.cpp
  src/Temperature.cpp
  src/LedControl.cpp
  src/EspInterface.cpp
  src/Error.cpp
  src/MqttInterface.cpp
)

set(BIN
  WifiLedController
)

include(mbed-cmake/arm_mbed.cmake)

# ------------------------------------------------------------------------------
# build the executable
add_executable(${BIN} ${BLINKLED_SRC} ${MBED_OBJECTS})
target_link_libraries(${BIN} ${MBED_LIBS})
