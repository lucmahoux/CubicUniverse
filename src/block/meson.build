#src/block/meson.build

block_files = files([
  'block_renderer.c', 'block_renderer.h',
  'block_utils.c', 'block_utils.h',
  'block_list_loader.c', 'block_list_loader.h',
  'blockstates.c', 'blockstates.h'
])

block_inc = include_directories('.')
block_lib = static_library(
  'block',
  block_files,
  dependencies: [utils_dep, render_dep]
)

block_dep = declare_dependency(
  link_with: block_lib,
  include_directories: block_inc,
)

source += block_files
inc += block_inc
