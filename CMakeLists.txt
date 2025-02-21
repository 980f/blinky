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
  cortexm/bitbanding.cpp
  cortexm/clockstarter.cpp
  cortexm/core_cm3.cpp
  cortexm/core_cmfunc.cpp
  cortexm/cortex-m3.cpp
  cortexm/nvic.cpp
  cortexm/systick.cpp
  cortexm/bitbanding.h
  cortexm/clocks.h
  cortexm/core_cm3.h
  cortexm/cruntime.h
  cortexm/nvic.h
  cortexm/msservice.h
  cortexm/systick.h
  cortexm/stm32/gpio.cpp
  cortexm/stm32/afio.cpp
  cortexm/stm32/pinconfigurator.cpp
  cortexm/stm32/serialport.cpp
  cortexm/stm32/stm32.cpp
  cortexm/stm32/uart.cpp
  ezcpp/sharedtimer.cpp

)

include_directories(".")

#it would be nice if we could define the things herein sooner, but I haven't figured out how to forward reference the source list.
include(cortexm/postamble.cmake)
