cmake_minimum_required(VERSION 3.26)

project(SM72445)

set(LIBRARY ${PROJECT_NAME})

enable_language(CXX)

set(CMAKE_CXX_STANDARD 17) # Requires std::optional in C++17.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB LIBRARY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp)

add_library(${LIBRARY} STATIC
	${LIBRARY_SOURCES}
)

target_include_directories(${LIBRARY} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

add_library(${LIBRARY}::${LIBRARY} ALIAS ${LIBRARY})

if(NOT CMAKE_CROSSCOMPILING)
	option(SM72445_CODE_COVERAGE "Enable gcovr code coverage for SM72445" OFF)

	include(FetchContent)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest
		GIT_TAG origin/main
	)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	enable_testing()

	target_compile_options(${LIBRARY} PRIVATE
		$<$<BOOL:${SM72445_CODE_COVERAGE}>:--coverage>
	)

	set(TEST_EXECUTABLE ${LIBRARY}_Test)

	file(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Test/*.cpp)

	add_executable(${TEST_EXECUTABLE}
		${TEST_SOURCES}
	)

	target_link_libraries(${TEST_EXECUTABLE} PRIVATE
		${PROJECT_NAME}::${LIBRARY}
		GTest::gtest_main
		GTest::gmock
	)

	target_link_options(${TEST_EXECUTABLE} PRIVATE
		$<$<BOOL:${SM72445_CODE_COVERAGE}>:--coverage>
	)

	include(GoogleTest)
	gtest_discover_tests(${TEST_EXECUTABLE})

	if(SM72445_CODE_COVERAGE)
		set(GCOVR_COMMAND gcovr --root ${CMAKE_SOURCE_DIR} --gcov-executable gcov-13 --filter '.*SM72445/.*' --exclude '.*\.test\..*' ${CMAKE_CURRENT_BINARY_DIR})
		set(SILENT_RUN_COMMAND ./${TEST_EXECUTABLE} > /dev/null)

		add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD
			COMMENT "Running gcovr Code Coverage for SM72445 Tests..."
			COMMAND ${SILENT_RUN_COMMAND} && ${GCOVR_COMMAND} || echo "Code coverage failed. All tests must pass for code coverage to run."
		)
	endif()
endif()