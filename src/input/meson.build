#src/input/meson.build

input_files = files([])

input_inc = include_directories('.')
input_lib = static_library(
  'input',
  input_files,
  dependencies: []

input_dep = declare_dependency(
  link_with: input_lib,
  include_directories: input_inc
)

source += input_files
inc += input_inc
