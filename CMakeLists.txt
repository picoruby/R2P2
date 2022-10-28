cmake_minimum_required(VERSION 3.24)

#set(CMAKE_VERBOSE_MAKEFILE ON)

include(pico_sdk_import.cmake)

project(R2P2)

#set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
  # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
  -Wno-format
  -Wno-unused-function
  -Wno-maybe-uninitialized
)

add_definitions(
  -DNDEBUG
  -DMRBC_REQUIRE_32BIT_ALIGNMENT
  -DMRBC_CONVERT_CRLF
  -DMRBC_USE_MATH
)

file(GLOB SRCS src/*.c)
add_executable(${PROJECT_NAME} ${SRCS})

set(PICORBC ${CMAKE_SOURCE_DIR}/lib/picoruby/bin/picorbc)

add_custom_target(mrbdir ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/mrb
)

set(RUBY_FILES prsh_task usb_task)

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
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/prsh-cortex-m0plus/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  pico_stdlib
  pico_bootsel_via_double_reset
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
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/prsh-cortex-m0plus/mrbgems
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/repos/prsh-cortex-m0plus/mruby-pico-compiler/include
)

pico_add_extra_outputs(${PROJECT_NAME})

