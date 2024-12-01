project('aoc2024', 'c')

days = [['day_01', 'src/day_01.c']]

thread_dep = dependency('threads')

foreach day : days
	executable(day)
endforeach

test('vector tests', executable('vector_tests', 'src/vector.c', 'src/vector_test.c'))