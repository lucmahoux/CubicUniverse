project('CubicUniverse', 'c',
        version: '0.1',
        default_options: [
          'warning_level=3',
          'c_std=c99',
          'c_args=-D_POSIX_C_SOURCE=200809L'
        ])

source =  []
inc = []

cc = meson.get_compiler('c')

glfw3_dep = dependency('glfw3')
glew_dep = dependency('glew')
opengl_dep = dependency('opengl')
math_dep = cc.find_library('m', required: true)

subdir('src')

if get_option('enable_tests') == true
  subdir('tests')
endif

executable('CubicUniverse', source,
    include_directories: inc,
    dependencies: [
      glfw3_dep, glew_dep, opengl_dep, math_dep,
      utils_dep, render_dep, block_dep, chunk_dep, game_dep
    ])
