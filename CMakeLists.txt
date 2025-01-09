cmake_minimum_required(VERSION 3.24)

#set(CMAKE_VERBOSE_MAKEFILE ON)

include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

# Initialize the SDK
pico_sdk_init()

execute_process (COMMAND date +%Y%m%d OUTPUT_VARIABLE CMAKE_BUILDDATE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE CMAKE_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND git describe --tags --abbrev=0 OUTPUT_VARIABLE CMAKE_VERSION_NUMBER OUTPUT_STRIP_TRAILING_WHITESPACE)
set (R2P2_VERSION   ${CMAKE_VERSION_NUMBER})
set (R2P2_BUILDDATE ${CMAKE_BUILDDATE})
set (R2P2_REVISION  ${CMAKE_REVISION})

if(DEFINED ENV{PICORUBY_MSC_FLASH})
  set (MSC_NAME FLASH_MSC)
  add_definitions(-DPICORUBY_MSC_FLASH)
  remove_definitions(-DPICORUBY_MSC_SD)
elseif(DEFINED ENV{PICORUBY_MSC_SD})
  set (MSC_NAME SD_MSC)
  add_definitions(-DPICORUBY_MSC_SD)
  remove_definitions(-DPICORUBY_MSC_FLASH)
else()
  message(FATAL_ERROR "PICORUBY_MSC_xxxx is not defined.")
endif()

if((DEFINED ENV{PICO_WIFI}) AND DEFINED ENV{PICO_BLE})
  message(FATAL_ERROR "Both BLE and WiFi cannot be enabled for Pico W")
elseif(DEFINED ENV{PICO_WIFI})
  set(PICO_BOARD pico_w)
  set(R2P2_NAME R2P2_WIFI)
  set(BUILD_CONFIG r2p2_wifi-cortex-m0plus)
  set(BUILD_DIR build_pico_wifi)
elseif(DEFINED ENV{PICO_BLE})
  set(PICO_BOARD pico_w)
  set(R2P2_NAME R2P2_BLE)
  set(BUILD_CONFIG r2p2_ble-cortex-m0plus)
  set(BUILD_DIR build_pico_ble)
elseif(DEFINED ENV{PICO2})
  set(PICO_BOARD pico2)
  set(R2P2_NAME R2P2_PICO2)
  set(BUILD_CONFIG r2p2-cortex-m33)
  set(BUILD_DIR build_pico2)
  add_compile_options(
    -mcpu=cortex-m33
    #-march=armv8-m.main+fp+dsp
    #-mabi=aapcs-linux
    #-mfloat-abi=softfp
    -fno-strict-aliasing
    -fno-unroll-loops
    -mslow-flash-data
  )
else()
  set(R2P2_NAME R2P2)
  set(BUILD_CONFIG r2p2-cortex-m0plus)
  set(BUILD_DIR build_pico)
endif()

project("${R2P2_NAME}-${MSC_NAME}-${R2P2_VERSION}-${R2P2_BUILDDATE}-${R2P2_REVISION}")

add_compile_options(-Wall
  # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
  -Wno-format
  -Wno-unused-function
  -Wno-maybe-uninitialized
  -ffunction-sections
  -fdata-sections
  -fshort-enums
  -mthumb
)

add_definitions(
  # These defines should not contradict defines in
  # picoruby/build_config/${BUILD_CONFIG}.rb
  -DMRC_CUSTOM_ALLOC
  -DMRBC_REQUIRE_32BIT_ALIGNMENT
  -DMRBC_CONVERT_CRLF
  -DMRBC_USE_FLOAT=2
  -DMRBC_USE_MATH
  -DMRBC_TICK_UNIT=1
  -DMRBC_TIMESLICE_TICK_COUNT=10
)

if(DEFINED ENV{PICORUBY_DEBUG})
  add_definitions(
    -DPICORUBY_DEBUG
  )
else()
  add_definitions(
    -DNDEBUG
  )
endif()

if(DEFINED ENV{PICO_WIFI})
  add_definitions(
    -DUSE_WIFI
  )
endif()

file(GLOB SRCS src/*.c)
set(SOURCE_FILES
  ${SRCS}
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-filesystem-fat/ports/rp2040/flash_disk.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-filesystem-fat/ports/rp2040/sd_disk.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-filesystem-fat/ports/rp2040/msc_disk.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-gpio/ports/rp2040/gpio.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-i2c/ports/rp2040/i2c.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-spi/ports/rp2040/spi.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-adc/ports/rp2040/adc.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-uart/ports/rp2040/uart.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-pwm/ports/rp2040/pwm.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-machine/ports/rp2040/machine.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-io-console/ports/rp2040/io-console.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-watchdog/ports/rp2040/watchdog.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-rng/ports/rp2040/rng.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-env/ports/rp2040/env.c
)
# only used in BLE build
if(DEFINED ENV{PICO_BLE})
  list(APPEND SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-cyw43/ports/rp2040/cyw43.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-ble/ports/rp2040/ble.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-ble/ports/rp2040/ble_peripheral.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-ble/ports/rp2040/ble_central.c
  )
endif()
# only used in WiFi build
if(DEFINED ENV{PICO_WIFI})
  list(APPEND SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-cyw43/ports/rp2040/cyw43.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-net/ports/rp2040/common.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-net/ports/rp2040/dns.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-net/ports/rp2040/tcp.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-net/ports/rp2040/udp.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-net/ports/rp2040/mqtt.c
  )
endif()
add_executable(${PROJECT_NAME}
  ${SOURCE_FILES}
)

set(PICORBC ${CMAKE_SOURCE_DIR}/lib/picoruby/bin/picorbc)

add_custom_target(mrbdir ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/${BUILD_DIR}/mrb
)

set(RUBY_FILES main_task)

add_dependencies(${PROJECT_NAME}
  mrbdir
  ${RUBY_FILES}
)

foreach(rb ${RUBY_FILES})
add_custom_target(${rb}
  COMMAND ${PICORBC} -B${rb} -o${CMAKE_SOURCE_DIR}/${BUILD_DIR}/mrb/${rb}.c ${CMAKE_SOURCE_DIR}/mrblib/${rb}.rb
)
endforeach(rb)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/${BUILD_DIR}/mrb
  ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/${BUILD_CONFIG}/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  pico_stdlib
  # pico_stdio_usb
  pico_bootsel_via_double_reset
  tinyusb_device
  tinyusb_board
  hardware_flash
  hardware_adc
  hardware_uart
  hardware_i2c
  hardware_spi
  hardware_pwm
  pico_runtime
  mruby
)

# Workaround for Pico2
if(NOT DEFINED ENV{PICO2})
  target_link_libraries(${PROJECT_NAME} PRIVATE
    hardware_sleep
    hardware_rtc
  )
endif()

# btstack is used in BLE build
if(DEFINED ENV{PICO_BLE})
  target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_btstack_ble
    pico_btstack_cyw43
  )
  target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-ble/include
  )
endif()

if(DEFINED ENV{PICO_WIFI})
  # these are used only in WiFi build
  target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mbedtls
    pico_mbedtls
  )
  target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include-net
  )
#else()
#  # Even the BLE build needs a pico_cyw43_arch, only in a different flavor
#  target_link_libraries(${PROJECT_NAME} PRIVATE
#    pico_cyw43_arch_none
#  )
endif()


target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${PICO_SDK_PATH}/src/rp2_common/pico_stdio_usb/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-machine/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-mrubyc/lib/mrubyc/src
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-filesystem-fat/lib/ff14b/source
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/${BUILD_CONFIG}/mrbgems
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/repos/${BUILD_CONFIG}/mruby-compiler2/include
)

pico_add_extra_outputs(${PROJECT_NAME})

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND gzip -kf ${PROJECT_NAME}.uf2
  COMMAND zip -r ${PROJECT_NAME}.uf2.zip ${PROJECT_NAME}.uf2
)

