cmake_minimum_required(VERSION 3.18)
project(fstrcmp LANGUAGES C)

add_library(fstrcmp
  lib/fstrcmp.c
  lib/fmemcmp.c
  lib/nmalloc.c
)

target_include_directories(fstrcmp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/lib)
