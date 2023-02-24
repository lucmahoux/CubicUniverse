#src/chunk/meson.build

chunk_files = files([
  'chunk.c', 'chunk.h',
  'chunk_utils.c', 'chunk_utils.h',
  'chunk_loader.c', 'chunk_loader.h',
  'chunk_renderer.c', 'chunk_renderer.h'
])

chunk_inc = include_directories('.')
chunk_lib = static_library(
  'chunk',
  chunk_files,
  dependencies: [render_dep, block_dep, utils_dep, file_system_dep]
)

chunk_dep = declare_dependency(
  link_with: chunk_lib,
  include_directories: chunk_inc
)

source += chunk_files
inc += chunk_inc
