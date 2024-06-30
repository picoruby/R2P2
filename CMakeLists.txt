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

if(DEFINED ENV{PICO_W})
  set(PICO_BOARD pico_w)
  if(DEFINED ENV{PICO_W_NETWORK})
    set(R2P2_NAME R2P2_W_NET)
    set(BUILD_CONFIG r2p2_w_network-cortex-m0plus)
  else()
  set (R2P2_NAME R2P2_W)
    set(BUILD_CONFIG r2p2_w-cortex-m0plus)
  endif()
else()
  set (R2P2_NAME R2P2)
  set (BUILD_CONFIG r2p2-cortex-m0plus)
endif()

project("${R2P2_NAME}-${MSC_NAME}-${R2P2_VERSION}-${R2P2_BUILDDATE}-${R2P2_REVISION}")

add_compile_options(-Wall
  # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
  -Wno-format
  -Wno-unused-function
  -Wno-maybe-uninitialized
)

add_definitions(
  # These defines should not contradict defines in
  # picoruby/build_config/${BUILD_CONFIG}.rb
  -DNDEBUG
  -DMRBC_REQUIRE_32BIT_ALIGNMENT
  -DMRBC_CONVERT_CRLF
  -DMRBC_USE_FLOAT=2
  -DMRBC_USE_MATH
  -DMRBC_TICK_UNIT=1
  -DMRBC_TIMESLICE_TICK_COUNT=10
  -DMRBC_USE_FLOAT=2
)

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
)
if(DEFINED ENV{PICO_W})
  list(APPEND SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-cyw43/ports/rp2040/cyw43.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-ble/ports/rp2040/ble.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-ble/ports/rp2040/ble_peripheral.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-ble/ports/rp2040/ble_central.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-net/ports/rp2040/net.c
  )
  if(DEFINED ENV{PICO_W_NETWORK})
  list(APPEND SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-net/ports/rp2040/net.c
  )
  endif()
endif()
add_executable(${PROJECT_NAME}
  ${SOURCE_FILES}
)

set(PICORBC ${CMAKE_SOURCE_DIR}/lib/picoruby/bin/picorbc)

add_custom_target(mrbdir ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/mrb
)

set(RUBY_FILES r2p2_task usb_task)

add_dependencies(${PROJECT_NAME}
  mrbdir
  ${RUBY_FILES}
)

foreach(rb ${RUBY_FILES})
add_custom_target(${rb}
  COMMAND ${PICORBC} -B${rb} -o${CMAKE_SOURCE_DIR}/build/mrb/${rb}.c ${CMAKE_SOURCE_DIR}/mrblib/${rb}.rb
)
endforeach(rb)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/${BUILD_CONFIG}/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  pico_stdlib
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
  hardware_sleep
  hardware_rtc
  mruby
)

if(DEFINED ENV{PICO_W})
  # these libraries are both used in base PICO_W build and the network-enabled build
  target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_btstack_ble
    pico_btstack_cyw43
  )
  # add target_link_libraries for networking if networking is enabled
  if(DEFINED ENV{PICO_W_NETWORK})
    target_link_libraries(${PROJECT_NAME} PRIVATE
      pico_cyw43_arch_lwip_threadsafe_background
      pico_lwip_mbedtls
      pico_mbedtls
    )
    target_include_directories(${PROJECT_NAME} PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/include-net
    )
  endif()
  target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-ble/include
  )
endif()


target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${PICO_SDK_PATH}/src/rp2_common/pico_stdio_usb/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/include/hal_no_impl
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-mrubyc/repos/mrubyc/src
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-filesystem-fat/lib/ff14b/source
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/${BUILD_CONFIG}/mrbgems
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/repos/${BUILD_CONFIG}/mruby-pico-compiler/include
)

pico_add_extra_outputs(${PROJECT_NAME})

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND gzip -kf ${PROJECT_NAME}.uf2
  COMMAND zip -r ${PROJECT_NAME}.uf2.zip ${PROJECT_NAME}.uf2
)

