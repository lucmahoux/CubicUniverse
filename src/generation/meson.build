#src/generation/meson.build

generation_files = files([
    'noise.c', 'noise.h'
])

generation_inc = include_directories('.')
generation_lib = static_library(
  'generation',
  generation_files,
  dependencies: [utils_dep]
)

generation_dep = declare_dependency(
  link_with: generation_lib,
  include_directories: generation_inc
)

source += generation_files
inc += generation_inc
