enable_testing()

get_filename_component(PRJ_HOME ${CMAKE_CURRENT_SOURCE_DIR}/../.. ABSOLUTE)

include_directories("${PRJ_HOME}/include")

file(GLOB sources "${PRJ_HOME}/src/*.c")
list(REMOVE_ITEM sources "${PRJ_HOME}/src/main.c")

file(GLOB tests "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
list(REMOVE_ITEM tests "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp")

foreach(file ${tests})
	set(name)
	get_filename_component(name ${file} NAME_WE)
	add_executable("${name}_tests"
		${sources}
		${file}
		"${CMAKE_CURRENT_LIST_DIR}/src/main.cpp")
	target_link_libraries("${name}_tests" gtest_main)
	add_test(NAME ${name} COMMAND "${name}_tests")
endforeach()
