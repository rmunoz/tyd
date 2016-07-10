SET(PROJECT_TEST_DIR "${CMAKE_SOURCE_DIR}/test/")

INCLUDE(CTest)

INCLUDE(DownloadProject.cmake)
download_project(PROJ                googletest
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             master
                 "UPDATE_DISCONNECTED 1"
)

# prevent googletest from overriding our compiler/linker options
# when building with visual studio
SET(GTEST_FORCE_SHARED_CRT ON CACHE BOOL "" FORCE)

ADD_SUBDIRECTORY(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

ENABLE_TESTING()

SET(TEST_SUITES
    task_models_test_suite
    task_record_models_test_suite
    tasks_manager_test_suite
  )

FOREACH(TEST_SUITE ${TEST_SUITES})

  INCLUDE_DIRECTORIES(
      ${CMAKE_CURRENT_BINARY_DIR}
      ${CMAKE_BINARY_DIR}
      ${PROJECT_INCLUDE_DIRS}
    )

  ADD_EXECUTABLE(
      ${TEST_SUITE}
      ${PROJECT_TEST_DIR}/${TEST_SUITE}.cpp
      ${PROJECT_LIB}
    )

  TARGET_LINK_LIBRARIES(
      ${TEST_SUITE}
      ${CMAKE_THREAD_LIBS_INIT}
      LINK_PUBLIC
      ${PROJECT_LIB_NAME}
      gtest
      gmock_main
    )

  ADD_TEST(NAME ${TEST_SUITE} COMMAND ${TEST_SUITE})

  ADD_CUSTOM_TARGET(
      "run_${TEST_SUITE}"
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_SUITE}
      DEPENDS ${TEST_SUITE}
    )

  SET(
      RUN_ALL_TESTS_CMD
      ${RUN_ALL_TESTS_CMD} ${CMAKE_CURRENT_BINARY_DIR}/${TEST_SUITE} &&
    )

ENDFOREACH(TEST_SUITE)

SET(RUN_ALL_TESTS_CMD ${RUN_ALL_TESTS_CMD} /bin/true)

ADD_CUSTOM_TARGET(
    "run_all_tests"
    COMMAND ${RUN_ALL_TESTS_CMD}
    DEPENDS ${TEST_SUITES}
  )

