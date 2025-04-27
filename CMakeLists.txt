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

project("${R2P2_NAME}-${MSC_NAME}-${R2P2_VERSION}-${R2P2_BUILDDATE}-${R2P2_REVISION}")

add_compile_options(-Wall
  # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
  -Wno-format
  -Wno-unused-function
  -ffunction-sections
  -fdata-sections
  -fshort-enums
  -mthumb
)

if(PICORUBY_DEBUG)
  add_compile_definitions(PICORUBY_DEBUG=1)
  add_compile_options(
    -O0
    -g3
  )
else()
  add_compile_definitions(NDEBUG=1)
endif()

file(GLOB SOURCE_FILES CONFIGURE_DEPENDS
  src/*.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/*/ports/rp2040/*.c
)
if(USE_WIFI)
  add_compile_definitions(USE_WIFI=1)
  file(GLOB ADD_WIFI_SOURCE_FILES CONFIGURE_DEPENDS
    ${PICO_SDK_PATH}/src/rp2_common/pico_mbedtls/pico_mbedtls.c
  )
  list(APPEND SOURCE_FILES ${ADD_WIFI_SOURCE_FILES})
else()
  file(GLOB REMOVE_WIFI_SRCS CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-cyw43/ports/rp2040/*.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-net/ports/rp2040/*.c
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-ble/ports/rp2040/*.c
    ${PICO_SDK_PATH}/src/rp2_common/pico_mbedtls/pico_mbedtls.c
  )
  list(REMOVE_ITEM SOURCE_FILES ${REMOVE_WIFI_SRCS})
endif()

add_executable(${PROJECT_NAME}
  ${SOURCE_FILES}
)

if(PICO_BOARD STREQUAL "pico" OR PICO_BOARD STREQUAL "pico_w")
  add_compile_definitions(PICO_RP2040=1)
  target_compile_definitions(${PROJECT_NAME} PRIVATE FLASH_TARGET_OFFSET=0x00140000)
elseif(PICO_BOARD STREQUAL "pico2" OR PICO_BOARD STREQUAL "pico2_w")
  add_compile_definitions(PICO_RP2350=1)
  target_compile_definitions(${PROJECT_NAME} PRIVATE FLASH_TARGET_OFFSET=0x00340000)
endif()

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

target_link_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/${MRUBY_CONFIG}/lib
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
  pico_aon_timer
  mruby
)

# Workaround for Pico2
if(PICO_BOARD STREQUAL "pico" OR PICO_BOARD STREQUAL "pico_w")
  target_link_libraries(${PROJECT_NAME} PRIVATE
    hardware_sleep
    hardware_rtc
  )
endif()

if(USE_WIFI)
  # these are used only in WiFi build
  target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_cyw43_arch_lwip_threadsafe_background
    pico_btstack_ble
    pico_btstack_cyw43
  )
  target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-ble/include
  )
endif()

if(PICORUBY_MSC_FLASH)
  add_compile_definitions(PICORUBY_MSC_FLASH=1)
elseif(PICORUBY_MSC_SD)
  add_compile_definitions(PICORUBY_MSC_SD=1)
else()
  message(FATAL_ERROR "PICORUBY_MSC_xxxx is not defined.")
endif()

if(PICORB_VM_MRUBY)
  add_compile_definitions(PICORB_VM_MRUBY=1)
  target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-mruby/include
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-mruby/lib/mruby/include
  )
elseif(PICORB_VM_MRUBYC)
  add_compile_definitions(PICORB_VM_MRUBYC=1)
  target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-mrubyc/lib/mrubyc/src
  )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/${BUILD_DIR}/mrb
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/repos/${MRUBY_CONFIG}/mruby-compiler2/lib/prism/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/repos/${MRUBY_CONFIG}/mruby-compiler2/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/${MRUBY_CONFIG}/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-net/include

  ${CMAKE_CURRENT_LIST_DIR}
  ${PICO_SDK_PATH}/src/rp2_common/pico_stdio_usb/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-machine/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-filesystem-fat/lib/ff14b/source
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/${MRUBY_CONFIG}/mrbgems
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-mbedtls/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-mbedtls/mbedtls/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-mbedtls/mbedtls/library
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-net/include
)

pico_add_extra_outputs(${PROJECT_NAME})

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND gzip -kf ${PROJECT_NAME}.uf2
  COMMAND zip -r ${PROJECT_NAME}.uf2.zip ${PROJECT_NAME}.uf2
)

