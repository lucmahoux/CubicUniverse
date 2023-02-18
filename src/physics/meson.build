#src/physics/meson.build

physics_files = files([])

physics_inc = include_directories('.')
physics_lib = static_library(
  'physics',
  physics_files,
  dependencies: []
)

physics_dep = declare_dependency(
  link_with: physics_lib,
  include_directories: physics_inc
)

source += physics_files
inc += physics_inc
