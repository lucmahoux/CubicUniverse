#src/utils/meson.build

utils_files = files([
  'data_struct/graph_marker.c', 'data_struct/graph_marker.h',
  'data_struct/hashmap.c', 'data_struct/hashmap.h',
  'data_struct/matrix.c', 'data_struct/matrix.h',
  'data_struct/queue.c', 'data_struct/queue.h',
  'data_struct/list.c', 'data_struct/list.h',
  'data_struct/vector.c', 'data_struct/vector.h',
  'utils.c', 'utils.h',
  'stb_image.h',
  'stb_image_write.h',
  'types/block.h',
  'types/chunk.h',
  'types/texture.h',
  'types/open_gl.h',
  'types/physics_utils.h', 'types/physics_utils.c'
])

utils_inc = include_directories('.')
utils_lib = static_library('utils', utils_files)

utils_dep = declare_dependency(
  link_with: utils_lib,
  include_directories: utils_inc
)

source += utils_files
inc += utils_inc
