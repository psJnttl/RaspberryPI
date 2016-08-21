# https://cmake.org/cmake/help/v2.8.12/cmake.html
# https://cmake.org/cmake-tutorial/
# https://cmake.org/Wiki/CMake_Useful_Variables

cmake_minimum_required (VERSION 2.8)  # Ubuntu 14.04 LTS
set (PROJECT_NAME PDC8544scroller)

project (${PROJECT_NAME} C ASM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1")   # low compiler optimisation
# http://stackoverflow.com/questions/5179202/gcc-g-what-will-happen
# http://stackoverflow.com/questions/89603/how-does-the-debugging-option-g-change-the-binary-executable
# https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html#Debugging-Options
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )  # adds debugging information to executable
## https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles") # Do not use the standard system startup files when linking.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -o kernel.elf")


# Linker script is at the project root where configuration was run.
# The script is needed to align memory more efficiently.
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-T,${CMAKE_CURRENT_BINARY_DIR}/linkerScript.rpi")

# 1st entry has dibs on 0x8000
add_executable(${PROJECT_NAME}
startup.S
kernel.c
gpio.c
sysTimer.c
interrupts.c
uart.c
pdc8544.c
spi.c
pwm.c
armTimer.c
)

add_custom_command( TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMENT "ELF to binary: kernel.img"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_OBJCOPY} ./${PROJECT_NAME} -O binary ./kernel.img
)
add_custom_command( TARGET ${PROJECT_NAME}
    COMMENT "Disassemble: kernel.asm"
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_OBJDUMP} ./${PROJECT_NAME} -D > ./kernel.asm
)
add_custom_command( TARGET ${PROJECT_NAME}
    COMMENT "list symbols: kernel.nm"
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_NM} ./${PROJECT_NAME} > ./kernel.nm
)
add_custom_command( TARGET ${PROJECT_NAME}
    COMMENT "intel hex format: kernel.hex"
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_OBJCOPY} ./${PROJECT_NAME} -O ihex ./kernel.hex
)
