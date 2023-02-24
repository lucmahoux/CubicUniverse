#src/file_system/meson.build

file_system_files = files(['fs.c', 'fs.h'])

file_system_inc = include_directories('.')
file_system_lib = static_library(
  'file_system',
  file_system_files
)

file_system_dep = declare_dependency(
  link_with: file_system_lib,
  include_directories: file_system_inc
)

source += file_system_files
inc += file_system_inc
