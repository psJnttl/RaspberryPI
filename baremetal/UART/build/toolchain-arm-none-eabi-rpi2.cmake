# assuming toolchain is included in PATH

# https://cmake.org/cmake/help/v2.8.12/cmake.html#module:CMakeForceCompiler
include(CMakeForceCompiler)

# https://cmake.org/cmake/help/v2.8.12/cmake.html#command:set
# The Generic system name is used for embedded targets (targets without OS) in
# CMake
set(CMAKE_SYSTEM_NAME  Generic)      # Not Linux
set(CMAKE_SYSTEM_PROCESSOR  ARMV7L)  # cross compiling so not the same as CMAKE_HOST_SYSTEM_PROCESSOR
# uname -r on Raspbian Wheezy returns 'unknown', but it's Broadcom's 2836 chip
# -m gives armv7l, could also use BCM2836

# The toolchain has a common prefix.
set(X_COMPILER_PREFIX arm-none-eabi-)

# Cross compiler used, not building standard Linux.
# https://cmake.org/cmake/help/v2.8.12/cmake.html#variable:CMAKE_LANG_COMPILER_ID
CMAKE_FORCE_C_COMPILER(${X_COMPILER_PREFIX}gcc GNU)

# CMakeCache.txt will have entries for CMAKE_NM, CMAKE_OBJCOPY, CMAKE_OBJDUMP
# as long as toolchain is in PATH. No need to set them here.

# ARM C compiler flags
# https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon-vfpv4")  # floating point HW
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard")  # FPU-specific calling conventions
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a")    # ARM architecture
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=cortex-a7")  # ARM processor which GCC should tune for
# One flag can be used to replace two:
# -mcpu=cortex-a7    vs   -march=armv7-a AND -mtune=cortex-a7 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-a7")  # ARM arch and the processor which GCC should tune for

# Won't end up in CMakeCache.txt, must push to cache
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")

# Add the raspberry-pi 2 definition so conditional compilation works
add_definitions(-DRPI2=1)
