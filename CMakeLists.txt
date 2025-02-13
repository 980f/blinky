cmake_minimum_required(VERSION 3.30)
project(blinky)

set(CMAKE_CXX_STANDARD 20)

set(CortexmVendor stm32)
set(VendorPartname f411)
set(VendorVariant ce)
set(BoardName blackpill)

include(cortexm/cortexm-all.cmake)

set(PROJECT_SPECIFIC_SOURCES
  blinky.cpp
  cortexm/stm32/blackpill.cpp
)
#it would be nice if we could define the things herein sooner, but I haven't figured out how to forward reference the source list.
include(cortexm/postamble.cmake)
