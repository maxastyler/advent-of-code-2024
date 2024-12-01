project('aoc2024', 'c')

days = [['day_01', 'src/day_01/day_01.c', 'src/vector.c']]

thread_dep = dependency('threads')

foreach day : days
	executable(day)
endforeach

test('vector tests', executable('vector_tests', 'src/vector.c', 'src/vector_test.c'))
test('hashmap tests', executable('hashmap_tests', 'src/hashmap.c', 'src/hashmap_test.c'))