cmake_minimum_required(VERSION 3.16)
project(o_du_daps LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# DPDK via pkg-config (common on Ubuntu with dpdk-dev)
find_package(PkgConfig REQUIRED)
pkg_check_modules(DPDK REQUIRED libdpdk)

add_executable(o_du_daps
  src/main.cpp
  src/common/Log.cpp
  src/dpdk/DpdkEal.cpp
  src/dpdk/DpdkPort.cpp
  src/dpdk/MbufPool.cpp
  src/f1/GtpU.cpp
  src/f1/F1uDataplane.cpp
  src/f1/SctpLite.cpp
  src/f1/F1cControlplane.cpp
  src/du/UEContextManager.cpp
  src/du/DapsManager.cpp
  src/rlc/RlcEntity.cpp
  src/rlc/RlcBearer.cpp
  src/mac/MacScheduler.cpp
  src/mac/MacDapsGlue.cpp
)

target_include_directories(o_du_daps PRIVATE include ${DPDK_INCLUDE_DIRS})
target_compile_options(o_du_daps PRIVATE ${DPDK_CFLAGS_OTHER} -O2 -Wall -Wextra -Wpedantic)
target_link_libraries(o_du_daps PRIVATE ${DPDK_LIBRARIES})
target_link_directories(o_du_daps PRIVATE ${DPDK_LIBRARY_DIRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(o_du_daps PRIVATE Threads::Threads)
