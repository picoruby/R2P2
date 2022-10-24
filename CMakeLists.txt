cmake_minimum_required(VERSION 3.24)

#set(CMAKE_VERBOSE_MAKEFILE ON)

include(pico_sdk_import.cmake)

project(prsh-rp2)

#set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
  # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
  -Wno-format
  -Wno-unused-function
  -Wno-maybe-uninitialized
)

add_executable(${PROJECT_NAME}
  ${CMAKE_SOURCE_DIR}/src/hal.c
  ${CMAKE_SOURCE_DIR}/src/main.c
  ${CMAKE_SOURCE_DIR}/src/usb_descriptors.c
  ${CMAKE_SOURCE_DIR}/src/flash_disk.c
  ${CMAKE_SOURCE_DIR}/src/io_console.c
  ${CMAKE_SOURCE_DIR}/src/msc_disk.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/prsh_arm-none-eabi/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  pico_stdlib
  tinyusb_device
  tinyusb_board
  mruby
)

include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${PICO_SDK_PATH}/src/rp2_common/pico_stdio_usb/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/include/hal_no_impl
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-mrubyc/repos/mrubyc/src
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-filesystem-fat/lib/ff14b/source
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/prsh_arm-none-eabi/mrbgems
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/repos/prsh_arm-none-eabi/mruby-pico-compiler/include
)

pico_add_extra_outputs(${PROJECT_NAME})

