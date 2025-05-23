if(NOT EIGEN_TEST_NOQT)
  find_package(Qt4)
  if(QT4_FOUND)
    include(${QT_USE_FILE})
  endif()
endif(NOT EIGEN_TEST_NOQT)

if(QT4_FOUND)
  add_executable(Tutorial_sparse_example Tutorial_sparse_example.cpp Tutorial_sparse_example_details.cpp)
  target_link_libraries(Tutorial_sparse_example ${EIGEN_STANDARD_LIBRARIES_TO_LINK_TO} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

  add_custom_command(
    TARGET Tutorial_sparse_example
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/../html/
    COMMAND Tutorial_sparse_example ARGS ${CMAKE_CURRENT_BINARY_DIR}/../html/Tutorial_sparse_example.jpeg
  )

  add_dependencies(all_examples Tutorial_sparse_example)
endif(QT4_FOUND)

check_cxx_compiler_flag("-std=c++23" EIGEN_COMPILER_SUPPORT_CPP23)
if(EIGEN_COMPILER_SUPPORT_CPP23)
  add_executable(random_cpp23 random_cpp23.cpp)
  target_link_libraries(random_cpp23 ${EIGEN_STANDARD_LIBRARIES_TO_LINK_TO})
  add_dependencies(all_examples random_cpp23)
  ei_add_target_property(random_cpp23 COMPILE_FLAGS "-std=c++23")

  add_custom_command(
    TARGET random_cpp23
    POST_BUILD
    COMMAND random_cpp23
    ARGS >${CMAKE_CURRENT_BINARY_DIR}/random_cpp23.out
  )
endif()
