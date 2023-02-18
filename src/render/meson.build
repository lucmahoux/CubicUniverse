#src/render/meson.build

render_files = files([
  'axes.c', 'axes.h',
  'camera.c', 'camera.h',
  'shader.c', 'shader.h',
  'skybox.c', 'skybox.h',
  'window.c', 'window.h',
  'line_renderer.c', 'line_renderer.h',
  'open_gl.c', 'open_gl.h',
  'texture_atlas.c', 'texture_atlas.h'
])

render_inc = include_directories('.')
render_lib = static_library(
  'render',
  render_files,
  dependencies: [utils_dep]
)

render_dep = declare_dependency(
  link_with: render_lib,
  include_directories: render_inc
)

source += render_files
inc += render_inc
