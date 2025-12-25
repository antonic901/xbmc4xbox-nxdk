cmake_minimum_required(VERSION 3.18)
project(fribidi LANGUAGES C)

add_library(fribidi
  lib/fribidi.c
  lib/fribidi-bidi-types.c
  lib/fribidi-brackets.c
  lib/fribidi-bidi.c
  lib/fribidi-run.c
  lib/fribidi-joining-types.c
  lib/fribidi-joining.c
  lib/fribidi-shape.c
  lib/fribidi-arabic.c
  lib/fribidi-mirroring.c
)

add_definitions(
  -DHAVE_CONFIG_H
  -DFRIBIDI_LIB_STATIC
)

target_include_directories(fribidi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
