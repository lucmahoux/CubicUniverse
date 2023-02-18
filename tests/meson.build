#tests/meson.build

criterion_dep = dependency('criterion', required: false)

if criterion_dep.found()
  hashmap = executable('hashmap', 'test_hashmap.c', include_directories: inc,
    dependencies: [utils_dep, math_dep, criterion_dep])
  list = executable('list', 'test_list.c', include_directories: inc,
    dependencies: [utils_dep, math_dep, criterion_dep])
  matrix = executable('matrix', 'test_matrix.c', include_directories: inc,
    dependencies: [utils_dep, math_dep, criterion_dep])
  queue = executable('queue', 'test_queue.c', include_directories: inc,
    dependencies: [utils_dep, math_dep, criterion_dep])

  test('HashMap', hashmap)
  test('List', list)
  test('Matrix', matrix)
  test('Queue - FIFO', queue)
else
  warning('Criterion not found -- cannot run the testsuite!')
endif
