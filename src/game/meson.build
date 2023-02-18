#src/game/meson.build

game_files = files([
  'game.c', 'game.h'
])

game_inc = include_directories('.')
game_lib = static_library(
  'game',
  game_files,
  dependencies: [block_dep, chunk_dep, render_dep, utils_dep]
)

game_dep = declare_dependency(
  link_with: game_lib,
  include_directories: game_inc
)

source += game_files
inc += game_inc
