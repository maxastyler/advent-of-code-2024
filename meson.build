project('aoc2024', 'c')

days = [['day_01', 'src/day_01/day_01.c', 'src/vector.c', 'src/hashmap.c'],
        ['day_02', 'src/day_02/day_02.c', 'src/aoclib.c'],
	['day_03', 'src/day_03/day_03.c', 'src/aoclib.c'],
	['day_04', 'src/day_04/day_04.c', 'src/aoclib.c'],
	['day_05', 'src/day_05/day_05.c', 'src/aoclib.c', 'src/gen_hashmap.c'],
	['day_06', 'src/day_06/day_06.c', 'src/aoclib.c', 'src/gen_hashmap.c'],
	['day_07', 'src/day_07/day_07.c', 'src/aoclib.c', ],]

foreach day : days
	executable(day)
endforeach

test('vector tests', executable('vector_tests', 'src/vector.c', 'src/vector_test.c'))
test('hashmap tests', executable('hashmap_tests', 'src/hashmap.c', 'src/hashmap_test.c'))
test('aoclib tests', executable('aoclib_tests', 'src/aoclib_test.c', 'src/aoclib.c'))
test('gen_hashmap tests', executable('gen_hashmap_tests', 'src/gen_hashmap_test.c', 'src/gen_hashmap.c'))
test('gen_vector tests', executable('gen_vector_tests', 'src/gen_vector_test.c', 'src/gen_vector.c'))