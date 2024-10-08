cmake_minimum_required(VERSION 3.15)
project(DigiCron LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

file(GLOB EPOXY_SOURCES
    ${CMAKE_SOURCE_DIR}/devdeps/EpoxyDuino/cores/epoxy/*.cpp
    ${CMAKE_SOURCE_DIR}/devdeps/EpoxyDuino/cores/epoxy/*.h
)

file(GLOB HCMS_SOURCES
    ${CMAKE_SOURCE_DIR}/lib/HCMS39xx/src/font5x7.h
)

file(GLOB WASM3_SOURCES
    ${CMAKE_SOURCE_DIR}/lib/wasm3-arduino/src/*.c
    ${CMAKE_SOURCE_DIR}/lib/wasm3-arduino/src/*.h
)

add_library(EpoxyDuino STATIC ${EPOXY_SOURCES})
add_library(HCMS39xx STATIC ${HCMS_SOURCES})
add_library(Wasm3 STATIC ${WASM3_SOURCES})

set_target_properties(HCMS39xx PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(EpoxyDuino PUBLIC ${CMAKE_SOURCE_DIR}/devdeps/EpoxyDuino/cores/epoxy)
target_include_directories(HCMS39xx PUBLIC ${CMAKE_SOURCE_DIR}/lib/HCMS39xx/src)
target_include_directories(Wasm3 PUBLIC ${CMAKE_SOURCE_DIR}/lib/wasm3-arduino/src)

file(GLOB SOURCES
    ${CMAKE_SOURCE_DIR}/firmware/*.cpp
)

add_executable(digicron ${SOURCES})

add_definitions(
    -DDC_SIMULATOR
)

target_compile_options(digicron PRIVATE
    -Iapptest
    -Wno-main
    -Wno-unused-command-line-argument
    -Wno-delete-incomplete
)

set_target_properties(digicron PROPERTIES LINK_FLAGS "--bind --embed-file rootfs@/")

target_include_directories(digicron PRIVATE ${CMAKE_SOURCE_DIR}/devdeps/EpoxyDuino/cores/epoxy)
target_include_directories(HCMS39xx PUBLIC ${CMAKE_SOURCE_DIR}/lib/HCMS39xx/src)
target_include_directories(Wasm3 PUBLIC ${CMAKE_SOURCE_DIR}/lib/wasm3-arduino/src)

target_link_libraries(digicron PRIVATE EpoxyDuino HCMS39xx Wasm3)